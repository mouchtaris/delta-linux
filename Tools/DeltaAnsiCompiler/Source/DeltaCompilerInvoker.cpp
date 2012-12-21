#include "DeltaCompilerInvoker.h"
//
#include "DeltaCompilerInit.h"
#include "DeltaMetaCompiler.h"
#include "DDebug.h"
#include "uinit.h"
#include "ustrings.h"
#include "DeltaStdLibFuncNames.h"
//
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
//
#undef	_ASSERT
#define _ASSERT(expr)		DASSERT(expr);
#define _USER_ASSERT(expr)	if (expr)																	\
								{}																		\
							else {																		\
								fprintf(stderr, "Error %s:%d: NOT(%s).\n", __FILE__, __LINE__, #expr);	\
								_ASSERT(false)															\
								exit(1);																\
							}
#define C_ARRAY_LENGTH(CAR)	(sizeof(CAR)/sizeof((CAR)[0]))

//
typedef char*			c_str_t;
typedef char const*		c_const_str_t;
#define ARRAY_OF(T)		T*
//

/////////////////////////////

static bool ContainsOnlyWhitespace (char const* const str, unsigned int const strlen) {
	bool								containsOnlyWhitespace	= true;
	char const*			ite				= &str[0];
	char const* const	end				= str + strlen;
	for (char c = *ite; containsOnlyWhitespace && ite != end; c = *++ite)
		switch (c) {
			case ' ' :
			case '\t':
			case '\n':
				break;
			default:
				containsOnlyWhitespace = false;
		}
	return containsOnlyWhitespace;
}

/////////////////////////////

static ARRAY_OF(c_const_str_t) const LoadLibraryFunctionFromFile (	char* const						funcdesc,
																	unsigned int					funcdesclen,
																	std::string const&				path,
																	std::list<ARRAY_OF(c_str_t)>&	funcsList
) {
	std::vector<std::string> funcVector;
	std::ifstream in(path.c_str());
	if(in) {
		while(!in.eof()) {
			in.getline(funcdesc, funcdesclen - 1);
			if(!ContainsOnlyWhitespace(funcdesc, funcdesclen) && funcdesc[0] != '\0')
				funcVector.push_back(funcdesc);
		}
	}
	char** result = NULL;
	if(!funcVector.empty()) {
		std::vector<std::string>::size_type const		funcVectorLength	(funcVector.size());
		std::vector<std::string>::const_iterator		ite					(funcVector.begin());
		std::vector<std::string>::const_iterator const	end					(funcVector.end());
		ARRAY_OF(c_str_t) const							funcs				= DNEWARR(c_str_t, funcVectorLength + 1);

		unsigned int i = 0;
		while (ite != end) {
			_ASSERT( i < funcVectorLength )
			DPTR(funcs)[i++] = ucopystr(*ite++);
			DPTR(DPTR(funcs)[i-1]);
		}
		_ASSERT( i == funcVectorLength )
		DPTR(funcs)[i] = NULL;

		funcsList.push_back(funcs);

		result = funcs;
	}

	return const_cast<ARRAY_OF(c_const_str_t) const>(result);
}

/////////////////////////////

///////////////////////////////////////////////////////////
// Options
// --bytecode-path=path
// --debug={anything}
// --functions=functions_file_path
// --parent={anything}
// --output-directory=dirpath
// --help={anything}
// --symbolic-name={anything}
// --icode
// --text
#define Option_FIRST			0x00
#define Option_MIN				Option_FIRST
//
#define Option_BytecodePath		Option_MIN
#define Option_Debug			0x01
#define Option_Functions		0x02
#define Option_Parent			0x03
#define Option_OutputDirectory	0x04
#define Option_SymbolicName		0x05
#define Option_Icode			0x06
#define Option_Text				0x07
#define Option_Help				0x08
//
#define Option_LAST				Option_Help
#define Option_MAX				Option_LAST
#define Options_NUMBER			(Option_MAX + 1)
#define Options_LENGTH			(Options_NUMBER + 1)	// +1 for NULL termination

static char const _ShortOptions[Options_LENGTH] = {
	'$'					, // Option_BytecodePath	= 0x00
	'$'					, // Option_Debug			= 0x01
	'$'					, // Option_Functions		= 0x02
	'$'					, // Option_Parent			= 0x03
	'$'					, // Option_OutputDirectory	= 0x04
	'$'					, // Option_SymbolicName	= 0x05
	'$'					, // Option_Icode			= 0x06
	'$'					, // Option_Text			= 0x07
	'$'					, // Option_Help			= 0x08
	'\0'
};

static c_const_str_t _LongOptions[Options_LENGTH] = {
	"bytecode-path"		, // Option_BytecodePath	= 0x00
	"debug"				, // Option_Debug			= 0x01
	"functions"			, // Option_Functions		= 0x02
	"parent"			, // Option_Parent			= 0x03
	"output-directory"	, // Option_OutputDirectory	= 0x04
	"symbolic-name"		, // Option_SymbolicName	= 0x05
	"icode"				, // Option_Icode			= 0x06
	"text"				, // Option_Text			= 0x07
	"help"				, // Option_Help			= 0x08
	NULL
};

static bool	_OptionAcceptsArgument[Options_LENGTH] = {
	true				, // Option_BytecodePath	= 0x00
	false				, // Option_Debug			= 0x01
	true				, // Option_Functions		= 0x02
	false				, // Option_Parent			= 0x03
	true				, // Option_OutputDirectory	= 0x04
	true				, // Option_SymbolicName	= 0x05
	false				, // Option_Icode			= 0x06
	false				, // Option_Text			= 0x07
	false				, // Option_Help			= 0x08
	NULL
};

/////////////////////////////

static unsigned int _ShortOptionToIndex (char const option) {
	for (c_const_str_t opt_ptr = &_ShortOptions[0]; *opt_ptr != '\0'; ++opt_ptr) {
		_ASSERT( opt_ptr - &_ShortOptions[0] < Options_LENGTH )
		if (*opt_ptr == option)
			return opt_ptr - &_ShortOptions[0];
	}
	_USER_ASSERT( !"Wrong short command given" )
	return ~0x00;
}

/////////////////////////////

static unsigned int _LongOptionToIndex (c_const_str_t const option) {
	for (UPTR(c_const_str_t) opt_ptr = &_LongOptions[0]; *opt_ptr != NULL; ++opt_ptr) {
		_ASSERT( opt_ptr - &_LongOptions[0] < Options_LENGTH )
		if (strcmp(*opt_ptr, option) == 0)
			return opt_ptr - &_LongOptions[0];
	}
	_USER_ASSERT( !"Wrong long command given" )
	return ~0x00;
}

/////////////////////////////

struct DeltaCompilerInvokerCommandLineArguments {
	std::list<std::string>		bytecodePaths;
	bool						productionMode;
	bool						productionModeSet;
	std::list<std::string>		functionsFiles;
	bool						useParentDirectories;
	bool						useParentDirectoriesSet;
	bool						needHelp;
	bool						needHelpSet;
	std::string					outputDirectory;
	bool						outputDirectorySet;
	std::list<std::string>		inputFiles;
	std::list<std::string>		symbolicNames;
	std::string					programName;
	bool						programNameSet;
	bool						intermediateCode;
	bool						intermediateCodeSet;
	bool						textualOutput;
	bool						textualOutputSet;
	//
	typedef std::list<std::string>::const_iterator StringListIter;
	bool						iteratorsInvalidated;
	StringListIter				bytecodePathsIterator;
	StringListIter 				bytecodePathsIteratorEnd;
	StringListIter				functionsFilesIterator;
	StringListIter 				functionsFilesIteratorEnd;
	StringListIter				inputFilesIterator;
	StringListIter 				inputFilesIteratorEnd;
	StringListIter				symbolicNamesIterator;
	StringListIter 				symbolicNamesIteratorEnd;
							
	
	DeltaCompilerInvokerCommandLineArguments (void):
		bytecodePaths				(),
		productionMode				(true),
		productionModeSet			(false),
		functionsFiles				(),
		useParentDirectories		(false),
		useParentDirectoriesSet		(false),
		needHelp					(false),
		needHelpSet					(false),
		outputDirectory				(),
		outputDirectorySet			(false),
		inputFiles					(),
		symbolicNames				(),
		programName					(),
		programNameSet				(false),
		intermediateCode			(false),
		intermediateCodeSet			(false),
		textualOutput				(false),
		textualOutputSet			(false),
		//							(?)
		iteratorsInvalidated		(true),
		bytecodePathsIterator		(),
		bytecodePathsIteratorEnd	(),
		functionsFilesIterator		(),
		functionsFilesIteratorEnd	(),
		inputFilesIterator			(),
		inputFilesIteratorEnd		(),
		symbolicNamesIterator		(),
		symbolicNamesIteratorEnd	()
		{ }

	void	AddByteCodePath (c_const_str_t const path) {
				bytecodePaths.push_back(path);
				iteratorsInvalidated = true;
			}
	
	void	ProductionModeOff (void) {
				_USER_ASSERT( !productionModeSet )
				productionMode		= false;
				productionModeSet	= true;
			}

	void	AddFunctionsFile (c_const_str_t const path) {
				functionsFiles.push_back(path);
				iteratorsInvalidated = true;
			}
				
	void	UseParentDirectories (void) {
				_USER_ASSERT( !useParentDirectoriesSet )
				useParentDirectories	= true;
				useParentDirectoriesSet	= true;
			}
			
	void	NeedHelp (void) {
				_USER_ASSERT( !needHelpSet )
				needHelp	= true;
				needHelpSet = true;
			}
			
	void	SetOutputDirectory (c_const_str_t const outdir) {
				_USER_ASSERT( !outputDirectorySet )
				outputDirectory		= outdir;
				outputDirectorySet	= true;
			}
			
	void	AddInputFile (c_const_str_t const inputFile) {
				inputFiles.push_back(inputFile);
				iteratorsInvalidated = true;
			}
				
	void	AddSymbolicName (c_const_str_t const symbolicName) {
				symbolicNames.push_back(symbolicName);
				iteratorsInvalidated = true;
			}
				
	void	SetProgramName (c_const_str_t const _programName) {
				_ASSERT( !programNameSet )
				programName = _programName;
				programNameSet = true;
			}
	
	void	EnableIntermediateCode (void) {
				_USER_ASSERT( !intermediateCodeSet )
				intermediateCode	= true;
				intermediateCodeSet	= true;
			}
	
	void	EnableTextualOutput (void) {
				_USER_ASSERT( !textualOutputSet )
				textualOutput		= true;
				textualOutputSet	= true;
			}
	//
	void	ResetAllIterators (void) {
				bytecodePathsIterator		= bytecodePaths.begin();
				bytecodePathsIteratorEnd	= bytecodePaths.end();
				functionsFilesIterator		= functionsFiles.begin();
				functionsFilesIteratorEnd	= functionsFiles.end();
				inputFilesIterator			= inputFiles.begin();
				inputFilesIteratorEnd		= inputFiles.end();
				symbolicNamesIterator		= symbolicNames.begin();
				symbolicNamesIteratorEnd	= symbolicNames.end();
				iteratorsInvalidated		= false;
			}
	bool				HasNextByteCodePath (void) const
							{ _USER_ASSERT( !iteratorsInvalidated ) return bytecodePathsIterator != bytecodePathsIteratorEnd; }
	std::string const&	NextByteCodePath (void)
							{ _USER_ASSERT( HasNextByteCodePath() ) return *bytecodePathsIterator++; }
	bool				HasNextFunctionsFile (void) const
							{ _USER_ASSERT( !iteratorsInvalidated ) return functionsFilesIterator != functionsFilesIteratorEnd; }
	std::string const&	NextFunctionsFile (void)
							{ _USER_ASSERT( HasNextFunctionsFile() ) return *functionsFilesIterator++; }
	bool				HasNextInputFile (void) const
							{ _USER_ASSERT( !iteratorsInvalidated ) return inputFilesIterator != inputFilesIteratorEnd; }
	std::string const&	NextInputFile (void)
							{ _USER_ASSERT( HasNextInputFile() ) return *inputFilesIterator++; }
	bool				HasNextSymbolicName (void) const
							{ _USER_ASSERT( !iteratorsInvalidated ) return symbolicNamesIterator != symbolicNamesIteratorEnd; }
	std::string const&	NextSymbolicName (void)
							{ _USER_ASSERT( HasNextSymbolicName() ) return *symbolicNamesIterator++; }
};

/////////////////////////////

static void _BytecodeCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ data.AddByteCodePath(value); }

