// DMemoryManager.cpp
// Defensive programming library.
// Memory manager implementation.
// A. Savidis, November 2002.
// TODO: Remove all validation tests and trasfer to DDEBUG.
// This will remain as a pure memory manager.
//
//

#include <malloc.h>
#include <memory.h>
#include <new.h>

#include "utypes.h"
#include "uptr.h"
#include "uarraydoublelist.h"
#include "dmemorymanager.h"
#include "dsmallobjectallocator.h"
#include "dallocationstats.h"
#include "ddebug.h"

// TODO: Add MEMORYMANAGER_DEFENSE macro for defensive 
// decoration of memory blocks.
//
//	#define	MEMORYMANAGER_NEWDELETE
#define	MALLOCFREE_SUBSTITUTE
//	#define	DRECORD_ALLOCATIONSTATS
//	#define	MEMORYMANAGER_SMALLOBJECTS

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
// The allocation block image is the following:
// |HEADER|MEMORYBLOCKINFO|USER SIZE|USER MEMORY|TRAILER|
//
class MemoryBlockInfo {

	private:
	util_ui32		userSize;
	util_ui32		realSize;
	void*			userAddr;
	void*			realAddr;

#ifdef	MALLOCFREE_SUBSTITUTE
	void*			memoryBlock;	// Memory block pointer in allocated blocks.
#endif

	public:
	static bool				CheckByte (util_ui8* addr, util_ui32 size, util_ui8 byte);

	static bool				CheckMemEqual (util_ui8* mem1, util_ui8* mem, util_ui32 n);

	void*					GetUserAddr (void)
								{ return userAddr; }

	util_ui32				GetUserSize (void)
								{ return userSize; }

	void*					GetRealAddr (void)
								{ return realAddr; }

	util_ui32				GetRealSize (void)
								{ return realSize; }

#ifdef	MALLOCFREE_SUBSTITUTE
	void*					GetMemoryBlock (void)
								{ return memoryBlock; }
#endif

	bool					IsValid (void);

	static MemoryBlockInfo*	GetInfo (void* userPtr)
								{ return (MemoryBlockInfo*) POFFS(userPtr, -((util_i32) sizeof(::MemoryBlockInfo))); }

#ifdef	MALLOCFREE_SUBSTITUTE
	MemoryBlockInfo (void* realAddr, util_ui32 realSize, void* memoryBlock);
#else
	MemoryBlockInfo (void* realAddr, util_ui32 realSize);
#endif

	~MemoryBlockInfo();
};

// Extra bytes overloading user-memory for memory management, and
// bug defense purposes.
//
#define	DMEMORY_EXTRA_SIZE	\
			(sizeof(::MemoryBlockInfo) + DMEMORY_USERSIZE_BYTES + DMEMORY_HEADER_SIZE + DMEMORY_TRAILER_SIZE)

// Offset from the beginning of real-memory to get memory block address.
//
#define	DMEMORY_BLOCK_OFFS \
			(DMEMORY_HEADER_SIZE)

// Offset from the beginning of user-memory to get memory block address.
//
#define	DMEMORY_BLOCK_FROMUSERMEM_OFFS \
			-((util_i16) (sizeof(::MemoryBlockInfo) + DMEMORY_USERSIZE_BYTES))

// Offset from the beginning of real memory to get user-memory address.
//
#define	DMEMORY_USERADDR_OFFS	\
			(sizeof(::MemoryBlockInfo) + DMEMORY_BLOCK_OFFS + DMEMORY_USERSIZE_BYTES)

// Offset from the beginning of real-memory to get user-size address.
//
#define	DMEMORY_USERSIZE_OFFS	\
			(sizeof(::MemoryBlockInfo) + DMEMORY_BLOCK_OFFS)

// Offset from the beginning of user-memory to get user-size address.
//
#define	DMEMORY_USERSIZE_FROMUSERMEM_OFFS	\
			(-DMEMORY_USERSIZE_BYTES)

/////////////////////////////////////////////////////////////

