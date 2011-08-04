// DeltaLocal.h
// Debug information for local variables.
// Notice that all declared variables in Delta, either
// in a function or outside are actually local variables.
// ScriptFighter Project.
// A. Savidis, Feburary 2005.
// Refactored August 2005, removed scope as it was redundant.
//

#ifndef	DELTALOCAL_H
#define	DELTALOCAL_H

#include <string.h>
#include <stdio.h>

#include "DeltaStdDefs.h"
#include "GenericReader.h"
#include "GenericWriter.h"
#include "DDebug.h"
#include "utypes.h"
#include <functional>

////////////////////////////////////////////////////////////////////////////

class DBYTECODE_CLASS DeltaDebugLocal {

	private:
	std::string	name;
	util_ui16	offset, line;

	public:
	bool		IsValid (void) const 
					{ return !name.empty() && line != 0; }

	bool		MatchesName (const char* s) const
					{ DASSERT(IsValid()); return name == s; }

	const std::string&	
				GetName (void) const 
					{ DASSERT(IsValid()); return name; }

	util_ui16	GetLine (void) const 
					{ DASSERT(IsValid()); return line; }

	bool		MatchesOffset (util_ui16 i) const // DBG
					{ DASSERT(IsValid()); return offset == i; }

	util_ui16	GetOffset (void)  const // DBG
					{ DASSERT(IsValid()); return offset; }

	void		WriteText (FILE* fp) const; // COMP
	bool		Read (GenericReader& reader); // DBG
	void		Write (GenericWriter& writer) const; // COMP

	util_ui32	SizeOf (void) const; // COMP
	void*		Serialize (void* buffer, util_ui32* size) const; // COMP

	UOVERLOADED_VOID_ASSIGN_VIA_COPY_CONSTRUCTOR(DeltaDebugLocal)

	DeltaDebugLocal (void) : 
		offset	(0),
		line	(0) {}

	DeltaDebugLocal (const char* _name, util_ui16 _offset, util_ui16 _line) :
		name	(DNULLCHECK(_name)),
		offset	(_offset),
		line	(_line){}

	DeltaDebugLocal (const DeltaDebugLocal& l) 
		{ DASSERT(l.IsValid()); new (this) DeltaDebugLocal(l.name.c_str(), l.offset, l.line); }
};

////////////////////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
