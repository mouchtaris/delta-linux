// DeltaClosureVarProxy.h
// Variables belonging to a closure are accessed via a
// proxy so that they can be sharable and dynamically updatable
// (Proxy and State pattern together).
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#ifndef	DELTACLOSUREVARPROXY_H
#define	DELTACLOSUREVARPROXY_H

#include "DeltaValueTypes.h"
#include "DeltaCollectableContainer.h"

#define	DELTA_CLOSUREVAR_CLASSID	"ClosureVar"
#define	CLOSUREVAR_GET_INST(val)	VGET_INST(DeltaClosure, val, DELTA_CLOSUREVAR_CLASSID)

/////////////////////////////////////////////////////////////

class DVM_CLASS DeltaClosureCreator;

class DVM_CLASS DeltaClosureVarProxy : public CollectableContainer {

	DFRIENDDESTRUCTOR()

	private:
	DeltaValue*				var;			// Wrapped sharable value.
	bool					isDynamic;		// After exiting the host function it becomes dynamic.
	DeltaClosureCreator*	creator;

	virtual ~DeltaClosureVarProxy();

	public:

	virtual void			ExplicitDestructor (void);
	virtual const char*		GetExtClassString (void) const 
								{ return DELTA_CLOSUREVAR_CLASSID; }
	virtual void			DeleteMemory (void) 
								{ DDELETE(this); }

	virtual void			ConvertToString (DeltaString* at, void* closure = (void*) 0);
	virtual void			ConvertToStringBySkippingUserCode (DeltaString* at, void* closure = (void*) 0);
	virtual void			ToValue (DeltaValue* at);
	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL(CollectableContainer, DELTA_CLOSUREVAR_CLASSID)

	void					SubstitudeWithDynamicVar (DeltaValue* dynVar);
	bool					IsDynamic (void) const	{ return isDynamic; }
	const DeltaClosureCreator*	
							GetCreator (void) const { return creator; }
	DeltaValue*				GetVar (void) const		{ return var; }

	DeltaClosureVarProxy (DeltaValue* _var, DeltaClosureCreator* _creator);
};

/////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
