// DeltaCollectableContainer.cpp
// Implementation of the container GC logic.
// ScriptFighter project.
// A. Savidis, Original version: May 2003.
// Updated the tracing GC algorithm to be simpler and faster,
// using the object residency information. January 2005.
// Updated June 2009, causing automatically all not yet collected 
// referrers to also collect.
//
#include "DeltaCollectableContainer.h"
#include "DeltaValue.h"
#include "DDebug.h"
#include "ContainerDelayedCollector.h"
#include "DeltaTable.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "usystem.h"
#include <algorithm>

//	#define	DELTA_PROFILE_GC

/////////////////////////////////////////////////////////////

#ifdef	DELTA_PROFILE_GC

static util_ui32 gc_iterations	= 0;
static util_ui32 gc_slow		= 0;
static util_ui32 gc_t			= 0;
static util_ui32 gc_accum		= 0;
static util_ui32 gc_entered		= 0;

struct gc_profiler {
	~gc_profiler() { 
		umodalmessage(
			"GC Profiling", 
			uconstructstr(
				"Total %u collection rounds with %u of them being slow (>2 msecs)\n"
				"and a total of %u msecs elapsed time for slow rounds.", 
				gc_iterations, gc_slow, gc_accum
			)
		); 
	}
};
static gc_profiler _profiler;
#define	DELTA_PROFILE_GC_ENTER()	if (!gc_entered++) { gc_t = ugettime(); ++gc_iterations; } else
#define	DELTA_PROFILE_GC_EXIT()		if (!--gc_entered) { if ((gc_t = (ugettime() - gc_t)) > 2) ++gc_slow, gc_accum += gc_t; } else
#else
#define	DELTA_PROFILE_GC_ENTER()
#define	DELTA_PROFILE_GC_EXIT()	
#endif

/////////////////////////////////////////////////////////////

CollectableContainer::ConstContList* CollectableContainer::visitList = (ConstContList*) 0;

void CollectableContainer::Initialise (void) 
	{ unew(visitList); }
				
void CollectableContainer::CleanUp (void)
	{ udelete(visitList); }

/////////////////////////////////////////////////////////////

CollectableContainer::CollectableContainer (void) : 
		stackReferrers		(0),
		refCounter			(0), 
		extRefCounter		(0),
		forcedCollection	(false){}

CollectableContainer::~CollectableContainer() 
	{  ExplicitDestructor(); }

void CollectableContainer::ExplicitDestructor (void) {

	if (GetDyingState().IsCompleted())
		return;

	DASSERT(GetDyingState().IsInProgress());
	DASSERT(!refCounter == !referrers.size());

	referrers.clear();
	refCounter = 0;

	GetDyingState().SetCompleted();
}

/////////////////////////////////////////////////////////////

bool CollectableContainer::Contains (const DeltaValue& val) const
	{ return val.GetResidencyContext() == this; }

void CollectableContainer::SetContained (DeltaValue* val)
	{ val->SetResidencyType(DeltaValue::Contained, this); }

/////////////////////////////////////////////////////////////

bool CollectableContainer::IsIndirectlyReferencable (void) const { 

	DASSERT(!DPTR(visitList)->size());
	bool result = IsReferencableRecursion();
	
	if (DPTR(visitList)->size()) {
		std::for_each(
			DPTR(visitList)->begin(),
			DPTR(visitList)->end(),
			ResetVisitedFunctor
		);
		DPTR(visitList)->clear();
	}

	return result;
}

//*****************************

bool CollectableContainer::IsReferencableRecursion (void) const { 

	DASSERT(!IsDirectlyReferencable());	// Recursion is performed only when not reachable directly.

	// If reachability is tested while the container is being collected
	// we know it is not reachable.
	if (GetDyingState().IsInProgress() || GetDyingState().IsCompleted())
		return false;

	// The reference counter should count the referring objects.
	DASSERT(refCounter == referrers.size());

	// If visited, it has to be in the visit list, while
	// we charge no referencability due to this.
	if (visitState.IsInProgress()) {
		DASSERT(
			std::find(
				DPTR(visitList)->begin(),
				DPTR(visitList)->end(),
				this
			) != DPTR(visitList)->end()
		);
		return false;
	}

	// Mark it as visited and put it in the visit list.
	visitState.SetInProgress(true);
	DPTR(visitList)->push_back(this);
	
	if (refCounter			&& 
		referrers.size()	&&
		std::find_if(
					referrers.begin(),
					referrers.end(),
					ReferencabilityPred()
				) != referrers.end())
		return true;
	else
		return IsAltReferencable();
}

//*****************************

void CollectableContainer::CheckReferrer::operator ()(const std::pair<DeltaValue*, bool>& p) const {
	const DeltaValue* val = p.first;
	DASSERT(
		val->Type()  == DeltaValue_ExternId			|| 
		val->Type()  == DeltaValue_Table			||
		val->Type()  == DeltaValue_ProgramFunc		||
		val->Type()  == DeltaValue_MethodFunc		||
		(val->Type() == DeltaValue_LibraryFunc	&& val->GetLibraryFuncArgsBinder())
	);
}

bool CollectableContainer::InvariantCheck (void) const {
	std::for_each(
		referrers.begin(),
		referrers.end(),
		CheckReferrer()
	);
	return true;	// Pseudo predicate, just to be put in an assert.
}

//*****************************

