#ifndef __OPTIONS__HPP__
#define __OPTIONS__HPP__

#include <string>
#include "CommandLineArgumentsFactory_Forward.h"

#define MAIN_COMMAND_LONG_NAME						"main"
#define HELP_COMMAND_LONG_NAME						"help"
#define BYTECODE_PATH_COMMAND_LONG_NAME				"bytecode-path"
#define DLLIMPORT_PATH_COMMAND_LONG_NAME			"dllimport-path"
#define NEGOTIATION_PORT_COMMAND_LONG_NAME			"negotiation-port"
#define X_PROFILER_OUTPUT_FILE_COMMAND_LONG_NAME	"X:profiler-output-file"
#define X_DELTA_VM_STACK_SIZE_COMMAND_LONG_NAME		"X:delta-vm-stack-size"
#define X_DELTA_VM_NAME_COMMAND_LONG_NAME			"X:delta-vm-name"
//
#define DEFAULT_PROFILER_OUTPUT_FILEPATH_VALUE		"delta_execution_profiling.txt"
#define	DEFAULT_DELTA_VM_STACK_SIZE_VALUE			4096
#define DEFAULT_DELTA_VM_NAME						"main-vm"

// --main
// --help
// --bytecode-path=path;path;path
// --dllimport-path=path;path
// --negotiation-port=port
// --X:profiler-output-file=filepath
// --X:delta-vm-stack-size=4096
// --X:delta-vm-name=main-vm
class CommandLineArguments {
	friend class CommandLineArgumentsFactory;
	std::string		mainFilePath;
	bool			helpNeeded;
	std::string		bytecodePath;
	std::string		dllimportPath;
	int				negotiationPort;
	std::string		profilerOutput;
	int				deltaVmStackSize;
	std::string		deltaVmName;
	
	void			_SetMain (std::string const&);
	void			_NeedHelp (void);
	void			_SetBytecodePath (std::string const&);
	void			_SetDllImportPath (std::string const&);
	void			_SetNegotiationPort (int const);
	void			_SetProfilerOutput (std::string const&);
	void			_SetDeltaVmStackSize (int const);
	void			_SetDeltaVmName (std::string const&);
	
	bool			mainFilePathSet;
	bool			helpNeededSet;
	bool			bytecodePathSet;
	bool			dllimportPathSet;
	bool			negotiationPortSet;
	bool			profilerOutputSet;
	bool			deltaVmStackSizeSet;
	bool			deltaVmNameSet;

public:
	CommandLineArguments (void);

	std::string const&	GetMainFilePath (void) const;
	std::string const&	GetBytecodePath (void) const;
	std::string const&	GetDllImportPath (void) const;
	bool const			IsHelpNeeded (void) const;
	int const			GetNegotiationPort (void) const;
	std::string const&	GetProfilerOutputPath (void) const;
	int const			GetDeltaVmStackSize (void) const;
	std::string const&	GetDetlaVmName (void) const;
	
	bool const			IsMainFilePathGiven (void) const;
	bool const			IsHelpNeededGiven (void) const;
	bool const			IsBytecodePathGiven (void) const;
	bool const			IsDllImportPathGiven (void) const;
	bool const			IsNegotiationPortGiven (void) const;
	bool const			IsProfilerOutputPathGiven (void) const;
	bool const			IsDeltaVmStackSizeSet (void) const;
	bool const			IsDeltaVmNameSet (void) const;
};

#endif //__OPTIONS__HPP__