// CompileOptions.h
// Compilation option for the compiler
// (moved here from CompilerAPI).
// ScriptFighter Project.
// Y.Lilis, August 2011.
//

#ifndef	COMPILEOPTIONS_H
#define	COMPILEOPTIONS_H

#include "DDebug.h"
#include "utypes.h"
#include "DeltaCompErrorMsg.h"
#include "GenericWriter.h"
#include "DeltaCompilerDefs.h"
#include <list>
#include <string>
#include <stack>

///////////////////////////////////////////////////////////////

class CompileOptions {

	private:

	std::string		byteCodePath;
	std::string		srcFile;
	bool			isDynamicCode;					// If compiling from text string.
	std::string		dynamicCodeOrOriginalSource;

	struct DCOMPLIB_CLASS CompileFlags {
		bool		productionMode;					// FALSE
	};

	CompileFlags	compileFlags;

	///////////////////////////////////////////////////////////////

	public:
	const std::string&	GetOriginalSourceFile (void) const;
	const std::string&	GetSourceFile (void) const;

	bool				IsDynamicCode (void) const;
	const std::string&	GetDynamicCode (void) const;

	const std::string&	GetByteCodePath (void) const;
	void				SetByteCodePath (const std::string& path);

	// Compile flags.
	bool				GetProductionMode (void) const;
	void				SetProductionMode (bool val);

	///////////////////////////////////////////////////////////////

	void				InitialiseForText (const char* text);
	void				InitialiseForFile (const char* inFile, const char* optSymbolicName = (const char*) 0);

	///////////////////////////////////////////////////////////////

	void				Clear (void);

	CompileOptions (void);
	~CompileOptions () {}
};

///////////////////////////////////////////////////////////////

#define COMPOPTIONS_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), CompileOptions)))

#define COMPOPTIONS	COMPOPTIONS_EX(GET_COMPONENT_DIRECTORY())

///////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.