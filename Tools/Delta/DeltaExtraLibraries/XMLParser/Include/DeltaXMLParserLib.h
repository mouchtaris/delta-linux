// DeltaXMLParserLib.h
// XML parser library for Delta.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#ifndef	DELTAXMLPARSERLIB_H
#define	DELTAXMLPARSERLIB_H

#include "utypes.h"

#if defined(DXMLPARSER_EXPORT) && defined(DXMLPARSER_IMPORT)
#error "Can't define both DXMLPARSER_EXPORT and DXMLPARSER_IMPORT!"
#endif

#if	defined(DXMLPARSER_EXPORT)
#define	DXMLPARSER_CLASS	UDLL_EXPORT_CPP
#define	DXMLPARSER_FUNC		UDLL_EXPORT_CPP
#define	DXMLPARSER_CFUNC	UDLL_EXPORT_C
#elif defined(DXMLPARSER_IMPORT)
#define	DXMLPARSER_CLASS	UDLL_IMPORT_CPP
#define	DXMLPARSER_FUNC		UDLL_IMPORT_CPP
#define	DXMLPARSER_CFUNC	UDLL_IMPORT_C
#else
#define	DXMLPARSER_CLASS
#define	DXMLPARSER_FUNC extern
#define	DXMLPARSER_CFUNC extern "C"
#endif

// For normal build-time DLL deployment.
DXMLPARSER_CFUNC void 	Install_DeltaXMLParser_Lib (void);
DXMLPARSER_CFUNC void 	CleanUp_DeltaXMLParser_Lib (void);

#endif	// Do not add stuff beyond this point.