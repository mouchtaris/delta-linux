// uhash.cpp
// Implementation of hash utility functions.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#include "DDebug.h"
#include "uhash.h"

//------------------------------------------------------------------

UTILLIB_FUNC util_ui32 uhash (const std::string& s, util_ui32 hashSize) {

  	util_ui32 h = 0;

  	for (const char* p = s.c_str(); *p; ++p) {

		h = (h << 4) + *p;
		util_ui32 g = h & 0xf0000000;

		if (g)
			h = (h ^ (g >> 24)) ^ g;
  	}
 
	return h % hashSize;
}

//------------------------------------------------------------------
