/**
 *	UndefineAllWin32Shit.h
 *
 *	-- Utility Library --
 *
 *	December 2005
 */

// #ifndef UTIL_UNDEFINEALLWIN32SHIT_H // --> NO GUARDS PLEASE !!!
// #define UTIL_UNDEFINEALLWIN32SHIT_H

////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN // Be lean and mean but not win

#undef CreateDialog
#undef Yield
#undef GetClassInfo
#undef GetMessage
#undef DispatchMessage

////////////////////////////////////////////////////////////////////////

// #endif // UTIL_UNDEFINEALLWIN32SHIT_H
