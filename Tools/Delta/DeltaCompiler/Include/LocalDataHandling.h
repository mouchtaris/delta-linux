// LocalDataHandling.h
// Method added at February, 2005.
// A. Savidis.
//

#ifndef	LOCALDATAHANDLING_H
#define	LOCALDATAHANDLING_H

#include "CompilerComponentDirectory.h"
#include "utypes.h"
#include "Symbol.h"
#include "InterCode.h"

class BlockInfo;

//////////////////////////////////////////////////////

class LocalDataHandler {

	USE_COMPILER_COMPONENT_DIRECTORY();

private:
	BlockInfo*						currBlock;
	util_ui16						currBlockId;
	util_ui16						blockLocals;
	std::list<util_ui16>*			blockIdStack;
	std::list<util_ui16>*			blockLocalsStack;
	std::list<DeltaQuadAddress>*	blockBeginStack;

	void EmitBlockBegin (util_ui16 blockId, bool isMainProgramBlock);
	void EmitBlockEnd	(util_ui16 blockId, bool isMainProgramBlock);

public:

	void		Initialise (void);
	void		CleanUp (void);

	void		OnGlobalBegin (void);
	util_ui16	OnGlobalEnd (void);

	void		OnBlockBegin (void);
	void		OnBlockEnd (void);

	void		OnFunctionBegin (void);
	void		OnFunctionEnd (void);	// Returns local data block size.

	void		OnNewLocalVar (DeltaSymbol* sym);
	util_ui16	GetCurrBlockId (void);	// Gets the unique (to curr function) block id.

	LocalDataHandler();
	~LocalDataHandler();
};

//////////////////////////////////////////////////////

#define LOCALDATA_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), LocalDataHandler)))

#define LOCALDATA	LOCALDATA_EX(COMPONENT_DIRECTORY())

//////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.