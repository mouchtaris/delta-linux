/**
 *	main.cpp
 *
 *	-- Delta Compiler --
 *
 *	Driver program for the Delta Compiler.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "DeltaMetaCompiler.h"
#include "DeltaStdLibFuncNames.h"
#include "DeltaCompilerInit.h"
#include "StagedCompilationHandler.h"
#include "ustrings.h"

#include "Common.h"
#include "StringUtils.h"

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#define	FILENAME(name, ext) \
			(std::string(name) + std::string(ext)).c_str()

#define	SRC(file)		FILENAME(file, ".dsc")
#define	BIN(file)		FILENAME(file, ".dbc")
#define	TXT(file)		FILENAME(file, ".txt")
#define	ICODE(file)		FILENAME(file, ".idc")

//-------------------------------------------------------//
//---- Global Variables ---------------------------------//

std::list<const char **> libfuncs;

//-------------------------------------------------------//
//---- utility functors ---------------------------------//

#define MAX_LIBRARY_FUNC_DESCRIPTION	1024

void LoadLibraryFunctionFile(DeltaCompiler* compiler, const std::string& path) {
	StdStringVec funcVector;
	std::ifstream in(path.c_str());
	if(in) {
		char str[MAX_LIBRARY_FUNC_DESCRIPTION];
		while(!in.eof()) {
			in.getline(str, MAX_LIBRARY_FUNC_DESCRIPTION - 1);
			const std::string value = util::strip<std::string>(str, UTIL_WHITESPACE);
			if(!value.empty())
				funcVector.push_back(value);
		}
	}
	if(!funcVector.empty()) {
		const char **funcs = DNEWARR(const char*, funcVector.size() + 1);
		for(unsigned i = 0; i < funcVector.size(); ++i)
			funcs[i] = ucopystr(funcVector[i]);
		funcs[funcVector.size()] = (const char *) 0;
		DPTR(compiler)->AddExternFuncs(funcs);
		libfuncs.push_back(funcs);
	}
}

//-------------------------------------------------------//
//---- free functions -----------------------------------//

static void onerror (const char* error, void*) {
	std::cout << error;
	std::cout.flush();
}

//-------------------------------------------------------//
//---- main function ------------------------------------//

int main(int argc, char* argv[]) {
	//-- Parse command line options
	boost::program_options::options_description desc("USAGE");
	desc.add_options()
		("help,h",		"produce this help message")
		("icode,i",		"dump i-code quads in a file")
		("text,t",		"dump text code in a file")
		("debug,g",		"produce debug symbols")

		("input,f",
			boost::program_options::value<StdStringVec>(),
			"input source file for compilation")
		("funcfile,e",
			boost::program_options::value<StdStringVec>(),
			"external library function files")
		("symbolic_names,s",
			boost::program_options::value<StdStringVec>(),
			"symbolic names for the files to be compiled")
		("bytecode_path,b",
			boost::program_options::value<std::string>(),
			"byte code loading paths (as a single string with semicolon separated values)")
		("output_path",
			boost::program_options::value<std::string>()->default_value("./"),
			"target directory of output files")
		("output,o",
			boost::program_options::value<std::string>()->default_value(""),
			"target name for output files")
		("stage_output_path",
			boost::program_options::value<std::string>()->default_value("./"),
			"target directory of stage output files")
		("build_server_host",
			boost::program_options::value<std::string>()->default_value("localhost"),
			"build server host")
		("build_server_port",
			boost::program_options::value<util_ui32>()->default_value(0),
			"build server port")
		("debugged,d",	"activate debugger backend")
		("negotiationport,n",
			boost::program_options::value<util_ui32>()->default_value(0),
			"debug client-supplied negotiation port")
	;

	boost::program_options::positional_options_description p;
	p.add("input", -1);

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

	//-- Check if out directory exists
	std::string output = vars["output"].as<std::string>();
	const std::string output_path = vars["output_path"].as<std::string>();
	if (!boost::filesystem::exists(output_path)) {
		std::cout << "Output path " << output_path << " not found!" << std::endl;
		system("pause");
		return -1;
	}
	const std::string stage_output_path = vars["stage_output_path"].as<std::string>();
	if (!boost::filesystem::exists(stage_output_path)) {
		std::cout << "Stage output path " << stage_output_path << " not found!" << std::endl;
		system("pause");
		return -1;
	}

	DeltaCompilerInit::VMOptions vmOptions;
	vmOptions.debug = vars.count("debugged");
	vmOptions.negotiationPort = vars["negotiationport"].as<util_ui32>();
	DeltaCompilerInit::Initialise(vmOptions);

	DeltaMetaCompiler* compiler = DNEW(DeltaMetaCompiler);
	DPTR(compiler)->SetErrorCallback(onerror);

	const std::string host = vars["build_server_host"].as<std::string>();
	util_ui32 port = vars["build_server_port"].as<util_ui32>();

	if (!host.empty() && port)
		StagedCompilationHandler::Initialise(host, port);

	if(vars.count("bytecode_path"))
		DPTR(compiler)->SetByteCodePath(vars["bytecode_path"].as<std::string>());
	DPTR(compiler)->AddExternFuncs(DeltaStdLib_FuncNames());

	//-- Set compilation flags
	DPTR(compiler)->SetProductionMode(!vars.count("debug"));

	//-- Load external library function file
	if(vars.count("funcfile"))
		BOOST_FOREACH(const std::string& file, vars["funcfile"].as<StdStringVec>())
			LoadLibraryFunctionFile(compiler, file);

	//-- Compile
	if(vars.count("input")) {
		StdStringVec symbolicNames = vars.count("symbolic_names") ?
			vars["symbolic_names"].as<StdStringVec>() : StdStringVec();
		unsigned i = 0;
		BOOST_FOREACH(const std::string& file, vars["input"].as<StdStringVec>()) {
			const std::string symbolic = i < symbolicNames.size() ? symbolicNames[i++] : file;
			if (!host.empty() && port)
				StagedCompilationHandler::HandleSource(symbolic, stage_output_path, compiler);
			if (DPTR(compiler)->Compile(file.c_str(), symbolic.c_str()) && !DPTR(compiler)->ErrorsExist()) {
				
				if (output.empty()) {
					//-- If output not specified use the file's base name
					output = file;
					std::replace(output.begin(), output.end(), '\\', '/');
					if (output.find('/') != std::string::npos)
						output = output.substr(output.find_last_of('/') +1);
					output = output.substr(0, output.find_last_of('.'));
				}

				//-- Dump binary code
				const std::string out = output_path + output;
				DPTR(compiler)->DumpBinaryCode(BIN(out));

				//-- Dump i-code and text t-code
				if (vars.count("icode"))
					DPTR(compiler)->DumpInterCode(ICODE(out));
				if (vars.count("text"))
					DPTR(compiler)->DumpTextCode(TXT(out));
			}
			DPTR(compiler)->CleanUp();
		}
	}
	DDELETE(compiler);

	BOOST_FOREACH(const char **funcs, libfuncs) {
		for(uint i = 0; funcs[i]; ++i)
			DDELARR(funcs[i]);
		DDELARR(funcs);
	}
	if (!host.empty() && port)
		StagedCompilationHandler::CleanUp();
	DeltaCompilerInit::CleanUp();
	return 0;
}
