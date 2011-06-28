// ContainerDelayedCollector.h
// A garbage collection add-on for delayed collection of containers.
// When an object gets collected in Delta, it may cause a chain 
// reaction of collections. There are scenarios where deletion of
// containers in this chain is undesirable since those are still
// referred by objects that are certified to be collected later.
// ScriptFighter Project.
// A. Savidis, February 2006.
//

#ifndef	CONTAINERDELAYEDCOLLECTOR_H
#define	CONTAINERDELAYEDCOLLECTOR_H

#include "DeltaCollectableContainer.h"
#include "utypes.h"
#include "ufunctors.h"
#include <map>
#include <list>

#define	DELTA_COLLECTOR_ENTER		ContainerDelayedCollector::GetSingletonPtr()->Entering
#define	DELTA_COLLECTOR_EXIT		ContainerDelayedCollector::GetSingletonPtr()->Exiting
#define	DELTA_COLLECTOR_HAS_ENTERED	ContainerDelayedCollector::GetSingletonPtr()->HasEntered

/////////////////////////////////////////////////////////////////

class DVM_CLASS ContainerDelayedCollector {

	private:
	static ContainerDelayedCollector* singletonPtr;

	ubag<CollectableContainer*>			forCollection;
	std::list<CollectableContainer*>	collectedWhileCommitting;
	util_ui32							collectionNesting;
	bool								committing;

	void								CommitCollections (void);
	void								AddWhileCommitting (CollectableContainer* cont);

	/////////////////////////////////////////////////////////////////

	public:
	static ContainerDelayedCollector*	GetSingletonPtr (void);
	static void							SingletonCreate (void);
	static void							SingletonDestroy (void);

	void								Entering (void);
	void								Exiting (void);
	bool								HasEntered (void) const 
											{ return collectionNesting; }
	void								Add (CollectableContainer* cont);
	bool								IsCommitting (void);

	ContainerDelayedCollector (void);
	~ContainerDelayedCollector();
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.