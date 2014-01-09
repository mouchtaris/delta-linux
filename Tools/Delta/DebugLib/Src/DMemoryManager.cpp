// DMemoryManager.cpp
// Defensive programming library.
// Memory manager implementation.
// A. Savidis, November 2002.
//
// TODO: MAKE IT THREAD SAFE WITH A MUTEX.
//
//

#ifdef	_MAC_
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#include "utypes.h"
#include "uptr.h"
#include "uarraydoublelist.h"
#include "DMemoryManager.h"
#include "DSmallObjectAllocator.h"
#include "DAllocationStats.h"
#include "DDebug.h"
#include "DAssert.h"

//	#define	MEMORYMANAGER_NEWDELETE
//	#define	MEMORYMANAGER_SMALLOBJECTS
//	#define	MALLOCFREE_SUBSTITUTE
//	#define	DRECORD_ALLOCATIONSTATS

#if	defined(MEMORYMANAGER_SMALLOBJECTS) && !defined(DDEBUG_USE)
#undef	MEMORYMANAGER_SMALLOBJECTS
#endif

//-----------------------------------------------------------

#ifdef	DRECORD_ALLOCATIONSTATS

#define	ALLOCATIONSTAT_MAX	1024
#define	ALLOCATIONSTAT_FILE	"dsmallallocstats.txt"

static AllocationStats* allocationStats;

#define	DINITALLOCSTATS()			allocationStats = new (DMemoryManager::GetMemory(sizeof(AllocationStats))) AllocationStats
#define	DRECORDALLOCSIZE(size)		DASSERT(allocationStats); allocationStats->Record(size)
#define	DREPORTALLOCSTATS()			DASSERT(allocationStats); allocationStats->Report(ALLOCATIONSTAT_FILE)
#define	DCLEANALLOCSTATS()			DASSERT(allocationStats); allocationStats->~AllocationStats(); DMemoryManager::ReleaseMemory(allocationStats)

#else

#define	DINITALLOCSTATS()
#define	DRECORDALLOCSIZE(size)
#define	DREPORTALLOCSTATS()
#define	DCLEANALLOCSTATS()

#endif	// DRECORD_ALLOCATIONSTATS

//-----------------------------------------------------------

#ifdef	MALLOCFREE_SUBSTITUTE

// This class represents a heap memory block, that can be allocated
// or not.
//

#define	MEMORYBLOCK_MAX_TOTAL	0x3FFFC

/////////////////////////////////////////////////////////////
// Constructor parameters for memory block as a specific class.
//
struct MemoryBlockConstrParms {

	void*			addr;
	util_ui32		size;

	MemoryBlockConstrParms (void* _addr, util_ui32 _size) :
		addr(_addr),
		size(_size) {}

	MemoryBlockConstrParms (const MemoryBlockConstrParms& parms) :
		addr(parms.addr),
		size(parms.size) {}

	MemoryBlockConstrParms (void) :
		addr((void*) 0),
		size(0) {}

	~MemoryBlockConstrParms(){}
};

/////////////////////////////////////////////////////////////

class MemoryBlock : public udoublylinked {

	//**************************

	private:
	void*			addr;
	util_ui32		size;

	//**************************

	public:
	static MemoryBlock* GetBlock (void* addr) {
				MemoryBlock* p;
				memcpy(&p, POFFS(addr, -((util_i32) sizeof(MemoryBlock*))), sizeof(MemoryBlock*));
				return p;
			}

	bool			ExactlyBeforeMe (const MemoryBlock& block) const;

	bool			ExactlyAfterMe (const MemoryBlock& block) const;

	void*			GetEnd (void) const { return POFFS(addr, size); }

	void*			GetStart (void) const { return addr; }

	util_ui32		GetSize (void) const { return size; }

	void*			ReduceLeft (util_ui32 reduceSize);

	void*			ReduceRight (util_ui32 reduceSize);

	void			GrowLeft (util_ui32 growSize) 
						{ PINC(addr, -((util_i32) growSize)); size += growSize; }

	void			GrowRight (util_ui32 growSize)
						{ size += growSize; }

	void			init (void)
						{ addr = (void*) 0; size = 0; }

	void			Set (void* _addr, util_ui32 _size) {
						addr = _addr;
						size = _size;
					}

	MemoryBlock (void){}
	~MemoryBlock(){}
};

/////////////////////////////////////////////////////////////
// Returns true if 'block' memory is exactly before 'this'.
//
bool MemoryBlock::ExactlyBeforeMe (const MemoryBlock& block) const {
	return block.GetEnd()==GetStart();

}

/////////////////////////////////////////////////////////////
// Returns true if 'block' memory is exactly after 'this'.
//
bool MemoryBlock::ExactlyAfterMe (const MemoryBlock& block) const {
	return block.GetStart()==GetEnd();
}

/////////////////////////////////////////////////////////////
// ReduceLeft removes heading part of the block memory, 
// setting size and address accordingly. It is to be used only for allocation.
//
void* MemoryBlock::ReduceLeft (util_ui32 reduceSize) {

	DASSERT(size > reduceSize);

	void* retAddr = addr;
	PINC(addr, reduceSize);
	size -= reduceSize;

	return retAddr;
}

