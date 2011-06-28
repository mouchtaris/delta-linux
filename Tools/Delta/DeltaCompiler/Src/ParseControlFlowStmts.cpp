// ParseControlFlowStmts.cpp.
// Translation rules for control flow statements.
// ScriptFighter Project.
// A. Savidis, October 1999 (original 1st version).
// Re-factored last August 2006.
// Fixed a bug on the retlist management of loop stmts, January 2008.
//

#include <string.h>
#include <limits.h>
#include <math.h>

#include "DDebug.h"
#include "ParseActions.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaCompErrorMsg.h"
#include "Symbol.h"
#include "TypeCheck.h"
#include "DeltaCompErrorDefs.h"

//------------------------------------------------------------------
// CONTROL FLOW.

// If the condition is not a boolean expression, i.e., it does not
// have already jump statements for true and false, we make explicitly
// some.
//

DeltaExpr* Translate_Condition (DeltaExpr* cond) {

	NULL_EXPR_CHECK(cond);

	if (!TypeCheck_UseAsBoolean(cond))
		return (DeltaExpr*) 0;

	DPTR(cond)->CheckUninitialised();

	// If the condition is a constant expression, we have to
	// produce the necessary code for backpatching.
	//
	if (DPTR(cond)->IsComputableBoolean()) {

		DeltaExpr* result	= DNEW(DeltaExpr);
		DPTR(result)->type	= DeltaExprBoolean;

		// Since true condition leads to the execution of the statement in code
		// that naturally follows, we only need to generate a jump in case the
		// expression is false.
		//
		if (!(DPTR(result)->boolConst = DPTR(cond)->ToBool())) {
			DPTR(result)->falseList = QUADS.MakeList(QUADS.NextQuadNo());
			QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
		}

		DPTR(result)->SetUnparsed(DPTR(cond)->GetUnparsed());
		return result;
	}
	else
	if (DPTR(cond)->type != DeltaExprLogical) { // Used now as a logical expression.

		DeltaExpr* result = DNEW(DeltaExpr);
		DPTR(result)->type = DeltaExprLogical;
		result->sym = cond->IsTemp() ? cond->sym : DELTASYMBOLS.NewTemp();

		DELTA_EXPR_EMIT_BOOL_TEST(cond, result, true);

		DPTR(result)->SetUnparsed(DPTR(cond)->GetUnparsed());
		return result;
	}
	else 
		return cond;
}

///////////////////////////////////////////////////////////////////

DeltaQuadAddress Translate_N (void) {
	QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	return QUADS.CurrQuadNo();
}

///////////////////////////////////////////////////////////////////

void Translate_IfStmtPrefix (void) 
	{ ParseParms::EnteringIf(); }

void Translate_ElseStmtPrefix (void)
	{  ParseParms::ExitingIfFollowedByElse(); ParseParms::EnteringElse(); }

///////////////////////////////////////////////////////////////////

void Translate_IfStmt (DeltaExpr* cond, DeltaQuadAddress Mquad) {

	ParseParms::ConditionValue condValue = ParseParms::CondCantTell;

	if (cond) {

		if (cond->IsComputableBoolean()) {	// Warning for unreachable code.

			DASSERT(cond->type == DeltaExprBoolean);
			if (!DPTR(cond)->boolConst)
				{ DELTACOMP_WARNING_NEVER_IF(); condValue =  ParseParms::CondFalse; }
			else	// All vars initialised in if (true) are guaranteed to remain initialised.
				{ DELTACOMP_WARNING_ALWAYS_IF(); condValue =  ParseParms::CondTrue; }
		}

		QUADS.Backpatch(DPTR(cond)->trueList, Mquad);
		QUADS.Backpatch(cond->falseList, QUADS.NextQuadNo());
	}

	ParseParms::ExitingIf(condValue);
}

///////////////////////////////////////////////////////////////////

