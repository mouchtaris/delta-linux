// DeltaDebugExtensionsStdDefs.h
// Script Fighter Project.
// A. Savidis, (Original) June 2006, (RF) June 2007 .
//

#ifndef	DELTADEBUGEXTENSIONSSTDDEFS_H
#define	DELTADEBUGEXTENSIONSSTDDEFS_H

/////////////////////////////////////////////////////////////////

#include "DeltaVirtualMachine.h"
#include "utypes.h"

#include <functional>
#include <list>

/////////////////////////////////////////////////////////////////

#define	DELTA_DEBUG_MUTEX_NAME				"VM_DEBUG_MUTEX"
#define	DELTA_GLOBALCODE_DEFLINE			1 // Assume global code always defined at line 1.

enum DeltaDebugCallStackEntryType {
	DeltaDebugCallStackEntry_LibraryFunc	=	0,
	DeltaDebugCallStackEntry_UserFunc		=	1,
	DeltaDebugCallStackEntry_GlobalCode		=	2,
	DeltaDebugCallStackEntry_Undef			=	3
};

#define	DELTA_DEBUG_CALLSTACK_SIZE			4096
#define	DELTA_DEBUG_CALLSTACK_EMPTY_INDEX	-1

typedef	util_ui32 DeltaDebugCallStackFuncValue;

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
