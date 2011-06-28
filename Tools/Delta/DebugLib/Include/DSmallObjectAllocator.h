// DSmallObjectAllocator.h
// This is a template class for specialised memory managers,
// that are very optimised to work for fixed-size small objects
// in constant allocation / de-allocation time. They are compatible
// with the main memory manager in that they use four extra 
// bytes preceeding user-memory for carrying user-size. This is to be
// used for dispatching to the proper memory manager.
// A. Savidis, February 2003.
//

#ifndef	DSMALLOBJECTALLOCATOR_H
#define	DSMALLOBJECTALLOCATOR_H

#include <malloc.h>
#include "utypes.h"
#include "uptr.h"
#include "DMemoryManager.h"

//--------------------------------------------------------------------

#define	DDEBUG_ALLOCATOR_SIZE_MASK			0x80000000
#define	DDEBUG_ALLOCATOR_SIZECLEARBIT_MASK	0x7fffffff
#define	DDEBUG_MAXALLOCATION_SIZE			DDEBUG_ALLOCATOR_SIZECLEARBIT_MASK
#define	DDEBUG_USERSIZE_BYTES				((util_i16) sizeof(util_ui32))

// The ptr points at the size information. We index that 
//
inline bool dsmallobjectflagraised (void* p) {
	return (*((util_ui32*) p) & DDEBUG_ALLOCATOR_SIZE_MASK) != 0;
}

inline void dclearsmallobjectflag(void* p) {
	*((util_ui32*) p) &= DDEBUG_ALLOCATOR_SIZECLEARBIT_MASK;
}

//////////////////////////////////////////////////////////////////

class DSmallAllocatorSuper {

	public:
	virtual void*					Allocate (util_ui32 size) = 0;
	virtual void					Free (void* userMem) = 0;
	virtual bool					HasMemory (void) = 0;

	static DSmallAllocatorSuper*	GetBest (util_ui32 size);
	static DSmallAllocatorSuper*	GetAllocator (void* userMem)
										{ return GetBest(*((util_ui32*) userMem)); }

	static void						Initialise (void);
	static void						CleanUp (void);

	virtual ~DSmallAllocatorSuper(){}
};

//--------------------------------------------------------------------

#endif	// Do not add stuff beyond this point.