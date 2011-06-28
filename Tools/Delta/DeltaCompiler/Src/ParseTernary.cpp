// ParseTernary.cpp.
// Translation rules for ternary expressions.
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
#include "Optimizer.h"

//------------------------------------------------------------------
// TERNARY

DeltaExpr*	Translate_Ternary (
		DeltaExpr*			cond,
		DeltaExpr*			expr1,
		DeltaExpr*			expr2,
		DeltaQuadAddress	M1quad,
		DeltaQuadAddress	M2quad,
		DeltaQuadAddress	Nquad
	) {

	if (cond && expr1 && expr2) {

		DeltaExpr* result = DNEW(DeltaExpr);

		DPTR(result)->sym =	DPTR(expr1)->IsTemp() ? expr1->sym : 
							DPTR(expr2)->IsTemp() ? expr2->sym : 
							DELTASYMBOLS.NewTemp();

		QUADS.Backpatch(DPTR(cond)->trueList, M1quad);
		QUADS.Backpatch(cond->falseList, M2quad);
		QUADS.Backpatch(QUADS.MakeList(Nquad), QUADS.NextQuadNoPlus(2));

		// We need to exclude from basic block optimization the
		// two assignments to the temp variable, as both are needed. 
		// This is an exceptional case where temp value is retained
		// across its basic block.
		//
		QUADS.Emit(DeltaIC_ASSIGN, expr2, NIL_EXPR,	result);
		OPTIMIZER.ExcludeFromTempElimination(QUADS.CurrQuadNo());

		QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR, QUADS.NextQuadNoPlus(2));
		QUADS.Emit(DeltaIC_ASSIGN, expr1, NIL_EXPR,	result);
		OPTIMIZER.ExcludeFromTempElimination(QUADS.CurrQuadNo());

		return result;
	}
	else
		return (DeltaExpr*) 0;
}

//------------------------------------------------------------------
