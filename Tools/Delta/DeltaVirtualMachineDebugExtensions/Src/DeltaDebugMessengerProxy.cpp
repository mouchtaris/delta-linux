// DeltaDebugMessengerProxy.cpp
// Proxy to the module posting messages to the debugger User Interface.
// Debug support extensions.
// Script Fighter Project.
// A. Savidis, June 2006 (original).
// RF to separate class (July 2008).
//

#include "DeltaDebugMessengerProxy.h"
#include "DDebug.h"
#include "uptr.h"

////////////////////////////////////////////////////////////////////////////

DeltaDebugMessengerProxy::DeltaDebugMessengerProxy (void) {
	isConnected = false;
	unullify(onInvalidBreakPoint);
	unullify(onValidBreakPoint);
	unullify(onInvalidCond);
	unullify(onMostRecentFuncResults);
	unullify(onStopPoint);
	unullify(onSendCurrFunction);
}

DeltaDebugMessengerProxy::~DeltaDebugMessengerProxy () {}

/////////////////////////////////////////////////////////////////////////////

void DeltaDebugMessengerProxy::NotifyInvalidBreakPoint(
		const char*	source, 
		util_ui32	line, 
		util_ui32	newLine,
		const char*	cond
	) const {	
		LOCK_BLOCK(mutex); 
		if (onInvalidBreakPoint)
			(*onInvalidBreakPoint)(
				source, 
				line, 
				newLine, 
				cond
			); 
	}

/////////////////////////////////////////////////////////////////////////////

void DeltaDebugMessengerProxy::NotifyValidBreakPoint(
		const char*	source, 
		util_ui32	line, 
		const char*	cond
	) const {	
		LOCK_BLOCK(mutex); 
		if (onValidBreakPoint)
			(*onValidBreakPoint)(
				source, 
				line, 
				cond
			); 
	}

/////////////////////////////////////////////////////////////////////////////

void DeltaDebugMessengerProxy::NotifyInvalidCondition(
		const char*	source, 
		util_ui32	line, 
		util_ui32	newLine,
		const char*	cond
	) const {	
		LOCK_BLOCK(mutex); 
		if (onInvalidCond)
			(*onInvalidCond)(
				source, 
				line, 
				newLine, 
				cond
			); 
	}

/////////////////////////////////////////////////////////////////////////////

void DeltaDebugMessengerProxy::NotifyMostRecentFuncResults (
		const std::list< std::pair<std::string, std::string> >& results
	) const {	
		LOCK_BLOCK(mutex); 
		if (onMostRecentFuncResults)
			(*onMostRecentFuncResults)(results); 
	}

/////////////////////////////////////////////////////////////////////////////

void DeltaDebugMessengerProxy::NotifyStopPoint (
		const char*		source, 
		util_ui32		line, 
		bool			isGlobal,
		const char*		cond
	) const {	
		LOCK_BLOCK(mutex); 
		if (onStopPoint)
			(*onStopPoint)(
				source, 
				line, 
				isGlobal, 
				cond
			); 
	}

/////////////////////////////////////////////////////////////////////////////

void DeltaDebugMessengerProxy::NotifySendCurrFunction(
		const char*		func,	
		util_ui32		defLine,
		util_ui32		callLine,
		util_ui32		scope,
		const char*		call
	) const {
		LOCK_BLOCK(mutex); 
		if (onSendCurrFunction)
			(*onSendCurrFunction)(
				func, 
				defLine, 
				callLine, 
				scope, 
				call
			);
	}

/////////////////////////////////////////////////////////////////////////////

void DeltaDebugMessengerProxy::NotifyDynamicCode (const char* code) const {
	LOCK_BLOCK(mutex); 
	if (onSendDynamicCode)
		(*onSendDynamicCode)(code);
}

/////////////////////////////////////////////////////////////////////////////
