/**
 *	LibraryAPIDecls.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef UTIL_LIBRARYAPIDECLS_H
#define UTIL_LIBRARYAPIDECLS_H

namespace util {

////////////////////////////////////////////////////////////////////////

#pragma warning (disable:4251)

#define UTIL_EXPORT		__declspec(dllexport)
#define UTIL_C_EXPORT	extern "C" __declspec(dllexport)
#define UTIL_IMPORT		__declspec(dllimport)
#define UTIL_C_IMPORT	extern "C" __declspec(dllimport)

////////////////////////////////////////////////////////////////////////
// Base API convenience defines
//
#ifdef BASELIBRARY_EXPORTS
#	define _BASE_API	UTIL_EXPORT
#	define _BASE_C_API	UTIL_C_EXPORT
#else
#	define _BASE_API	UTIL_IMPORT
#	define _BASE_C_API	UTIL_C_IMPORT
#endif

////////////////////////////////////////////////////////////////////////

} // namespace util

#endif // UTIL_LIBRARYAPIDECLS_H