static void _ProductionModeCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ _ASSERT( value == NULL ) data.ProductionModeOff(); }

static void _FunctionsCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ data.AddFunctionsFile(value); }

static void _ParentCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ _ASSERT( value == NULL ) data.UseParentDirectories(); }

static void _OutputDirectoryCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ data.SetOutputDirectory(value); }
	
static void _SymbolicNameCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ data.AddSymbolicName(value); }

static void _IntermediateCodeCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ _ASSERT( value == NULL ) data.EnableIntermediateCode(); }

static void _TextualOutputCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ _ASSERT( value == NULL ) data.EnableTextualOutput(); }

	
static void _HelpCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ _ASSERT( value == NULL ) data.NeedHelp(); }

static void _PlainArgumentCommandLineArgumentHandler (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ data.AddInputFile(value); }


/////////////////////////////

void (* const _CommandLineArgumentsHandlers[Options_LENGTH]) (c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data) = {
	&_BytecodeCommandLineArgumentHandler		, // Option_BytecodePath	= 0x00
	&_ProductionModeCommandLineArgumentHandler	, // Option_Debug			= 0x01
	&_FunctionsCommandLineArgumentHandler		, // Option_Functions		= 0x02
	&_ParentCommandLineArgumentHandler			, // Option_Parent			= 0x03
	&_OutputDirectoryCommandLineArgumentHandler	, // Option_OutputDirectory	= 0x04
	&_SymbolicNameCommandLineArgumentHandler	, // Option_SymbolicName	= 0x05
	&_IntermediateCodeCommandLineArgumentHandler, // Option_Icode			= 0x06
	&_TextualOutputCommandLineArgumentHandler	, // Option_Text			= 0x07
	&_HelpCommandLineArgumentHandler			, // Option_Help			= 0x08
	NULL
};

