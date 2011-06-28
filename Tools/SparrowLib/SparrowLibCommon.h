/**
 *	SparrowLibCommon.h
 *
 *	-- Sparrow IDE extension for Delta --
 *
 *	Common include file for the SparrowLib extension
 *	for Delta.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2007
 */
#ifndef SPARROW_LIB_COMMON_H_
#define SPARROW_LIB_COMMON_H_

#include "Common.h"
#include "LibraryAPIDecls.h"

///////////////////////////////////////////////////////////
// DEFINE DLL EXPORT //////////////////////////////////////

#ifdef SPARROWLIB_EXPORTS
#	define _SPARROW_LIB_API		UTIL_EXPORT
#	define _SPARROW_LIB_C_API	UTIL_C_EXPORT
#else
#	define _SPARROW_LIB_API		UTIL_IMPORT
#	define _SPARROW_LIB_C_API	UTIL_C_IMPORT
#endif

#endif	///< SPARROW_LIB_COMMON_H_
