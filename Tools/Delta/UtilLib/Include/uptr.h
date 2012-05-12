// uptr.h
// Pointer arithmetic helper macros.
// Moved from the central delta util here.
// ScriptFighter Project.
// A. Savidis, July 2002.
//

#ifndef	UPTR_H
#define	UPTR_H

#include <string.h>
#include <memory.h>
#include "utypes.h"

template <const unsigned> struct upointer2ui {};
template<> struct upointer2ui<2> { typedef util_ui32 type; };	// Intentionally 32 bits.
template<> struct upointer2ui<4> { typedef util_ui32 type; };
template<> struct upointer2ui<8> { typedef util_ui64 type; };

struct upointerbytes { enum { value = sizeof(void*) }; };
typedef upointer2ui<upointerbytes::value>::type util_uiptr;

template <class T>
bool uptrinside (const T* p, const T* left, const T* right) {	// [left, right], inclusive
	return	((util_uiptr) left)	<=	((util_uiptr) p) &&
			((util_uiptr) p)	<=	((util_uiptr) (right));
}

template <typename T> 
void uzeromemory (T* p) { memset(p, 0, sizeof(T)); }

template <class T, const util_ui32 N>
void uzeroarray (T (&P)[N]) { memset(&P, 0, sizeof(P)); }

template <typename T>
void umemcpy(T& dest, const T& src) { memcpy(&dest, &src, sizeof(T)); }

template <typename T>
void umemcpy(T* dest, const T* src) { memcpy(dest, src, sizeof(T)); }

template <typename T>
T* ucopybuffer (const T* buffer, util_ui32 n) {
		T* result = DNEWARR(T, n);
		memcpy(result, buffer, n * sizeof(T));
		return result;
}

//---------------------------------------------------------------

template <class T> void* POFFS (const void* p, T j) // Byte offset.
	{ return ((util_ui8*) p) + j; }

template <class T> void PINC (void*& p, T j) // Byte offset.
	{ p = POFFS(p, j); }

template <class T> void unullify (T*& p) 
	{ p = (T*) 0;}

inline util_ui32 PDIFF(const void* bigger, const void* smaller)
	{ return ((util_uiptr) bigger) - ((util_uiptr) smaller); }

inline util_uiptr PADDR(const void* p) 
	{ return (util_uiptr) p; }

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