/////////////////////////////

static void _DispatchShortCommand (char const command, c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ (*_CommandLineArgumentsHandlers[_ShortOptionToIndex(command)])(value, data); }
	
static void _DispatchLongCommand (c_const_str_t const command, c_const_str_t const value, DeltaCompilerInvokerCommandLineArguments& data)
	{ (*_CommandLineArgumentsHandlers[_LongOptionToIndex(command)])(value, data); }

/////////////////////////////

static std::string const _GetLongCommand (char const* const commandLine) {
	unsigned int i = 0;
	while (commandLine[i] != '=')
		++i;
	_USER_ASSERT(i > 0);
	return std::string(commandLine + 0, commandLine + i);
}

/////////////////////////////

struct CompilationUnit {
	typedef std::string	String;
	
	static CompilationUnit const
					Create (  std::string const& 	inputFilePath
							, std::string const& 	symbolicName		= ""
							, bool const			useParentDirectory	= false
							, std::string const& 	outputDirectory		= "./"
	) {
		std::string inputFileName;
		std::string inputFileDirName;
		// Extract basename and dirname for inputFilePath
		{
			size_t const lastPathSeparator = inputFilePath.find_last_of('/');
			_USER_ASSERT( lastPathSeparator < inputFilePath.length() - 1 || lastPathSeparator == std::string::npos)
			if (lastPathSeparator == std::string::npos) {
				inputFileName		= inputFilePath;
				inputFileDirName	= "./";
			}
			else {
				inputFileName		= inputFilePath.substr(lastPathSeparator + 1);
				inputFileDirName 	= inputFilePath.substr(0, lastPathSeparator);
			}
		}
		
		std::string inputFileExtension;
		std::string inputFileBaseName;
		// Extract basename BASEname and basename extension
		{
			size_t const extensionDotIndex = inputFileName.find_last_of('.');
			if (extensionDotIndex == std::string::npos)
				{}
			else {
				inputFileExtension	= inputFileName.substr(extensionDotIndex + 1);
				inputFileBaseName	= inputFileName.substr(0, extensionDotIndex);
			}
		}
		
		std::string dot(".");
		// Create compilation-unit
		return CompilationUnit(	inputFilePath,
								symbolicName.empty()? inputFilePath : symbolicName,
								inputFileExtension,
								inputFileBaseName,
								outputDirectory + '/' + (useParentDirectory? inputFileDirName : dot) + '/' + inputFileBaseName
								);
	}
	
