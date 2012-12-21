// GlobalData.h
// ScriptFigher Project.
// A. Savidis, August 2005.
//

#ifndef	GLOBALDATA_H
#define	GLOBALDATA_H

#include "CompilerComponentDirectory.h"
#include "LocalDataHandling.h"
#include "ParseActions.h"

///////////////////////////////////////////////////////////

class GlobalData {
	USE_COMPILER_COMPONENT_DIRECTORY();
	public:
	void	Start (void) {
				LOCALDATA.OnGlobalBegin(); 
				TRANSLATOR.MakeReservedGlobalSymbols();
			}

	void	End (void) {
				PARSEPARMS.SetGlobalDataSize(
					LOCALDATA.OnGlobalEnd()
				);
			}
};

///////////////////////////////////////////////////////////

#define GLOBALDATA_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), GlobalData)))

#define GLOBALDATA	GLOBALDATA_EX(COMPONENT_DIRECTORY())

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
