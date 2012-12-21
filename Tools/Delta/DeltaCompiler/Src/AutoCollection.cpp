// AutoCollection.cpp
// Auto collection implementation (mainly static data).
// A. Savidis, December 2006.
//

#include "AutoCollection.h"

void AutoCollectableDestructor::operator() (AutoCollectable* item) const
	{ item->Delete(); }

/////////////////////////////////////////////////////////////////

void AutoCollector::ValidateAll (void) {
	std::for_each(
		items.begin(),
		items.end(),
		AutoCollectable::ValidateFunctor()
	);
}

/////////////////////////////////////////////////////////////////
