// ParseArithmetic.cpp.
// Translation rules for arithmetic expressions.
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
// ARITHMETIC.
//

static DeltaNumberValueType CheckDivisionByZero (DeltaNumberValueType& num) {
	if (!num) {
		DELTACOMP_ERROR_DIVISION_BY_ZERO();
		return 1;
	}
	else
		return num;
}

//******************************

DeltaExpr*	Translate_ArithmeticExpression (
		DeltaExpr*		e1, 
		DeltaICOpcode	op, 
		DeltaExpr*		e2, 
		const char*		opStr
	) {

	NULL_EXPR_PAIR_CHECK(e1, e2);

	DPTR(e1)->CheckUninitialised();
	DPTR(e2)->CheckUninitialised();

	DeltaExpr* result = NIL_EXPR;

	// Due to overloading all arith operators should be generally allowed when vars or
	// table ctors are met.
	bool warning =	DPTR(e1)->GetType() == DeltaExprVar					|| 
					DPTR(e2)->GetType() == DeltaExprVar					||
					DPTR(e1)->GetType() == DeltaExprTableConstruction	|| 
					DPTR(e2)->GetType() == DeltaExprTableConstruction;

	if (!TypeCheck_InArithmetic(e1, op, opStr, !warning) || !TypeCheck_InArithmetic(e2, op, opStr, !warning))
		if (!warning)
			return result;

	// Try static computation os string op string or num op num.
	if (DPTR(e1)->type == DeltaExprNumber && DPTR(e2)->type == DeltaExprNumber) {
		
		DeltaNumberValueType resultValue;

		switch (op) {
			case DeltaIC_ADD : resultValue = e1->numConst + e2->numConst; break;
			case DeltaIC_SUB : resultValue = e1->numConst - e2->numConst; break;
			case DeltaIC_MUL : resultValue = e1->numConst * e2->numConst; break;
			case DeltaIC_DIV : resultValue = e1->numConst / CheckDivisionByZero(e2->numConst); break;
			case DeltaIC_MOD : {
				resultValue = ((util_i32) e1->numConst) % ((util_i32) CheckDivisionByZero(e2->numConst)); 
				break;
			}

			default : DASSERT(false);
		}

		result = DNEW(DeltaExpr);
		DPTR(result)->SetNumber(resultValue);
	}
	else
	if (DPTR(e1)->type == DeltaExprString && DPTR(e2)->type == DeltaExprString) {
		if (op == DeltaIC_ADD) {
			result = DNEW(DeltaExpr);
			DPTR(result)->SetString(uconstructstr("%s%s", e1->strConst.c_str(), e2->strConst.c_str()));
		}
		else
			DELTACOMP_ERROR_INVALID_OP_FOR_STRING_CONSTS(opStr);
	}
	else {
		result = DNEW(DeltaExpr);
		DPTR(result)->type	=	DeltaExprArithmetic;
		DPTR(result)->sym	=	DPTR(e1)->IsTemp() ? e1->sym : 
								DPTR(e2)->IsTemp() ? e2->sym : DELTASYMBOLS.NewTemp();
		QUADS.Emit(
			op,
			e1,
			e2,
			result
		);
	}
	
	return result;
}

//------------------------------------------------------------------
