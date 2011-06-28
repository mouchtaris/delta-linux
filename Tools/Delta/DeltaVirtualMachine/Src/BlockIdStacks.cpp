// DeltaBlockIdStacks.cpp
// Handling of block id stacks for each functions called by a VM.
// Script Fighter Project.
// A. Savidis, June 2007 (original).
// RF to separate class (July 2008).
// RF transferred from debugger to core VM (January 2010).
//

#include "BlockIdStacks.h"

/////////////////////////////////////////////////////////////////

DeltaBlockIdStacks::DeltaBlockIdStacks (void) {
	currBlockIdStack	= blockIdStacks.end();
	globalBlockExited	= false;
}

DeltaBlockIdStacks::~DeltaBlockIdStacks() {}

/////////////////////////////////////////////////////////////////

DeltaBlockIdStacks::BlockList& DeltaBlockIdStacks::GetTopBlockIdStack (void)
	{ DASSERT(blockIdStacks.size() >= DELTA_BLOCKIDSTACKS_RESERVED); return blockIdStacks.back(); }

/////////////////////////////////////////////////////////////////
// We push / pop even empty blocks (0 locals) to guarantee
// continuity on the numbering of block ids.

void DeltaBlockIdStacks::EnterBlock (util_ui16 blockId, util_ui16 totalLocals) {
	BlockList& blocks = GetTopBlockIdStack();
	DASSERT(IsValidAsNextBlock(blocks, blockId));
	blocks.push_front(Block(blockId, totalLocals));
}

void DeltaBlockIdStacks::ExitBlock (util_ui16 blockId, util_ui16 totalLocals) {
	BlockList& blocks = GetTopBlockIdStack();
	DASSERT(!blocks.empty());

	Block block = blocks.front();
	DASSERT(block.first == blockId && block.second == totalLocals);

	blocks.pop_front();
	DASSERT(IsValidAsNextBlock(blocks, block.first));
}

/////////////////////////////////////////////////////////////////

util_ui16 DeltaBlockIdStacks::ExitBlocksUntil (util_ui16 blockId) {
	
	BlockList& blocks		= GetTopBlockIdStack();
	util_ui16 totalLocals	= 0;

	while (true) {
		DASSERT(!blocks.empty());
		const Block& block = blocks.front();
		if (blockId == block.first)
			return totalLocals;
		else {
			util_ui16 currBlockId = block.first;
			DASSERT(blockId < currBlockId);

			totalLocals += block.second;
			blocks.pop_front();

			DASSERT(IsValidAsNextBlock(blocks, currBlockId));
		}
	}
}

/////////////////////////////////////////////////////////////////

util_ui16 DeltaBlockIdStacks::ExitMultipleBlocks (util_ui16 n) {

	BlockList& blocks		= GetTopBlockIdStack();
	util_ui16 totalLocals	= 0;

	while (n--) {
		DASSERT(!blocks.empty());
		totalLocals += blocks.front().second;
		blocks.pop_front();
	}

	return totalLocals;
}

/////////////////////////////////////////////////////////////////

util_ui16 DeltaBlockIdStacks::ExitAllBlocksOfCurrentContext (void) {
	if (IsAtGlobalSpace()) {	// We never pop the main global block.

		BlockList& blocks		= GetTopBlockIdStack();
		util_ui16 totalLocals	= 0;

		while (blocks.size() > DELTA_BLOCKIDSTACKS_RESERVED) 
			{ totalLocals += blocks.front().second; blocks.pop_front(); }

		DASSERT(blocks.front().first == DELTA_MAIN_BLOCK_VALUE);
		return totalLocals;
	}
	else
		return ExitAllBlocksOfCurrentCall();
}

/////////////////////////////////////////////////////////////////

util_ui16 DeltaBlockIdStacks::ExitAllBlocksOfCurrentCall (void) {

	DASSERT(!IsAtGlobalSpace());

	BlockList& blocks		= GetTopBlockIdStack();
	util_ui16 totalLocals	= 0;

	while (!blocks.empty())
		{ totalLocals += blocks.front().second; blocks.pop_front(); }
	return totalLocals;
}

/////////////////////////////////////////////////////////////////

bool DeltaBlockIdStacks::HasOnlyGlobalBlocks (void) const {
	DASSERT(!blockIdStacks.empty());
	return blockIdStacks.size() ==  DELTA_BLOCKIDSTACKS_RESERVED;
}

