// DeltaVars.h
// A class serving as a placeholder for local variables in a specific
// context, e.g. global or for a single function.
// Follows the style of treating all variables as locals.
// ScriptFighter Project.
// A. Savidis, August 2005.
//

#ifndef	DELTAVARS_H
#define	DELTAVARS_H

#include "DeltaLocal.h"
#include "DeltaBlock.h"
#include "GenericReader.h"
#include "GenericWriter.h"
#include "utypes.h"

class DBYTECODE_CLASS DeltaVars {

	private:
	util_ui16				totalBlocks;
	DeltaDebugBlock*		blocks;

	public:
	void					SetTotalBlocks (util_ui16 _totalBlocks);
	util_ui16				GetTotalBlocks (void) const
								{ return totalBlocks; }

	const DeltaDebugBlock&	GetBlock (util_ui16 blockId) const;

	void					AddVar (const char* name, util_ui16 blockId, util_ui16 offset, util_ui16 line);
	bool					GetVar (const std::string& name, util_ui16 blockId, util_ui16* offset) const;
	bool					GetVar (std::string* name, util_ui16 blockId, util_ui16 offset) const;

	void					WriteText (FILE* fp) const; // COMP
	bool					Read (GenericReader& reader); // DBG
	void					Write (GenericWriter& writer) const; // COMP

	void					Clear (void);

	DeltaVars (void);
	~DeltaVars();
};

#endif	// Do not ad stuff beyond this point.
