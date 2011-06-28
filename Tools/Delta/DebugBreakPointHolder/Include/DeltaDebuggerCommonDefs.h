// DeltaDebuggerCommonDefs.h
// A. Savidis, June 2009.
//

#ifndef	DELTADEBUGERCOMMONDEFS_H
#define	DELTADEBUGERCOMMONDEFS_H

#include "utypes.h"


#if defined(DVMDEBUGCOMMON_EXPORT) && defined(DVMDEBUGCOMMON_IMPORT)
#error "Can't define both DVMDEBUGCOMMON_EXPORT and DVMDEBUGCOMMON_IMPORT!"
#endif

#if	defined(DVMDEBUGCOMMON_EXPORT)
#define	DVMDEBUGCOMMON_CLASS	UDLL_EXPORT_CPP_CLASS
#define	DVMDEBUGCOMMON_FUNC		UDLL_EXPORT_CPP
#define	DVMDEBUGCOMMON_CFUNC	UDLL_EXPORT_C
#elif defined(DVMDEBUGCOMMON_IMPORT)
#define	DVMDEBUGCOMMON_CLASS	UDLL_IMPORT_CPP_CLASS
#define	DVMDEBUGCOMMON_FUNC		UDLL_IMPORT_CPP
#define	DVMDEBUGCOMMON_CFUNC	UDLL_IMPORT_C
#else
#define	DVMDEBUGCOMMON_CLASS
#define	DVMDEBUGCOMMON_FUNC		extern
#define	DVMDEBUGCOMMON_CFUNC	extern
#endif

#endif	// Do not add stuff beyond this point.

