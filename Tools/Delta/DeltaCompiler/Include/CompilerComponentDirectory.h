// CompilerComponentDirectory.h
// Parsing context for pure parsers.
// ScriptFighter Project.
// Y.Lilis, July 2011.
//

#ifndef COMPILERCOMPONENTDIRECTORY_H
#define COMPILERCOMPONENTDIRECTORY_H

#include "ucomponentdirectory.h"

////////////////////////////////////////////////////////////////////////

typedef ucomponentdirectory CompilerComponentDirectory;

////////////////////////////////////////////////////////////////////////

#define USE_COMPILER_COMPONENT_DIRECTORY()											\
	public:																			\
		CompilerComponentDirectory* GetCompilerComponentDirectory(void) const		\
			{ return DPTR(compilerComponentDirectory); }							\
		void SetCompilerComponentDirectory(CompilerComponentDirectory* handler)		\
			{ compilerComponentDirectory = handler; }								\
	private:																		\
		CompilerComponentDirectory* compilerComponentDirectory

#define INIT_COMPILER_COMPONENT_DIRECTORY(instance, directory)	\
	(instance)->SetCompilerComponentDirectory(directory)

#define COMPONENT_DIRECTORY()	GetCompilerComponentDirectory()

#define CTOR_INIT_COMPONENT_DIRECTORY	\
	compilerComponentDirectory((CompilerComponentDirectory*) 0)

////////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
