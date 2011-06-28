#include "CommandLineArguments.h"
//
#include "DDebug.h"
//
#define ASSERT(EXPR)	DASSERT(EXPR);

/////////////////////////////

CommandLineArguments::CommandLineArguments (void):
	mainFilePath(),
	helpNeeded(false),
	bytecodePath(),
	dllimportPath(),
	negotiationPort(0),
	profilerOutput(DEFAULT_PROFILER_OUTPUT_FILEPATH_VALUE),
	deltaVmStackSize(DEFAULT_DELTA_VM_STACK_SIZE_VALUE),
	deltaVmName(DEFAULT_DELTA_VM_NAME),
	//
	mainFilePathSet(false),
	helpNeededSet(false),
	bytecodePathSet(false),
	dllimportPathSet(false),
	negotiationPortSet(false),
	profilerOutputSet(false),
	deltaVmStackSizeSet(false),
	deltaVmNameSet(false)
	{ }

/////////////////////////////

void CommandLineArguments::_SetMain (std::string const& _mainFilePath) {
	ASSERT( !mainFilePathSet )
	mainFilePath = _mainFilePath;
	mainFilePathSet = true;
}

/////////////////////////////

void CommandLineArguments::_NeedHelp (void) {
	ASSERT( !helpNeededSet )
	helpNeeded = true;
	helpNeededSet = true;
}

/////////////////////////////

void CommandLineArguments::_SetBytecodePath (std::string const& _bytecodePath) {
	ASSERT( !bytecodePathSet )
	bytecodePath = _bytecodePath;
	bytecodePathSet = true;
}

/////////////////////////////

void CommandLineArguments::_SetDllImportPath (std::string const& _dllimportPath) {
	ASSERT( !dllimportPathSet )
	dllimportPath = _dllimportPath;
	dllimportPathSet = true;
}

/////////////////////////////

void CommandLineArguments::_SetNegotiationPort (int const _negotiationPort) {
	ASSERT( !negotiationPortSet )
	negotiationPort = _negotiationPort;
	negotiationPortSet = true;
}

/////////////////////////////

void CommandLineArguments::_SetProfilerOutput (std::string const& _profilerOutput) {
	ASSERT( !profilerOutputSet )
	profilerOutput = _profilerOutput;
	profilerOutputSet = true;
}

/////////////////////////////

void CommandLineArguments::_SetDeltaVmStackSize (int const _deltaVmStackSize) {
	ASSERT( !deltaVmStackSizeSet )
	deltaVmStackSize = _deltaVmStackSize;
	deltaVmStackSizeSet = true;
}

/////////////////////////////

void CommandLineArguments::_SetDeltaVmName (std::string const& _deltaVmName) {
	ASSERT( !deltaVmNameSet )
	deltaVmName = _deltaVmName;
	deltaVmNameSet = true;
}

/////////////////////////////
//////////////////////////////////////////////////////////
/////////////////////////////

bool const CommandLineArguments::IsHelpNeeded (void) const
	{ return helpNeeded; }

/////////////////////////////

std::string const& CommandLineArguments::GetProfilerOutputPath (void) const
	{ return profilerOutput; }

/////////////////////////////

std::string const& CommandLineArguments::GetDllImportPath (void) const
	{ return dllimportPath; }

bool const CommandLineArguments::IsDllImportPathGiven (void) const
	{ return dllimportPathSet; }

/////////////////////////////

std::string const& CommandLineArguments::GetBytecodePath (void) const
	{ return bytecodePath; }

bool const CommandLineArguments::IsBytecodePathGiven (void) const
	{ return bytecodePathSet; }

/////////////////////////////

int const CommandLineArguments::GetDeltaVmStackSize (void) const
	{ return deltaVmStackSize; }

bool const CommandLineArguments::IsDeltaVmStackSizeSet (void) const
	{ return deltaVmStackSizeSet; }

/////////////////////////////

std::string const& CommandLineArguments::GetDetlaVmName (void) const
	{ return deltaVmName; }
	
bool const CommandLineArguments::IsDeltaVmNameSet (void) const
	{ return deltaVmNameSet; }

/////////////////////////////

std::string const& CommandLineArguments::GetMainFilePath (void) const
	{ return mainFilePath; }

bool const CommandLineArguments::IsMainFilePathGiven (void) const
	{ return mainFilePathSet; }

/////////////////////////////