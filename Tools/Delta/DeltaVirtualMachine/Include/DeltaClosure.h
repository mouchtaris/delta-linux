// DeltaClosure.h
// Function closure.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, September 2010.
//

#ifndef	DELTACLOSURE_H
#define	DELTACLOSURE_H

#include "DeltaValue.h"
#include "DeltaClosureVarProxy.h"
#include "DeltaCollectableContainer.h"
#include <vector>

#define	DELTA_CLOSURE_CLASSID				"Closure"
#define	DELTA_NIL_CLOSURE					((DeltaClosure*) 0)
#define	CLOSURE_GET_INST(val)				VGET_INST(DeltaClosure, val, DELTA_CLOSURE_CLASSID)
#define	DELTA_CLOSURE_PSEUDO_EXTERNID_VALUE	((void*) 0)

/////////////////////////////////////////////////////////////

class DVM_CLASS DeltaClosure : public CollectableContainer {

	friend class DVM_CLASS DeltaClosureCreator;
	friend class DVM_CLASS DeltaClosureHandling;

	private:
	typedef std::vector<DeltaClosureVarProxy*>			ClosureVars;
	typedef std::map<DeltaClosureVarProxy*, util_ui32>	PosMap;
	util_ui32				total;
	ClosureVars				vars;								// Stored by order of first reference in the inner function.
	PosMap					varPositions;						// From a var to its position in the vector.
	DeltaClosure*			used;								// Some closure vars may come from an outer closure.
	DeltaValue				pseudoExternId;						// A pseudo extern id used to refer to closure vars.

	const ClosureVars&		GetVars (void) const
								{ return vars; }
	bool					IsReadyPrecondition (void) const;
	void					AddVar (DeltaClosureVarProxy* var);
	void					DisableExpansion (void);			// Due to error.

	/////////////////////////////////////////////////////////////

	public:
	util_ui32				GetTotal (void) const
								{ DASSERT(IsReadyPrecondition()); return total; }
	DeltaValue*				GetVar (util_ui32 offset) const;		// Order of use inside inner function.
	DeltaClosureVarProxy*	GetProxyVar(util_ui32 offset) const;	// Order of use inside inner function.
	void					SetUses (DeltaClosure* _used)
								{ used = _used; }
	bool					IsInside (DeltaValue* val) const;

	//***********************************

	virtual void			ExplicitDestructor (void);
	virtual void			IncRefCounter (DeltaValue* from);
	virtual void			DecRefCounter (DeltaValue* from);
	virtual const char*		GetExtClassString (void) const 
								{ return DELTA_CLOSURE_CLASSID; }
	virtual void			DeleteMemory (void) 
								{ DDELETE(this); }

	virtual void			ConvertToString (DeltaString* at, void* closure = (void*) 0);
	virtual void			ConvertToStringBySkippingUserCode (DeltaString* at, void* closure = (void*) 0);
	virtual void			ToValue (DeltaValue* at);
	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL(CollectableContainer, DELTA_CLOSURE_CLASSID)

	/////////////////////////////////////////////////////////////

	DeltaClosure (util_ui32 _total);
	virtual ~DeltaClosure();
};

/////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
