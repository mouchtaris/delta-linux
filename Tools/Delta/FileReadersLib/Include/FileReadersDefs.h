// FileReadersDefs.h
// Common defs for DLL exporting mainly.
// A. Savidis, July 2008.
//

#ifndef	FILEREADERSDEFS_H
#define	FILEREADERSDEFS_H

#include "utypes.h"

//------------------------------------------------------

#if defined(READERS_EXPORT) && defined(READERS_IMPORT)
#error "Can't define both READERS_EXPORT and READERS_IMPORT!"
#endif

#if	defined(READERS_EXPORT)
#define	READERS_CLASS	UDLL_EXPORT_CPP_CLASS
#define	READERS_FUNC	UDLL_EXPORT_CPP
#define	READERS_CFUNC	UDLL_EXPORT_C
#elif defined(READERS_IMPORT)
#define	READERS_CLASS	UDLL_IMPORT_CPP_CLASS
#define	READERS_FUNC	UDLL_IMPORT_CPP
#define	READERS_CFUNC	UDLL_IMPORT_C
#else
#define	READERS_CLASS
#define	READERS_FUNC extern
#define	READERS_CFUNC extern
#endif

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.
