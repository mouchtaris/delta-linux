// ContainerDelayedCollector.cpp
// ScriptFighter Project.
// A. Savidis, February 2006.
// Updated November 2008, changed collection list to a bag.
//

#include "ContainerDelayedCollector.h"
#include "ufunctors.h"
#include "DDebug.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////

ContainerDelayedCollector* ContainerDelayedCollector::singletonPtr = (ContainerDelayedCollector*) 0;

ContainerDelayedCollector* ContainerDelayedCollector::GetSingletonPtr (void) {
	DASSERT(singletonPtr);
	return singletonPtr;
} 

void ContainerDelayedCollector::SingletonCreate (void) {
	DASSERT(!singletonPtr);
	singletonPtr = DNEW(ContainerDelayedCollector);
}

void ContainerDelayedCollector::SingletonDestroy (void) {
	DASSERT(singletonPtr);
	DDELETE(singletonPtr);
	singletonPtr = (ContainerDelayedCollector*) 0;
}

//////////////////////////////////////////////////////////////////////////

bool ContainerDelayedCollector::IsCommitting (void)
	{ return committing; }

void ContainerDelayedCollector::CommitCollections (void) {

	DASSERT(!collectionNesting &&!committing);
	committing = true;

	if (forCollection.size())
		std::for_each(
			forCollection.begin(), 
			forCollection.end(),
			utuple_firstfunctoriser< std::pair<CollectableContainer*, bool> >(
				CollectableContainer::DeleteFunctor()
			)
		);

	if (collectedWhileCommitting.size()) {	// More collections posted while committing. 
		for (	std::list<CollectableContainer*>::iterator i = collectedWhileCommitting.begin();
				i != collectedWhileCommitting.end();
				++i		)
				CollectableContainer::DeleteFunctor()(*i);	// May cause extra insertions in this list.
		collectedWhileCommitting.clear();
	}

	forCollection.clear();
	committing = false;
}

//////////////////////////////////////////////////////////////////////////

void ContainerDelayedCollector::Entering (void) {
	if (!committing) {
		DASSERT(
			(forCollection.empty() || collectionNesting) &&	// Already something to collect => nesting > 0.
			!committing										// When committing no other collections can be issues.
		);
		++collectionNesting; 
	}
	else
		DASSERT(!forCollection.empty() && !collectionNesting);
}

//******************************

void ContainerDelayedCollector::Exiting (void) {
	if (!committing) {
		DASSERT(collectionNesting && !committing);
		if (!--collectionNesting) {
			CommitCollections();
		}
	}
	else
		DASSERT(!forCollection.empty() && !collectionNesting);
}

//////////////////////////////////////////////////////////////////////////

void ContainerDelayedCollector::AddWhileCommitting (CollectableContainer* cont) {

	DASSERT(cont->GetDyingState().IsInProgress());
	
	DASSERT(
		!collectionNesting	&&							// No collection nesting while committing.
		committing			&&							// Sure committing.
		forCollection.find(cont) == forCollection.end()	// No double deletions.
	); 

	collectedWhileCommitting.push_back(cont);			// Added separately.
}

//////////////////////////////////////////////////////////////////////////

void ContainerDelayedCollector::Add (CollectableContainer* cont) {
	if (!committing) {
		DASSERT(cont->GetDyingState().IsInProgress());
		
		DASSERT(
			collectionNesting	&&							// Delayed collection means nested collection.
			!committing			&&							// When committing no other collections can be issues.
			forCollection.find(cont) == forCollection.end()	// No double deletions.
		); 

		forCollection.insert(cont);
	}
	else
		AddWhileCommitting(cont);
}

//////////////////////////////////////////////////////////////////////////
	
ContainerDelayedCollector::ContainerDelayedCollector (void) : 
	collectionNesting(0),
	committing(false){}

ContainerDelayedCollector::~ContainerDelayedCollector() 
	{ DASSERT(!collectionNesting); }

//////////////////////////////////////////////////////////////////////////

