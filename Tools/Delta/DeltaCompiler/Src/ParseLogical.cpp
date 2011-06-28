// ParseLogical.cpp.
// Translation rules for logical expressions.
// ScriptFighter Project.
// A. Savidis, October 1999 (original 1st version).
// Re-factored last August 2006.
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
// BACKPATCHING. LOGICAL OPERATORS.

DeltaExpr* Translate_ExprORExpr (DeltaExpr* e1, DeltaQuadAddress Mquad, DeltaExpr* e2) {
	
	NULL_EXPR_PAIR_CHECK(e1, e2);

	if (!TypeCheck_UseAsBoolean(e1) || !TypeCheck_UseAsBoolean(e2))
		return NIL_EXPR;

	DPTR(e1)->CheckUninitialised();
	DPTR(e2)->CheckUninitialised();

	DPTR(e2)->PreEvaluateIfConstBool();

	QUADS.Backpatch(DPTR(e1)->falseList, Mquad);
	DeltaExpr* result = DNEW(DeltaExpr);

	if (DPTR(e1)->type == DeltaExprBoolean && DPTR(e2)->type == DeltaExprBoolean)
		DPTR(result)->SetBoolean(e1->boolConst || e2->boolConst);
	else
		DPTR(result)->type = DeltaExprLogical;

	if (DPTR(e2)->type  != DeltaExprLogical) {
		DASSERT(!e2->trueList && !e2->falseList);
		DELTA_EXPR_EMIT_BOOL_TEST(e2, result, true);
		result->trueList = QUADS.Merge(e1->trueList, result->trueList);
	}
	else {
		DASSERT(e2->trueList && e2->falseList);
		result->trueList  = QUADS.Merge(QUADS.Merge(e1->trueList, e2->trueList), result->trueList);
		result->falseList = QUADS.Merge(e2->falseList, result->falseList);
	}

	return result;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translate_ExprANDExpr (DeltaExpr* e1, DeltaQuadAddress Mquad, DeltaExpr* e2) {

	NULL_EXPR_PAIR_CHECK(e1, e2);

	if (!TypeCheck_UseAsBoolean(e1) || !TypeCheck_UseAsBoolean(e2))
		return NIL_EXPR;

	DPTR(e1)->CheckUninitialised();
	DPTR(e2)->CheckUninitialised();
	DPTR(e2)->PreEvaluateIfConstBool();

	QUADS.Backpatch(DPTR(e1)->trueList, Mquad);
	DeltaExpr* result = DNEW(DeltaExpr);

	if (DPTR(e1)->type == DeltaExprBoolean && DPTR(e2)->type == DeltaExprBoolean)
		DPTR(result)->SetBoolean(e1->boolConst && e2->boolConst);
	else
		DPTR(result)->type = DeltaExprLogical;

	if (DPTR(e2)->type != DeltaExprLogical) {
		DASSERT(!e2->trueList && !e2->falseList);
		DELTA_EXPR_EMIT_BOOL_TEST(e2, result, true);
		result->falseList =	QUADS.Merge(e1->falseList, result->falseList);
	}
	else {
		DASSERT(e2->trueList && e2->falseList);
		result->trueList  =	QUADS.Merge(e2->trueList, result->trueList);
		result->falseList =	QUADS.Merge(QUADS.Merge(e1->falseList, e2->falseList), result->falseList);
	}

	return result;
}

///////////////////////////////////////////////////////////////////

DeltaExpr*	Translate_RelationalExpr (
		DeltaExpr*		e1, 
		DeltaExpr*		e2, 
		DeltaICOpcode	relOp, 
		const char*		opStr
	) {

	NULL_EXPR_PAIR_CHECK(e1, e2);

	if (!TypeCheck_InRelational(e1, relOp == DeltaIC_JEQ || relOp == DeltaIC_JNE)) {
		DELTACOMP_ERROR_ILLEGAL_OPERAND_IN_RELATIONAL(opStr, "LHS");
		return NIL_EXPR;
	}
	else
	if (!TypeCheck_InRelational(e2, relOp == DeltaIC_JEQ || relOp == DeltaIC_JNE)) {
		DELTACOMP_ERROR_ILLEGAL_OPERAND_IN_RELATIONAL(opStr, "RHS");
		return NIL_EXPR;
	}
	else
	if (!TypeCheck_InRelational(e1, e2, relOp)) {
		DELTACOMP_ERROR_INCOMPATIBLE_OPERANDS_IN_RELATIONAL(opStr);
		return NIL_EXPR;
	}

	DPTR(e1)->CheckUninitialised();
	DPTR(e2)->CheckUninitialised();

	DeltaExpr* result	= DNEW(DeltaExpr);
	DPTR(result)->type	= DeltaExprLogical;
	
	// If the expressions are invariant compile-time values, then the
	// relational operator is applicable at compile-time and the result
	// is a computed constant boolean value. The cases are the following:
	// (1) string constants
	// (2) boolean constants
	// (3) numeric constants
	// (4) functions (program or library)
	//
	if (DPTR(e1)->IsInvariantValue() && DPTR(e2)->IsInvariantValue()) {
		
		bool resultValue;
		
		if (DPTR(e1)->type == DeltaExprString || DPTR(e2)->type == DeltaExprString) {
			
			if (e1->type == DeltaExprString && e2->type == DeltaExprString) {
			
				util_i32 cmpResult = strcmp(e1->strConst.c_str(), e2->strConst.c_str());

				switch (relOp) {

					case DeltaIC_JGT : resultValue = cmpResult > 0;		break;
					case DeltaIC_JGE : resultValue = cmpResult >= 0;	break;
					case DeltaIC_JLT : resultValue = cmpResult < 0;		break;
					case DeltaIC_JLE : resultValue = cmpResult <= 0;	break;
					case DeltaIC_JEQ : resultValue = !cmpResult;		break;
					case DeltaIC_JNE : resultValue = !!cmpResult;		break;

					default: DASSERT(false);
				}
			}
			else {
				DASSERT(relOp == DeltaIC_JEQ || relOp == DeltaIC_JNE);
				resultValue = false;
			}
		}
		else
		if (DPTR(e1)->type == DeltaExprNumber || DPTR(e2)->type == DeltaExprNumber) {

			DASSERT(e1->type == DeltaExprNumber && e2->type==DeltaExprNumber);

			DeltaNumberValueType a = DPTR(e1)->numConst;
			DeltaNumberValueType b = DPTR(e2)->numConst;

			switch (relOp) {

				case DeltaIC_JGT : resultValue = a > b;		break;
				case DeltaIC_JGE : resultValue = a >= b;	break;
				case DeltaIC_JLT : resultValue = a < b;		break;
				case DeltaIC_JLE : resultValue = a <= b;	break;
				case DeltaIC_JEQ : resultValue = a == b;	break;
				case DeltaIC_JNE : resultValue = a != b;	break;

				default: DASSERT(false);
			}
		}
		else	// Nil equals only its self and differs with everything else.
		if (DPTR(e1)->type == DeltaExprNil || DPTR(e2)->type == DeltaExprNil) {
			if (relOp == DeltaIC_JEQ)
				resultValue = (DPTR(e1)->type == DPTR(e2)->type);
			else
			if (relOp == DeltaIC_JNE)
				resultValue = (DPTR(e1)->type != DPTR(e2)->type);
			else
				DASSERT(false);
		}
		else
		if (DPTR(e1)->type == DeltaExprBoolean || DPTR(e2)->type == DeltaExprBoolean) {

			DASSERT(e1->IsComputableBoolean() && e2->IsComputableBoolean());

			if (relOp == DeltaIC_JEQ)
				resultValue = (DPTR(e1)->ToBool() == DPTR(e2)->ToBool());
			else
			if (relOp == DeltaIC_JNE)
				resultValue = (DPTR(e1)->ToBool() != DPTR(e2)->ToBool());
			else
				DASSERT(false);
		}
		else
		if (DPTR(e1)->type == DeltaExprProgramFunction	|| 
			DPTR(e2)->type == DeltaExprProgramFunction	||
			DPTR(e1)->type == DeltaExprLibraryFunction	|| 
			DPTR(e2)->type == DeltaExprLibraryFunction) {

			DASSERT(
				(e1->type==DeltaExprProgramFunction && 
				 e2->type==DeltaExprProgramFunction) ||
				(e1->type==DeltaExprLibraryFunction && 
				 e2->type==DeltaExprLibraryFunction)
			);

			// Equality for functions means they correspond to the
			// same compile time symbol.
			//
			if (relOp == DeltaIC_JEQ)
				resultValue = (e1->sym == e2->sym);
			else
			if (relOp == DeltaIC_JNE)
				resultValue = (e1->sym != e2->sym);
			else
				DASSERT(false);
		}

		DPTR(result)->SetBoolean(resultValue);
	}
	else {
		DPTR(result)->SetTypeTag(TagBool);
		DPTR(result)->GetTypeInfo().Set(TagBool);

		result->trueList	= QUADS.MakeList(QUADS.NextQuadNo());
		result->falseList	= QUADS.MakeList(QUADS.NextQuadNoPlus(1));

		QUADS.Emit(relOp, e1, e2, NIL_EXPR);
		QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	}

	return result;
}

///////////////////////////////////////////////////////////////////

DeltaExpr* Translate_NOTExpression (DeltaExpr* expr) {

	NULL_EXPR_CHECK(expr);

	if (!TypeCheck_UseAsBoolean(expr))
		return (DeltaExpr*) 0;

	DPTR(expr)->CheckUninitialised();

	DeltaExpr* result	= DNEW(DeltaExpr);
	DPTR(result)->type	= DeltaExprLogical;

	if (DPTR(expr)->IsComputableBoolean()) {
		result->type		= DeltaExprBoolean;
		result->boolConst	= !DPTR(expr)->ToBool();
	}
	else {
		if (DPTR(expr)->type != DeltaExprLogical) {
			result->sym = expr->IsTemp() ? expr->sym : DELTASYMBOLS.NewTemp();
			DELTA_EXPR_EMIT_BOOL_TEST(expr, result, false);
		}
		else {
			result->trueList  = expr->falseList;
			result->falseList = expr->trueList;
		}
	}

	return result;
}
//------------------------------------------------------------------
