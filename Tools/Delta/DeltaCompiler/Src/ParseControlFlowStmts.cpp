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

DeltaExpr* Translator::Translate_Condition (DeltaExpr* cond) {

	NULL_EXPR_CHECK(cond);

	if (!TYPECHECKER.Check_UseAsBoolean(cond))
		return (DeltaExpr*) 0;

	DPTR(cond)->CheckUninitialised();

	// If the condition is a constant expression, we have to
	// produce the necessary code for backpatching.
	//
	if (DPTR(cond)->IsComputableBoolean()) {

		DeltaExpr* result	= EXPRFACTORY.New();
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

		DeltaExpr* result = EXPRFACTORY.New();
		DPTR(result)->type = DeltaExprLogical;
		result->sym = cond->IsTemp() ? cond->sym : DELTASYMBOLS.NewTemp();

		TRANSLATOR.DELTA_EXPR_EMIT_BOOL_TEST(cond, result, true);

		DPTR(result)->SetUnparsed(DPTR(cond)->GetUnparsed());
		return result;
	}
	else 
		return cond;
}

///////////////////////////////////////////////////////////////////

DeltaQuadAddress Translator::Translate_N (void) {
	QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	return QUADS.CurrQuadNo();
}

///////////////////////////////////////////////////////////////////

void Translator::Translate_IfStmtPrefix (void) 
	{ PARSEPARMS.EnteringIf(); }

void Translator::Translate_ElseStmtPrefix (void)
	{  PARSEPARMS.ExitingIfFollowedByElse(); PARSEPARMS.EnteringElse(); }

///////////////////////////////////////////////////////////////////

void Translator::Translate_IfStmt (DeltaExpr* cond, DeltaQuadAddress Mquad) {

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

	PARSEPARMS.ExitingIf(condValue);
}

///////////////////////////////////////////////////////////////////

void Translator::Translate_IfElseStmt (
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

	PARSEPARMS.ExitingElse(condValue);
}

///////////////////////////////////////////////////////////////////

void Translator::BackpatchLoop (Stmt* stmt, DeltaQuadAddress contQuad, DeltaQuadAddress breakQuad, DeltaQuadAddress stmtBegin) {

	if (stmt) {

		QUADS.BackpatchBlockExitsDown(DPTR(stmt)->contList, contQuad);
		QUADS.BackpatchExplicitTrapDisables(DPTR(stmt)->contList, stmtBegin);
		QUADS.Backpatch(DPTR(stmt)->contList, contQuad);

		QUADS.BackpatchBlockExitsDown(DPTR(stmt)->breakList, breakQuad);
		QUADS.BackpatchExplicitTrapDisables(DPTR(stmt)->breakList, stmtBegin);
		QUADS.Backpatch(stmt->breakList, breakQuad);

		DDELETE(stmt);
	}
}

///////////////////////////////////////////////////////////////////

void Translator::Translate_WhilePrefix (void) 
	{ PARSEPARMS.EnteringLoop(); }

Stmt* Translator::Translate_WhileStmt (
		DeltaExpr*			cond, 
		DeltaQuadAddress	M1quad,	// Before while condition.
		DeltaQuadAddress	M2quad,	// Before while stmt (while stmt start)
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

		BackpatchLoop(stmt, QUADS.CurrQuadNo(), QUADS.NextQuadNo(), M2quad);
	}

	PARSEPARMS.ExitingLoop(condValue);
	QUADS.SetQuadLine(M2quad, stmtLine);

	return NEW_STMT;
}

///////////////////////////////////////////////////////////////////

void Translator::Translate_ForOpening (void) 
	{}

void Translator::Translate_ForPrefix (DeltaQuadAddress	initListQuad, util_ui32	initListLine) {
	PARSEPARMS.EnteringLoop(); 
	QUADS.SetQuadLine(initListQuad,initListLine, true);
}

//*****************************

