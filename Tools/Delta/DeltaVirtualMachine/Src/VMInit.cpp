// VMInit.cpp
// Global virtual machine initialisation of singletons.
// A. Savidis, February 2003.
//

#include "VMInit.h"
#include "DeltaVMRegistry.h"
#include "DeltaValueValidator.h"
#include "DeltaCollectableContainer.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaObjectValidator.h"
#include "ContainerDelayedCollector.h"
#include "DeltaExternIdFactory.h"
#include "DeltaExternId.h"
#include "DeltaArgumentsRecycler.h"
#include "DeltaTableFactory.h"
#include "DeltaExceptionHandling.h"
#include "DeltaClosureHandling.h"
#include "DeltaString.h"
#include "OperatorOverloading.h"
#include "DeltaDebugDynamicActivator.h"
#include "DeltaProfiler.h"

namespace VirtualMachinePackage {

DVM_FUNC void Initialise (void) {

	CollectableContainer::Initialise();
	DeltaLibFuncBinder::SingletonCreate();
	DeltaVMRegistry::SingletonCreate();
	DeltaValueValidator::SingletonCreate();
	DeltaObjectValidator::SingletonCreate();
	ContainerDelayedCollector::SingletonCreate();
	DeltaExternIdFactory::SingletonCreate();
	DeltaExternId::SingletonCreate();
	DeltaValueFactory::SingletonCreate();
	DeltaTableFactory::SingletonCreate();
	DeltaExceptionHandling::SingletonCreate();
	DeltaClosureHandling::SingletonCreate();
	DeltaVirtualMachine::Initialise();
	DeltaString::SingletonCreate();
	OperatorOverloading::SingletonCreate();
	DeltaDebugDynamicActivator::SingletonCreate();
	DeltaProfiler::SingletonCreate();
}

DVM_FUNC void CleanUp (void) {

	DeltaLibFuncBinder::SingletonDestroy();

	// Those may cause GC so we have to enter delayed
	// collection and then commit on exiting.

	DELTA_COLLECTOR_ENTER();
		DeltaVMRegistry::SingletonDestroy();
		DeltaVirtualMachine::CleanUp();
		OperatorOverloading::SingletonDestroy();
		DeltaValueFactory::GetSingleton().UnUseAll();
		DeltaValueFactory::SingletonDestroy();
		DeltaTableFactory::CollectAllUncollected();
		DeltaExternIdFactory::SingletonDestroy();
		DeltaExceptionHandling::SingletonDestroy();
		DeltaClosureHandling::SingletonDestroy();
	DELTA_COLLECTOR_EXIT();

	DASSERT(!DeltaTableFactory::GetSingleton().GetTotalUsed()); 
	DeltaTableFactory::SingletonDestroy();	// Just cleans-up memory.
	
	CollectableContainer::CleanUp();
	ContainerDelayedCollector::SingletonDestroy();
	DeltaObjectValidator::SingletonDestroy();
	DeltaValueValidator::SingletonDestroy();
	DeltaExternId::SingletonDestroy();
	DeltaString::SingletonDestroy();
	DeltaDebugDynamicActivator::SingletonDestroy();
	DeltaProfiler::SingletonDestroy();

}
}