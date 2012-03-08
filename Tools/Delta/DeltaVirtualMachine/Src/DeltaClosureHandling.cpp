// DeltaClosureHandling.cpp
// Global handling of closures for the VM.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#include "DeltaClosureHandling.h"
#include "DeltaVirtualMachine.h"
#include "Execute.h"
#include "ContainerDelayedCollector.h"

DeltaClosureHandling* DeltaClosureHandling::singletonPtr = (DeltaClosureHandling*) 0;

USINGLETON_APISTYLE_IMPL_PUBLICSTDMETHODS(DeltaClosureHandling)
USINGLETON_APISTYLE_IMPL_GETTER(DeltaClosureHandling)

/////////////////////////////////////////////////////////////

DeltaClosureHandling::DeltaClosureHandling() {}
DeltaClosureHandling::~DeltaClosureHandling(){}

/////////////////////////////////////////////////////////////

void DeltaClosureHandling::PushFunc (
		DeltaVirtualMachine*	vm, 
		DeltaClosure*			closure
	) {

	DASSERT(!closure || DPTR(closure)->IsReadyPrecondition());
	DASSERT(!closure == (!DPTR(vm)->GetCurrFuncInfo() || !DPTR(vm)->GetCurrFuncInfo()->HasClosureVars()));
	DASSERT(!closure || DPTR(closure)->GetTotal() == DPTR(vm)->GetCurrFuncInfo()->GetClosureVarsInfo().size());
	
	closureStack.push_front(ClosureEntry(vm, closure));
	creatorStack.push_front(DeltaClosureCreator());
}

/////////////////////////////////////////////////////////////

void DeltaClosureHandling::PopFunc (DeltaVirtualMachine* vm) {

	DASSERT(!closureStack.empty() && !creatorStack.empty());

	closureStack.pop_front();
	creatorStack.front().HandlePopAllStackVars(vm);
	
	DELTA_COLLECTOR_ENTER();
	creatorStack.pop_front();
	DELTA_COLLECTOR_EXIT();
}

/////////////////////////////////////////////////////////////

void DeltaClosureHandling::PopFunc (
		DeltaVirtualMachine*	vm, 
		DeltaValue*				sp,
		util_ui32				n
	) {

	DASSERT(!closureStack.empty() && !creatorStack.empty());

	closureStack.pop_front();
	creatorStack.front().HandlePopStackVars(vm, sp, n);

	DELTA_COLLECTOR_ENTER();
	creatorStack.pop_front();
	DELTA_COLLECTOR_EXIT();
}

/////////////////////////////////////////////////////////////

void DeltaClosureHandling::PopBlock (
		DeltaVirtualMachine*	vm, 
		DeltaValue*				block, 
		util_ui32				n
	) {
	DASSERT(!closureStack.empty() && !creatorStack.empty());
	creatorStack.front().HandlePopStackVars(vm, block, n);
}

/////////////////////////////////////////////////////////////

const DeltaClosure* DeltaClosureHandling::GetTopClosure (void) const 
	{ return GetTopClosurePrivileged(); }

DeltaClosure* DeltaClosureHandling::GetTopClosurePrivileged (void) const
	{ DASSERT(!closureStack.empty()); return closureStack.front().second; }

/////////////////////////////////////////////////////////////

const DeltaClosure*	DeltaClosureHandling::GetClosure (util_ui32 stackIndex) const {
	DASSERT(stackIndex < (util_ui32) closureStack.size());
	for (ClosureStack::const_reverse_iterator i = closureStack.rbegin(); i != closureStack.rend(); ++i, --stackIndex)
		if (!stackIndex)
			return i->second;
	DASSERT(false);
	return (DeltaClosure*) 0;
}

/////////////////////////////////////////////////////////////

DeltaClosure* DeltaClosureHandling::MakeClosureIfNeeded (
		DeltaVirtualMachine*	vm,
		const DeltaStdFuncInfo*	func
	) {
	DASSERT(!closureStack.empty() && !creatorStack.empty());
	DASSERT(DPTR(vm)->IsValidFuncInfo(func));

	if (func->HasClosureVars())
		return GetCreator().NewClosure(func->GetClosureVarsInfo(), vm);	// May return null in case of error.
	else
		return DELTA_NIL_CLOSURE;
}

/////////////////////////////////////////////////////////////
// Called when the vm causes a primary error (is primary failing).

void DeltaClosureHandling::PopCallsAffectedByError (DeltaVirtualMachine* vm){

	DELTA_DETERMINE_CALLSTACK_ENTRIES_TO_POP_ON_ERROR(
		DeltaVirtualMachine*,
		i->first,
		ClosureStack::iterator i = closureStack.begin(),
		UEMPTY,
		&*i != &closureStack.back(),	// Not last (bottom of stack)
		++i,
		vm
	);

	while (callsToPop--) {
		DASSERT(!closureStack.empty() && !creatorStack.empty());
		PopFunc(closureStack.front().first);
	}
}

/////////////////////////////////////////////////////////////
// Called when the vm gets destroyed.

void DeltaClosureHandling::RemoveAllCalls (DeltaVirtualMachine* vm) {

	ClosureStack::iterator	i_closure	= closureStack.begin();
	CreatorStack::iterator	i_creator	= creatorStack.begin();	
	
	while (i_closure != closureStack.end()) {
		DASSERT(i_creator != creatorStack.end());
		if (i_closure->first == vm) {
			closureStack.erase(i_closure++);
			creatorStack.erase(i_creator++);
		}
		else
			++i_closure, ++i_creator;
	}
}

/////////////////////////////////////////////////////////////

void DeltaVirtualMachine::SetUserFuncClosure (DeltaClosure* closure, const DeltaStdFuncInfo* funcInfo) {

	if (!closure == !funcInfo->HasClosureVars()) {	// Closure provision matches function requirements.
		if (closure && DPTR(closure)->GetTotal() != (util_ui32) funcInfo->GetClosureVarsInfo().size()) {
			if (DPTR(closure)->GetTotal() < (util_ui32) funcInfo->GetClosureVarsInfo().size()) {	// Less than needed.
				SetErrorCode(DELTA_INVALID_SUPPLIED_CLOSURE_ERROR)->PrimaryError(
					"in calling '%s' supplied closure has only %u vars (while %u vars are required)", 
					funcInfo->GetName().c_str(),
					DPTR(closure)->GetTotal(),
					funcInfo->GetClosureVarsInfo().size()
				);
				unullify(closure);
			}
			else	// More than needed.
				Warning(
					"in calling '%s' a closure is supplied with %u vars (only %u vars are required)", 
					funcInfo->GetName().c_str(),
					DPTR(closure)->GetTotal(),
					funcInfo->GetClosureVarsInfo().size()
				);
		}
	}
	else	// Closure provision mismatch regarding function requirements.
	if (!closure)	// No closure when needed.
		SetErrorCode(DELTA_NO_SUPPLIED_CLOSURE_ERROR)->PrimaryError(
			"in calling '%s' no closure supplied (requires one with %u vars)", 
			funcInfo->GetName().c_str(),
			funcInfo->GetClosureVarsInfo().size()
		);
	else	// Closure when not needed.
		Warning(
			"in calling '%s' a closure is supplied (with %u vars) but none is required)", 
			funcInfo->GetName().c_str(),
			DPTR(closure)->GetTotal()
		);
	userFuncClosure = closure;
}

/////////////////////////////////////////////////////////////