/////////////////////////////////////////////////////////////
// ReduceRight removes trailing part of the block memory, needing
// only to set size.
//
void* MemoryBlock::ReduceRight (util_ui32 reduceSize) {

	DASSERT(size > reduceSize);

	size -= reduceSize;

	return POFFS(addr, size);
}

/////////////////////////////////////////////////////////////

typedef uarraydoublelist<MemoryBlock, MEMORYBLOCK_MAX_TOTAL> MemoryBlockArray;
static	MemoryBlockArray*	freeBlocks;
static	MemoryBlockArray*	usedBlocks;

#define	MEMORYBLOCK_RESERVED_HEAP \
	2*sizeof(MemoryBlockArray)

static	void*			heapMemory;
static	void*			userHeap;
static	util_ui32		allocatedUserMemory;
static bool				memoryManagerInitalised;
static util_ui32		heapSize;

#endif	// MALLOCFREE_SUBSTITUTE

//-----------------------------------------------------------

void DMemoryManager::Initialise (void) {

#ifdef	MALLOCFREE_SUBSTITUTE

	heapSize = DMEMORY_MAX_HEAPSIZE + MEMORYBLOCK_RESERVED_HEAP;
	heapMemory = malloc(heapSize);
	DASSERT(heapMemory);

	// Use placement operator to initialise the arrays of
	// memory blocks.
	//
	freeBlocks = new (heapMemory) MemoryBlockArray;
	usedBlocks = new (POFFS(heapMemory, sizeof(MemoryBlockArray))) MemoryBlockArray;

	userHeap = POFFS(heapMemory, MEMORYBLOCK_RESERVED_HEAP);
	memset(userHeap, DMEMORY_FREED_BYTE, DMEMORY_MAX_HEAPSIZE);

	MemoryBlock* firstBlock = freeBlocks->allocate();
	firstBlock->Set(userHeap, DMEMORY_MAX_HEAPSIZE);
#endif
}

/////////////////////////////////////////////////////////////

void DMemoryManager::CleanUp (void) {

	DREPORTALLOCSTATS();
	DCLEANALLOCSTATS();

#ifdef	MALLOCFREE_SUBSTITUTE

	freeBlocks->clear();
	freeBlocks = (MemoryBlockArray*) 0;

	usedBlocks->clear();
	usedBlocks = (MemoryBlockArray*) 0;

	free(heapMemory);
#endif
}

/////////////////////////////////////////////////////////////
// The present algorithm tries to find a block that
// is closest to the required size. If not, it shrinks
//
void* DMemoryManager::GetMemory (util_ui32 size) {

#ifdef	MALLOCFREE_SUBSTITUTE

	// If the requested memory exceeds the current heap size,
	// this is a normal not enough memory case.
	//
	util_ui32 neededSize = size + sizeof(MemoryBlock*);

	if (allocatedUserMemory + neededSize > DMEMORY_MAX_HEAPSIZE) {
		DASSERT(!"Heap memory exceeded with request!");
		return (void*) 0;
	}

	MemoryBlock* block = freeBlocks->head();
	MemoryBlock* best = (MemoryBlock*) 0;

	while (block) {

		if (block->GetSize()==neededSize) {
			best = block;
			break;
		}

		// If no best found yet, get the first in which the
		// required size can fit. Else, select a new block
		// with size closer to the required one.
		//
		if (!best) {
			if (block->GetSize() > neededSize)
				best = block;
		}
		else
		if (best->GetSize() > block->GetSize() && block->GetSize() > neededSize)
			best = block;

		block = (MemoryBlock*) block->getnext();

	}

	// If no best block found, this is a fragmentation problem.
	//
	if (!best) {
		DASSERT(!"Cannot allocate requested block due to fragmentation!");
		return (void*) 0;
	}

	DASSERT(neededSize <= best->GetSize()); 
	void* addr = best->GetStart();

	if (best->GetSize()==neededSize)	// Perfect fit, whole block used.
		freeBlocks->release(best);
	else								// Got more than needed, just reduce the block.
		best->ReduceLeft(neededSize);

	// Now create a new allocation block.
	//
	MemoryBlock* newBlock = usedBlocks->allocate();
	newBlock->Set(addr, neededSize);
	allocatedUserMemory += neededSize;

	memcpy(addr, &newBlock, sizeof(MemoryBlock*));
	return POFFS(addr, sizeof(MemoryBlock*));
#else
	return malloc(size);
#endif
}

/////////////////////////////////////////////////////////////

