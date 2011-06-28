// DeltaCompilerDefs.h
// Delta compiler API.
// ScriptFighter Project.
// A. Savidis, March 2009.
//

#ifndef	DELTACOMPILERDEFS_H
#define	DELTACOMPILERDEFS_H

#include "utypes.h"

#define	DELTA_COMPILER_ERROR_PREFIX			"Error"
#define	DELTA_COMPILER_WARNING_PREFIX		"Warning"
#define	DELTA_COMPILER_STARTED_PREFIX		"Started"
#define	DELTA_COMPILER_FINISHED_PREFIX		"Finished"
#define	DELTA_COMPILER_FAILED_PREFIX		"Failed"

//---------------------------------------------------------------

#if defined(DCOMPLIB_EXPORT) && defined(DCOMPLIB_IMPORT)
#error "Can't define both DCOMPLIB_EXPORT and DCOMPLIB_IMPORT!"
#endif

#if	defined(DCOMPLIB_EXPORT)
#define	DCOMPLIB_CLASS	UDLL_EXPORT_CPP_CLASS
#define	DCOMPLIB_FUNC	UDLL_EXPORT_CPP
#define	DCOMPLIB_CFUNC	UDLL_EXPORT_C
#elif defined(DCOMPLIB_IMPORT)
#define	DCOMPLIB_CLASS	UDLL_IMPORT_CPP_CLASS
#define	DCOMPLIB_FUNC	UDLL_IMPORT_CPP
#define	DCOMPLIB_CFUNC	UDLL_IMPORT_C
#else
#define	DCOMPLIB_CLASS
#define	DCOMPLIB_FUNC	extern
#define	DCOMPLIB_CFUN	extern
#endif

#endif	// Do not add stuff beyond this point.