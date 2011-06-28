/* A simple C, error reporting function.
 * ScriptFighter.
 * A. Savidis, October 1999.
 */

#ifndef	DELTACOMPERRORMSG_H
#define	DELTACOMPERRORMSG_H

#include "utypes.h"
#include "DeltaCompilerDefs.h"

typedef void (*DeltaCompErrorCallback)(const char*);

DCOMPLIB_FUNC void						DeltaCompError (const char* format,...);
DCOMPLIB_FUNC void						DeltaCompWarning (const char* format,...);
DCOMPLIB_FUNC void						DeltaCompMsg (const char* format,...);
					
DCOMPLIB_FUNC void						DeltaCompResetErrors (void);
DCOMPLIB_FUNC void						DeltaCompResetWarnings (void);

DCOMPLIB_FUNC util_ui32					DeltaCompErrorsExist (void);
DCOMPLIB_FUNC util_ui32					DeltaCompWarningsExist (void);

DCOMPLIB_FUNC void						DeltaCompSetErrorCallback (void (*callback)(const char*));
DCOMPLIB_FUNC DeltaCompErrorCallback	DeltaCompGetErrorCallback (void);
DCOMPLIB_FUNC void						DeltaCompSetCurrentFile (const char* fname);

#endif	/* Do not add stuff beyond this point. */