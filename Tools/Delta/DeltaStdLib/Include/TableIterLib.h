// TableIterLib.h
// Table iteration library.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#ifndef	TABLEITER_H
#define	TABLEITER_H

#include "DeltaLibraryObjectCreator.h"
#include "DeltaStdClassNames.h"

DVM_FUNC void	DeltaTableIter_Make (DeltaValue* at);
extern	void	Install_DeltaTableIter_Lib (DeltaObject* mainAPI);
extern	void	CleanUp_DeltaTableIter_Lib (void);

#endif	// Do not add stuff beyond this point.