	String const&	GetInputFilePath (void)				const { return inputFilePath			; }
	String const&	GetSymbolicName (void)				const { return symbolicName				; }
	String const&	GetInputFileExtension (void)		const { return inputFileExtension		; }
	String const&	GetInputFileBaseName (void)			const { return inputFileBaseName		; }
	//
	String const	GetBinaryFilePath (void)			const { return DBC(outputFilePathStem)	; }
	String const	GetIntemediateCodeFilePath (void)	const { return IDC(outputFilePathStem)	; }
	String const	GetTextFilePath (void)				const { return TXT(outputFilePathStem)	; }
	
private:
	String const		inputFilePath;
	String const		symbolicName;
	String const		outputFilePathStem;
	//
	String const		inputFileExtension;
	String const		inputFileBaseName;

	CompilationUnit (
			String const& _inputFilePath,
			String const& _symbolicName,
			String const& _inputFileExtension,
			String const& _inputFileBaseName,
			String const& _outputFilePathStem)
	:
		inputFilePath		(_inputFilePath),
		symbolicName		(_symbolicName),
		outputFilePathStem	(_outputFilePathStem),
		inputFileExtension	(_inputFileExtension),
		inputFileBaseName	(_inputFileBaseName)
		{ }
};

/////////////////////////////

