// DebugMonitors.cpp
// Support for user-defined monitors of objects and extern ids.
// ScriptFighter Project.
// A. Savidis, July 2008.
//

#include "DebugExprParseActions.h"
#include "DDebug.h"
#include "ufunctors.h"
#include <string>
#include <algorithm>

DebugExprParseActions::MonitorMap*	DebugExprParseActions::monitors = (MonitorMap*) 0;

///////////////////////////////////////////////////////////////////

void DebugExprParseActions::InitialiseMonitors (void) {
	DASSERT(!monitors);
	monitors = DNEW(MonitorMap);
}

///////////////////////////////////////////////////////////////////

void DebugExprParseActions::CleanUpMonitors (void) {

	std::for_each(
		DPTR(monitors)->begin(),
		DPTR(monitors)->end(),
		udestroy_second< std::pair<std::string, DeltaValue*> >()
	);
	udelete(monitors);
}

///////////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::NewMonitor (const std::string& s, DebugEvaluatedExpr* expr) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();

	if (DPTR(monitors)->find(s) !=  DPTR(monitors)->end()) {
		Error("Monitor '%s' already exists!", s.c_str());
		return (DebugEvaluatedExpr*) 0;
	}
	else {
		
		DeltaValue* rvalue	= DPTR(expr)->GetResult();
		if (rvalue->Type() != DeltaValue_Table && rvalue->Type() != DeltaValue_ExternId) {
			Error(
				"Monitor '%s' of illegal type '%s' (%s or %s expected)!", 
				s.c_str(), 
				rvalue->TypeStr(),
				DeltaValue::GetTypeStr(DeltaValue_Table),
				DeltaValue::GetTypeStr(DeltaValue_ExternId)
			);
			return (DebugEvaluatedExpr*) 0;
		}
		else {
			DeltaValue* monitor = DNEW(DeltaValue);
			DPTR(monitor)->Assign(rvalue);
			(*DPTR(monitors))[s] = monitor;
			return expr;
		}
	}
}

///////////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::DeleteMonitor (const std::string& s) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	
	MonitorMap::iterator i = DPTR(monitors)->find(s);
	if (i == DPTR(monitors)->end()) {
		Error("Monitor '%s' not found !", s.c_str());
		return (DebugEvaluatedExpr*) 0;
	}
	else {
		DeltaValue* result = NewTempValue();
		DPTR(result)->Assign(i->second);

		DDELETE(i->second);
		DPTR(monitors)->erase(i);

		return DebugEvaluatedExpr::New(result, false);
	}
}

///////////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::GetMonitor (const std::string& s) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	
	MonitorMap::iterator i = DPTR(monitors)->find(s);
	if (i != DPTR(monitors)->end()) 
		return DebugEvaluatedExpr::New(i->second, false);
	else {
		Error("Monitor '%s' not found !", s.c_str());
		return (DebugEvaluatedExpr*) 0;
	}
}

///////////////////////////////////////////////////////////////////
