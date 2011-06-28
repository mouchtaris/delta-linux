// VMInit.h
// Global virtual machine initialisation of singletons.
// A. Savidis, February 2003.
//

#ifndef	VMINIT_H
#define	VMINIT_H

#include "DeltaStdDefs.h"

namespace VirtualMachinePackage {
	DVM_FUNC void Initialise (void);
	DVM_FUNC void CleanUp (void);
}

#endif	// Do not add stuff beyond this point.