// AutoCollection.cpp
// Auto collection implementation (mainly static data).
// A. Savidis, December 2006.
//

#include "AutoCollection.h"

AutoCollector* AutoCollector::singletonPtr = (AutoCollector*) 0;

void AutoCollectableDestructor::operator() (AutoCollectable* item) const
	{ item->Delete(); }

/////////////////////////////////////////////////////////////////

void AutoCollector::SingletonCreate (void) {
	if (!singletonPtr)
		singletonPtr = DNEW(AutoCollector);
}

void AutoCollector::SingletonDestroy (void) {
	if (singletonPtr) {
		DDELETE(singletonPtr);
		singletonPtr = (AutoCollector*) 0;
	}
}

void AutoCollector::ValidateAll (void) {
	std::for_each(
		items.begin(),
		items.end(),
		AutoCollectable::ValidateFunctor()
	);
}

/////////////////////////////////////////////////////////////////
