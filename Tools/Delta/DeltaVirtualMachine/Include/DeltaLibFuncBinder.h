// DeltaLibFuncBinder.h
// This is the central library function manager shared by all
// virtual machines. Initially, prior to execution, all library
// functions are installed in the binder, while VMs may use
// the binder (either upon code loading or upon extern func
// execution) to translate a function name to a real code
// function pointer.
// ScriptFighter Project.
// A. Savidis, June 2001.
//

#ifndef	DELTALIBFUNCBINDER_H
#define	DELTALIBFUNCBINDER_H

#include "DeltaVirtualMachine.h"
#include "uvector.h"
#include <string>
#include <map>
#include <list>

#define	DELTA_LIBFUNC_INTERNAL_NAME			"<internal>"
#define	DELTA_LIBFUNC_INTERNAL_SERIALNO		0

///////////////////////////////////////////////////////////

class DVM_CLASS DeltaLibFuncBinder {

	private:
	typedef std::pair<DeltaLibraryFunc, util_ui32>	WithAddr;
	typedef std::pair<std::string, util_ui32>		WithName;

	typedef std::map<std::string, WithAddr>			ByName;
	typedef std::map<DeltaLibraryFunc, WithName>	ByAddr;
	typedef std::map<util_ui32, std::string>		BySerial;
	typedef	std::list<util_ui32>					SerialPool;

	// Serial numbers are introduced to uniquely refer to library
	// functions without exposing names or addresses in th call stack
	// of the debugger.
	//
	static	ByName*									byName;
	static	ByAddr*									byAddr;
	static	BySerial*								bySerial;
	static	SerialPool*								serialPool;
	static  util_ui32								totalFuncs;
	static	util_ui32								serialNo;

	// The following are functions that can be invoked as library functions
	// but are to entirely hidden from Delta code and can never be invoked as
	// library functions.
	//
	static ubag<DeltaLibraryFunc>*					internalFuncs;

	///////////////////////////////////////////////////////////

	public:
	
	static bool					IsSingletonCreated (void);
	static void					SingletonCreate (void);
	static void					SingletonDestroy (void);

	static void					Initialise (void);
	static void					CleanUp (void);

	static void					AddInternal (DeltaLibraryFunc addr);
	static void					RemoveInternal (DeltaLibraryFunc addr);
	static bool					IsInternal (DeltaLibraryFunc addr);

	static bool					Add (const std::string& name, DeltaLibraryFunc addr);
	static bool					Remove (const std::string& name, DeltaLibraryFunc addr);
	static DeltaLibraryFunc		Get (const std::string& name);

	static bool					IsValidAddress (DeltaLibraryFunc addr)
									{ return !!GetNameByAddress(addr) || IsInternal(addr); }

	static util_ui32			GetSerialNo (const std::string& name);
	static util_ui32			GetSerialNo (DeltaLibraryFunc addr);

	static const char*			GetName (util_ui32 serialNo);
	static const char*			GetNameByAddress (DeltaLibraryFunc addr);
};

///////////////////////////////////////////////////////////
#endif	// Do not add stuff beyond this point.