void DeltaBlockIdStacks::PushBlockIdStack (void) {
	blockIdStacks.push_back(BlockList()); 
	AtTopBlockIdStack();
}

/////////////////////////////////////////////////////////////////

void DeltaBlockIdStacks::PopBlockIdStack (void) {

	DASSERT(blockIdStacks.size() > DELTA_BLOCKIDSTACKS_RESERVED);

	if (blockIdStacks.size() == DELTA_BLOCKIDSTACKS_RESERVED) { // Trying to pop global block.
		DASSERT(!globalBlockExited); // Retain it, but only once request to pop it possible.
		globalBlockExited = true;
	}
	else
		blockIdStacks.pop_back();

	AtTopBlockIdStack();
}

/////////////////////////////////////////////////////////////////

void DeltaBlockIdStacks::AtTopBlockIdStack (void) {
	DASSERT(blockIdStacks.size() >= DELTA_BLOCKIDSTACKS_RESERVED);
	currBlockIdStack = blockIdStacks.end();
	--currBlockIdStack;
	DASSERT(&*currBlockIdStack == &blockIdStacks.back()); // Should be the last element.
}

//*****************************

void DeltaBlockIdStacks::AtPreviousBlockIdStack (void) {
	DASSERT(blockIdStacks.size() >= DELTA_BLOCKIDSTACKS_RESERVED);
	DASSERT(currBlockIdStack != blockIdStacks.begin());
	--currBlockIdStack;
}

//*****************************

void DeltaBlockIdStacks::AtBlockIdStack (util_ui32 index) {

	DASSERT(blockIdStacks.size() >= DELTA_BLOCKIDSTACKS_RESERVED);
	DASSERT(blockIdStacks.size() > index);

	for (currBlockIdStack = blockIdStacks.begin(); index; --index, ++currBlockIdStack)
		DASSERT(currBlockIdStack != blockIdStacks.end());
}

/////////////////////////////////////////////////////////////////

void DeltaBlockIdStacks::AtNextBlockIdStack (void) {
	
	DASSERT(blockIdStacks.size() >= DELTA_BLOCKIDSTACKS_RESERVED);
	DASSERT(currBlockIdStack != blockIdStacks.end());
	DASSERT(&*currBlockIdStack != &blockIdStacks.back()); // Not last element.

	++currBlockIdStack;

	DASSERT(currBlockIdStack != blockIdStacks.end());
}

/////////////////////////////////////////////////////////////////

DeltaBlockIdStacks::BlockList& DeltaBlockIdStacks::GetCurrBlockIdStack (void) {
	
	DASSERT(blockIdStacks.size() >= DELTA_BLOCKIDSTACKS_RESERVED);
	DASSERT(currBlockIdStack != blockIdStacks.end());
	
	return *currBlockIdStack;
}

/////////////////////////////////////////////////////////////////

DeltaBlockIdStacks::BlockList& DeltaBlockIdStacks::GetGlobalBlockIdStack (void) {
	DASSERT(blockIdStacks.size() >= DELTA_BLOCKIDSTACKS_RESERVED);
	return blockIdStacks.front();
}

/////////////////////////////////////////////////////////////////
// If the current block id stack is the global one.
//
bool DeltaBlockIdStacks::IsAtGlobalSpace (void) const 
	{ return &GetCurrBlockIdStack() == &GetGlobalBlockIdStack(); }

/////////////////////////////////////////////////////////////////
// Cond 1: When empty, next is always the main block.
// Cond 2: Pushed (next) blocks have higher order than those below.
// Cond 3: If next and current blocks have the same id they are the main block.
//
bool DeltaBlockIdStacks::IsValidAsNextBlock (const BlockList& blockIdStack, util_ui16 blockId) const {
	return	(blockIdStack.empty() && blockId == DELTA_MAIN_BLOCK_VALUE) ||	
			(blockIdStack.front().first < blockId)						||	
			(blockIdStack.front().first == DELTA_MAIN_BLOCK_VALUE && blockId == DELTA_MAIN_BLOCK_VALUE);
}

bool DeltaBlockIdStacks::IsNotAnActiveBlock (const BlockList& blockIdStack, util_ui16 blockId) const{
	DASSERT(!blockIdStack.empty());
	return	blockId != DELTA_IGNORE_BLOCKID_VALUE &&	// If we don't care of the block it is assumed active.
			blockId > blockIdStack.front().first;		// It has higher order that the next block
}

/////////////////////////////////////////////////////////////////
