// ExceptionHandlingActions.cpp
// Parse actions for exceptions.
// ScriptFighter Project.
// Delta Compiler.
// A. Savidis, July 2006.
//

#include "DDebug.h"
#include "ParseActions.h"
#include "InterCode.h"
#include "LocalDataHandling.h"
#include "DeltaCompErrorDefs.h"

//------------------------------------------------------------------
// Both TRAPENABLE and TRAPDISABLE instructions are later patched
// to hold the address of the actual TRAP block.

DeltaQuadAddress Translator::Translate_TRY (void) {
	QUADS.Emit(DeltaIC_TRAPENABLE, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	return QUADS.CurrQuadNo();
}

///////////////////////////////////////////////////
// TRAPDISABLE is the invoked as a fallback to the TRY stmt
// when no excpetion is thrown.

DeltaQuadAddress Translator::Translate_TRAP (void) {
	QUADS.Emit(DeltaIC_TRAPDISABLE, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	return QUADS.CurrQuadNo();
}

///////////////////////////////////////////////////

DeltaQuadAddress Translator::Translate_TrapJumpOver (void) {
	QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	return QUADS.CurrQuadNo();
}

///////////////////////////////////////////////////

void Translator::Translate_TrapStart (
		util_ui32	enable, 
		util_ui32	disable,
		DeltaExpr*	exceptionVar	
	) {
	std::string constName;
	if (DPTR(exceptionVar)->IsInvariantValue())
		constName = DPTR((DeltaSymbol*) DNULLCHECK(DPTR(exceptionVar)->userData))->GetName();
	else {
		DeltaSymbol* sym = DNULLCHECK(exceptionVar->sym);
		if (DPTR(sym)->IsLibraryConst() || DPTR(sym)->IsImportedLibVar())
			constName = sym->GetName();
	}

	if (!constName.empty())
		DELTACOMP_ERROR_LVALUE_NEEDED_IN_EXCEPTION_VAR(constName);
	else {
		QUADS.Emit(
			DeltaIC_TRAP,
			EXPRFACTORY.Make(
				(DeltaNumberValueType) LOCALDATA.GetCurrBlockId()
			), 
			NIL_EXPR, 
			exceptionVar
		);

		// The TRAPENABLE / TRAPDISABLE are set to target to their first TRAP block.

		if (QUADS.GetQuad(enable).GetLabel() == DELTA_NIL_QUAD_LABEL) {
			DASSERT(QUADS.GetQuad(disable).GetLabel() == DELTA_NIL_QUAD_LABEL);
			QUADS.Patch(enable, QUADS.CurrQuadNo());
			QUADS.Patch(disable, QUADS.CurrQuadNo());
		}
		else
			DASSERT(
				QUADS.GetQuad(enable).GetLabel() != DELTA_NIL_QUAD_LABEL &&
				QUADS.GetQuad(disable).GetLabel() != DELTA_NIL_QUAD_LABEL
			);
	}
}

///////////////////////////////////////////////////

void Translator::Translate_TrapEnd (util_ui32 jumpOver) {
	// This patch no longer concerns a single block but should concern all blocks.
	// This patch no longer concerns a single block but should concern all blocks.
	QUADS.Patch(jumpOver, QUADS.NextQuadNo());
}

///////////////////////////////////////////////////

Stmt* Translator::Translate_THROW (DeltaExpr* expr) {
	if (expr)
		QUADS.Emit(DeltaIC_THROW, NIL_EXPR, NIL_EXPR, DPTR(expr)->AdaptIfBool());
	return NIL_STMT;
}

//------------------------------------------------------------------
