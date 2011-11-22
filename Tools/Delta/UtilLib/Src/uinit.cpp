// uinit.cpp
// Global initialisation of the util package.
// A. Savidis, December 2005.
//

#include "uvalidatable.h"
#include "uerrorclass.h"
#include "ulatedestructor.h"
#include "udynamiclibloader.h"
#include "uinit.h"

static util_ui32 initCounter = 0;

namespace UtilPackage {

	UTILLIB_FUNC void Initialise (void) {
		if (!initCounter++) {
			ValidatableHandler::SingletonCreate();
			uerror::SingletonCreate();
			GlobalLateDestructor::SingletonCreate();
			udynamiclibloader::SingletonCreate();
		}
	}

	UTILLIB_FUNC void CleanUp (void) {
		if (!--initCounter) {
			ValidatableHandler::SingletonDestroy();
			uerror::SingletonDestroy();
			GlobalLateDestructor::SingletonDestroy();
			udynamiclibloader::SingletonDestroy();
		}
	}
}