struct DeltaCompilerInvokerData {
	void 											ReconfigureFromArguments (void);
	void											ConfigureDeltaCompiler (void);
	//
	void											CleanUpDeltaCompiler (void);
	void											Deconfigure (void);
	//
	DeltaCompilerInvokerCommandLineArguments&		GetArguments (void) const
														{ return *DPTR(mutableArgs_ptr); }	
	//
	std::list<CompilationUnit>						units;
	DeltaCompilerInvokerCommandLineArguments const*	args_ptr;
	DeltaMetaCompiler*								compiler;
	
	
	DeltaCompilerInvokerData (DeltaCompilerInvokerCommandLineArguments* const _args_ptr):
		units						(),
		args_ptr					(DPTR(_args_ptr)),
		compiler					((DeltaMetaCompiler*) 0),
		mutableArgs_ptr				(DPTR(_args_ptr)),
		customFunctionsDescriptions	(),
		deltaCompilerConfigured		(false),
		selfConfigured				(false)
	{
		unew(compiler);
		static bool constructed = false;
		_USER_ASSERT( !constructed )
		constructed = true;
	}

		
	~DeltaCompilerInvokerData (void) {
		udelete(args_ptr);
		
		FunctionsIterator const	end	(customFunctionsDescriptions.end()		);
		FunctionsIterator		i	(customFunctionsDescriptions.begin()	);
		for (; i != end; ++i) {
			for (c_str_t* c_str_ptr = &(*i)[0]; *c_str_ptr != NULL; ++c_str_ptr)
				DDELARR(*c_str_ptr);
			DDELARR(*i);
		}

		udelete(compiler);
	}

private:
	typedef std::list<ARRAY_OF(c_str_t)>			FunctionsList;
	typedef FunctionsList::iterator					FunctionsIterator;
	
