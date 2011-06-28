// DeltaClosureCreator.cpp
// Creator of function closures.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#include "DeltaClosureCreator.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "ContainerDelayedCollector.h"
#include "DeltaClosureHandling.h"

/////////////////////////////////////////////////////////////

DeltaClosureCreator::DeltaClosureCreator(void) 
	{}

/////////////////////////////////////////////////////////////

DeltaClosureCreator::~DeltaClosureCreator() {

	DASSERT(usedVars.empty());
	DASSERT(AreAllClosuresUsingMyCreatedVarsAsDynamicOnly());

	DELTA_COLLECTOR_ENTER();
	for (ClosureList::iterator i = closures.begin(); i != closures.end(); ++i)
		(*i)->DecRefCounter((DeltaValue*) 0);
	DELTA_COLLECTOR_EXIT();
}

/////////////////////////////////////////////////////////////
// When the creator produces closures and makes new var proxies too, then those
// proxies may only refer to stack vars of the current function. Thus, when the function
// is popped and thus the creator is popped, such vars must have been popped too, meaning
// they may only appear within the creator's closures as dynamic.

bool DeltaClosureCreator::AreAllClosuresUsingMyCreatedVarsAsDynamicOnly (void) const {

	for (ClosureList::const_iterator i = closures.begin(); i != closures.end(); ++i) {
		
		typedef DeltaClosure::ClosureVars ClosureVars;
		DASSERT(DPTR(*i)->IsReadyPrecondition());

		const ClosureVars& vars = DPTR(*i)->GetVars();
		for (util_ui32 j = 0; j < (util_ui32) vars.size(); ++j) {
			DeltaClosureVarProxy* var = vars[j];
			if (DPTR(var)->GetCreator() == this && !DPTR(var)->IsDynamic())
				return false;
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////

DeltaClosureVarProxy* DeltaClosureCreator::Get (DeltaValue* val) {
	VarMap::iterator i = usedVars.find(val);
	return i != usedVars.end() ? i->second : (DeltaClosureVarProxy*) 0;
}

/////////////////////////////////////////////////////////////
// When a proxy var is created it records its creator.

DeltaClosureVarProxy* DeltaClosureCreator::New (DeltaValue* val) 
	{ return usedVars[val] = DNEWCLASS(DeltaClosureVarProxy, (val, this)); }

/////////////////////////////////////////////////////////////
// Any shared closure var referring to a popped variable will
// be turned to a dynamic one and will be removed from registered
// stack variables.

void DeltaClosureCreator::HandlePopStackVars (
		DeltaVirtualMachine*	vm,
		DeltaValue*				sp, 
		util_ui32				n
	) {
	if (!usedVars.empty())
		while (n--) {
			DASSERT(DPTR(vm)->ValidateStackValuePtr(sp));
			VarMap::iterator i = usedVars.find(sp++);
			if (i != usedVars.end()) {
				DPTR(i->second)->SubstitudeWithDynamicVar(DNEWCLASS(DeltaValue, (i->first)));
				usedVars.erase(i);
			}
		}
}

/////////////////////////////////////////////////////////////

void DeltaClosureCreator::HandlePopAllStackVars (DeltaVirtualMachine* vm) {
	if (!usedVars.empty()) {
		for (VarMap::iterator i = usedVars.begin(); i != usedVars.end(); ++i)
			if (!i->second->IsDynamic()) {
				DASSERT(DPTR(vm)->ValidateStackValuePtr(i->first));
				DPTR(i->second)->SubstitudeWithDynamicVar(DNEWCLASS(DeltaValue, (i->first)));
			}
		usedVars.clear();
	}
}

/////////////////////////////////////////////////////////////

DeltaClosure* DeltaClosureCreator::NewClosure (
		const DeltaStdFuncInfo::ClosureVarsInfo&	info,
		DeltaVirtualMachine*						vm
	) {

	typedef DeltaStdFuncInfo::ClosureVarsInfo ClosureVarsInfo;
	DeltaClosure* closure = DNEWCLASS(DeltaClosure, ( (util_ui32) info.size()));
	DPTR(closure)->IncRefCounter((DeltaValue*) 0);

	bool usesOuterClosure = false;
	for (ClosureVarsInfo::const_iterator i = info.begin(); i != info.end(); ++i) {
	
		DeltaValue*				val = (DeltaValue*) 0;
		DeltaClosureVarProxy*	var = (DeltaClosureVarProxy*) 0;

		switch (i->GetType()) {

			case DeltaClosureVarInfo::VarTypeLocal	 : val = DPTR(vm)->GetLocal(i->GetOffset());	break;
			case DeltaClosureVarInfo::VarTypeGlobal	 : val = DPTR(vm)->GetGlobal(i->GetOffset());	break;
			case DeltaClosureVarInfo::VarTypeFormal	 : {
				if (!(val = DPTR(vm)->GetFormal(i->GetOffset()))) {	// No value for formal was supplied!
					DASSERT(DPTR(vm)->HasProducedError());
					DPTR(closure)->DisableExpansion();
					DPTR(closure)->DecRefCounter((DeltaValue*) 0);	// Collect instantly.
					return (DeltaClosure*) 0;						// Failed to make a closure.
				}
				break;
			}

			case DeltaClosureVarInfo::VarTypeClosure : {
				usesOuterClosure = true;
				var = DPTR(CLOSURE_HANDLERS->GetTopClosure())->GetProxyVar(i->GetOffset());
				break;
			}

			default : DASSERT(false);
		}

		if (!var && !(var = Get(DNULLCHECK(val))))	// Not an existing closure var.
			var = New(val);							// Make a new one.

		DPTR(closure)->AddVar(var);
	}
	
	DASSERT(DPTR(closure)->IsReadyPrecondition());
	if (usesOuterClosure)
		DPTR(closure)->SetUses(DNULLCHECK(CLOSURE_HANDLERS->GetTopClosurePrivileged()));

	closures.push_back(closure);
	return closure;
}

/////////////////////////////////////////////////////////////
