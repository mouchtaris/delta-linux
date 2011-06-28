// DeltaCollectableSimple.h
// Part of the Delta Extensible Garbage Collection capability.
// A very powerful feature allowing garbage collection to be extended
// for non-builtin simple user defined dynamic types. The class will only
// have to be used for non-container types.
//
// ScriptFighter Project.
// A. Savidis, June 2001.
// Last revision, May 2003.
//

#ifndef	DELTACOLLECTABLESIMPLE_H
#define	DELTACOLLECTABLESIMPLE_H

#include "DDebug.h"
#include "utypes.h"
#include "uvalidatable.h"
#include "DeltaStdDefs.h"

#define	COLLECTABLE_SIMPLE_TYPE	"CollectableSimple"

/////////////////////////////////////////////////////
// This is the class through which delta garbage collectable
// non-container (simple) dynamic data types (i.e. allocated ptrs) can be created.
// 
class DVM_CLASS CollectableSimple : public virtual Validatable {

	DFRIENDDESTRUCTOR()

	private:

	/////////////////////////////////////////////////////

	void*			val;			// The dynamic item.
	std::string		typeStr;		// Type string of collectable wrapped type.
	util_ui32		refCounter;		// Usage reference counter. On creation set to 0.
	void (*destructor) (void*);		// The destructor function.

	CollectableSimple (const char* _typeStr, void* _val, void (*_destructor) (void*)) : 
				val			(_val),
				typeStr		(_typeStr),
				refCounter	( 0 ),
				destructor	(_destructor) {}

	virtual			~CollectableSimple(){ }

	void			Destroy (void) {
						Validate();
						(*destructor)(val);
						DDELETE(this);
					}

	public:

	/////////////////////////////////////////////////////
	// Upon assignment, the dest decreases its ref counter, while the src
	// increases it. Upon destruction, the ref counter is decreased.
	//
	void			IncRefCounter (void)
						{ ++refCounter; }

	void			DecRefCounter (void) {
						if (!--refCounter)
							Destroy();
					}

	void*			GetValue (void) 
						{ return val; }	

	const char*		GetExtClassString (void) const 
						{ return typeStr.c_str(); }

	// You can use this destructor in all cases that a you have an object, locally
	// storing a reference to a collectable simple, which is about to get destroyed.
	//
	static void		Destructor (void* val) {
						CollectableSimple* v = VGET_INST(CollectableSimple, val, COLLECTABLE_SIMPLE_TYPE);
						DASSERT(v);
						DPTR(v)->DecRefCounter();
					}

	DYNAMIC_CAST_DUAL_NOBASE_INLINE_IMPL(COLLECTABLE_SIMPLE_TYPE, typeStr)

	/////////////////////////////////////////////////////

	static CollectableSimple*	
					New (const char* typeStr, void* val, void (*destructor)(void*))
						{ return DNEWCLASS(CollectableSimple, (typeStr, val, destructor)); }
};

//////////////////////////////////////////////////////////////

inline	void DELTA_EGC_SIMPLE_INCREFCOUNTER (void* val) {
		CollectableSimple* simple = VGET_INST(CollectableSimple, val, COLLECTABLE_SIMPLE_TYPE);
		DASSERT(simple);
		simple->IncRefCounter();
}

inline	void DELTA_EGC_SIMPLE_DECREFCOUNTER (void* val) {
		CollectableSimple* simple = VGET_INST(CollectableSimple, val, COLLECTABLE_SIMPLE_TYPE);
		DASSERT(simple);
		simple->DecRefCounter();
}

inline	void* DELTA_EGC_SIMPLE_GETVALUE (void* val) {
		CollectableSimple* simple = VGET_INST(CollectableSimple, val, COLLECTABLE_SIMPLE_TYPE);
		DASSERT(simple);
		return simple->GetValue();
}

//-----------------------------------------------------------

#endif	// Do not add stuff beyond this point.
