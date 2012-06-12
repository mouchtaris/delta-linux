// DAssert.h
// Just the assert debug version.
// Defensive programming library.
// A. Savidis,
// Updated February 2003.
//

#ifndef	DASSERT_H
#define	DASSERT_H

#include "utypes.h"
#include <string>

	#define	DDEBUG_STRIP_OFF_ASSERT

//------------------------------------------------------

#if defined(DDEBUG_EXPORT) && defined(DDEBUG_IMPORT)
#error "Can't define both DDEBUG_EXPORT and DDEBUG_IMPORT!"
#endif

#if	defined(DDEBUG_EXPORT)
#define	DDEBUG_CLASS	UDLL_EXPORT_CPP_CLASS
#define	DDEBUG_FUNC		UDLL_EXPORT_CPP
#define	DDEBUG_CFUNC	UDLL_EXPORT_C
#elif defined(DDEBUG_IMPORT)
#define	DDEBUG_CLASS	UDLL_IMPORT_CPP_CLASS
#define	DDEBUG_FUNC		UDLL_IMPORT_CPP
#define	DDEBUG_CFUNC	UDLL_IMPORT_C
#else
#define	DDEBUG_CLASS
#define	DDEBUG_FUNC extern
#define	DDEBUG_CFUNC extern
#endif

//------------------------------------------------------

#ifdef	_DEBUG
#define	DDEBUG_USE
#endif

#ifdef	DDEBUG_USE

#define	DASSERT(cond)	\
    _DASSERT(cond, "File:" __FILE__ ",expr:" #cond)

DDEBUG_FUNC void dassert (bool cond, const char* expr);

template <class T> void _DASSERT (T cond, const char* expr)
	{ dassert(!!cond, expr); }

#else	// !DDEBUG_USE

#ifdef	DDEBUG_STRIP_OFF_ASSERT
// Strip-off for maximum speed.
#define	DASSERT
#else	
// Use the default assertion mechanism.
#include <assert.h>
#define	DASSERT	assert
#endif

#endif

//------------------------------------------------------

#endif	// Do not add stuff beyond this point.