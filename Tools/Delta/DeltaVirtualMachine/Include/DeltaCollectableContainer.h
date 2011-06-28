// DeltaCollectableContainer.h
// This is a base class for all delta containers to ensure they 
// support the garbage collection logic. All containers are collected when there
// is referencable object (anyhow) to refer to them.
// ScriptFighter project.
// A. Savidis, Original version: May 2003.
// Udated (.cpp file) January 2005.
//

#ifndef	DELTACOLLECTABLECONTAINER_H
#define	DELTACOLLECTABLECONTAINER_H

#include "uhash.h"
#include "utypes.h"
#include "ucycliceval.h"
#include "uvalidatable.h"
#include "DeltaString.h"
#include "DDebug.h"
#include "DeltaStdDefs.h"

#include <list>
#include <functional>

#define	COLLECTABLE_CONTAINER_TYPE	"CollectableContainer"

//-----------------------------------------------------------

class DVM_CLASS DeltaValue;
class DVM_CLASS ContainerDelayedCollector;

class DVM_CLASS CollectableContainer : public virtual Validatable {

	/////////////////////////////////////////////////////////////

	DFRIENDDESTRUCTOR()

	protected:
	struct ReferencabilityPred;
	friend struct ReferencabilityPred;
	friend class ContainerDelayedCollector;

	typedef ubag<DeltaValue*>						ValList;
	typedef std::list<CollectableContainer*>		ContList;
	typedef std::list<const CollectableContainer*>	ConstContList;

	util_ui32				stackReferrers;				// How many stack vars directly refer to it.					
	ValList					referrers;					// Objects referring to container.
	util_ui32				refCounter;					// All normal references are counted here.
	util_ui32				extRefCounter;				// No-object references stored here.

	static ConstContList*	visitList;					// For cycle elimination during tracing of external refs.
	uworkstate				printState;					// If in printing calculation process.
	mutable uworkstate		visitState;					// In cycle detection algorithm.
	uworkstate				dyingState;					// If destructor call in progress.
	bool					forcedCollection;			// If was forced to die even when referencable.

	struct ReferencabilityPred : public std::unary_function<std::pair<DeltaValue*, bool>, bool> {
		bool operator()(const std::pair<DeltaValue*, bool>&) const;
	};
	
	struct CheckReferrer : public std::unary_function<std::pair<DeltaValue*, bool>, void> {
		void operator()(const std::pair<DeltaValue*, bool>&) const;
	};
	
	bool					InvariantCheck (void) const;

	/////////////////////////////////////////////////////////////

	static void				ResetVisitedFunctor (const CollectableContainer* cont) 
								{ DPTR(cont)->GetVisitState().SetInProgress(false); }

	void					AddReference (DeltaValue* referring);						
	void					RemoveReference (DeltaValue* referring);

	void					CollectAllReferrersRecursion (void);
	void					CollectAllReferrers (void);

	bool					IsReferencableRecursion (void) const;
	bool					IsDirectlyReferencable (void) const
								{ return stackReferrers || extRefCounter; }
	bool					IsIndirectlyReferencable (void) const;
	virtual bool			IsAltReferencable (void) const 
								{ return false; }
	
	bool					IsCollected (void) const;
	void					Collect (void);
	void					PickAndSetAsDyingAllAliveReferrers (ContList& garbage);
	void					PickAndSetAsDyingAliveReferrer (CollectableContainer* referrer, ContList& garbage);
	virtual void			AltPickAndSetAsDyingAllAliveReferrers (ContList& garbage) {}

	void					ExplicitlyDestroyAll (ContList& garbage);

	/////////////////////////////////////////////////////////////

	uworkstate&				GetVisitState (void) const
								{ return visitState; }
	uworkstate&				GetPrintState (void)
								{ return printState; }
	const uworkstate&		GetDyingState (void) const 
								{ return dyingState; }
	uworkstate&				GetDyingState (void)
								{ return dyingState; }

	/////////////////////////////////////////////////////////////

	public:
	struct AutoPrintState {
		CollectableContainer& cont;
		AutoPrintState(CollectableContainer& _cont) : cont(_cont){ cont.GetPrintState().SetInProgress(true); }
		~AutoPrintState() { cont.GetPrintState().SetInProgress(false); }
	};

	static void				Initialise (void);						
	static void				CleanUp (void);

	virtual void			IncRefCounter (DeltaValue* from);	// When being refered by an object. 
	virtual void			DecRefCounter (DeltaValue* from);	// When stop being referred by an object.
	void 					IncRefCounter (DeltaValue& from)
								{ IncRefCounter(&from); }
	void					DecRefCounter (DeltaValue& from)
								{ DecRefCounter(&from); }
	const ValList&			GetReferrers (void) const
								{ return referrers; }
	void					ForceCollection (void);
	bool					IsReferencable (void) const
								{ return IsDirectlyReferencable() || IsIndirectlyReferencable(); }

	virtual void			ExplicitDestructor (void);			// Destructor logic should go into this.
	virtual void			DeleteMemory (void) = 0;			// Every subclass should define it as needed (mem clear).

	virtual void			ConvertToString (DeltaString* at, void* closure = (void*) 0) = 0;
	virtual void			ConvertToStringBySkippingUserCode (DeltaString* at, void* closure = (void*) 0) = 0;
	virtual void			ToValue (DeltaValue* at) = 0;

	struct DeleteFunctor : public std::unary_function<CollectableContainer*, void> {
		void operator()(CollectableContainer* p) const { p->DeleteMemory(); }
	};

	/////////////////////////////////////////////////////////////

	const char*				GetExtClassString (void) const 
								{ return COLLECTABLE_CONTAINER_TYPE; }

	DYNAMIC_CAST_NOBASE_INLINE_IMPL(COLLECTABLE_CONTAINER_TYPE)

	/////////////////////////////////////////////////////////////

	CollectableContainer (void);
	virtual ~CollectableContainer();
};

/////////////////////////////////////////////////////////////

inline	void DELTA_EGC_CONTAINER_INCREFCOUNTER (void* val, DeltaValue* r) {
		CollectableContainer* cont = VGET_INST(CollectableContainer, val, COLLECTABLE_CONTAINER_TYPE);
		DASSERT(cont);
		cont->IncRefCounter(r);
}

inline	void DELTA_EGC_CONTAINER_INCREFCOUNTER (void* val, DeltaValue& r)
	{ DELTA_EGC_CONTAINER_INCREFCOUNTER (val, &r); }

inline	void DELTA_EGC_CONTAINER_DECREFCOUNTER (void* val, DeltaValue* r) {
		CollectableContainer* cont = VGET_INST(CollectableContainer, val, COLLECTABLE_CONTAINER_TYPE);
		DASSERT(cont);
		cont->DecRefCounter(r);
}
inline	void DELTA_EGC_CONTAINER_DECREFCOUNTER (void* val, DeltaValue& r)
	{ DELTA_EGC_CONTAINER_DECREFCOUNTER(val, &r); }

inline	CollectableContainer* DELTA_EGC_CONTAINER_GETVALUE (void* val) {
		CollectableContainer* cont = VGET_INST(CollectableContainer, val, COLLECTABLE_CONTAINER_TYPE);
		DASSERT(cont);
		return cont;
}

#define	DELTA_CONTAINER_TOSTRING_PROCESSING() \
		CollectableContainer::AutoPrintState __tostring__(*this)

//-----------------------------------------------------------

#endif	// Do not ad stuff beyond this point.

