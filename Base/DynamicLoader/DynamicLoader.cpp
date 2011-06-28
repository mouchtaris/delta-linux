/**
 *	DynamicLoader.cpp
 *
 *	-- Utility Library --
 *	Header file defining a mechanism for dynamically
 *	loading libraries.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007.
 */
#include "DynamicLoader.h"
#include "Streams.h"

namespace util
{
	//-------------------------------------------------------//
	//---- class DynamicLoader ------------------------------//

	DynamicLoader::DynamicLoader(const std::string& name)
	{
		//-- Load Dynamicly Linked Library
#ifdef WIN32
		hDll = LoadLibraryA(name.c_str());
		if (!hDll) {
			DWORD err = GetLastError();
			TRACEOUT << "DLL: '" << name << "' is missing: [" << err << "]" << std::endl;
		}
#endif	//WIN32
	}

	//-----------------------------------------------------------------------

	DynamicLoader::~DynamicLoader(void)
	{
		//-- Release Dynamic Library
#ifdef WIN32
		FreeLibrary(hDll);
#endif	//WIN32
	}

	//-----------------------------------------------------------------------

	bool DynamicLoader::Call(const std::string& function)
	{
		typedef void (*proc)(void);
		proc F = (proc)GetProcAddress(hDll, function.c_str());

		if (F == 0)
			return false;

		F();
		return true;
	}

	//-----------------------------------------------------------------------
}
