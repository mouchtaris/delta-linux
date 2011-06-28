// RcLoaderDefs.h
// Mainly defs for DLL creation.
// A. Savidis, July 2008.
//

#ifndef	RCLOADERDEFS_H
#define	RCLOADERDEFS_H

#include "utypes.h"

#if defined(RCLOAD_EXPORT) && defined(RCLOAD_IMPORT)
#error "Can't define both RCLOAD_EXPORT and RCLOAD_IMPORT!"
#endif

#if	defined(RCLOAD_EXPORT)
#define	RCLOAD_CLASS	UDLL_EXPORT_CPP_CLASS
#define	RCLOAD_FUNC	UDLL_EXPORT_CPP
#define	RCLOAD_CFUNC	UDLL_EXPORT_C
#elif defined(RCLOAD_IMPORT)
#define	RCLOAD_CLASS	UDLL_IMPORT_CPP_CLASS
#define	RCLOAD_FUNC	UDLL_IMPORT_CPP
#define	RCLOAD_CFUNC	UDLL_IMPORT_C
#else
#define	RCLOAD_CLASS
#define	RCLOAD_FUNC extern
#define	RCLOAD_CFUNC extern
#endif

#endif	// Do not add stuff beyond this point.