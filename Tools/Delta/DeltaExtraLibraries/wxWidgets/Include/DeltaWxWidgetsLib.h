// DeltaWxWidgetsLib.h
// Wx Widgets library for Delta.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#ifndef	DELTAWXWIDGETSLIB_H
#define	DELTAWXWIDGETSLIB_H

#include "utypes.h"

#if defined(DWXWIDGETS_EXPORT) && defined(DWXWIDGETS_IMPORT)
#error "Can't define both DWXWIDGETS_EXPORT and DWXWIDGETS_IMPORT!"
#endif

#if	defined(DWXWIDGETS_EXPORT)
#define	DWXWIDGETS_CLASS	UDLL_EXPORT_CPP
#define	DWXWIDGETS_FUNC		UDLL_EXPORT_CPP
#define	DWXWIDGETS_CFUNC	UDLL_EXPORT_C
#elif defined(DWXWIDGETS_IMPORT)
#define	DWXWIDGETS_CLASS	UDLL_IMPORT_CPP
#define	DWXWIDGETS_FUNC		UDLL_IMPORT_CPP
#define	DWXWIDGETS_CFUNC	UDLL_IMPORT_C
#else
#define	DWXWIDGETS_CLASS
#define	DWXWIDGETS_FUNC extern
#define	DWXWIDGETS_CFUNC extern
#endif

#endif	// Do not add stuff beyond this point.