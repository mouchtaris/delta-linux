/**
 *	DynamicMain.h
 *
 *	-- Utility Library --
 *	Header file defining a generic main for dynamically
 *	loading libraries.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007.
 */
#ifndef DYNAMIC_MAIN_H_
#define DYNAMIC_MAIN_H_

//-------------------------------------------------------//
//---- Dll Main -----------------------------------------//

#ifdef WIN32
#	define DECLARE_PLUGIN(INITIALIZE, CLEANUP)											\
	BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)	\
	{																					\
		switch (ul_reason_for_call)														\
		{																				\
			case DLL_PROCESS_ATTACH:													\
				return INITIALIZE;														\
			case DLL_THREAD_ATTACH:														\
			case DLL_THREAD_DETACH:														\
				break;																	\
			case DLL_PROCESS_DETACH:													\
				return CLEANUP;															\
		}																				\
		return TRUE;																	\
}

#	define DECLARE_PLUGIN_DEFAULT		\
	DECLARE_PLUGIN(true, true)

#endif //WIN32

#endif	//DYNAMIC_MAIN_H_
