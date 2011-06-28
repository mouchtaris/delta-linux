// RcLibFuncNames.h
// Array of extern funcs from rc loader library.
// Include only at one place.
// A. Savidis, January 2000.
//

#ifndef	RCLIBFUNCNAMES_H
#define	RCLIBFUNCNAMES_H

#include "utypes.h"

static const char* rcLibFuncNames[] = {
	"rcload",
	(char*) 0
};

static UPTR(const char*) RcLib_FuncNames (void) { 
	return rcLibFuncNames;
}

#endif	// Do not add stuff beyond this point.