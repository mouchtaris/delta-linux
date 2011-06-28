// DSmallObjectAllocator.cpp
// Declaration of the allocators.
// A. Savidis, February 2003.
//

#include "DDebug.h"

#ifdef	DDEBUG_USE

#include "DSmallObjectAllocator.h"
#include "DSmallAllocatorTemplate.h"
#include "ufunctors.h"

//--------------------------------------------------------------------

#define	DSMALLBLOCK_MAXSIZE	 124

typedef  DSmallObjectAllocator<DBlock16, DBlock16Exp, DBlock16Max>		DBlock16Allocator;
typedef  DSmallObjectAllocator<DBlock32, DBlock32Exp, DBlock32Max>		DBlock32Allocator;
typedef  DSmallObjectAllocator<DBlock64, DBlock64Exp, DBlock64Max>		DBlock64Allocator;
typedef  DSmallObjectAllocator<DBlock128, DBlock128Exp, DBlock128Max>	DBlock128Allocator;

static DBlock16Allocator*	allocator16;
static DBlock32Allocator*	allocator32;
static DBlock64Allocator*	allocator64;
static DBlock128Allocator*	allocator128;

static DSmallAllocatorSuper*	bestAllocators[DSMALLBLOCK_MAXSIZE+1];
static bool						hasInitialised;
static bool						cleanedUp = false;

//////////////////////////////////////////////////////////////////

DSmallAllocatorSuper* DSmallAllocatorSuper::GetBest (util_ui32 size) {

	DASSERT(size);
	DASSERT(!cleanedUp);

	if (size > DSMALLBLOCK_MAXSIZE)
		return (DSmallAllocatorSuper*) 0;
	else {
		Initialise();
		return bestAllocators[size];
	}
}

//////////////////////////////////////////////////////////////////

void DSmallAllocatorSuper::Initialise (void) {

	if (!hasInitialised) {

		hasInitialised = true;

		util_ui8 i;

		allocator16 = new (malloc(sizeof(DBlock16Allocator))) DBlock16Allocator;
		for (i = 0; i <= DBlock16 -DDEBUG_USERSIZE_BYTES; ++i)
			bestAllocators[i] = allocator16;

		allocator32 = new (malloc(sizeof(DBlock32Allocator))) DBlock32Allocator;
		for (i = DBlock16 - DDEBUG_USERSIZE_BYTES+1; i <= DBlock32 - DDEBUG_USERSIZE_BYTES; ++i)
			bestAllocators[i] = allocator32;
		
		allocator64 = new (malloc(sizeof(DBlock64Allocator))) DBlock64Allocator;
		for (i = DBlock32 - DDEBUG_USERSIZE_BYTES+1; i <= DBlock64 - DDEBUG_USERSIZE_BYTES; ++i)
			bestAllocators[i] = allocator64;

		allocator128 = new (malloc(sizeof(DBlock128Allocator))) DBlock128Allocator;
		for (i = DBlock64 - DDEBUG_USERSIZE_BYTES+1; i <= DBlock128 - DDEBUG_USERSIZE_BYTES; ++i)
			bestAllocators[i] = allocator128;
	}
}

//////////////////////////////////////////////////////////////////

void DSmallAllocatorSuper::CleanUp (void) {

	return;	// IGNORED

	if (hasInitialised) {

		cleanedUp = true;

		ucalldestructor(allocator16);
		free(allocator16);

		ucalldestructor(allocator32);
		free(allocator32);

		ucalldestructor(allocator64);
		free(allocator64);

		ucalldestructor(allocator128);
		free(allocator128);

		allocator16		= (DBlock16Allocator*) 0;
		allocator32		= (DBlock32Allocator*) 0;
		allocator64		= (DBlock64Allocator*) 0;
		allocator128	= (DBlock128Allocator*) 0;

		for (util_ui8 i=0; i<=DSMALLBLOCK_MAXSIZE; ++i)
			bestAllocators[i]=(DSmallAllocatorSuper*) 0;

		hasInitialised = false;
	}
}

//--------------------------------------------------------------------

#endif	// DDEBUG_USE