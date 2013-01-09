// LocalDataHandling.cpp
// Management of local data. Uses a number of stacks
// to ensure the minimum required amount of local data
// is used by functions.
// Method added at February, 2005.
// A. Savidis.
// Repaired an error in giving offsets to block locals
// (they had to gain offsets always above their outer block
// locals, but they did not; done by keeping block tree and
// supplying offsets at the end of functions or global code).
// A. Savidis, August 2010.
//

#include "utypes.h"
#include "DDebug.h"
#include "LocalDataHandling.h"
#include "ParseActions.h"
#include "Symbol.h"
#include "ufunctors.h"
#include <list>
#include <algorithm>

#define	IS_MAIN_PROGRAM_BLOCK	true
#define	NOT_MAIN_PROGRAM_BLOCK	false

////////////////////////////////////////////////////////////////////

typedef std::list<DeltaSymbol*>		SymList;
typedef std::list<BlockInfo*>		BlockList;

class BlockInfo {
	private:
	SymList		symbols;
	BlockList	innerBlocks;
	BlockInfo*	parent;

	bool		In (const DeltaSymbol* sym) const {
					return	std::find(
								symbols.begin(),
								symbols.end(),
								sym
							) != symbols.end();
				}

	void		RemoveFromParent (void) {
					if (parent) {
						BlockList::iterator i =	std::find(
													DPTR(parent)->innerBlocks.begin(), 
													DPTR(parent)->innerBlocks.end(), 
													this
												);
						DASSERT(i != DPTR(parent)->innerBlocks.end());
						DPTR(parent)->innerBlocks.erase(i); 
						unullify(parent); 
					}
				}

	//******************************

	public:
	BlockInfo*	AddBlock (void) {
					BlockInfo* info = DNEWCLASS(BlockInfo, (this));
					innerBlocks.push_back(info);
					return info;
				}				
	
	static void	Cut (BlockInfo*& block) {
					DPTR(block)->RemoveFromParent();
					udelete(block);
				}

	BlockInfo*	GetParent (void)
					{ return parent; }

	BlockInfo*	GetTopParent (void) 
					{ return !parent? this : DPTR(parent)->GetTopParent(); }

	void		AddSymbol (DeltaSymbol* sym) 
					{ DASSERT(!In(sym)); symbols.push_back(sym); }
	
	void		GiveOffsetsToSymbols (util_ui32 start = 0) {
					for (SymList::iterator i = symbols.begin(); i != symbols.end(); ++i)
						DPTR(*i)->SetOffset(start++);
					for (BlockList::iterator i = innerBlocks.begin(); i != innerBlocks.end(); ++i)
						DPTR(*i)->GiveOffsetsToSymbols(start);
				}

	BlockInfo (void)				: parent((BlockInfo*)0){}
	BlockInfo (BlockInfo* _parent)	: parent(_parent){}

	~BlockInfo() {
		std::for_each(
			innerBlocks.begin(),
			innerBlocks.end(),
			uptrdestructorfunctor<BlockInfo*>()
		);
	}
};

//******************************

#define	ENTER_BLOCK()									\
	currBlock = DPTR(currBlock)->AddBlock()

#define	EXIT_BLOCK()									\
	currBlock = DPTR(currBlock)->GetParent()

#define	ENTER_FUNCTION()								\
	ENTER_BLOCK()

#define	EXIT_FUNCTION()									\
	if (true) {											\
		DPTR(currBlock)->GiveOffsetsToSymbols();		\
		BlockInfo* del = currBlock;						\
		EXIT_BLOCK();									\
		BlockInfo::Cut(del);							\
	} else

#define	ENTER_MAIN_BLOCK()								\
	if (true) {											\
		DASSERT(!currBlock);							\
		currBlock = DNEW(BlockInfo);					\
	} else

#define	EXIT_MAIN_BLOCK()								\
	EXIT_FUNCTION()

////////////////////////////////////////////////////////////////////

template <class T> T top_and_pop (std::list<T>* s) {
	DASSERT(!DPTR(s)->empty());
	T v = DPTR(s)->front();
	DPTR(s)->pop_front();
	return v;
}

////////////////////////////////////////////////////////////////////

void LocalDataHandler::Initialise (void) {
	currBlockId	= DELTA_MAIN_BLOCK_VALUE;
	blockLocals = 0;
	unullify(currBlock);
}

//******************************

void LocalDataHandler::CleanUp (void) {
	DPTR(blockIdStack)->clear();
	DPTR(blockLocalsStack)->clear();
	DPTR(blockBeginStack)->clear();
	if (currBlock) 
		{ DDELETE(currBlock->GetTopParent()); unullify(currBlock); }
}

////////////////////////////////////////////////////////////////////

