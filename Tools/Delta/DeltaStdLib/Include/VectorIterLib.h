// VectorIterLib.h
// Garbage collectable vector iterator type API for Delta, built
// via STL and integrated via the EGC mechanism.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#ifndef	VECTORLIBITER_H
#define	VECTORLIBITER_H

#include "DeltaLibraryObjectCreator.h"
#include "DeltaStdClassNames.h"

DVM_FUNC void	DeltaVectorIter_Make (DeltaValue* at);
extern	void	Install_DeltaVectorIter_Lib (DeltaObject* mainAPI);
extern	void	CleanUp_DeltaVectorIter_Lib (void);

#endif	// Do not add stuff beyond this point.