bool CollectableContainer::ReferencabilityPred::operator ()(const std::pair<DeltaValue*, bool>& p) const {

	const DeltaValue* val = p.first;

	// Verify the referrer type.
	DASSERT(
		val->Type()	 == DeltaValue_ExternId		|| 
		val->Type()	 == DeltaValue_Table		||
		val->Type()	 == DeltaValue_ProgramFunc	||
		val->Type()	 == DeltaValue_MethodFunc	||
		(val->Type() == DeltaValue_LibraryFunc	&& val->GetLibraryFuncArgsBinder())
	);

	if (val->GetResidencyType() == DeltaValue::Internal)	// Internal referrer.
		return true;
	else {
		DASSERT(val->GetResidencyType() == DeltaValue::Contained);
		CollectableContainer* cont = (CollectableContainer*) val->GetResidencyContext();
		return DPTR(cont)->IsDirectlyReferencable() || DPTR(cont)->IsReferencableRecursion();
	}
}

/////////////////////////////////////////////////////////////

void CollectableContainer::AddReference (DeltaValue* r) {

	DASSERT(!referrers.in(r));

	DASSERT(
		r->Type()  == DeltaValue_ExternId			|| 
		r->Type()  == DeltaValue_Table				||
		r->Type()  == DeltaValue_ProgramFunc		||
		r->Type()  == DeltaValue_MethodFunc			||
		(r->Type() == DeltaValue_LibraryFunc && r->GetLibraryFuncArgsBinder())
	);

	if (r->GetResidencyType() == DeltaValue::Stacked)
		++stackReferrers;

	referrers.insert(r);
}

/////////////////////////////////////////////////////////////

void CollectableContainer::RemoveReference (DeltaValue* r) { 

	DASSERT(refCounter);
	DASSERT(referrers.in(r));

	if (r->GetResidencyType() == DeltaValue::Stacked) 
		{ DASSERT(stackReferrers);  --stackReferrers; }
	referrers.remove(r);
}

/////////////////////////////////////////////////////////////

void CollectableContainer::IncRefCounter (DeltaValue* obj) { 

	// There is no way to start referring to an object while in its
	// destruction sequence, since we do not have destructors.

	DASSERT(!dyingState.IsInProgress());

	DASSERT(InvariantCheck());

	if (obj)
		{ AddReference(obj); ++refCounter; }
	else
		++extRefCounter;
}

/////////////////////////////////////////////////////////////

bool CollectableContainer::IsCollected (void) const
	{ return GetDyingState().HasStarted(); }

void CollectableContainer::ForceCollection (void) {
	if (!IsCollected()) {
		DASSERT(!forcedCollection);	// No die hard objects.
		forcedCollection = true; 
		Collect();
	}
}

void CollectableContainer::Collect (void) {
	
	DELTA_COLLECTOR_ENTER();

	ContList garbage;
	PickAndSetAsDyingAllAliveReferrers(garbage);
	ExplicitlyDestroyAll(garbage);

	DELTA_COLLECTOR_EXIT();
}

//*****************************

void CollectableContainer::PickAndSetAsDyingAliveReferrer (CollectableContainer* referrer, ContList& garbage) {

	// Either forced to die or else can't have a visible referrer.
	DASSERT(forcedCollection || !referrer->IsDirectlyReferencable());

	if (!referrer->IsCollected()) {
		DASSERT(!referrer->forcedCollection);
		if (forcedCollection)
			referrer->forcedCollection = true;	// Inherits forced collection style.
		referrer->PickAndSetAsDyingAllAliveReferrers(garbage);
	}
}

//*****************************

void CollectableContainer::PickAndSetAsDyingAllAliveReferrers (ContList& garbage) {

	DASSERT(!IsCollected());

	GetDyingState().SetInProgress(true);
	garbage.push_back(this);
	ContainerDelayedCollector::GetSingletonPtr()->Add(this);

	if (referrers.size())
		for (ValList::iterator i = referrers.begin();  i != referrers.end(); ++i) {

			// The residency context must be Contained in this call
			// for all referrers, else the callee would have been reachable.
			// This is only broken upon forced collection.

			DASSERT(
				(	i->first->GetResidencyType() == DeltaValue::Contained &&
					i->first->GetResidencyContext()							)	||
				(	forcedCollection &&  
					i->first->GetResidencyType() == DeltaValue::Internal && 
					!i->first->GetResidencyContext()						)
			);

			if (CollectableContainer* cont = (CollectableContainer*) i->first->GetResidencyContext())
				PickAndSetAsDyingAliveReferrer(cont, garbage);
		}

	AltPickAndSetAsDyingAllAliveReferrers(garbage);
}

//*****************************

void CollectableContainer::ExplicitlyDestroyAll (ContList& garbage) {
	if (garbage.size())
		std::for_each(
			garbage.begin(),
			garbage.end(),
			std::mem_fun(&CollectableContainer::ExplicitDestructor)
		);
}

/////////////////////////////////////////////////////////////

void CollectableContainer::DecRefCounter (DeltaValue* obj) { 

	if (!GetDyingState().IsInProgress() && !GetDyingState().IsCompleted()) {

		DASSERT(InvariantCheck());

		DELTA_PROFILE_GC_ENTER();

		if (obj)
			{ RemoveReference(obj);	--refCounter; }
		else
			{ DASSERT(extRefCounter); --extRefCounter; }

		if (!IsReferencable())
			Collect();

		DELTA_PROFILE_GC_EXIT();

	}
}

/////////////////////////////////////////////////////////////
