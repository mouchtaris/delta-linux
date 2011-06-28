/**
 *	main.cpp
 *
 *	-- Delta Compiler --
 *
 *	Driver program for the Delta compiler.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "CompilerAPI.h"
#include "DeltaStdLibFuncNames.h"
#include "DeltaCompilerInit.h"
#include "DDebug.h"
#include "Optimizer.h"
#include "uinit.h"

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

void LoadLibraryFunctionFile(const std::string& path) {
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
		DeltaCompiler::AddExternFuncs(funcs);
		libfuncs.push_back(funcs);
	}
}

//-------------------------------------------------------//
//---- free functions -----------------------------------//

static void onerror (const char* error)
{
	std::cerr << error;
	std::cerr.flush();
}

int main(int argc, char* argv[])
{
	try {
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
			("output_path,o",
				boost::program_options::value<std::string>()->default_value("./"),
				"target directory of output files")
		;

		boost::program_options::positional_options_description p;
		p.add("input", -1);

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
			  options(desc).positional(p).run(), vm);
		boost::program_options::notify(vm);

		//-- Check arguments for help
		if (argc == 1 || vm.count("help")) {
			std::cout << desc << std::endl;
			system("pause");
			return -1;
		}

		//-- Check if out directory exists
		const std::string output_path = vm["output_path"].as<std::string>();
		if (!boost::filesystem::exists(output_path)) {
			std::cout << "Output path " << output_path << " not found!" << std::endl;
			system("pause");
			return -1;
		}

		dinit(onerror);
		UtilPackage::Initialise();
		DeltaCompilerInit::Initialise();

		DeltaCompiler::SetErrorCallback(onerror);
		if(vm.count("bytecode_path"))
			DeltaCompiler::SetByteCodePath(vm["bytecode_path"].as<std::string>());
		DeltaCompiler::AddExternFuncs(DeltaStdLib_FuncNames());

		//-- Set debug or production mode
		DeltaCompiler::SetProductionMode(!vm.count("debug"));

		//-- Load external library function file
		if(vm.count("funcfile"))
			BOOST_FOREACH(const std::string& file, vm["funcfile"].as<StdStringVec>())
				LoadLibraryFunctionFile(file);

		//-- Compile
		if(vm.count("input"))
		{
			StdStringVec symbolicNames = vm.count("symbolic_names") ?
				vm["symbolic_names"].as<StdStringVec>() : StdStringVec();
			unsigned i = 0;
			BOOST_FOREACH(const std::string& file, vm["input"].as<StdStringVec>())
			{
				const std::string symbolic = i < symbolicNames.size() ? symbolicNames[i++] : file;
				if (DeltaCompiler::Compile(file.c_str(), symbolic.c_str()) && !DeltaCompErrorsExist())
				{
					//-- Get file's base name
					std::string basename = file;
					std::replace(basename.begin(), basename.end(), '\\', '/');
					if (basename.find('/') != std::string::npos)
						basename = basename.substr(basename.find_last_of('/') +1);
					basename = basename.substr(0, basename.find_last_of('.'));

					//-- dump binary code
					const std::string out = output_path + basename;
					DeltaCompiler::DumpBinaryCode(BIN(out));

					//-- dump i-code and text t-code
					if (vm.count("icode"))
						DeltaCompiler::DumpInterCode(ICODE(out));
					if (vm.count("text"))
						DeltaCompiler::DumpTextCode(TXT(out));
				}
				DeltaCompiler::CleanUp();
			}
		}

		BOOST_FOREACH(const char **funcs, libfuncs) {
			for(uint i = 0; funcs[i]; ++i)
				DDELARR(funcs[i]);
			DDELARR(funcs);
		}
		DeltaCompilerInit::CleanUp();
		UtilPackage::CleanUp();
		dclose();
	}
	catch(...) { std::cout << *argv << ": Invalid arguments." << std::endl;	}
	return 0;
}