void DMemoryManager::ReleaseMemory (void* mem) {

#ifdef	MALLOCFREE_SUBSTITUTE

	MemoryBlock* block = MemoryBlock::GetBlock(mem);

	// Then, we try to integrate the released memory with existing
	// blocks, if memory is consecutive, by getting potential blocks
	// referring to memory before and after.
	//
	MemoryBlock* blockBefore = (MemoryBlock*) 0;
	MemoryBlock* blockAfter = (MemoryBlock*) 0;
	MemoryBlock* head = freeBlocks->head();

	while (head) {

		if (head->ExactlyAfterMe(*block)) {
			DASSERT(!blockBefore);	// There can be only one before it.
			blockBefore = head;
		}
		else
		if (head->ExactlyBeforeMe(*block)) {
			DASSERT(!blockAfter);	// There can be only one after it.
			blockAfter = head;
		}

		head = (MemoryBlock*) head->getnext();
	}

	// Record address and size, and release the allocated
	// block.
	//
	void*		addr		= block->GetStart();
	util_ui32	fullSize	= block->GetSize();
	usedBlocks->release(block);
	allocatedUserMemory -= fullSize;

	// Now check if we have to create a new free block or
	// integrate it appropriately with the existing blocks
	// if memory is consecutive.
	//
	if (!blockBefore &&!blockAfter) {
		block = freeBlocks->allocate();
		block->Set(addr, fullSize);
	}
	else
	if (blockBefore && blockAfter) {

		// We integrate the three blocks in one.
		//
		blockBefore->GrowRight(fullSize);
		blockBefore->GrowRight(blockAfter->GetSize());
		freeBlocks->release(blockAfter);
	}
	else
	if (blockBefore)
		blockBefore->GrowRight(fullSize);
	else
	if (blockAfter)
		blockAfter->GrowLeft(fullSize);
#else
	free(mem);
#endif
}

/////////////////////////////////////////////////////////////

void DMemoryManager::VerifyAll (void) {

#ifdef	MALLOCFREE_SUBSTITUTE

	// Firstly verify all used blocks.
	//
	MemoryBlock* block = usedBlocks->head();

	while (block) {
		// Perform any test here?
		//
		block = (MemoryBlock*) block->getnext();
	}

	// Then verify all free blocks.
	//
	block = freeBlocks->head();

	while (block) {
		// Perform any memory painting test here?
		// Can have a byte flag (first byte) which can be:
		// 11110000 for globally unused blocks and 00001111 for re-cycled
		// blocks and have different memory painting style.
		//
		block = (MemoryBlock*) block->getnext();
	}

#endif

}

//-----------------------------------------------------------
// Overloading of default operators. We cannot overload those
// operators if we have a fully active debug library using the
// new memory manager.
//

#if		defined(DDEBUG_USE) && defined(DDEBUG_NEWDELETE)
#ifdef	MEMORYMANAGER_NEWDELETE
#undef	MEMORYMANAGER_NEWDELETE
#endif
#endif

/////////////////////////////////////////////////////////////

#ifdef	MEMORYMANAGER_NEWDELETE

void operator delete (void* p) {

#ifdef	MALLOCFREE_SUBSTITUTE
	if (!memoryManagerInitalised)
		DASSERT(false);
#endif

#ifdef	MEMORYMANAGER_SMALLOBJECTS
	if (!dsmallobjectflagraised(p)) 
		DSmallAllocatorSuper::GetAllocator(p)->Free(p);
	else
#endif
		DMemoryManager::ReleaseMemory(p);
}

/////////////////////////////////////////////////////////////

void operator delete[] (void* p) {

#ifdef	MALLOCFREE_SUBSTITUTE
	if (!memoryManagerInitalised)
		DASSERT(false);
#endif

#ifdef	MEMORYMANAGER_SMALLOBJECTS
	if (!dsmallobjectflagraised(p)) 
		DSmallAllocatorSuper::GetAllocator(p)->Free(p);
	else
#endif
		DMemoryManager::ReleaseMemory(p);
}

/////////////////////////////////////////////////////////////

void* operator new[] (size_t n) {

#ifdef	MALLOCFREE_SUBSTITUTE
	if (!memoryManagerInitalised) {
		memoryManagerInitalised = true;
		DMemoryManager::Initialise();
		DINITALLOCSTATS();
	}
#endif

#ifdef	MEMORYMANAGER_SMALLOBJECTS
	DSmallAllocatorSuper* alloc = DSmallAllocatorSuper::GetBest(n);

	if (alloc && alloc->HasMemory()) {
		DRECORDALLOCSIZE(n);
		return alloc->Allocate(n);
	}
	else
#endif
		return DMemoryManager::GetMemory(n);
}

/////////////////////////////////////////////////////////////

void* operator new (size_t n) {

#ifdef	MALLOCFREE_SUBSTITUTE
	if (!memoryManagerInitalised) {
		memoryManagerInitalised = true;
		DMemoryManager::Initialise();
		DINITALLOCSTATS();
	}
#endif

#ifdef	MEMORYMANAGER_SMALLOBJECTS
	DSmallAllocatorSuper* alloc = DSmallAllocatorSuper::GetBest(n);

	if (alloc && alloc->HasMemory()) {
		DRECORDALLOCSIZE(n);
		return alloc->Allocate(n);
	}
	else
#endif
		return DMemoryManager::GetMemory(n);
}

#endif	// MEMORYMANAGER_NEWDELETE

//-----------------------------------------------------------
