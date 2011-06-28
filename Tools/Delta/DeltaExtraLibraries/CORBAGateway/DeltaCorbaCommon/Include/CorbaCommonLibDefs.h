#ifndef	CORBA_COMMON_LIB_DEFS_H
#define	CORBA_COMMON_LIB_DEFS_H

#include "utypes.h"

#if defined(CORBACOMMON_EXPORT) && defined(CORBACOMMON_IMPORT)
#error "Can't define both CORBACOMMON_EXPORT and CORBACOMMON_IMPORT!"
#endif

#if	defined(CORBACOMMON_EXPORT)
#define	CORBACOMMON_CLASS	UDLL_EXPORT_CPP
#define	CORBACOMMON_FUNC	UDLL_EXPORT_CPP
#define	CORBACOMMON_CFUNC	UDLL_EXPORT_C
#elif defined(CORBA_CLIENT_LIB_IMPORT)
#define	CORBACOMMON_CLASS	UDLL_IMPORT_CPP
#define	CORBACOMMON_FUNC	UDLL_IMPORT_CPP
#define	CORBACOMMON_CFUNC	UDLL_IMPORT_C
#else
#define	CORBACOMMON_CLASS
#define	CORBACOMMON_FUNC	extern
#define	CORBACOMMON_CFUNC	extern
#endif

#endif	// Do not add stuff beyond this point.
