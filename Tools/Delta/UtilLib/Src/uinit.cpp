// uinit.cpp
// Global initialisation of the util package.
// A. Savidis, December 2005.
//

#include "uvalidatable.h"
#include "uerrorclass.h"
#include "ulatedestructor.h"
#include "udynamiclibloader.h"
#include "uinit.h"

namespace UtilPackage {

	UTILLIB_FUNC void Initialise (void) {
		ValidatableHandler::SingletonCreate();
		uerror::SingletonCreate();
		GlobalLateDestructor::SingletonCreate();
		udynamiclibloader::SingletonCreate();
	}

	UTILLIB_FUNC void CleanUp (void) {
		ValidatableHandler::SingletonDestroy();
		uerror::SingletonDestroy();
		GlobalLateDestructor::SingletonDestroy();
		udynamiclibloader::SingletonDestroy();
	}
}