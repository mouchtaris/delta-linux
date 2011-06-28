#include "CommandLineArgumentsFactory.h"
//
#include "DDebug.h"
#include <ctype.h>
#include <string.h>
//
#include "CommandLineArguments.h"
//
#include <ostream>
//
#define ASSERT(EXPR) DASSERT(EXPR);
//

static bool ExtractShortCommand (char const* const line, std::string& command, std::string& arg) {
	bool isShort = false;
	if (line[0] == '-' && isalpha(line[1])) {
		command = line[1];
		if (line[2] != '\0')
			arg = (line + 2);
		else
			arg.clear();
		isShort = true;
	}
	return isShort;
}

static bool ExtractLongCommand (char const* const line, std::string& command, std::string& arg) {
	bool isLong = false;
	if (line[0] == '-' && line[1] == '-' && isalpha(line[2])) {
		char const* const equalSign = strchr(line, '=');
		if (equalSign != NULL) {
			ASSERT( *equalSign == '=' )
			command.assign(line + 2, equalSign);
			arg = equalSign + 1;
		}
		else {
			command = line + 2;
			arg.clear();
		}
		isLong = true;
	}
	return isLong;
}

#ifdef _DEBUG
#	define LOG_FINE(ARGS)															\
	{																				\
		printf("[OptionsFactory]/f: ");												\
		printf ARGS;																\
		puts("");																	\
	}
#else
#	define LOG_FINE(ARGS)
#endif

#define ERROR(ARGS)																	\
	{																				\
		printf("[OptionsFactory]/ERROR: ");											\
		printf ARGS ;																\
		puts("");																	\
		fflush(stdout);																\
		exit(1);																	\
	}																				\

#define OPTION_REQUIRES_ARGUMENT_ERROR												\
	ERROR(("Option --%s requires an argument (none given)", command_c_str))
