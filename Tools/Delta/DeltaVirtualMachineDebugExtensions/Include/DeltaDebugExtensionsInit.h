// DeltaDebugExtensionsInit.h
// Global initialisation to add debug extensions support.
// A. Savidis, June 2007.
//

#ifndef	DELTADEBUGEXTENSIONSINIT_H
#define	DELTADEBUGEXTENSIONSINIT_H

#include "DeltaDebugExtensions.h"

namespace VirtualMachineDebugExtensionsPackage {
	inline void Initialise (void) {
		DeltaDebugExtensions::SingletonCreate();
		DeltaDebugExtensions::Install();
	}
	inline void CleanUp (void) {
		DeltaDebugExtensions::SingletonDestroy();
	}
}

#endif	// Do not add stuff beyond this point.