// ShMemLib.h
// Simple shared memory facilities for virtual machines running
// IN THE SAME PROCESS, i.e., not interprocess shared memory.
// ScriptFigher Project.
// A. Savidis, December 1999.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#ifndef	SHMEMLIB_H
#define	SHMEMLIB_H

#include "DeltaLibraryObjectCreator.h"
#include "ShMemLibP.h"

extern void Install_DeltaShMem_Lib (DeltaObject* mainAPI);
extern void CleanUp_DeltaShMem_Lib (void);

#endif	// Do not add stuff beyond this point.