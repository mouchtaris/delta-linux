/**
 *	DynamicLoader.h
 *
 *	-- Utility Library --
 *	Header file defining a mechanism for dynamically
 *	loading libraries.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007.
 */
#ifndef UTIL_DYNAMIC_LOADER_H_
#define UTIL_DYNAMIC_LOADER_H_

#include "Common.h"
#include "LibraryAPIDecls.h"

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN		///< Exclude rarely-used stuff from Windows headers
#	include <windows.h>
#endif	// WIN32

namespace util
{
	//----------------------------
	//-- class DynamicLoader

	class _BASE_API DynamicLoader
	{
	public:
		///--- constructors / destructor
		DynamicLoader(const std::string& dll);
		~DynamicLoader(void);

		bool Call(const std::string& function);

		template <class Rtype>
		const Rtype Call(const std::string& function) {
			typedef Rtype (*proc)(void);
			proc F = (proc)GetProcAddress(hDll, function.c_str());

			if (F == 0)
				return Rtype();

			return F();
		}



		operator bool (void) const	{ return hDll != 0; }

	private:
#ifdef WIN32
		HINSTANCE hDll;
#endif	//WIN32
	};
}

#endif	//UTIL_DYNAMIC_LOADER_H_