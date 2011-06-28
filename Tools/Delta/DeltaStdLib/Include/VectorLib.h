// ListLib.h
// Garbage collectable vector type API for delta, built
// via STL and integrated via the EGC mechanism.
// ScriptFighter Project.
// A. Savidis, June 2001.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#ifndef	VECTORLIB_H
#define	VECTORLIB_H

#include "DeltaLibraryObjectCreator.h"
#include "DeltaCollectableContainer.h"
#include "DeltaStdClassNames.h"
#include "uvector.h"

DVM_FUNC uvector<DeltaValue>*	DeltaVector_Get (void* val);
DVM_FUNC CollectableContainer*	DeltaVector_Make (DeltaValue& val);

extern void Install_DeltaVector_Lib (DeltaObject* mainAPI);
extern void CleanUp_DeltaVector_Lib (void);

#define	DEFORMAT_VECTORBOUNDS	\
			"Vector index '%d' out of vector bounds in '%s'"
#define	DEFORMAT_VECTOREMPTY	\
			"Vector empty in '%s'"

#endif	// Do not add stuff beyond this point.