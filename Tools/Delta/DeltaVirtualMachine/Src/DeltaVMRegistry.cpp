// VMRegistry.cpp
// Registration service for virtual machines created
// within the same program.
// ScriptFighter Project.
// A. Savidis, December 1999.
// Updated December 2005, removing validation facilities,
// while using the ThreadLib mutex wrapper.
// Updated November 2006, cleaning-up the registry requires
// exlpicit deletion of remaining VMs.
// Refactored July 2007, substituted the old fashioned hash
// table with STL map.
//

#include <string.h>
#include "DDebug.h"
#include "DeltaVMRegistry.h"
#include "utypes.h"
#include "DeltaRuntimeError.h"
#include "uthreadlib.h"
#include "DeltaVirtualMachine.h"

//-----------------------------------------------------------------
// When we link to a debugger, the debug thread accesses
// in parallel with the run-machine the VM registry.
//
DeltaVMRegistry* DeltaVMRegistry::singletonPtr = (DeltaVMRegistry*) 0;

void DeltaVMRegistry::SingletonCreate (void) {
	DASSERT(!singletonPtr);
	singletonPtr = DNEW(DeltaVMRegistry);
}

void DeltaVMRegistry::SingletonDestroy (void) {
	DASSERT(singletonPtr);
	DDELETE(singletonPtr);
	singletonPtr = (DeltaVMRegistry*) 0;
}

DeltaVMRegistry* DeltaVMRegistry::GetPtr (void) 
	{ DASSERT(singletonPtr); return DPTR(singletonPtr); }

//-----------------------------------------------------------------

void DeltaVMRegistry::CleanUp (void) {
	
	DASSERT(singletonPtr);

	registryMutex.WaitToLock();
	std::map<std::string, DeltaVirtualMachine*> copied_vms = vms;
	registryMutex.UnLock();

	std::for_each(
		copied_vms.begin(),
		copied_vms.end(),
		udestroy_second<std::pair<std::string, DeltaVirtualMachine*> >()
	);

	registryMutex.WaitToLock();
	DASSERT(vms.empty());
	registryMutex.UnLock();
}

//-----------------------------------------------------------------

void DeltaVMRegistry::Register (const char* id, DeltaVirtualMachine* vm) {
	
	DASSERT(singletonPtr);
	registryMutex.WaitToLock();

	DASSERT(vms.find(id) == vms.end());
	vms[id] = vm;
	ptrs.push_back(vm);

	registryMutex.UnLock();
}

//-----------------------------------------------------------------

void DeltaVMRegistry::Cancel (const char* id) {

	DASSERT(singletonPtr);
	registryMutex.WaitToLock();

	std::map<std::string, DeltaVirtualMachine*>::iterator i = vms.find(id);
	DASSERT(i != vms.end());
	
	std::list<DeltaVirtualMachine*>::iterator j = std::find(ptrs.begin(), ptrs.end(), i->second);
	DASSERT(j != ptrs.end());
	ptrs.erase(j);

	vms.erase(i);
	registryMutex.UnLock();
}

bool DeltaVMRegistry::Exists (DeltaVirtualMachine* vm) 
	{ return std::find(ptrs.begin(), ptrs.end(), vm) != ptrs.end(); }

//-----------------------------------------------------------------

DeltaVirtualMachine* DeltaVMRegistry::Get (const char* id) {
	
	DASSERT(singletonPtr);

	registryMutex.WaitToLock();

	std::map<std::string, DeltaVirtualMachine*>::iterator i = vms.find(id);
	DeltaVirtualMachine* vm = i == vms.end() ? ( DeltaVirtualMachine*) 0 : i->second;

	registryMutex.UnLock();

	return vm;
}

//-----------------------------------------------------------------

DeltaVirtualMachine* DeltaVMRegistry::Get (util_ui16 order) {
	
	DASSERT(order < Total());

	std::map<std::string, DeltaVirtualMachine*>::iterator i;

	for (i = vms.begin(); order; --order, ++i)
		DASSERT(i != vms.end());

	return i->second;
}

//-----------------------------------------------------------------
