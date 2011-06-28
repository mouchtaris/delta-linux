/**
 *	IDECommon.h
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#ifndef IDE_COMMON_H_
#define IDE_COMMON_H_

#include "LibraryAPIDecls.h"

///////////////////////////////////////////////////////////
// DEFINE DLL EXPORT //////////////////////////////////////

#ifdef IDECORE_EXPORTS
#	define _IDE_API		UTIL_EXPORT
#	define _IDE_C_API	UTIL_C_EXPORT
#else
#	define _IDE_API		UTIL_IMPORT
#	define _IDE_C_API	UTIL_C_IMPORT
#endif

#endif	//IDE_COMMON_H_
