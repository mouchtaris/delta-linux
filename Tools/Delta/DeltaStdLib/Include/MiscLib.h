// MiscLib.h
// Mischellaneous library functions for the DeltaVirtualMachine.
// ScriptFighter Project.
// A. Savidis, November 1999.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#ifndef	MISCLIB_H
#define	MISCLIB_H

#include "DeltaLibraryObjectCreator.h"
#include <string>
#include <functional>

#define	DELTA_DLLIMPORT_INSTALL_NAME	"Install"
#define	DELTA_DLLIMPORT_CLEANUP_NAME	"CleanUp"

// The default print function uses console stdout. To override
// this behaviour, simply set your own print function, which
// will display the resulting string the way needed.
//
extern void 	DeltaSet_print_OutputFunc (void (*)(const char*));
extern void		DeltaAdd_dllimport_Path (const std::string& path, bool prioritised);
extern void 	Install_DeltaMisc_Lib (DeltaObject* mainAPI);
extern void 	CleanUp_DeltaMisc_Lib (void);

#endif	// Do not add stuff beyond this point.