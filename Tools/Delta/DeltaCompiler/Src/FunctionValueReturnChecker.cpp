// FunctionValueReturnChecker.cpp 
// Checking if all control paths of a function return a value.
// ScriptFighter Project.
// A. Savidis, May 2009.
//

#include "DDebug.h"
#include "FunctionValueReturnChecker.h"
#include "DeltaCompErrorMsg.h"
#include "Symbol.h"
#include "ParseParms.h"
#include "ParseActions.h"
#include "Expr.h"
#include "DeltaCompErrorDefs.h"

USINGLETON_INSTANCE(usingleton<FunctionValueReturnCheckerClass>)

/////////////////////////////////////////////////////////////

bool FunctionValueReturnCheckerClass::Check (DeltaCodeAddress i) {
	
	while (true) {

		DASSERT(i < QUADS.Total() && QUADS.GetQuad(i).opcode != DeltaIC_FUNCENTER);

		if (QUADS.GetQuad(i).opcode == DeltaIC_FUNCRET) {
			ClearMarked();
			bool r = ReturnExprWasFound(); 
			ResetReturnExprFound(); 
			return results[i] = r;
		}
		else
		if (IsResult(i))									// Already met and we knw the result
			return results[i];
		else
		if (IsMarked(i))									// Already met without a result is an loop
			return true;									// We assume it is ok
		else {

			Mark(i);
			DeltaQuad& quad = QUADS.GetQuad(i);
		
			if (quad.opcode == DeltaIC_RETURNVAL)			// Does the quad return an expression?
				SetReturnExprFound();
			else
			if (DeltaIsBranchOpcode(quad.opcode))
				if (quad.opcode == DeltaIC_JUMP)
					return results[i] = Check(quad.label);	// Unique path? must succeed
				else {										// Try both paths taking the jump or not
					MarkedList l(marked);					// Store the marked list at this point before testing first path
					bool r = Check(quad.label);
					if (marked.empty())						// If it was cleared it means we reached function end
						marked = l;							// But we have to restore the list just before trying a second path

					return results[i] = r && Check(i + 1);
				}
			else
				++i;
		}
	}
}
	
/////////////////////////////////////////////////////////////

void FunctionValueReturnCheckerClass::CheckFunction (DeltaCodeAddress startQuad) {
	
	DASSERT(QUADS.GetQuad(startQuad).opcode == DeltaIC_FUNCENTER);

	ResetReturnExprFound();
	DeltaSymbol* func = QUADS.GetQuad(startQuad).arg1->sym;

	if (!Check(startQuad + 1))
		DELTACOMP_WARNING_NOT_ALL_EXIT_PATHS_RETURN_A_VALUE(func);

	ClearMarked();
	ClearResults();
}

/////////////////////////////////////////////////////////////