	DeltaCompilerInvokerCommandLineArguments*		mutableArgs_ptr;
	FunctionsList									customFunctionsDescriptions;
	bool											deltaCompilerConfigured;
	bool											selfConfigured;
};

void DeltaCompilerInvokerData::ReconfigureFromArguments (void) {
	_ASSERT( !selfConfigured )
	selfConfigured = true;

	GetArguments().ResetAllIterators();
	units.clear();

	// Configure compilation units
	{
		_ASSERT( units.empty() )
		while (DPTR(args_ptr)->HasNextInputFile())
			units.push_back(
					CompilationUnit::Create(
							GetArguments().NextInputFile(),
							DPTR(args_ptr)->HasNextSymbolicName()? GetArguments().NextSymbolicName() : "",
							DPTR(args_ptr)->useParentDirectories,
							DPTR(args_ptr)->outputDirectorySet? DPTR(args_ptr)->outputDirectory : "./"
					)
			);
	}
}

void DeltaCompilerInvokerData::ConfigureDeltaCompiler (void) {
	_ASSERT( !deltaCompilerConfigured )
	deltaCompilerConfigured = true;
	
	// Configura compiler from configuration data
	//
	// Add Std functions
	compiler->AddExternFuncs(DeltaStdLib_FuncNames());
	//
	// Set production-mode
	compiler->SetProductionMode(DPTR(args_ptr)->productionMode);
	// Add bytecode paths
	while (DPTR(args_ptr)->HasNextByteCodePath())
		compiler->SetByteCodePath(GetArguments().NextByteCodePath());
	// Add functions files
	{
		char functionDescriptionBuffer[DELTA_COMPILER_INVOKER__MAX_FUNCTION_DESCRIPTION_LENGTH];
		while (DPTR(args_ptr)->HasNextFunctionsFile())
			compiler->AddExternFuncs(
					LoadLibraryFunctionFromFile(
							functionDescriptionBuffer,
							DELTA_COMPILER_INVOKER__MAX_FUNCTION_DESCRIPTION_LENGTH,
							GetArguments().NextFunctionsFile(),
							customFunctionsDescriptions
					)
			);
	}
}

/////////////////////////////

void DeltaCompilerInvokerData::CleanUpDeltaCompiler (void) {
	_ASSERT( deltaCompilerConfigured )
	deltaCompilerConfigured = false;
	
	compiler->CleanUp();
}

/////////////////////////////

void DeltaCompilerInvokerData::Deconfigure (void) {
	_ASSERT( selfConfigured )
	selfConfigured = false;
}

/////////////////////////////
/////////////////////////////
/////////////////////////////