void Translate_IfElseStmt (
		DeltaExpr*			cond, 
		DeltaQuadAddress	M1quad, 
		DeltaQuadAddress	Nquad,
		DeltaQuadAddress	M2quad
	) {

	ParseParms::ConditionValue condValue = ParseParms::CondCantTell;

	if (cond) {

		if (cond->IsComputableBoolean()) { // Warning for unreachable code.

			DASSERT(cond->type == DeltaExprBoolean);
			if (!DPTR(cond)->boolConst)
				{ DELTACOMP_WARNING_ALWAYS_ELSE(); condValue =  ParseParms::CondFalse; }
			else
				{ DELTACOMP_WARNING_NEVER_ELSE(); condValue =  ParseParms::CondTrue; }
		}

		QUADS.Backpatch(DPTR(cond)->trueList, M1quad);
		QUADS.Backpatch(cond->falseList, M2quad);
		QUADS.Backpatch(QUADS.MakeList(Nquad), QUADS.NextQuadNo());
	}

	ParseParms::ExitingElse(condValue);
}

///////////////////////////////////////////////////////////////////

static void BackpatchLoop (Stmt* stmt, DeltaQuadAddress contQuad, DeltaQuadAddress breakQuad) {

	if (stmt) {

		QUADS.BackpatchBlockExitsDown(DPTR(stmt)->contList, contQuad);
		QUADS.Backpatch(DPTR(stmt)->contList, contQuad);

		QUADS.BackpatchBlockExitsDown(DPTR(stmt)->breakList, breakQuad);
		QUADS.Backpatch(stmt->breakList, breakQuad);

		DDELETE(stmt);
	}
}

///////////////////////////////////////////////////////////////////

void Translate_WhilePrefix (void) 
	{ ParseParms::EnteringLoop(); }

Stmt* Translate_WhileStmt (
		DeltaExpr*			cond, 
		DeltaQuadAddress	M1quad,	// Before while condition.
		DeltaQuadAddress	M2quad,	// Before while stmt.
		Stmt*				stmt,
		util_ui32			stmtLine
	) {

	ParseParms::ConditionValue condValue = ParseParms::CondCantTell;

	if (cond) {

		if (cond->IsComputableBoolean()) {

			DASSERT(cond->type == DeltaExprBoolean);

			if (DPTR(cond)->boolConst)
				{ DELTACOMP_WARNING_WHILE_TRUE(); condValue = ParseParms::CondTrue; }
			else
				{ DELTACOMP_WARNING_WHILE_FALSE(); condValue = ParseParms::CondFalse; }
		}

		QUADS.Backpatch(DPTR(cond)->trueList, M2quad);
		QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR, M1quad);	// Loop iteration jump.
		QUADS.Backpatch(cond->falseList, QUADS.NextQuadNo());

		BackpatchLoop(stmt, QUADS.CurrQuadNo(), QUADS.NextQuadNo());
	}

	ParseParms::ExitingLoop(condValue);
	QUADS.SetQuadLine(M2quad, stmtLine);

	return NEW_STMT;
}

///////////////////////////////////////////////////////////////////

void Translate_ForOpening (void) 
	{}

void Translate_ForPrefix (DeltaQuadAddress	initListQuad, util_ui32	initListLine) {
	ParseParms::EnteringLoop(); 
	QUADS.SetQuadLine(initListQuad,initListLine, true);
}

//*****************************

