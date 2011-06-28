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

DeltaQuadAddress Translate_TRY (void) {
	QUADS.Emit(DeltaIC_TRAPENABLE, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	return QUADS.CurrQuadNo();
}

///////////////////////////////////////////////////

DeltaQuadAddress Translate_TRAP (void) {
	QUADS.Emit(DeltaIC_TRAPDISABLE, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	return QUADS.CurrQuadNo();
}

///////////////////////////////////////////////////

DeltaQuadAddress Translate_TrapJumpOver (void) {
	QUADS.Emit(DeltaIC_JUMP, NIL_EXPR, NIL_EXPR, NIL_EXPR);
	return QUADS.CurrQuadNo();
}

///////////////////////////////////////////////////

void Translate_TrapStart (
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
			DeltaExpr::Make(
				(DeltaNumberValueType) LocalDataHandler::GetCurrBlockId()
			), 
			NIL_EXPR, 
			exceptionVar
		);
		QUADS.Patch(enable, QUADS.CurrQuadNo());
		QUADS.Patch(disable, QUADS.CurrQuadNo());
	}
}

///////////////////////////////////////////////////

void Translate_TrapEnd (util_ui32 jumpOver) {
	QUADS.Patch(jumpOver, QUADS.NextQuadNo());
}

///////////////////////////////////////////////////

Stmt* Translate_THROW (DeltaExpr* expr) {
	if (expr)
		QUADS.Emit(DeltaIC_THROW, NIL_EXPR, NIL_EXPR, DPTR(expr)->AdaptIfBool());
	return NIL_STMT;
}

//------------------------------------------------------------------