#define OPTION_REASSIGN_ERROR(variableStem)											\
	ERROR(("Option --%s has already been assigned a value (\"%s\")",				\
			command_c_str, o_##variableStem .c_str() ))
#define OPTION_DOES_NOT_ACCEPT_ARGUMENT_ERROR										\
	ERROR(("Option --%s does not accept any arguments (\"%s\" given)",				\
			command_c_str, value.c_str() ))
#define OPTION_RESET_ERROR															\
	ERROR(("Option --%s has alread been set", command_c_str))

#define IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITH_ARGUMENT_ELSE(commandLineName, variableStem)	\
	if (strcmp(command_c_str, commandLineName) == 0)							\
		if (value.empty())														\
			OPTION_REQUIRES_ARGUMENT_ERROR										\
		else																	\
		if (o_##variableStem##Set)												\
			OPTION_REASSIGN_ERROR(variableStem)									\
		else {																	\
			LOG_FINE(("Setting " commandLineName " to \"%s\"", value.c_str()))	\
			o_##variableStem = value;											\
			o_##variableStem##Set = true;										\
		}																		\
	else																		\

#define IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITHOUT_ARGUMENT_ELSE(commandLineName, variableStem)	\
	if (strcmp(command_c_str, commandLineName) == 0)							\
		if (!value.empty())														\
			OPTION_DOES_NOT_ACCEPT_ARGUMENT_ERROR								\
		else																	\
		if (o_##variableStem##Set)												\
			OPTION_RESET_ERROR													\
		else {																	\
			LOG_FINE(("Setting " commandLineName " on"))						\
			o_##variableStem = true;											\
			o_##variableStem##Set = true;										\
		}																		\
	else																		\

#define DECLARE_VARIABLES_FOR_OPTION_WITH_ARGUMENT(variableStem)				\
	bool o_##variableStem##Set = false; std::string o_##variableStem;			\

#define DECLARE_VARIABLES_FOR_OPTION_WITHOUT_ARGUMENT(variableStem)				\
	bool o_##variableStem##Set = false; bool o_##variableStem;					\

void CommandLineArgumentsFactory::InitialiseOptionsFromArguments (
		int const				argc,
		char const* const		argv[],
		CommandLineArguments&	options
) {
	DECLARE_VARIABLES_FOR_OPTION_WITH_ARGUMENT(main)
	DECLARE_VARIABLES_FOR_OPTION_WITHOUT_ARGUMENT(help)
	DECLARE_VARIABLES_FOR_OPTION_WITH_ARGUMENT(bytecodePath)
	DECLARE_VARIABLES_FOR_OPTION_WITH_ARGUMENT(dllimportPath)
	DECLARE_VARIABLES_FOR_OPTION_WITH_ARGUMENT(negotiationPort)
	DECLARE_VARIABLES_FOR_OPTION_WITH_ARGUMENT(x_profilerOutputPath)
	DECLARE_VARIABLES_FOR_OPTION_WITH_ARGUMENT(x_deltaVmStackSize)
	DECLARE_VARIABLES_FOR_OPTION_WITH_ARGUMENT(x_deltaVmName)
	//
	for (char const* const* arg_ptr = &argv[0]; (*arg_ptr) != NULL; ++arg_ptr) {
		std::string command;
		std::string value;
		if (ExtractShortCommand(*arg_ptr, command, value))
			ERROR(("short command -%c not understood", command.c_str()[0]))
		else
		if (ExtractLongCommand(*arg_ptr, command, value)) {
			char const* const command_c_str = command.c_str();
			IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITH_ARGUMENT_ELSE(MAIN_COMMAND_LONG_NAME, main)
			IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITHOUT_ARGUMENT_ELSE(HELP_COMMAND_LONG_NAME, help)
			IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITH_ARGUMENT_ELSE(BYTECODE_PATH_COMMAND_LONG_NAME, bytecodePath)
			IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITH_ARGUMENT_ELSE(DLLIMPORT_PATH_COMMAND_LONG_NAME, dllimportPath)
			IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITH_ARGUMENT_ELSE(NEGOTIATION_PORT_COMMAND_LONG_NAME, negotiationPort)
			IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITH_ARGUMENT_ELSE(X_PROFILER_OUTPUT_FILE_COMMAND_LONG_NAME, x_profilerOutputPath)
			IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITH_ARGUMENT_ELSE(X_DELTA_VM_STACK_SIZE_COMMAND_LONG_NAME, x_deltaVmStackSize)
			IF_SHOULD_HANDLE_THEN_HANDLE_LONG_OPTION_WITH_ARGUMENT_ELSE(X_DELTA_VM_NAME_COMMAND_LONG_NAME, x_deltaVmName)
			ERROR(("Long command option \"%s\" not understood", command.c_str()))
		}
		else
			ERROR(("plain arguments (\"%s\") not understood", *arg_ptr))
	}
	
	if (!o_mainSet && !o_helpSet)
		ERROR(("no main bytecode file specified"))
	else
		options._SetMain(o_main);
	
	if (o_helpSet)
		options._NeedHelp();
	
	if (o_bytecodePathSet)
		options._SetBytecodePath(o_bytecodePath);
	
	if (o_dllimportPathSet)
		options._SetDllImportPath(o_dllimportPath);
		
	if (o_negotiationPortSet) {
		long negotiationPort = strtol(o_negotiationPort.c_str(), NULL, 10);
		if (negotiationPort > 0xffff)
			ERROR(("Invalid port %lu", negotiationPort))
		else
			options._SetNegotiationPort(negotiationPort);
	}

	if (o_x_profilerOutputPathSet)
		options._SetProfilerOutput(o_x_profilerOutputPath);

	if (o_x_deltaVmStackSizeSet) {
		long deltaVmStackSize = strtol(o_x_deltaVmStackSize.c_str(), NULL, 10);
		if (deltaVmStackSize < 0 || deltaVmStackSize > 0xffff)
			ERROR(("Invalid stack size (<0 or >0xffff): %ld", deltaVmStackSize))
		else
			options._SetDeltaVmStackSize(deltaVmStackSize);
	}

	if (o_x_deltaVmNameSet)
		options._SetDeltaVmName(o_x_deltaVmName);
}

std::ostream& CommandLineArgumentsFactory::PrintHelpTo (std::ostream& out) {
	static const char tabdoubledash[] = "\t--";
	static const char anarg[] = "=something";
	return out
		<< std::endl << tabdoubledash << MAIN_COMMAND_LONG_NAME << anarg
		<< std::endl << tabdoubledash << HELP_COMMAND_LONG_NAME
		<< std::endl << tabdoubledash << BYTECODE_PATH_COMMAND_LONG_NAME << anarg
		<< std::endl << tabdoubledash << DLLIMPORT_PATH_COMMAND_LONG_NAME << anarg
		<< std::endl << tabdoubledash << NEGOTIATION_PORT_COMMAND_LONG_NAME << anarg
		<< std::endl << tabdoubledash << X_PROFILER_OUTPUT_FILE_COMMAND_LONG_NAME << anarg
		<< std::endl << tabdoubledash << X_DELTA_VM_STACK_SIZE_COMMAND_LONG_NAME << anarg
	;
}
