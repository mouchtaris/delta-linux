// DeltaJsonParserLib.h
// Handling export and import dll  
// for the entire jsonParser.dll.
// Giannhs Apostolidhs, january 2013.
//

#ifndef	DELTAJSONPARSERLIB_H
#define	DELTAJSONPARSERLIB_H

#include "utypes.h"

#if defined(DJSONPARSER_EXPORT) && defined(DJSONPARSER_IMPORT)
#error "Can't define both DJSONPARSER_EXPORT and DJSONPARSER_IMPORT!"
#endif

#if	defined(DJSONPARSER_EXPORT)
#define	DJSONPARSER_CLASS	UDLL_EXPORT_CPP
#define	DJSONPARSER_FUNC	UDLL_EXPORT_CPP
#define	DJSONPARSER_CFUNC	UDLL_EXPORT_C
#elif defined(DJSONPARSER_IMPORT)
#define	DJSONPARSER_CLASS	UDLL_IMPORT_CPP
#define	DJSONPARSER_FUNC	UDLL_IMPORT_CPP
#define	DJSONPARSER_CFUNC	UDLL_IMPORT_C
#else
#define	DJSONPARSER_CLASS
#define	DJSONPARSER_FUNC extern
#define	DJSONPARSER_CFUNC extern "C"
#endif

// For normal build-time DLL deployment.
DJSONPARSER_CFUNC void 	Install_DeltaJSONParser_Lib (void);
DJSONPARSER_CFUNC void 	CleanUp_DeltaJSONParser_Lib (void);

#endif	// Do not add stuff beyond this point.