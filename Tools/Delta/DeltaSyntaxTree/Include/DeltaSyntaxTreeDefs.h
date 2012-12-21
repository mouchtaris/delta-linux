// DeltaSyntaxTreeDefs.h
// Common defs for the delta syntax tree.
// Moved here from TreeNode.h.
// ScriptFighter Project.
// A. Savidis, August 2011, 
//

#ifndef	DELTASYNTAXTREEDEFS_H
#define	DELTASYNTAXTREEDEFS_H

#include "utypes.h"

#if defined(SYNTAXTREELIB_IMPORT) && defined(SYNTAXTREELIB_EXPORT)
#error "Can't define both SYNTAXTREELIB_EXPORT and SYNTAXTREELIB_IMPORT!"
#endif

#if	defined(SYNTAXTREELIB_EXPORT)
#define	SYNTAXTREELIB_CLASS	UDLL_EXPORT_CPP_CLASS
#define	SYNTAXTREELIB_FUNC	UDLL_EXPORT_CPP
#define	SYNTAXTREELIB_CFUNC	UDLL_EXPORT_C
#elif defined(SYNTAXTREELIB_IMPORT)
#define	SYNTAXTREELIB_CLASS	UDLL_IMPORT_CPP_CLASS
#define	SYNTAXTREELIB_FUNC	UDLL_IMPORT_CPP
#define	SYNTAXTREELIB_CFUNC	UDLL_IMPORT_C
#else
#define	SYNTAXTREELIB_CLASS
#define	SYNTAXTREELIB_FUNC	extern
#define	SYNTAXTREELIB_CFUN	extern
#endif

#endif	// Do not add stuff beyond this point.