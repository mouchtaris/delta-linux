// DeltaBlockIdStacks.h
// Handling of block id stacks for each functions called by a VM.
// Script Fighter Project.
// A. Savidis, June 2007 (original).
// RF to separate class (July 2008).
// RF transferred from debugger to core VM (January 2010).
//

#ifndef	DELTABLOCKIDSTACKS_H
#define	DELTABLOCKIDSTACKS_H

#include "DeltaValue.h"
#include "utypes.h"
#include <list>
#include <stack>

/////////////////////////////////////////////////////////////////

class DVM_CLASS DeltaBlockIdStacks {

	public:
	typedef std::pair<util_ui16, util_ui16>	Block;		// <block id, total locals>
	typedef std::list<Block>				BlockList;	

	private:
	std::list<BlockList>			blockIdStacks;
	std::list<BlockList>::iterator	currBlockIdStack;
	bool							globalBlockExited;

	BlockList&			GetCurrBlockIdStack (void);
	BlockList&			GetGlobalBlockIdStack (void);
	BlockList&			GetTopBlockIdStack (void);
	bool				IsAtGlobalSpace (void) const;
	bool				IsValidAsNextBlock (const BlockList& blockIdStack, util_ui16 blockId) const;

	public:

	//******************************
	// Those are needed by the debugger. Visiting blocks
	// (moving up / down) has *no* effect on the use of blocks 
	// as part of the vm execution.

	void				AtPreviousBlockIdStack (void);
	void				AtNextBlockIdStack (void);
	void				AtBlockIdStack (util_ui32 index);
	void				AtTopBlockIdStack (void);
	const BlockList&	GetCurrBlockIdStack (void) const 
							{ return const_cast<DeltaBlockIdStacks*>(this)->GetCurrBlockIdStack(); }
	const BlockList&	GetGlobalBlockIdStack (void) const 
							{return const_cast<DeltaBlockIdStacks*>(this)->GetGlobalBlockIdStack(); }
	util_ui32			GetTotalBlockIdStacks (void) const
							{ return (util_ui32) blockIdStacks.size(); }
	util_ui32			GetTopBlockIdStackIndex (void) const
							{ return (util_ui32) blockIdStacks.size() - 1; }
	bool				IsNotAnActiveBlock (const BlockList& blockIdStack, util_ui16 blockId) const;

	//******************************
	// Execution support.

	bool				HasOnlyGlobalBlocks (void) const;
	void				PushBlockIdStack (void);
	void				PopBlockIdStack (void);
	void				EnterBlock (util_ui16 blockId, util_ui16 totalLocals);
	void				ExitBlock (util_ui16 blockId, util_ui16 totalLocals);
	util_ui16			ExitBlocksUntil (util_ui16 blockId);
	util_ui16			ExitMultipleBlocks (util_ui16 blockId);
	util_ui16			ExitAllBlocksOfCurrentContext (void);
	util_ui16			ExitAllBlocksOfCurrentCall (void);

	DeltaBlockIdStacks (void);
	~DeltaBlockIdStacks();
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
