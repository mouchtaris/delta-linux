// DeltaClosure.cpp
// Function closure.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#include "DeltaClosure.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"

///////////////////////////////////////////////////////

DeltaClosure::DeltaClosure (util_ui32 _total) :	
	total	(_total),
		used	(DELTA_NIL_CLOSURE) {
	vars.reserve(_total);
	pseudoExternId.FromExternId(DELTA_CLOSURE_PSEUDO_EXTERNID_VALUE);
	pseudoExternId.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) this);
}

DeltaClosure::~DeltaClosure() 
	{ ExplicitDestructor(); }

///////////////////////////////////////////////////////

void DeltaClosure::ExplicitDestructor (void) {

	DASSERT(IsReadyPrecondition() || vars.empty());

	if (!vars.empty()) {
		for (util_ui32 i = 0; i < total; ++i)
			DPTR(vars[i])->DecRefCounter(&pseudoExternId);
		vars.clear();
	}
	pseudoExternId.Undefine();
}

///////////////////////////////////////////////////////

void DeltaClosure::IncRefCounter (DeltaValue* from) {
	DASSERT(from || !used);
	CollectableContainer::IncRefCounter(from);
	if (used)	// if 'from' is null it is still under creation and 'used' is null
		DPTR(used)->IncRefCounter(from);
}

void DeltaClosure::DecRefCounter (DeltaValue* from) {
	if (used && from)	// 'from' is null when the creator cancel its reference.
		DPTR(used)->DecRefCounter(from);
	CollectableContainer::DecRefCounter(from);
}

void DeltaClosure::ConvertToString (DeltaString* at, void* closure) 
	{ at->Add(uconstructstr("%s(%u vars)", GetExtClassString(), GetTotal())); }

void DeltaClosure::ConvertToStringBySkippingUserCode (DeltaString* at, void* closure) 
	{ ConvertToString(at, closure); }

void DeltaClosure::ToValue (DeltaValue* at) 
	{ at->Undefine(); }

///////////////////////////////////////////////////////

bool DeltaClosure::IsReadyPrecondition (void) const
	{ return total && total == (util_ui32) vars.size() && total == (util_ui32) varPositions.size(); }

///////////////////////////////////////////////////////

void DeltaClosure::DisableExpansion (void) {
	DASSERT(total > vars.size());
	total = (util_ui32) vars.size();
}

///////////////////////////////////////////////////////

void DeltaClosure::AddVar (DeltaClosureVarProxy* var) {
	DASSERT(total && total > vars.size() && vars.size() == varPositions.size());
	varPositions[var] = (util_ui32) vars.size();
	vars.push_back(var);
	var->IncRefCounter(&pseudoExternId);
}

///////////////////////////////////////////////////////

DeltaValue* DeltaClosure::GetVar (util_ui32 offset) const {
	DASSERT(IsReadyPrecondition());
	DASSERT(offset < total);
	return DPTR(vars[offset])->GetVar();
}

DeltaClosureVarProxy* DeltaClosure::GetProxyVar (util_ui32 offset) const {
	DASSERT(IsReadyPrecondition());
	DASSERT(offset < total);
	return vars[offset];
}

bool DeltaClosure::IsInside (DeltaValue* val) const {
	DASSERT(IsReadyPrecondition());
	for (util_ui32 i = 0; i < total; ++i)
		if (vars[i]->GetVar() == val)
			return val;
	return (DeltaValue*) 0;
}

///////////////////////////////////////////////////////

