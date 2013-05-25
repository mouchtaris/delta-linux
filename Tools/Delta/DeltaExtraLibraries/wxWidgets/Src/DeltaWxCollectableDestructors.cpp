// DeltaWxCollectableDestructors.cpp
// Destructor functions that will be called when the
// reference counter of the collectable reach zero.
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#include "DeltaWxCollectableDestructors.h"
#include "DeltaWxWrapperToExternIdMap.h"

using namespace wxWidgets;

////////////////////////////////////////////////////////////////

void LetWrapperLive(void* val)
{
	if (WrapperToExternIdMapGet().HasKey((util_ui32)val))
		WrapperToExternIdMapGet().RemoveEntry((util_ui32)val);
}

////////////////////////////////////////////////////////////////
