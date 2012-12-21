// ParseTerms.cpp.
// Translation rules for terms.
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
// TERM.

DeltaExpr* Translator::Translate_TermLvalue (DeltaExpr* lvalue, bool postop, DeltaICOpcode op) {

	NULL_EXPR_CHECK(lvalue);

	if (!TYPECHECKER.Check_Assign(lvalue))
		return NIL_EXPR;

	if (!TYPECHECKER.Check_InArithmetic(lvalue, op, op == DeltaIC_ADD ? "++" : "--"))
		return (DeltaExpr*) 0;

	DPTR(lvalue)->CheckUninitialised();

	DeltaExpr* result;

	// If post-increment or post-decrement, a temporary expression
	// is needed to return the original result.
	//
	if (postop) {

		result = EXPRFACTORY.New();
		DPTR(result)->sym = DELTASYMBOLS.NewTemp();

		if (lvalue->type == DeltaExprTableElement) {

			DeltaExpr* value = lvalue->AdaptIfTableContent();

			// Result is original content.
			//
			QUADS.Emit(
				DeltaIC_ASSIGN,
				value,
				(DeltaExpr*) 0,
				result
			);

			// Content is modified and assigned back to table element.
			//
			QUADS.Emit(
				op,
				value,
				EXPRFACTORY.One(),
				value
			);

			QUADS.Emit(
				DPTR(lvalue)->IsBoundedElement() ? DeltaIC_BOUNDEDOBJSET : DeltaIC_OBJSET,
				value,
				DPTR(lvalue)->index,
				lvalue
			);
		}
		else {

			// Result assigned the original lvalue.
			//
			QUADS.Emit(
				DeltaIC_ASSIGN,
				lvalue,
				(DeltaExpr*) 0,
				result
			);

			// That is subsequently modified.
			//
			QUADS.Emit(
				op,
				lvalue,
				EXPRFACTORY.One(),
				lvalue
			);
		}
	}
	else {

		if (lvalue->type == DeltaExprTableElement) {

			// Will always produce a new expression.
			//
			result = lvalue->AdaptIfTableContent();

			QUADS.Emit(
				op,
				result,
				EXPRFACTORY.One(),
				result
			);

			QUADS.Emit(
				DPTR(lvalue)->IsBoundedElement() ? DeltaIC_BOUNDEDOBJSET : DeltaIC_OBJSET,
				result,
				DPTR(lvalue)->index,
				lvalue
			);
		}
		else {

			// Apply the op to the lvalue.
			//
			QUADS.Emit(
				op,
				lvalue,
				EXPRFACTORY.One(),
				lvalue
			);

			result = EXPRFACTORY.New();

			// In case lvalue is not a temp symbol, we should use a temp
			// to carry the result, as the lvalue may be used more than once,
			// like in "t=[i++, i++, i++];"
			//
			if (!DPTR(lvalue)->IsTemp()) {

				DPTR(result)->sym = DELTASYMBOLS.NewTemp();

				QUADS.Emit(
					DeltaIC_ASSIGN,
					lvalue,
					NIL_EXPR,
					result
				);
			}
			else
				DPTR(result)->sym = DPTR(lvalue)->sym;
		}
	}

	// The type is always an arithmetic expression.
	//
	DPTR(result)->type = DeltaExprArithmetic;
	return result;
}

///////////////////////////////////////////////////////////////////
// Multiply by -1 to make this expression.
//
DeltaExpr* Translator::Translate_UMINUSExpression (DeltaExpr* expr) {

	NULL_EXPR_CHECK(expr);

	if (!TYPECHECKER.Check_InArithmetic(expr, DeltaIC_MUL, "-(unary)"))
		return (DeltaExpr*) 0;

	DPTR(expr)->CheckUninitialised();
	DeltaExpr* result = EXPRFACTORY.New();

	if (expr->type == DeltaExprNumber) {	// If constant, result is constant.
		result->type	 = DeltaExprNumber;
		result->numConst = -DPTR(expr)->numConst;
	}
	else {
		DPTR(result)->sym = expr->IsTemp() ? DPTR(expr)->sym : DELTASYMBOLS.NewTemp();
		
		QUADS.Emit(
			DeltaIC_MUL,
			expr,
			EXPRFACTORY.One(true),
			result
		);

		DPTR(result)->type = DeltaExprArithmetic;
	}

	return result;
}

//------------------------------------------------------------------
