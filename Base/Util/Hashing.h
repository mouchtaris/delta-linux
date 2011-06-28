/**
 *	Hashing.h
 *
 *	-- Utility Library --
 *	Header file with hash functions.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007.
 */
#ifndef UTIL_HASING_H_
#define UTIL_HASING_H_

#include "Common.h"

namespace util
{
	//----------------------------
	//-- function SuperFastHash

	uint32 SuperFastHash(const char* data, int len);
}

#endif	//UTIL_HASING_H_
