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

class GlobalData : public ucomponentdirectoryclient {
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

	GlobalData (ucomponentdirectory* directory) : ucomponentdirectoryclient(directory) {}
	~GlobalData() {}
};

///////////////////////////////////////////////////////////

#define GLOBALDATA_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), GlobalData)))

#define GLOBALDATA	GLOBALDATA_EX(GET_COMPONENT_DIRECTORY())

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
