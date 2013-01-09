// CompilerComponentDirectory.h
// Definitions for using a directory
// across the compiler components.
// ScriptFighter Project.
// Y.Lilis, July 2011.
//

#ifndef COMPILERCOMPONENTDIRECTORY_H
#define COMPILERCOMPONENTDIRECTORY_H

#include "ucomponentdirectory.h"

////////////////////////////////////////////////////////////////////////

#define GET_COMPONENT_DIRECTORY()	GetDirectory()

#define	COMPONENT_CLIENT_CTOR_ARGS_1(_arg)					GET_COMPONENT_DIRECTORY(), _arg
#define	COMPONENT_CLIENT_CTOR_ARGS_2(_arg1, _arg2)			GET_COMPONENT_DIRECTORY(), _arg1, _arg2
#define	COMPONENT_CLIENT_CTOR_ARGS_3(_arg1, _arg2, _arg3)	GET_COMPONENT_DIRECTORY(), _arg1, _arg2, _arg3

////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