DeltaCompilerInvoker::DeltaCompilerInvoker (int const argc, char* const argv[], OnErrorCallback const onError):
	data(DNEWCLASS(DeltaCompilerInvokerData, (DNEW(DeltaCompilerInvokerCommandLineArguments)) ))
{
	// Initialise
	{
		DPTR(DPTR(data)->compiler)->SetErrorCallback(onError);
	}
	
	// Extract command line arguments
	{
		for (int i = 1; i < argc; ++i) {
			char const* const arg = argv[i];
			//
			// if is short argument
			if (arg[0] == '-' && arg[1] != '-') {
				char const		shortCommand	= arg[1];
				c_const_str_t	value			= NULL;
				if (_OptionAcceptsArgument[_ShortOptionToIndex(shortCommand)]) {
					_USER_ASSERT(arg[2] == '=')
					value = arg + 3;
				}
				_DispatchShortCommand(shortCommand, value, DPTR(data)->GetArguments());
			}
			else // if is long argument
			if (arg[0] == '-' && arg[1] == '-') {
				std::string const	command			(_GetLongCommand(arg + 2));
				c_const_str_t const	command_c_str	= command.c_str();
				c_const_str_t		value			= NULL;
				if (_OptionAcceptsArgument[_LongOptionToIndex(command_c_str)]) {
					_USER_ASSERT( *(arg + 2 + command.length()) == '=' )
					value = arg + 2 + 1 + command.length();
					_USER_ASSERT(value[0] != '\0')
				}
				_DispatchLongCommand(command_c_str, value, DPTR(data)->GetArguments());
			}
			else // is plain argument
				_PlainArgumentCommandLineArgumentHandler(arg, DPTR(data)->GetArguments());
		}
		
		// Also program name
		DPTR(data)->GetArguments().SetProgramName(argv[0]);
	}
	
	// Configure data from command line arguments
	{
		DPTR(data)->ReconfigureFromArguments();
		DPTR(data)->ConfigureDeltaCompiler();
	}
	
}

/////////////////////////////

DeltaCompilerInvoker::~DeltaCompilerInvoker (void) {
	// Clean up delta compiler and configuration data
	{
		DPTR(data)->CleanUpDeltaCompiler();
		DPTR(data)->Deconfigure();
	}	
	//
	DDELETE(data);
}

/////////////////////////////

bool DeltaCompilerInvoker::NeedHelp (void) const
	{ return DPTR(DPTR(data)->args_ptr)->needHelp; }

/////////////////////////////

std::ostream& DeltaCompilerInvoker::WriteHelpTo (std::ostream& o) const {
	unsigned int const shortOptionsLength		= C_ARRAY_LENGTH	(_ShortOptions			);
	unsigned int const longOptionsLength		= C_ARRAY_LENGTH	(_LongOptions			);
	unsigned int const acceptsArgumentsLength	= C_ARRAY_LENGTH	(_OptionAcceptsArgument	);
	unsigned int const length					= acceptsArgumentsLength - 1; // -1 for NULL terminating element
	_ASSERT( shortOptionsLength	== longOptionsLength		)
	_ASSERT( longOptionsLength	== acceptsArgumentsLength	)
	c_const_str_t const empty = "";
	
	o << DPTR(DPTR(data)->args_ptr)->programName << std::endl << '\t';
	for (unsigned int i = 0; i < length; ++i) {
		char const			shortCommand	= _ShortOptions[i];
		c_const_str_t const	longCommand		= _LongOptions[i];
		bool const			acceptsArgument	= _OptionAcceptsArgument[i];
		if (shortCommand == '$')
			o << "    ";
		else
			o << '-' << shortCommand << ", ";
		o << "--" << longCommand << (acceptsArgument? "=argument" : empty) << std::endl << '\t';
	}
	
	return o;
}

/////////////////////////////

void DeltaCompilerInvoker::CompileAll (void) const {
	std::list<CompilationUnit>::const_iterator			i	(DPTR(data)->units.begin());
	std::list<CompilationUnit>::const_iterator const	end	(DPTR(data)->units.end());
	
	DeltaMetaCompiler* compiler = DPTR(data)->compiler;
	for (; i != end; ++i) {
		bool const compileOk	= DPTR(compiler)->Compile(i->GetInputFilePath().c_str(), i->GetSymbolicName().c_str());
		bool const noErrors		= !DPTR(compiler)->ErrorsExist();
		if (compileOk && noErrors) {
			DPTR(compiler)->DumpBinaryCode(i->GetBinaryFilePath().c_str());
			if (DPTR(DPTR(data)->args_ptr)->intermediateCode)
				DPTR(compiler)->DumpInterCode(i->GetIntemediateCodeFilePath().c_str());
			if (DPTR(DPTR(data)->args_ptr)->textualOutput)
				DPTR(compiler)->DumpTextCode(i->GetTextFilePath().c_str());
		}
		compiler->CleanUp();
	}
}

/////////////////////////////


