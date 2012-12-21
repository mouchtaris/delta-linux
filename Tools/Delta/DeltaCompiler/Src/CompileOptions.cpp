// CompileOptions.cpp
// Compilation option for the compiler
// (moved here from CompilerAPI).
// ScriptFighter Project.
// Y.Lilis, August 2011.
//

#include "CompileOptions.h"
#include "DeltaStdDefs.h"
#include "usystem.h"

////////////////////////////////////////////////////////

const std::string&	CompileOptions::GetOriginalSourceFile (void) const
	{ DASSERT(!IsDynamicCode()); return dynamicCodeOrOriginalSource; }

const std::string& CompileOptions::GetSourceFile (void) const
	{ return srcFile; }

bool CompileOptions::IsDynamicCode (void) const
	{ return isDynamicCode; }

const std::string& CompileOptions::GetDynamicCode (void) const
	{ DASSERT(IsDynamicCode()); return dynamicCodeOrOriginalSource; }

const std::string& CompileOptions::GetByteCodePath (void) const
	{ return byteCodePath; }

void CompileOptions::SetByteCodePath (const std::string& path) {
	byteCodePath = ugetenvironmentvar(DELTA_ENVIRONMENT_VAR_BYTECODE_PATH);
	if (!byteCodePath.empty())
		byteCodePath.append(";");
	byteCodePath.append(path);
}

bool CompileOptions::GetProductionMode (void) const
	{ return compileFlags.productionMode; }

void CompileOptions::SetProductionMode (bool val)
	{ compileFlags.productionMode = val; }

////////////////////////////////////////////////////////

void CompileOptions::InitialiseForText (const char* text) {
	srcFile							= DBG_DYNAMIC_SOURCE;
	dynamicCodeOrOriginalSource		= text;
	isDynamicCode					= true;
}

void CompileOptions::InitialiseForFile (const char* inFile, const char* optSymbolicName) {
	dynamicCodeOrOriginalSource		= inFile;
	isDynamicCode					= false;
	srcFile							= !optSymbolicName? inFile : optSymbolicName;
}

////////////////////////////////////////////////////////

void CompileOptions::Clear (void)
	{ byteCodePath.clear(); }

////////////////////////////////////////////////////////

CompileOptions::CompileOptions (void) :
	byteCodePath	(ugetenvironmentvar(DELTA_ENVIRONMENT_VAR_BYTECODE_PATH)),
	isDynamicCode	(false)
{
	compileFlags.productionMode = false;
}

////////////////////////////////////////////////////////