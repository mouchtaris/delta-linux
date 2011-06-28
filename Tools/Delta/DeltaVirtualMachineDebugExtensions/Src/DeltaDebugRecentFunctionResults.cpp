// DeltaDebugRecentFunctionResults.cpp
// Handling of recent function results during tracing.
// Debug support extensions.
// Script Fighter Project.
// A. Savidis, June 2007 (original) 
// RF to separate class (July 2008).
//

#include "DeltaDebugRecentFunctionResults.h"

/////////////////////////////////////////////////////////////////

DeltaDebugRecentFunctionResults::DeltaDebugRecentFunctionResults (void):
	funcsAlreadyAtStopPoint(0){}

DeltaDebugRecentFunctionResults::~DeltaDebugRecentFunctionResults(){}

void DeltaDebugRecentFunctionResults::Clear (void) {
	recentFuncResults.clear();
	while (funcNameStack.size())
		funcNameStack.pop();
}

/////////////////////////////////////////////////////////////////

void DeltaDebugRecentFunctionResults::EnteringNewStopPoint (void) {

	// We are tracing to an inner function from the original stop point?
	// If we have more funcs on the stack than the recorded #funcs at
	// the stop point then this is the case (we also clear recent results).
	//
	if (funcsAlreadyAtStopPoint < (util_ui16) funcNameStack.size()) 
		recentFuncResults.clear();
	funcsAlreadyAtStopPoint = (util_ui16) funcNameStack.size();
}

/////////////////////////////////////////////////////////////////

void DeltaDebugRecentFunctionResults::EnteringRecentlyCalledFunction (const std::string& name, DeltaVirtualMachine* vm) 
		{ funcNameStack.push(name); }

void DeltaDebugRecentFunctionResults::ExitingRecentlyCalledFunction (const std::string& name, DeltaVirtualMachine* vm) {

	bool isArtificialExit = name.empty() && !vm;	// These values are legal in case of error unwinding.
	DASSERT((name.size() && vm) || isArtificialExit);

	// If the name stack is empty, we clearly get out of the 
	// initial stop context.

	bool traceOutOfStopContext = funcNameStack.empty(); 	
	if (!traceOutOfStopContext) { // Not out of the stop context? normal pop.
		DASSERT((name.empty() && !vm) || funcNameStack.top() == name);	// What the VM says should match with what we keep.
		funcNameStack.pop();
	}
	else
	if (funcsAlreadyAtStopPoint)
		--funcsAlreadyAtStopPoint;

	if (isArtificialExit)
		{ recentFuncResults.clear(); return; }

	// When func name stack has less items than the #funcs at the initial stopped,
	// point it means we are also tracing out of the initial stop context.
	//
	if (traceOutOfStopContext || funcNameStack.size() < funcsAlreadyAtStopPoint) {
		recentFuncResults.clear();
		recentFuncResults.push_back(
			std::pair<std::string, std::string>	(
				name, 
				DPTR(vm)->GetReturnValue().ConvertToStringBySkippingUserCode()
			)
		);
	}
	else // When equal, we are exactly at the stop context, so we record the result.
	if (funcNameStack.size() == funcsAlreadyAtStopPoint)
		recentFuncResults.push_back(
			std::pair<std::string, std::string>(
				name,
				DPTR(vm)->GetReturnValue().ConvertToStringBySkippingUserCode()
			)
		);
}

/////////////////////////////////////////////////////////////////