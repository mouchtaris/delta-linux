// SocketLibDefs.h
// Common defs for DLL exporting mainly.
// A. Savidis, July 2008.
//

#ifndef	SOCKETLIBDEFS_H
#define	SOCKETLIBDEFS_H

#include "utypes.h"

//------------------------------------------------------

#if defined(SOCKETLIB_EXPORT) && defined(SOCKETLIB_IMPORT)
#error "Can't define both SOCKETLIB_EXPORT and SOCKETLIB_IMPORT!"
#endif

#if	defined(SOCKETLIB_EXPORT)
#define	SOCKETLIB_CLASS	UDLL_EXPORT_CPP_CLASS
#define	SOCKETLIB_FUNC	UDLL_EXPORT_CPP
#define	SOCKETLIB_CFUNC	UDLL_EXPORT_C
#elif defined(SOCKETLIB_IMPORT)
#define	SOCKETLIB_CLASS	UDLL_IMPORT_CPP_CLASS
#define	SOCKETLIB_FUNC	UDLL_IMPORT_CPP
#define	SOCKETLIB_CFUNC	UDLL_IMPORT_C
#else
#define	SOCKETLIB_CLASS
#define	SOCKETLIB_FUNC extern
#define	SOCKETLIB_CFUNC extern
#endif

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.
