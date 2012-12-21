// ParseAssignments.cpp.
// Translation rules for assignments.
// ScriptFighter Project.
// A. Savidis, October 1999 (original 1st version).
// Re-factored last August 2006.
//

#include <string.h>
#include <limits.h>
#include <math.h>

#include "DDebug.h"
#include "ParseActions.h"
#include "CompileOptions.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaCompErrorMsg.h"
#include "Symbol.h"
#include "Optimizer.h"
#include "TypeCheck.h"
#include "DeltaCompErrorDefs.h"

//------------------------------------------------------------------
// ASSIGNMENTS.

// The constant is created in current scope, hence, we check for
// name conflict only in the current scope.
//
DeltaExpr* Translator::Translate_ConstAssignExpression (const std::string& id, DeltaExpr* expr) {

	NULL_EXPR_CHECK(expr);
	DeltaSymbol* sym = DELTASYMBOLS.Lookup(id, PARSEPARMS.CurrScope().value());

	if (!sym) {
		
		sym = DELTASYMBOLS.NewSymbol(id, false);	// No run-time storage for constants.

		// Here we make a hidden var, not conflicting with the vars,
		// that carries with a non-optimized assignment the content
		// of the constant expression. This way the debugger can find
		// it as far as it uses the special prefix when a name is not
		// resolved otherwise.

		if (!COMPOPTIONS.GetProductionMode()) {	// Except when we are in debug mode.
			DeltaSymbol* var	= DELTASYMBOLS.NewSymbol(std::string(DELTA_HIDDENCONSTVAR_NAME_PREFIX) + id, true);
			DeltaExpr*	 result	= EXPRFACTORY.New();
			result->sym = var;
			QUADS.Emit(DeltaIC_ASSIGN, expr, NIL_EXPR, result);
			OPTIMIZER.ExcludeFromTempElimination(QUADS.CurrQuadNo());	// To ensure optimization will not strip it off.
		}
	}
	else {
		DELTACOMP_ERROR_CONST_REDEFINITION(id, sym);
		return NIL_EXPR;
	}

	if (!DPTR(expr)->IsInvariantValue()) {
		DELTACOMP_ERROR_ILLEGAL_ASSIGN_TO_CONST(id);
		return NIL_EXPR;
	}

	DPTR(sym)->isUserDefinedConst = true;
	DPTR(sym)->constExpr = EXPRFACTORY.Copy(DPTR(expr));
	DPTR(sym)->SetInitialised();

	Unparse_ConstDef(sym, expr);
	return expr;
}

///////////////////////////////////////////////////////////////////
// Assignments are the only case where we do not explicitly adapt the
// table content, since it is now explicitly used as an l-value.
//
DeltaExpr* Translator::Translate_AssignExpr (DeltaExpr* lvalue, DeltaExpr* expr) {

	NULL_EXPR_PAIR_CHECK(lvalue, expr);

	if (!TYPECHECKER.Check_Assign(lvalue))
		return NIL_EXPR;

	if (DPTR(lvalue->sym)->IsImportedLibVar()) {
		DELTACOMP_ERROR_BYTECODE_LIBRARY_VAR_IMMUTABLE(
			"assign operation =", 
			lvalue->sym->GetName()
		); 
		return NIL_EXPR;
	}

	expr = DPTR(expr)->AdaptIfBool();
	DPTR(expr)->CheckUninitialised();

	DeltaExpr* result = EXPRFACTORY.New();
	if (lvalue->IsVarDeclaration())
		DPTR(result)->userData	= expr;	// Store r-value for future reference.
	DPTR(result)->sym = DELTASYMBOLS.NewTemp();

	if (DPTR(lvalue)->GetType() != DeltaExprTableConstruction && DPTR(lvalue)->GetType() != DeltaExprNewSelf) {
		
		DPTR(lvalue)->SetTypeTag(DPTR(expr)->GetTypeTag());
		DPTR(lvalue)->GetTypeInfo().Set(DPTR(expr)->GetTypeInfo());

		DPTR(result)->SetTypeTag(DPTR(expr)->GetTypeTag());
		DPTR(result)->GetTypeInfo().Set(DPTR(expr)->GetTypeInfo());
	}
	else {
		DPTR(result)->SetTypeTag(TagAnyValue);
		DPTR(result)->GetTypeInfo().Set(TagAnyValue);
	}

	if (DPTR(lvalue)->type == DeltaExprTableElement) {

		QUADS.Emit(
			QUADS.WhichSetOpcode(lvalue),
			expr,
			DPTR(lvalue)->index,
			lvalue
		);

		// Do we have an 'x.self = y' for an 'x' originally made-up from a table
		// item 't.i' ? If yes, we have to post 't.i = x' explicitly after setting owner.
		// This will ensure expression 't.x.self = y' will eventually result
		// in updating the t.x element.
		//
		if (	DPTR(lvalue)->index->GetType()	== DeltaExprString			&&
				DPTR(lvalue)->index->strConst	== DELTA_SELF_POINTER_ID	&&
				DPTR(lvalue)->tableItem		) {

				DeltaExpr* prevTableItem = DPTR(lvalue)->tableItem;

				QUADS.Emit(
					QUADS.WhichSetOpcode(prevTableItem),
					lvalue,
					DPTR(prevTableItem)->index,
					prevTableItem
				);
			}

		QUADS.Emit(	// Extract back from the object the just set element.
			QUADS.WhichGetOpcode(lvalue),
			lvalue,
			DPTR(lvalue)->index,
			result
		);
	}
	else {
		QUADS.Emit(DeltaIC_ASSIGN, expr, NIL_EXPR, lvalue);
		QUADS.Emit(DeltaIC_ASSIGN, expr, NIL_EXPR, result);
	}

	OPTIMIZER.ExcludeFromCopyPropagation(QUADS.CurrQuadNo());
	DASSERT(result->type == DeltaExprVar);
	return result;
}

