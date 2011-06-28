// DeltaCreationInfo.h
// carrying creatin information for objects and extern ids.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, July 2010.
//

#ifndef	DELTACREATIONINFO_H
#define	DELTACREATIONINFO_H

#include "DeltaStdDefs.h"
#include "ustrings.h"

//////////////////////////////////////////////////////////////////

#define	DELTA_CREATION_TYPE_INTERNAL	"internal"
#define	DELTA_CREATION_TYPE_FILE		"file"
#define	DELTA_CREATION_FILE_HIDDEN		""
#define	DELTA_CREATION_LINE_HIDDEN		0

class DeltaCreationInfo {

	protected:
	std::string			creatorFile;
	util_ui32			creatorLine;
	std::string			extraInfo;
	std::string			desc;

	void				MakeDesc (void) {
							if (creatorFile.empty())
								desc = DELTA_CREATION_TYPE_INTERNAL;
							else {
								if (creatorLine)
									desc = uconstructstr(DELTA_CREATION_TYPE_FILE " '%s', line %u", creatorFile.c_str(), creatorLine);
								else
									desc =  uconstructstr(DELTA_CREATION_TYPE_INTERNAL " '%s'", creatorFile.c_str());
								if (!extraInfo.empty())
									desc += std::string(", ") + extraInfo;
							}
						}

	public:
	const std::string	GetDesc (void) 
							const { DASSERT(!desc.empty()); return desc; }

	void				SetCreatorInformation (
							const std::string&	file, 
							util_ui32			line  = DELTA_CREATION_LINE_HIDDEN, 
							const std::string&	extra = ""
						) {
							creatorFile = file;
							creatorLine = line; 
							extraInfo	= extra;
							MakeDesc();
						}

	DeltaCreationInfo (void) : creatorLine(DELTA_CREATION_LINE_HIDDEN){ MakeDesc(); }
	~DeltaCreationInfo(){}
};

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