Stmt*	Translator::Translate_ForStmt (
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

		QUADS.Backpatch(DPTR(cond)->trueList, M3quad);		// Loop enter jump.
		if (M1quad != M2quad)								// Non-constant expression
			QUADS.Backpatch(QUADS.MakeList(Nquad), M1quad);	// Loop iteration jump.
		else {
			DASSERT(cond->IsComputableBoolean());			// The only way for expr to produce no quads
			QUADS.Backpatch(QUADS.MakeList(Nquad), M3quad);	// In case it is true the stmt is reentered
		}

		QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR, M2quad);	// Loop closure jump.
		QUADS.Backpatch(cond->falseList, QUADS.NextQuadNo());

		BackpatchLoop(stmt, QUADS.CurrQuadNo(), QUADS.NextQuadNo(), M3quad);
	}

	PARSEPARMS.ExitingLoop(condValue);
	return NEW_STMT;
}

///////////////////////////////////////////////////////////////////

Stmt* Translator::Translate_ForeachPrefix (DeltaExpr* foreachVar, DeltaExpr* foreachIndex, DeltaExpr* foreachContainer) {

	if (!foreachVar || !foreachContainer)
		return NIL_STMT;

	if (!TYPECHECKER.Check_ForeachContainer(foreachContainer))
		return NIL_STMT;

	PARSEPARMS.EnteringLoop(); 

	DeltaExpr*		iterExpr	= EXPRFACTORY.MakeInternalVar(DELTASYMBOLS.NewTemp());
	DeltaExpr*		valueExpr	= EXPRFACTORY.MakeInternalVar(DELTASYMBOLS.NewTemp());
	Stmt*			stmt		= NEW_STMT;

	DPTR(stmt)->userData = iterExpr;	// We store it here as we need this in code gen.
	DPTR(foreachVar)->SetInitialised();

	QUADS.Emit(DeltaIC_FOREACHBEGIN,	foreachContainer, NIL_EXPR, iterExpr);
	QUADS.Emit(DeltaIC_FOREACHCHECKEND, foreachContainer, iterExpr, NIL_EXPR);

	QUADS.Emit(DeltaIC_FOREACHGETVAL,	iterExpr, NIL_EXPR, valueExpr);
	Translate_AssignExpr(foreachVar, valueExpr);

	if (foreachIndex) {
		DeltaExpr* indexExpr = EXPRFACTORY.MakeInternalVar(DELTASYMBOLS.NewTemp());
		QUADS.Emit(DeltaIC_FOREACHGETINDEX, iterExpr, NIL_EXPR, indexExpr);
		Translate_AssignExpr(foreachIndex, indexExpr);
		DPTR(foreachIndex)->SetInitialised();
	}

	return stmt;
}

//*****************************

Stmt* Translator::Translate_ForeachStmt (
		Stmt*				foreachPrefix, 
		Stmt*				foreachStmt, 
		DeltaQuadAddress	Mquad	//	first quad of stmt
	) {

	if (!foreachPrefix || !foreachStmt)
		return NIL_STMT;

	DeltaQuadAddress stmtStart = Mquad;
	do
		--Mquad;					// the foreach condition is actually check iterator end
	while (QUADS.GetQuad(Mquad).opcode != DeltaIC_FOREACHCHECKEND);

	QUADS.Emit(DeltaIC_FOREACHFWD, (DeltaExpr*) foreachPrefix->userData, NIL_EXPR, NIL_EXPR);
	QUADS.Patch(QUADS.CurrQuadNo(), Mquad);	// Loop iteration (a fwd performing a jump).

	QUADS.Patch(Mquad, QUADS.NextQuadNo());

	BackpatchLoop(foreachStmt, QUADS.CurrQuadNo(), QUADS.NextQuadNo(), stmtStart);
	QUADS.Emit(DeltaIC_FOREACHEND, DNULLCHECK((DeltaExpr*) foreachPrefix->userData), NIL_EXPR, NIL_EXPR);

	PARSEPARMS.ExitingLoop(); 
	return NEW_STMT;
}

//------------------------------------------------------------------
