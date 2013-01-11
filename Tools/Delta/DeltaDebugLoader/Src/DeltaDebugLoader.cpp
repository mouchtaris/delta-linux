// DeltaDebugLoader.cpp
// The program to load a Delta compiled program and
// wait for a client debug User Interface to connect and
// take execution control.
// ScriptFighter Project.
// A. Savidis, September 2005.
//

#include <stdio.h>
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaStdLib.h"
#include "VMInit.h"
#include "DebugServer.h"
#include "uinit.h"
#include "DeltaDebugExtensionsInit.h"
#include "DeltaPureVMFacade.h"
#include "DeltaDebuggedVMFacade.h"

////////////////////////////////////////////////////

static DeltaVirtualMachine* vm = (DeltaVirtualMachine*) 0;

////////////////////////////////////////////////////
// When stop is explicitly requested from the client,
// the debugger simply invalidates the execution of the running VM.
//
static bool go = false;
static void OnStartRequested (void)  
	{ go = true; }

static void OnStopRequested (void)  
	{ exit(0); }

static void MainLoop (void) {
	while (!go)
			;
	DPTR(vm)->Run();
}

static void onerror (const char* error) {
	fprintf(stderr, "%s", error);
	fflush(stderr);
}

////////////////////////////////////////////////////

int main (int argc, char** argv) {
	
	if (argc != 2)
		return 1;

	DeltaPureVMFacade::Initialise();
	dseterrorcallback(onerror);

	if (DeltaDebuggedVMFacade::Initialise(DeltaDebuggedVMFacade::NegotiationPortFromArguments(argc, argv))) {

		DeltaDebuggedVMFacade::SetOnStartCallback(OnStartRequested);
		DeltaDebuggedVMFacade::SetOnStopDebuggingCallback(OnStopRequested);

		fprintf(stdout, "\nLoading program '%s' for execution.\n", argv[1]);
		fflush(stdout);

		vm = DNEWCLASS(DeltaVirtualMachine, ("Debug"));
		DASSERT(vm);
		util_ui32 serialNo = vm->GetSerialNo();

		if (DPTR(vm)->Load(argv[1])) {
			if (!DeltaDebuggedVMFacade::IsClientConnected())
				go = true;
			MainLoop();
		}
		else {
			fprintf(stdout, "\nCould not load program '%s'.\n", argv[1]);
			fflush(stdout);
		}

		if (ValidatableHandler::Validate(vm, serialNo))
			DDELETE(vm);
	}

	DeltaDebuggedVMFacade::CleanUp();
	DeltaPureVMFacade::CleanUp();

	return 0;
}

////////////////////////////////////////////////////
