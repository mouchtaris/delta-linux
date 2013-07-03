#include "CommandLineArguments.h"
#include "CommandLineArgumentsFactory.h"
//
#include <iostream>
//
#include "VMCompLib.h"
#include "DeltaCompilerInit.h"
#include "DeltaMetaCompiler.h"
#include "BuildDependencies.h"
#include "DeltaPureVMFacade.h"
#include "DeltaDebuggedVMFacade.h"
#include "DeltaVirtualMachine.h"
#include "DebugServer.h"
#include "uinit.h"
#include "DDebug.h"
#include "usystem.h"
//
#define ASSERT(EXPR) DASSERT(EXPR);


// Callbacks
static void DeltaPureVirtualMachineFacadePrintingCallback (char const* const msg) {
	std::cout << msg;
}

static void DeltaVirtualMachineLoaderPrintingCallback (char const* const msg) {
	std::cerr << "[DeltaVM Loader]: " << msg << std::endl;
}

static void DeltaVirtualMachineErrorMessageCallback (char const* const msg) {
	std::cerr << "[DeltaVM ERROR]: " << msg << std::endl;
}


static bool VMDebuggingReadyToGo = false;
static void DeltaDebuggedVirtualMachineFacadeOnBeginDebuggingRequestCallback (void) {
	::DeltaPureVirtualMachineFacadePrintingCallback("starting debugging...");
	VMDebuggingReadyToGo = true;
}

static void DeltaDebuggedVirtualMachineFacadeOnEndDebuggingRequestCallback (void) {
	::DeltaPureVirtualMachineFacadePrintingCallback("explicit stop requested");
	exit(0);
}

/////////////////////////////

DEFAULT_COMPILERIFACE_INIT_IMPL(DefaultCompilerInit)

static bool Initialisations (void) {
	INSTALL_DEFAULT_COMPILERIFACE(DeltaMetaCompiler, DefaultCompilerInit);
	INSTALL_DEFAULT_BUILDDEPENDENCIESIFACE(DeltaBuildDependencies);
	DeltaPureVMFacade::Initialise(true);
	return DeltaDebuggedVMFacade::Initialise();
}

static void CleanUps (void) {
	DeltaDebuggedVMFacade::CleanUp();
	DeltaPureVMFacade::CleanUp();
}

static void ConfigureDeltaPureVMFacadeFromArguments (CommandLineArguments const& args) {
	if (args.IsBytecodePathGiven())
		DeltaPureVMFacade::SetByteCodeLoadingPath(args.GetBytecodePath(), true);
	if (args.IsDllImportPathGiven())
		DeltaPureVMFacade::SetDllImportPath(args.GetDllImportPath(), true);
	DeltaPureVMFacade::SetPrintCallback(&DeltaPureVirtualMachineFacadePrintingCallback);
}

static void ConfigureDeltaDebuggedVMFacade (void) {
	DeltaDebuggedVMFacade::SetOnStartCallback(&DeltaDebuggedVirtualMachineFacadeOnBeginDebuggingRequestCallback);
	DeltaDebuggedVMFacade::SetOnStopDebuggingCallback(&DeltaDebuggedVirtualMachineFacadeOnEndDebuggingRequestCallback);
}

static void WaitForDebuggingOkToGo (void) {
	::DeltaVirtualMachineLoaderPrintingCallback("waiting for OK-to-go from debugger");
	while (!VMDebuggingReadyToGo && DeltaDebuggedVMFacade::IsClientConnected()) {
		uprocesssleep(1000);
		::DeltaVirtualMachineLoaderPrintingCallback(".");
	}
	::DeltaVirtualMachineLoaderPrintingCallback(" going");
}

static void LoadAndExecuteMainBinaryFile (CommandLineArguments const& args) {
	DeltaVirtualMachine* const vm = DNEWCLASS(DeltaVirtualMachine,
			(args.GetDetlaVmName().c_str(), args.GetDeltaVmStackSize()));
	util_ui32 vmSerialNo = DPTR(vm)->GetSerialNo();

	if (DPTR(vm)->Load(args.GetMainFilePath().c_str())) {
		::DeltaVirtualMachineLoaderPrintingCallback("starting debugging...");
		DPTR(vm)->Run();

		if (DeltaVirtualMachine::RunTimeErrorsExist()) {
			::DeltaVirtualMachineErrorMessageCallback(uerror::GetSingleton().getreport().c_str());
			uerror::GetSingleton().clear();
		}
	}
	else {
		ASSERT(UERROR_ISRAISED())
		std::string errorMessage("Could not load file \"");
		errorMessage.reserve(4096);
		errorMessage += args.GetMainFilePath();
		errorMessage += "\" for execution: ";
		errorMessage += uerror::GetSingleton().getreport();
		errorMessage += "\n";
		uerror::GetSingleton().clear();
		::DeltaVirtualMachineLoaderPrintingCallback(errorMessage.c_str());
	}

	if (ValidatableHandler::Validate(vm, vmSerialNo))
		DDELETE(vm);
}

/////////////////////////////

int main (int argc, char *argv[]) {
	int result;
	CommandLineArguments options;
	{ CommandLineArgumentsFactory::InitialiseOptionsFromArguments(argc-1, argv+1, options); }

	if (options.IsHelpNeeded()) {
		CommandLineArgumentsFactory::PrintHelpTo(std::cout) << std::endl;
		result = EXIT_SUCCESS;
	}
	else
	if (Initialisations()) {
		ConfigureDeltaPureVMFacadeFromArguments(options);
		ConfigureDeltaDebuggedVMFacade();

		WaitForDebuggingOkToGo();
		LoadAndExecuteMainBinaryFile(options);

		DeltaPureVMFacade::WriteProfileReport(options.GetProfilerOutputPath());

		CleanUps();
		result = EXIT_SUCCESS;
	}
	else {
		std::cerr << "error while initialising components" << std::endl;
		result = EXIT_FAILURE;
	}

	return result;
}