void LocalDataHandler::EmitBlockBegin (util_ui16 blockId, bool isMainProgramBlock) {
	
	DPTR(blockLocalsStack)->push_front(blockLocals);
	blockLocals = 0;

	DPTR(blockBeginStack)->push_front(QUADS.NextQuadNo());

	QUADS.Emit(
		DeltaIC_BLOCKENTER,
		NIL_EXPR,	// Block locals
		TRANSLATOR.Translate_ConstValue(isMainProgramBlock),
		TRANSLATOR.Translate_ConstValue((DeltaNumberValueType) blockId)
	);
}

//******************************

void LocalDataHandler::EmitBlockEnd (util_ui16 blockId, bool isMainProgramBlock) {

	DeltaExpr* totalLocals		= TRANSLATOR.Translate_ConstValue((DeltaNumberValueType) blockLocals);
	blockLocals					= top_and_pop(blockLocalsStack);
	DeltaQuadAddress beginQuad	= top_and_pop(blockBeginStack);

	DeltaQuad& quad = QUADS.GetQuad(beginQuad);
	DASSERT(!quad.GetArg1() && quad.GetArg2());
	DASSERT(quad.GetResult()->numConst == blockId);
	DASSERT(quad.GetArg2()->boolConst == isMainProgramBlock);

	quad.SetArg1(totalLocals);

	QUADS.Emit(
		DeltaIC_BLOCKEXIT,
		totalLocals,
		quad.GetArg2(),			
		quad.GetResult()
	);
}

////////////////////////////////////////////////////////////////////

void LocalDataHandler::OnBlockBegin (void) { 

	ENTER_BLOCK();
	DPTR(blockIdStack)->push_front(currBlockId);

	if (DeltaSymbol* f = PARSEPARMS.CurrFunction())
		currBlockId = DPTR(f)->totalBlocks++;
	else
		currBlockId = PARSEPARMS.IncGlobalBlocks();	// Returns previous value.	

	EmitBlockBegin(currBlockId, NOT_MAIN_PROGRAM_BLOCK);
}

////////////////////////////////////////////////////////////////////

void LocalDataHandler::OnBlockEnd (void) { 
	EXIT_BLOCK();
	EmitBlockEnd(currBlockId, NOT_MAIN_PROGRAM_BLOCK);
	currBlockId = top_and_pop(blockIdStack);
}

////////////////////////////////////////////////////////////////////

void LocalDataHandler::OnFunctionBegin (void) {
	ENTER_FUNCTION();
	DPTR(blockIdStack)->push_front(currBlockId);
	currBlockId = DELTA_MAIN_BLOCK_VALUE;
}

////////////////////////////////////////////////////////////////////

void LocalDataHandler::OnFunctionEnd (void) {
	EXIT_FUNCTION();
	currBlockId = top_and_pop(blockIdStack);
}

////////////////////////////////////////////////////////////////////

void LocalDataHandler::OnGlobalBegin (void) {
	ENTER_MAIN_BLOCK();
	EmitBlockBegin(
		currBlockId = PARSEPARMS.IncGlobalBlocks(), 
		IS_MAIN_PROGRAM_BLOCK
	);
}

util_ui16 LocalDataHandler::OnGlobalEnd (void) {

	EXIT_MAIN_BLOCK();

	// We always include the total static vars as part of global data.

	util_ui32 n = (blockLocals += DELTASYMBOLS.GetTotalStaticVars());
	DASSERT(currBlockId == DELTA_MAIN_BLOCK_VALUE);

	EmitBlockEnd(currBlockId, IS_MAIN_PROGRAM_BLOCK);

	return n;
}

////////////////////////////////////////////////////////////////////

void LocalDataHandler::OnNewLocalVar (DeltaSymbol* sym) { 
	++blockLocals;
	DPTR(currBlock)->AddSymbol(sym);
}

////////////////////////////////////////////////////////////////////

util_ui16 LocalDataHandler::GetCurrBlockId (void) 
	{ return currBlockId; }

////////////////////////////////////////////////////////////////////

LocalDataHandler::LocalDataHandler (ucomponentdirectory* directory) :
	ucomponentdirectoryclient	(directory),
	currBlock					((BlockInfo*) 0),
	currBlockId					(DELTA_MAIN_BLOCK_VALUE),
	blockLocals					(0),	// Num of locals in current block (only in block scope).
	blockIdStack				((std::list<util_ui16>*) 0),
	blockLocalsStack			((std::list<util_ui16>*) 0),
	blockBeginStack				((std::list<DeltaQuadAddress>*) 0)
{
	unew(blockIdStack);
	unew(blockLocalsStack);
	unew(blockBeginStack);
}

//******************************

LocalDataHandler::~LocalDataHandler (void) {
	udelete(blockIdStack);
	udelete(blockLocalsStack);
	udelete(blockBeginStack);
}

////////////////////////////////////////////////////////////////////
