// DMemoryManager.h
// This is a special purpose heap memory manager that is to be used
// to substitute the builtin 'malloc' and 'free' functions.
// Defensive programming library.
// A. Savidis, November 2002.
//

#ifndef	DMEMORYMANAGER_H
#define	DMEMORYMANAGER_H

#include "utypes.h"

#define	DMEMORY_MAX_HEAPSIZE	0x1FFFFFFF	// 64MB of heap memory we require.
#define	DMEMORY_FREED_BYTE		'^'

//-----------------------------------------------------------

class DMemoryManager {

	public:
	static void		Initialise (void);
	static void		CleanUp (void);
	static void*	GetMemory (util_ui32 size);
	static void		ReleaseMemory (void* mem);
	static bool		IsValid (void* mem);
	static void		VerifyAll (void);
};

//-----------------------------------------------------------

#endif	// Do not add stuff beyond this point.