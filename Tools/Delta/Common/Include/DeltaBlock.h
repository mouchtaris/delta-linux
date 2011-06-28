// DeltaBlock.h
// Debug information for function blocks.
// ScriptFighter Project.
// A. Savidis, Feburary 2005.
//

#ifndef	DELTABLOCK_H
#define	DELTABLOCK_H

#include "utypes.h"
#include "DeltaLocal.h"
#include "GenericReader.h"
#include "GenericWriter.h"

#include <list>

///////////////////////////////////////////////////////'

class DBYTECODE_CLASS DeltaDebugBlock {

	private:
	util_ui16					blockId;
	std::list<DeltaDebugLocal>	locals;  // It is a list to preserve the apperarence order in the source.
	std::list<util_ui16>		offsets; // Produced after reading.

	void			ProduceOffsets (void);

	public:
	util_ui32		GetTotal (void) const 
						{ return (util_ui32) locals.size(); }

	const std::list<util_ui16>&
					GetOffsets (void) const // DBG
						{ return offsets; }

	void			Add (const DeltaDebugLocal& l); // COMP
	bool			Get (const std::string& name, util_ui16* offset) const;
	bool			Get (std::string* name, util_ui16 offset) const;

	const std::list<DeltaDebugLocal>&
					GetLocals (void) const { return locals; }

	util_ui16		GetBlockId (void) const 
						{ return blockId; }
	void			SetBlockId (util_ui16 i) // COMP
						 { blockId = i; }

	void			WriteText (FILE* fp) const; // COMP
	bool			Read (GenericReader& reader); // DBG
	void			Write (GenericWriter& writer) const; // COMP

	void			Clear (void) 
						{ locals.clear(); offsets.clear(); }

	//**********************************

	DeltaDebugBlock (void) : blockId(0){}
	~DeltaDebugBlock(){}
};

///////////////////////////////////////////////////////'

#endif	// Do not ad stuff beyond this point.
