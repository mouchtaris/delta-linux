// DeltaTableFactory.cpp
// ScriptFighter Project.
// A. Savidis, February 2006.
//

#include "DeltaTableFactory.h"

USINGLETON_INSTANCE(DELTATABLE_FACTORYCLASS)

void DeltaTableFactory::CollectAllUncollected (void) {
	ubag<DeltaTable*> used = GetSingleton().GetUsedObjects();
	for (ubag<DeltaTable*>::iterator i = used.begin(); i != used.end(); ++i) {
		DASSERT(GetSingleton().HasProduced(i->first));
		DPTR(i->first)->ForceCollection();
	}
}