///////////////////////////////////////////////////////////////////
// In this type of arithmetic expressions, we cannot compute the result
// at compile time without optimization techniques (i.e. constant propagation).
//
DeltaExpr* Translator::Translate_AssignArithExpr (DeltaExpr* lvalue, DeltaExpr* expr, DeltaICOpcode arithOp, const char* opStr) {

	NULL_EXPR_PAIR_CHECK(lvalue, expr);

	if (DPTR(lvalue->sym)->IsImportedLibVar()) {
		DELTACOMP_ERROR_BYTECODE_LIBRARY_VAR_IMMUTABLE(
			uconstructstr("arith assign operation %s", opStr),
			lvalue->sym->GetName()
		); 
		return NIL_EXPR; 
	}

	if (!TYPECHECKER.Check_Assign(lvalue)						||
		!TYPECHECKER.Check_InArithmetic(lvalue, arithOp, opStr)	||
		!TYPECHECKER.Check_InArithmetic(expr, arithOp, opStr))
		return NIL_EXPR;

	// Can never be actually a temporary variable, unless we 
	// apply the operator to a table element.
	//
	DASSERT(!DPTR(lvalue)->sym->IsTempVar() || DPTR(lvalue)->type == DeltaExprTableElement);

	DPTR(lvalue)->CheckUninitialised();
	DPTR(expr)->CheckUninitialised();

	// If l-value is a table element, make firstly the arithmetic
	// operation and then set the result as the content of the
	// table element. Else, set directly the lvalue as the result of
	// the arithmetic operation.
	//
	if (DPTR(lvalue)->type == DeltaExprTableElement) {

		DeltaExpr* result = EXPRFACTORY.New();
		DPTR(result)->sym = DELTASYMBOLS.NewTemp();

		// Used also as an r-value, so a temporary will be created
		// to gain the original table element, i.e. lvalue[index].
		//
		DeltaExpr* elem = DPTR(lvalue)->AdaptIfTableContent();

		// We apply the arithmetic operation.
		//
		QUADS.Emit(		// result = elem arithOp expr.
			arithOp,
			elem,
			expr,
			result
		);

		// And reflect the result on the table element.
		//
		QUADS.Emit(		// lvalue[index] = result.
			DPTR(lvalue)->IsBoundedElement() ? DeltaIC_BOUNDEDOBJSET : DeltaIC_OBJSET,
			result,
			DPTR(lvalue)->index,
			lvalue	
		);

		return result;
	}
	else {

		DeltaExpr* result = EXPRFACTORY.New();
		DPTR(result)->sym = lvalue->sym;

		QUADS.Emit(
			arithOp,
			lvalue,
			expr,
			lvalue
		);

		return result;
	}
}

//------------------------------------------------------------------
