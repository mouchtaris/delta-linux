// DeltaWxWidgetsLibStarter.cpp
// Handles the necessary initialisation and cleanup calls
// for the wxWidgets wrapper library.
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#include "DeltaWxWidgetsLibStarter.h"
#include "DeltaWxDefineMap.h"
#include "DeltaWxNativeInstanceToWrapperMap.h"
#include "DeltaWxWrapperToExternIdMap.h"

namespace wxWidgets			{
namespace LibraryStarter	{

////////////////////////////////////////////////////////////////

void Initialise (void)
{
	DefineMap::SingletonCreate();
	NativeInstanceToWrapperMap::SingletonCreate();
	WrapperToExternIdMap::SingletonCreate();
}

void CleanUp (void)
{
	WrapperToExternIdMapGet().CleanUp();
	NativeInstanceToWrapperMapGet().CleanUp();
	DefineMap::SingletonDestroy();
	WrapperToExternIdMap::SingletonDestroy();
	NativeInstanceToWrapperMap::SingletonDestroy();
}

////////////////////////////////////////////////////////////////

}	// LibraryStarter
}	// wxWidgets
