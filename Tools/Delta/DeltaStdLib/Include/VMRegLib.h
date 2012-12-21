// VMRegLib.h
// Virtual machine management functions.
// ScriptFigher Project.
// A. Savidis, December 1999.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#ifndef	VMREGLIB_H
#define	VMREGLIB_H

#include "DeltaString.h"
#include "DeltaLibraryObjectCreator.h"

///////////////////////////////////////////////////////////////////////////

#define	DELTA_VM_USERDATA_EXTRA_FIELD_KEY	"extra"

DVM_FUNC void					DeltaVM_ToString (DeltaString* at, void* val);
DVM_FUNC void					DeltaVM_Make (DeltaValue* at, DeltaVirtualMachine* val);
DVM_FUNC DeltaVirtualMachine*	DeltaVM_Get (const DeltaValue& val);
extern void						DeltaVM_AddLoadingPath (const std::string& path, bool prioritised);
extern const std::string		DeltaVM_MakeLoadingPath (void);

extern void						Install_DeltaVM_Lib (DeltaObject* mainAPI);
extern void						CleanUp_DeltaVM_Lib (void);

///////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
