// GlobalData.h
// ScriptFigher Project.
// A. Savidis, August 2005.
//

#ifndef	GLOBALDATA_H
#define	GLOBALDATA_H

#include "LocalDataHandling.h"
#include "ParseParms.h"

class GlobalData {
	
	public:
	static void			Start (void) {
							extern void ParseActions_MakeReservedGlobalSymbols (void);
							LocalDataHandler::OnGlobalBegin(); 
							ParseActions_MakeReservedGlobalSymbols();
						}

	static void			End (void) {
							ParseParms::SetGlobalDataSize(
								LocalDataHandler::OnGlobalEnd()
							);
						}
};

#endif	// Do not add stuff beyond this point.

