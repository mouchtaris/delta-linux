// FileIOLib.h
// File I/O library functions for the DeltaVirtualMachine.
// ScriptFighter Project.
// A. Savidis, December 1999.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#ifndef	FILEIOLIB_H
#define	FILEIOLIB_H

#include "DeltaLibraryObjectCreator.h"
#include "DeltaStdDefs.h"
#include "DeltaStdClassNames.h"
#include "DeltaStdLibTemplatesUtils.h"

DVM_FUNC FILE*	DeltaFile_Get (void* val, const std::string& typeStr);
DVM_FUNC FILE*	DeltaFile_Get (DeltaValue& obj);

VCLASSID_HEADER(FILE_CID)

#define	DLIB_GET_FILE(var) \
	DLIB_GET_VALIDATABLEVAL_ARG(var, FILE, FILE_TYPE_STR, FILE_CID)

extern void Install_DeltaFile_Lib (DeltaObject* mainAPI);
extern void CleanUp_DeltaFile_Lib (void);

#endif	// Do not add stuff beyond this point.