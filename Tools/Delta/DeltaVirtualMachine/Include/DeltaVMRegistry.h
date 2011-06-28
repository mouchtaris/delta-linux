// DeltaVMRegistry.h
// Registration service for virtual machines created
// within the same program. This will allow multiple
// virtual machines to be concurrently open and running,
// while calls to global functions defined within other
// VMs can be made.
// ScriptFighter Project.
// A. Savidis, december 1999.
//

#ifndef	DELTAVMREGISTRY_H
#define	DELTAVMREGISTRY_H

#include "DeltaByteCodeTypes.h"
#include "uthreadlib.h"
#include <string>
#include <map>

//--------------------------------------------------------------------
// This class is a singleton. A static instance will be created.
// Hence, avoid to instantiate virtual machines within the phase
// of static data initialisation.
//
class DVM_CLASS DeltaVirtualMachine;

class DVM_CLASS DeltaVMRegistry {

	public:
	typedef std::list<DeltaVirtualMachine*>	VMList;

	private:
	DFRIENDDESTRUCTOR()
	static DeltaVMRegistry* singletonPtr;

	umutex										registryMutex;
	std::map<std::string, DeltaVirtualMachine*> vms;
	VMList										ptrs;
	
	void CleanUp (void);

	DeltaVMRegistry (void){}
	~DeltaVMRegistry () { CleanUp(); }

	///////////////////////////////////////////////////////////////////////

	public:
	void					Register (const char* id, DeltaVirtualMachine* vm);
	void					Cancel (const char* id);
	DeltaVirtualMachine*	Get (const char* id);
	bool					Exists (DeltaVirtualMachine* vm);

	util_ui16				Total (void) const 
								{ return (util_ui16) vms.size(); }
	DeltaVirtualMachine*	Get (util_ui16 order);
	const VMList&			GetAll (void) const 
								{ return ptrs; }

	static void				SingletonCreate (void);
	static void				SingletonDestroy (void);

	static DeltaVMRegistry*	GetPtr (void);
};

inline DeltaVMRegistry& VMRegistry (void) 
	{ return *DeltaVMRegistry::GetPtr(); }

//--------------------------------------------------------------------

#endif	// Do not add stuff beyond this point.