#ifdef	MALLOCFREE_SUBSTITUTE
MemoryBlockInfo::MemoryBlockInfo (void* _realAddr, util_ui32 _realSize, void* _memoryBlock) {
#else
MemoryBlockInfo::MemoryBlockInfo (void* _realAddr, util_ui32 _realSize) {
#endif

	// Check that the block is correctly positioned in memory.
	//
	DASSERT(((void*) this) == POFFS(_realAddr, DMEMORY_BLOCK_OFFS));
	DASSERT(_realSize > DMEMORY_EXTRA_SIZE);

	// Initialise the memory block.
	//
#ifdef	MALLOCFREE_SUBSTITUTE
	memoryBlock	= _memoryBlock;
#endif

	realAddr	= _realAddr;
	userAddr	= POFFS(realAddr, DMEMORY_USERADDR_OFFS);
	realSize	= _realSize;
	userSize	= realSize - ((util_i32) DMEMORY_EXTRA_SIZE);

	// Extract user size and assert it is the same as the
	// calculated user size.
	//
	util_ui32 storedUserSize;
	memcpy(
		&storedUserSize, 
		POFFS(realAddr, DMEMORY_USERSIZE_OFFS), 
		DMEMORY_USERSIZE_BYTES
	);
	DASSERT(storedUserSize == userSize);

	// Now set the header, trailer, and wipe out
	// user memory.
	//
	memset(realAddr, DMEMORY_HEADER_BYTE, DMEMORY_HEADER_SIZE);
	memset(POFFS(userAddr, userSize), DMEMORY_TRAILER_BYTE, DMEMORY_TRAILER_SIZE);
	memset(userAddr, DMEMORY_ALLOC_BYTE, userSize);
}

/////////////////////////////////////////////////////////////
// Ther destructor wipes out completely the memory occupied for
// user memory, header, trailer, and memory block information.
//
MemoryBlockInfo::~MemoryBlockInfo() {
	memset(realAddr, DMEMORY_FREED_BYTE, realSize);
}

/////////////////////////////////////////////////////////////
// Checking that a memory block information is valid (i.e.
// no corruption).
//
inline bool MemoryBlockInfo::IsValid (void) {

	return	(realSize == userSize + DMEMORY_EXTRA_SIZE)								&&
			(userAddr == POFFS(realAddr, DMEMORY_USERADDR_OFFS))					&&
			( ((void*) this) == POFFS(userAddr, DMEMORY_BLOCK_FROMUSERMEM_OFFS))	&&

			CheckByte(
				(util_ui8*) realAddr, 
				DMEMORY_HEADER_SIZE, 
				DMEMORY_HEADER_BYTE
			) &&

			CheckByte(
				(util_ui8*) POFFS(userAddr, userSize), 
				DMEMORY_TRAILER_SIZE, 
				DMEMORY_TRAILER_BYTE
			)	&&

			CheckMemEqual(
				(util_ui8*) POFFS(userAddr, -DMEMORY_USERSIZE_BYTES), 
				(util_ui8*) &userSize,
				DMEMORY_USERSIZE_BYTES
			);
}

/////////////////////////////////////////////////////////////

bool MemoryBlockInfo::CheckByte (util_ui8* addr, util_ui32 size, util_ui8 byte) {

	while (size--)
		if (*addr++ != byte)
			return false;

	return true;
}

bool MemoryBlockInfo::CheckMemEqual (util_ui8* mem1, util_ui8* mem2, util_ui32 n) {

	while (n--)
		if (*mem1++ != *mem2++)
			return false;

	return true;
}

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
	static MemoryBlock* GetBlock (void* addr) 
				{ return (MemoryBlock*) POFFS(addr, -sizeof(MemoryBlock*)); }

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

	return;	// IGNORED

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

#ifdef	MEMBLOCKINFO
	util_ui32 neededSize = size + DMEMORY_EXTRA_SIZE;
#else
#ifdef	MALLOCFREE_SUBSTITUTE
	util_ui32 neededSize = size + sizeof(MemoryBlock*);
#else
	util_ui32 neededSize = size;
#endif
#endif

#ifdef	MALLOCFREE_SUBSTITUTE

	// If the requested memory exceeds the current heap size,
	// this is a normal not enough memory case.
	//
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
#else
	void* addr = malloc(neededSize);
#endif

#ifdef	MEMBLOCKINFO

	// Write the original requested user-memory size in the preceeding,
	// to user-memory, bytes.
	//
	memcpy(
		POFFS(addr, DMEMORY_USERSIZE_OFFS),
		&size,
		DMEMORY_USERSIZE_BYTES
	);

	// Then make the check block, and return the user address.
	//
	void* blockInfoAddr = POFFS(addr, DMEMORY_BLOCK_OFFS);

#ifdef	MALLOCFREE_SUBSTITUTE
	MemoryBlockInfo* blockInfo = new(blockInfoAddr) MemoryBlockInfo(addr, neededSize, newBlock);
#else
	MemoryBlockInfo* blockInfo = new(blockInfoAddr) MemoryBlockInfo(addr, neededSize);
#endif

	return blockInfo->GetUserAddr();
#else
	memcpy(addr, &newBlock, sizeof(MemoryBlock*));
	return POFFS(addr, sizeof(MemoryBlock*));
#endif
}

