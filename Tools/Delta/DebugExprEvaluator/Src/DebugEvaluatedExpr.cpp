// DebugEvaluatedExpr.cpp
// Parse actions for the debug expression evaluator.
// ScriptFighter Project.
// A. Savidis, February 2002.
//

#include "DebugEvaluatedExpr.h"
#include <algorithm>

std::list<DebugEvaluatedExpr*>*	DebugEvaluatedExpr::allEvaluatedExprs = (std::list<DebugEvaluatedExpr*>*) 0;

/////////////////////////////////////////////////////////////

void DebugEvaluatedExpr::SingletonCreate(void) 
	{ unew(allEvaluatedExprs); }

/////////////////////////////////////////////////////////////

void DebugEvaluatedExpr::SingletonDestroy(void) 
	{ udelete(allEvaluatedExprs); }

/////////////////////////////////////////////////////////////

void DebugEvaluatedExpr::CleanUp (void) {

	DASSERT(allEvaluatedExprs);

	std::for_each(
		DPTR(allEvaluatedExprs)->begin(),
		DPTR(allEvaluatedExprs)->end(),
		uptrdestructorfunctor<DebugEvaluatedExpr*>()
	);
	DPTR(allEvaluatedExprs)->clear();
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr*	DebugEvaluatedExpr::New (DeltaValue* r, DeltaValue* t, DeltaValue* i) {
	DASSERT(allEvaluatedExprs);
	DebugEvaluatedExpr* p = DNEWCLASS(DebugEvaluatedExpr, (r, t, i));
	DPTR(allEvaluatedExprs)->push_back(p);
	return p;
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr*	DebugEvaluatedExpr::New (DeltaValue* r, bool lv) {
	DASSERT(allEvaluatedExprs);
	DebugEvaluatedExpr* p = DNEWCLASS(DebugEvaluatedExpr, (r, lv));
	DPTR(allEvaluatedExprs)->push_back(p);
	return p;
}

/////////////////////////////////////////////////////////////
