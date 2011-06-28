// ListIterLib.h
// Garbage collectable list iterator type API for Delta, built
// via STL and integrated via the EGC mechanism.
// ScriptFighter Project.
// A. Savidis, June 2001.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#ifndef	LISTLIBITER_H
#define	LISTLIBITER_H

#include "DeltaLibraryObjectCreator.h"
#include "DeltaStdClassNames.h"

DVM_FUNC void	DeltaListIter_Make (DeltaValue* at);
extern	void	Install_DeltaListIter_Lib (DeltaObject* mainAPI);
extern	void	CleanUp_DeltaListIter_Lib (void);

#endif	// Do not add stuff beyond this point.