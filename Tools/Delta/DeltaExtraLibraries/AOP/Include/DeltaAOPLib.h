// DeltaAOPLib.h
// AOP library for Delta.
// ScriptFighter Project.
// Y. Lilis, August 2013.
//

#ifndef	DELTAAOPLIB_H
#define	DELTAAOPLIB_H

#include "utypes.h"

#if defined(DAOPLIB_EXPORT) && defined(DAOPLIB_IMPORT)
#error "Can't define both DAOPLIB_EXPORT and DAOPLIB_IMPORT!"
#endif

#if	defined(DAOPLIB_EXPORT)
#define	DAOPLIB_CLASS	UDLL_EXPORT_CPP
#define	DAOPLIB_FUNC	UDLL_EXPORT_CPP
#define	DAOPLIB_CFUNC	UDLL_EXPORT_C
#elif defined(DAOPLIB_IMPORT)
#define	DAOPLIB_CLASS	UDLL_IMPORT_CPP
#define	DAOPLIB_FUNC	UDLL_IMPORT_CPP
#define	DAOPLIB_CFUNC	UDLL_IMPORT_C
#else
#define	DAOPLIB_CLASS
#define	DAOPLIB_FUNC extern
#define	DAOPLIB_CFUNC extern "C"
#endif

#endif	// Do not add stuff beyond this point.