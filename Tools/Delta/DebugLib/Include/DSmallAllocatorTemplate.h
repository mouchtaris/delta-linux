// DSmallAllocatorTemplate.h
// The basic template for the small allocator.
// A. Savidis. February 2003.
//

#ifndef	DSMALLALLOCATORTEMPLATE_H
#define	DSMALLALLOCATORTEMPLATE_H

#include "DAssert.h"
#include "DSmallObjectAllocator.h"
#include <cstring>

//--------------------------------------------------------------------

enum DSmallBlockSize  {

	DBlock16	=	16,
	DBlock32	=	32,
	DBlock64	=	64,
	DBlock128	=	128
};

enum DBlockSizeExp  {

	DBlock16Exp		=	4,
	DBlock32Exp		=	5,
	DBlock64Exp		=	6,
	DBlock128Exp	=	7
};

enum DSmallMaxBlocks  {

	DBlock16Max		=	0x1fff,	// 8K blocks of 16 bytes
	DBlock32Max		=	0x1ff,	// 512 blocks of 32 bytes
	DBlock64Max		=	0xff,	// 256 blocks of 64 bytes
	DBlock128Max	=	0xff	// 256 blocks of 128 bytes
};

//--------------------------------------------------------------------

template <
	enum DSmallBlockSize	_BlockSize,		// Always be a power of 2.
	const DBlockSizeExp		_Exp,			// 2^_Exp should give block size.
	const util_ui16			_MaxBlocks		// Max blocks pre-allocated for this alocator.
	> class DSmallObjectAllocator : public DSmallAllocatorSuper {

	private:
	void*			memStart;
	void*			memEnd;
	util_ui16		totalFreeBlocks;
	util_ui32		totalSize;
	util_ui16		firstFreeBlock;
	util_ui8		shiftBits;

	//////////////////////////////////////////////////////////////////

	void*	GetBlockAddr (util_ui16 blockNo) 
				{ return POFFS(memStart, (blockNo << shiftBits)); }

	public:

	// In free, the userMem already points at the size byte,
	// while the small allocator bit is not raised. All these are
	// done by the caller anyway, in testing that the small allocator
	// is to be used.
	//
	void		Free (void* userMem) {

					// 1. Check that size matches allocator size.
					//
					util_ui32 size = *((util_ui32*) userMem);			
					DASSERT(GetBest(size) == this);
				
					// 2. Check that block fits correctly inside the allocated memory.
					// That means userMem > memStart, and userMem < memEnd, and
					// userMem - memStart perfectly dividable by block size.
					//		
					DASSERT( ((util_ui32) userMem) >= ((util_ui32) memStart) );
					DASSERT( ((util_ui32) userMem) <  ((util_ui32) memEnd) );
					util_ui32 byteDistance = PDIFF(userMem, memStart);
					DASSERT( (byteDistance & (shiftBits - 1)) == 0);
				
					// 3. Now put this in front of free block list.
					// The order of the freed block is byteDistance / block size,
					// taken again quickly via a shift.
					//
					*((util_ui32*) userMem) = firstFreeBlock;
					firstFreeBlock = byteDistance >> shiftBits;	// First free block is the freed block.
					++totalFreeBlocks;
				}
	
	//////////////////////////////////////////////////////////////////

	void*		Allocate (util_ui32 size) {

					if (!totalFreeBlocks) {

						// In this case, the index of the first block happens to be
						// the total blocks.
						//
						DASSERT(firstFreeBlock == _MaxBlocks);
						return (void*) 0;
					}
					else {

						// Get the user memory address for the first free block.
						//
						void* userMem = GetBlockAddr(firstFreeBlock);

						// Get the next block pointed by the first block.
						//
						util_ui32 nextBlock = 	*((util_ui32*) userMem);
						*((util_ui32*) userMem) = size;
						*((util_ui32*) userMem) |= DDEBUG_ALLOCATOR_SIZE_MASK;

						// Re-position the first free block, and decrease
						// number of free blocks.
						//
						firstFreeBlock = nextBlock;
						--totalFreeBlocks;

						// Return the actually accessible user memory.
						//
						return POFFS(userMem, DDEBUG_USERSIZE_BYTES);
					}
				}

	//////////////////////////////////////////////////////////////////

	bool		HasMemory (void) 
					{ return totalFreeBlocks > 0; }

	//////////////////////////////////////////////////////////////////

	DSmallObjectAllocator (void) {

		totalFreeBlocks = (util_ui16) _MaxBlocks;
		shiftBits		= (util_ui8) _Exp;
		memStart		= malloc(totalSize = totalFreeBlocks << shiftBits);
		memEnd			= POFFS(memStart, totalSize);
		firstFreeBlock	= 0;

		void*		currBlock	= memStart;
		util_ui32	blockNo		= 0;

		// Iterate through all blocks, and mark the first four bytes
		// with the index of the next free block.
		//
		while (blockNo < _MaxBlocks) {
			
			++blockNo;
			memcpy(currBlock, &blockNo, DDEBUG_USERSIZE_BYTES);
			PINC(currBlock, _BlockSize);
		}
	}

	~DSmallObjectAllocator(){ free(memStart); }
};

//--------------------------------------------------------------------

#endif	// Do not add stuff beyond this point.