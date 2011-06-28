// AS OF DECEMBER 5, 2006, THIS FILE IS NOT LONGER USED.
// ExprCleaner.cpp
// A way to release memory centrally for various items
// dynamically allocated in DeltaCompiler.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Re-factored December 2006, as I do not know how
// I thought of the old implementation...
//

#include "DDebug.h"
#include "ExprCleaner.h"
#include "Expr.h"

#define	BLOCK_SIZE	1024

DeltaExprCleaner* DeltaExprCleaner::singletonPtr = (DeltaExprCleaner*) 0;

void DeltaExprCleaner::SingletonCreate (void) {
	if (!singletonPtr)
		singletonPtr = DNEW(DeltaExprCleaner);
}

void DeltaExprCleaner::SingletonDestroy (void) {
	if (singletonPtr) {
		DDELETE(singletonPtr);
		singletonPtr = (DeltaExprCleaner*) 0;
	}
}

void DeltaExprCleaner::Register (DeltaExpr* expr) 
	{ exprs.push_back(expr); }

void DeltaExprCleaner::Cancel (DeltaExpr* expr) {
	if (!destroying) 
		exprs.remove(expr); 
}

/////////////////////////////////////
// Deletes all expressions.
//
void DeltaExprCleaner::CleanUp (void) {
	destroying = true;
	std::for_each(
		exprs.begin(),
		exprs.end(),
		uptrdestructorfunctor<DeltaExpr*>()
	);
	exprs.clear();
	destroying = false;
}

void DeltaExprCleaner::ValidateAll (void) {
	std::for_each(
		exprs.begin(),
		exprs.end(),
		DeltaExpr::ValidateFunctor()
	);
}

/////////////////////////////////////