Stmt*	Translate_ForStmt (
		DeltaExpr*			cond, 
		DeltaQuadAddress	M1quad,	// Evaluate for condition.
		DeltaQuadAddress	M2quad,	// Evaluate for closure.
		DeltaQuadAddress	M3quad,	// Beginning of stmt.
		DeltaQuadAddress	Nquad,	// After closure evaluation.
		Stmt*				stmt
	) {

	ParseParms::ConditionValue condValue = ParseParms::CondCantTell;

	if (cond) {

		if (cond->IsComputableBoolean()) {

			DASSERT(cond->type == DeltaExprBoolean);

			if (DPTR(cond)->boolConst)
				{ DELTACOMP_WARNING_FOR_TRUE(); condValue = ParseParms::CondTrue; }
			else
				{ DELTACOMP_WARNING_FOR_FALSE(); condValue = ParseParms::CondFalse; }
		}

		QUADS.Backpatch(DPTR(cond)->trueList, M3quad);	// Loop enter jump.
		QUADS.Backpatch(QUADS.MakeList(Nquad), M1quad);	// Loop iteration jump.

		QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR, M2quad);	// Loop closure jump.
		QUADS.Backpatch(cond->falseList, QUADS.NextQuadNo());

		BackpatchLoop(stmt, QUADS.CurrQuadNo(), QUADS.NextQuadNo());
	}

	ParseParms::ExitingLoop(condValue);
	return NEW_STMT;
}

///////////////////////////////////////////////////////////////////

Stmt* Translate_ForeachPrefix (DeltaExpr* foreachVar, DeltaExpr* foreachIndex, DeltaExpr* foreachContainer) {

	if (!foreachVar || !foreachContainer)
		return NIL_STMT;

	if (!TypeCheck_ForeachContainer(foreachContainer))
		return NIL_STMT;

	ParseParms::EnteringLoop(); 

	DeltaExpr*		iterExpr	= DeltaExpr::MakeInternalVar(DELTASYMBOLS.NewTemp());
	DeltaExpr*		valueExpr	= DeltaExpr::MakeInternalVar(DELTASYMBOLS.NewTemp());
	Stmt*			stmt		= NEW_STMT;

	DPTR(stmt)->userData = iterExpr;	// We store it here as we need this in code gen.
	DPTR(foreachVar)->SetInitialised();

	QUADS.Emit(DeltaIC_FOREACHBEGIN,	foreachContainer, NIL_EXPR, iterExpr);
	QUADS.Emit(DeltaIC_FOREACHCHECKEND, foreachContainer, iterExpr, NIL_EXPR);

	QUADS.Emit(DeltaIC_FOREACHGETVAL,	iterExpr, NIL_EXPR, valueExpr);
	Translate_AssignExpr(foreachVar, valueExpr);

	if (foreachIndex) {
		DeltaExpr* indexExpr = DeltaExpr::MakeInternalVar(DELTASYMBOLS.NewTemp());
		QUADS.Emit(DeltaIC_FOREACHGETINDEX, iterExpr, NIL_EXPR, indexExpr);
		Translate_AssignExpr(foreachIndex, indexExpr);
		DPTR(foreachIndex)->SetInitialised();
	}

	return stmt;
}

//*****************************

Stmt* Translate_ForeachStmt (
		Stmt*				foreachPrefix, 
		Stmt*				foreachStmt, 
		DeltaQuadAddress	Mquad
	) {

	if (!foreachPrefix || !foreachStmt)
		return NIL_STMT;

	do
		--Mquad;
	while (QUADS.GetQuad(Mquad).opcode != DeltaIC_FOREACHCHECKEND);

	QUADS.Emit(DeltaIC_FOREACHFWD, (DeltaExpr*) foreachPrefix->userData, NIL_EXPR, NIL_EXPR);
	QUADS.Patch(QUADS.CurrQuadNo(), Mquad);	// Loop iteration (a fwd performing a jump).

	QUADS.Patch(Mquad, QUADS.NextQuadNo());

	BackpatchLoop(foreachStmt, QUADS.CurrQuadNo(), QUADS.NextQuadNo());
	QUADS.Emit(DeltaIC_FOREACHEND, DNULLCHECK((DeltaExpr*) foreachPrefix->userData), NIL_EXPR, NIL_EXPR);

	ParseParms::ExitingLoop(); 
	return NEW_STMT;
}

//------------------------------------------------------------------
