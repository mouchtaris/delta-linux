/**
 *	main.cpp
 *
 *	-- Delta Virtual Machine with Debugger Extensions --
 *
 *	Driver program for the Delta Virtual Machine.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#include "Delta.h"
#include "DeltaStdLib.h"
#include "DeltaPureVMFacade.h"
#include "DeltaDebuggedVMFacade.h"
#include "DebugServer.h"

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <vector>
#include <string>

//-------------------------------------------------------//
//---- free functions -----------------------------------//

static void OnWarning(const char* warning) {
	std::cerr << warning;
	std::cerr.flush();
}

//-----------------------------------------------------------------------

static void OnStopRequested(void)
	{ exit(0); }

static volatile bool execute = false;

static void OnStartRequested (void) 
	{ execute = true; }

static void WaitStartMessage (void)
	{ while (!execute) ; }

//-----------------------------------------------------------------------

int main (int argc, char* argv[])
{
	//-- Parse command line options
	boost::program_options::options_description desc("USAGE");
	desc.add_options()
		("help,h",		"produce this help message")
		("input,f",
			boost::program_options::value<std::string>(),
			"input Delta binary file to run")
		("bytecode_path,b",
			boost::program_options::value<std::string>(),
			"byte code loading paths (as a single string with semicolon separated values)")
		("dllimport_path,b",
			boost::program_options::value<std::string>(),
			"additional paths for dllimport (as a single string with semicolon separated values)")
		("negotiationport,n",
			boost::program_options::value<unsigned int>(),
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

	DeltaPureVMFacade::Initialise();
	if(vars.count("bytecode_path"))
		DeltaPureVMFacade::SetByteCodeLoadingPath(vars["bytecode_path"].as<std::string>());
	if(vars.count("dllimport_path"))
		DeltaPureVMFacade::SetDllImportPath(vars["dllimport_path"].as<std::string>());
	DeltaVirtualMachine::SetWarningCallback(OnWarning);

	if (DeltaDebuggedVMFacade::Initialise(argc, argv)) {

		DeltaDebuggedVMFacade::SetOnStartCallback(OnStartRequested);
		DeltaDebuggedVMFacade::SetOnStopDebuggingCallback(OnStopRequested);

		std::cout << "Accepted debug client; loading Delta program." << std::endl;
		std::cout.flush();

		DeltaVirtualMachine* vm = DNEWCLASS(DeltaVirtualMachine, ("main_executable"));
		DASSERT(vm);
		if (!DPTR(vm)->Load(vars["input"].as<std::string>().c_str())) {
			DASSERT(UERROR_ISRAISED());
			std::cout << uerror::GetSingleton().getreport() << std::endl;
			uerror::GetSingleton().clear();
		}
		else {
			std::cout << "start debugging: " << vars["input"].as<std::string>() << std::endl;
			std::cout.flush();

			WaitStartMessage();
			DPTR(vm)->Run();

			if (DeltaVirtualMachine::RunTimeErrorsExist()) {
				std::cout << uerror::GetSingleton().getreport() << std::endl;
				uerror::GetSingleton().clear();
			}
		}
		DDELETE(vm);
	}

	DeltaDebuggedVMFacade::CleanUp();
	DeltaPureVMFacade::CleanUp();

	system("pause");
	return 0;
}

//-----------------------------------------------------------------------