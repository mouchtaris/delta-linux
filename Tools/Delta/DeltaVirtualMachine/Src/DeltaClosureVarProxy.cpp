// DeltaClosureVarProxy.cpp
// Closures.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#include "DeltaClosureVarProxy.h"
#include "DeltaValue.h"

///////////////////////////////////////////////////////

DeltaClosureVarProxy::DeltaClosureVarProxy (DeltaValue* _var, DeltaClosureCreator* _creator) : 
	var			(_var), 
	isDynamic	(false),
	creator		(_creator)
	{}

DeltaClosureVarProxy::~DeltaClosureVarProxy() 
	{ ExplicitDestructor(); }

void DeltaClosureVarProxy::ExplicitDestructor (void) {
	if (isDynamic) {	// Sharable var wrappers are responsible to destroy a dynamic var.
		udelete(var);
		isDynamic = false;
	}
}

///////////////////////////////////////////////////////

void DeltaClosureVarProxy::ConvertToString (DeltaString* at, void* closure) 
	{ at->Add(uconstructstr("%s(%s)", GetExtClassString(), var->ConvertToString().c_str())); }

void DeltaClosureVarProxy::ConvertToStringBySkippingUserCode (DeltaString* at, void* closure) 
	{  at->Add(uconstructstr("%s(%s)", GetExtClassString(), var->ConvertToStringBySkippingUserCode().c_str())); }

void DeltaClosureVarProxy::ToValue (DeltaValue* at) 
	{ at->Undefine(); }
	
///////////////////////////////////////////////////////

void DeltaClosureVarProxy::SubstitudeWithDynamicVar (DeltaValue* dynVar) {
	DASSERT(!isDynamic && var && dynVar && var != dynVar);
	var			= dynVar;
	DPTR(var)->SetResidencyType(DeltaValue::Contained, (CollectableContainer*) this);
	isDynamic	= true;
}

///////////////////////////////////////////////////////
