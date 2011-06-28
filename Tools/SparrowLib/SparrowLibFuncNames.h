/**
 *	SparrowLibFuncNames.h
 *
 *	-- Sparrow library functions for Delta --
 *
 *	Required header file defining library function
 *	names to the Delta compiler.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef	SPARROW_LIBRARY_FUNC_NAMES_H_
#define	SPARROW_LIBRARY_FUNC_NAMES_H_

#include "SparrowLibCommon.h"

#define SPW_NAMESPACE			"spw"
#define SPW_NAMESPACE_PREFIX	"spw::"

_SPARROW_LIB_C_API const char** LibraryFuncNames(void);

#endif	//SPARROW_LIBRARY_FUNC_NAMES_H_
