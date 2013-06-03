/**
 *	main.cpp
 *
 *	-- Aspect Compiler --
 *
 *	Driver program for the Aspect Compiler.
 *
 *	Yannis Lilis <lilis@ics.forth.gr>
 *	July 2012
 */
#include "Delta.h"
#include "VMCompLib.h"
#include "DeltaCompilerInit.h"
#include "DeltaMetaCompiler.h"
#include "BuildDependencies.h"
#include "DeltaPureVMFacade.h"
#include "AspectCompiler.h"
#include "BuildClient.h"
#include "ASTUtilVisitors.h"

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <vector>
#include <string>

typedef std::vector<std::string> StringVec;

//-------------------------------------------------------//
//---- free functions -----------------------------------//

static void OnWarning(const char* warning) {
	std::cerr << warning;
	std::cerr.flush();
}

//-----------------------------------------------------------------------

static void OnError (const char* error, void*) {
	std::cout << error;
	std::cout.flush();
}

//-----------------------------------------------------------------------

static std::string originalSource;
static std::string output_path;

static void OnParse(AST::Node* ast, void* closure) {
	BuildClient::DoGetSourceReferences(originalSource);
	BuildClient::WaitAnyMessage();
	if (!BuildClient::IsConnectionBroken() && BuildClient::GetResponseType() == Build_SourceReferences) {
		typedef std::map<util_ui32, AST::Node::SourceInfoReferences> InitialSourceReferences;
		InitialSourceReferences sourceRefs;
		BuildClient::GetSourceReferences(sourceRefs);
		BuildClient::ResponseProcessed();
		(AST::SourceReferenceSetter(sourceRefs))(ast);
	}
}

static void OnTransformation(
	const std::string&						source,
	const std::string&						text,
	const AspectCompiler::LineMappings&		lineMappings,
	const AspectCompiler::SourceReferences&	sourceRefs,
	util_ui32								index,
	bool									final,
	void*									closure
) {
	DeltaMetaCompiler::DumpSource(output_path + source, text);
	BuildClient::DoAddSource(originalSource, source, lineMappings, sourceRefs, "aspect", index, final);
	BuildClient::WaitAnyMessage();
	DASSERT(BuildClient::IsConnectionBroken() || BuildClient::GetResponseType() == Build_SourceAdded);
	BuildClient::ResponseProcessed();
}

//-----------------------------------------------------------------------

#ifdef	_DEBUG
#define	DEBUGGER_COMMON		"DebuggerCommonD.dll"
#define	DEBUGGER_BACKEND	"DebuggerBackendD.dll"
#else
#define	DEBUGGER_COMMON		"DebuggerCommon.dll"
#define	DEBUGGER_BACKEND	"DebuggerBackend.dll"
#endif

int main (int argc, char* argv[])
{
	//-- Parse command line options
	boost::program_options::options_description desc("USAGE");
	desc.add_options()
		("help,h",		"produce this help message")
		("input,f",
			boost::program_options::value<std::string>(),
			"input Delta source file to transform")
		("symbolic,s",
			boost::program_options::value<std::string>(),
			"symbolic name for the file to be transformed")
		("aspects,a",
			boost::program_options::value<StringVec>(),
			"aspect transformation binaries")
		("output_path",
			boost::program_options::value<std::string>()->default_value("./"),
			"target directory of output files")
		("bytecode_path,b",
			boost::program_options::value<std::string>(),
			"byte code loading paths (as a single string with semicolon separated values)")
		("dllimport_path,d",
			boost::program_options::value<std::string>(),
			"additional paths for dllimport (as a single string with semicolon separated values)")
		("build_server_host",
			boost::program_options::value<std::string>()->default_value("localhost"),
			"build server host")
		("build_server_port",
			boost::program_options::value<util_ui32>()->default_value(0),
			"build server port")
		("debugged,g", "activate debugger backend")
		("negotiationport,n",
			boost::program_options::value<util_ui32>()->default_value(0),
			"debug client-supplied negotiation port")
	;

	boost::program_options::positional_options_description p;
	p.add("aspects", -1);

	boost::program_options::variables_map vars;
	try {
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
			  options(desc).positional(p).run(), vars);
		boost::program_options::notify(vars);
	}
	catch(...) {
		std::cout << *argv << ": Invalid arguments." << std::endl;		
		system("pause");
		return -1;
	}

	//-- Check arguments for help
	if (argc == 1 || vars.count("help")) {
		std::cout << desc << std::endl;
		system("pause");
		return -1;
	}

	output_path = vars["output_path"].as<std::string>();
	if (!boost::filesystem::exists(output_path)) {
		std::cout << "Output path " << output_path << " not found!" << std::endl;
		system("pause");
		return -1;
	}

	DeltaCompilerInit::VMOptions vmOptions;
	vmOptions.debug = vars.count("debugged");
	vmOptions.negotiationPort = vars["negotiationport"].as<util_ui32>();

	DeltaCompilerInit::Initialise(vmOptions);

	DeltaDebugDynamicActivator::GetSingleton().AddLibrary(
		DEBUGGER_COMMON, DELTA_DEBUG_DYNAMIC_ACTIVATOR_NO_INIT, DELTA_DEBUG_DYNAMIC_ACTIVATOR_NO_CLEANUP
	);
	DeltaDebugDynamicActivator::GetSingleton().AddLibrary(
		DEBUGGER_BACKEND, "DeltaDebuggedVMFacade_Initialise", "DeltaDebuggedVMFacade_CleanUp"
	);
	DeltaVirtualMachine::SetWarningCallback(OnWarning);

	AspectCompiler* compiler = DNEW(AspectCompiler);
	DPTR(compiler)->SetErrorCallback(OnError);

	const std::string host = vars["build_server_host"].as<std::string>();
	util_ui32 port = vars["build_server_port"].as<util_ui32>();
	if (!host.empty() && port) {
		BuildClient::Initialise();
		BuildClient::Connect(host, port);
		DPTR(compiler)->SetTransformationCallback(OnTransformation);
		DPTR(compiler)->SetParseCallback(OnParse);
		originalSource = vars["symbolic"].as<std::string>();
	}

	if (DPTR(compiler)->ParseFile(vars["input"].as<std::string>(), vars["symbolic"].as<std::string>())) {
		const StringVec vec = vars["aspects"].as<StringVec>();
		AspectCompiler::StringList aspects = AspectCompiler::StringList(vec.begin(), vec.end());
		DPTR(compiler)->ApplyTransformations(aspects);
	}
	DDELETE(compiler);

	if (!host.empty() && port)
		BuildClient::CleanUp();
	DeltaCompilerInit::CleanUp();

	return 0;
}

//-----------------------------------------------------------------------