/////////////////////////////////////////////////////////////

void DMemoryManager::ReleaseMemory (void* mem) {

#ifdef	MEMBLOCKINFO
	util_ui32 userSize;
	memcpy(
		&userSize,
		POFFS(mem, DMEMORY_USERSIZE_FROMUSERMEM_OFFS),
		DMEMORY_USERSIZE_BYTES
	);

	// Get block information from user memory, check block validity,
	// and then get allocation block.
	//
	MemoryBlockInfo* blockInfo = (MemoryBlockInfo*) POFFS(mem, DMEMORY_BLOCK_FROMUSERMEM_OFFS);

	DASSERT(blockInfo->IsValid());
	DASSERT(blockInfo->GetUserSize()==userSize);
	MemoryBlock* block = (MemoryBlock*) blockInfo->GetMemoryBlock();

	// Now perform some extra checks to ensure matching between
	// block information and the allocated bock itself.
	//
	DASSERT(block->GetStart()==blockInfo->GetRealAddr());
	DASSERT(block->GetSize()==blockInfo->GetRealSize());

	// Now wipe out block info, and memory. 
	//
	blockInfo->~MemoryBlockInfo();
#else
#ifdef	MALLOCFREE_SUBSTITUTE
	MemoryBlock* block = MemoryBlock::GetBlock(mem);
#endif
#endif

#ifdef	MALLOCFREE_SUBSTITUTE

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
	free(blockInfo->GetRealAddr());
#endif
}

/////////////////////////////////////////////////////////////

bool DMemoryManager::IsValid (void* mem) {

	MemoryBlockInfo* blockInfo = (MemoryBlockInfo*) POFFS(mem, DMEMORY_BLOCK_FROMUSERMEM_OFFS);
	return blockInfo->IsValid();
}

void DMemoryManager::VerifyAll (void) {

#ifdef	MALLOCFREE_SUBSTITUTE

	// Firstly verify all used blocks.
	//
	MemoryBlock* block = usedBlocks->head();

	while (block) {

		MemoryBlockInfo* blockInfo = (MemoryBlockInfo*) POFFS(block->GetStart(), DMEMORY_BLOCK_OFFS);
		DASSERT(blockInfo->IsValid());

		block = (MemoryBlock*) block->getnext();
	}

	// Then verify all free blocks.
	//
	block = freeBlocks->head();

	while (block) {

		DASSERT(
			MemoryBlockInfo::CheckByte(
				(util_ui8*) block->GetStart(),
				block->GetSize(),
				DMEMORY_FREED_BYTE
			)
		);

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

	if (!dsmallobjectfreed(p)) 
		DMemoryManager::ReleaseMemory(p);
}

/////////////////////////////////////////////////////////////

void operator delete[] (void* p) {

#ifdef	MALLOCFREE_SUBSTITUTE
	if (!memoryManagerInitalised)
		DASSERT(false);
#endif

	if (!dsmallobjectfreed(p)) 
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
