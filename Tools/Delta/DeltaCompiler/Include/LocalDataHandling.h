// LocalDataHandling.h
// Method added at February, 2005.
// A. Savidis.
//

#ifndef	LOCALDATAHANDLING_H
#define	LOCALDATAHANDLING_H

#include "utypes.h"
#include "Symbol.h"

namespace LocalDataHandler {

	extern void			SingletonCreate (void);
	extern void			SingletonDestroy (void);

	extern void			Initialise (void);
	extern void			CleanUp (void);

	extern void			OnGlobalBegin (void);
	extern util_ui16	OnGlobalEnd (void);

	extern void			OnBlockBegin (void);
	extern void			OnBlockEnd (void);

	extern void			OnFunctionBegin (void);
	extern void			OnFunctionEnd (void);	// Returns local data block size.

	extern void			OnNewLocalVar (DeltaSymbol* sym);
	extern util_ui16	GetCurrBlockId (void);	// Gets the unique (to curr function) block id.
}

#endif	// Do not add stuff beyond this point.