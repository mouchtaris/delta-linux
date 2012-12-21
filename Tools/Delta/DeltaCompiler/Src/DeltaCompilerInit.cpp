// DeltaCompilerInit.cpp
// Global program start-up and ending actions.
// ScriptFigher Project.
// A. Savidis, May 2003.
//
#include "DeltaCompilerInit.h"
#include "DeltaMetaCompiler.h"
#include "BuildDependencies.h"
#include "Unparse.h"
#include "MetaCompilerLib.h"
#include "DeltaPureVMFacade.h"
#include "DeltaVirtualMachine.h"
#include "DeltaDebuggedVMFacade.h"
#include "DeltaDebugClientBreakPoints.h"
#include "VMCompLib.h"
#include "DDebug.h"
#include "uinit.h"

/////////////////////////////////////////////////////////

static util_ui32	initCounter		= 0;
static bool			initialising	= false;
static bool			cleaningup		= false;
static bool			debug			= false;

//-------------------------------------------------------

static void onError (const char* error) {
	fprintf(stderr, "%s", error);
	fflush(stderr);
}

static void OnWarning(const char* warning) {
	fprintf(stderr, "%s", warning);
	fflush(stderr);
}

//-------------------------------------------------------

static void OnStopRequested(void)
	{ exit(0); }

static volatile bool execute = false;

static void OnStartRequested (void) 
	{ execute = true; }

static void WaitStartMessage (void)
	{ while (!execute) ; }

/////////////////////////////////////////////////////////

namespace DeltaCompilerInit {

	DCOMPLIB_FUNC void Initialise (const VMOptions& vmOptions) {
		if (initialising)
			return;
		initialising = true;
		if (!initCounter++) {
			debug = vmOptions.debug;
			dinit(onError);
			UtilPackage::Initialise();
			Unparse_SingletonCreate();
			INSTALL_DEFAULT_COMPILERIFACE(DeltaMetaCompiler);						
			INSTALL_DEFAULT_BUILDDEPENDENCIESIFACE(DeltaBuildDependencies);
			
			DeltaPureVMFacade::Initialise();
			DeltaVirtualMachine::SetWarningCallback(OnWarning);
			
			if (debug) {
				DeltaDebuggedVMFacade::Initialise(vmOptions.negotiationPort);
				DeltaDebuggedVMFacade::SetOnStartCallback(OnStartRequested);
				DeltaDebuggedVMFacade::SetOnStopDebuggingCallback(OnStopRequested);
				WaitStartMessage();
			}

			Install_DeltaMetaCompiler_Lib();
		}
		initialising = false;
	}

	DCOMPLIB_FUNC void CleanUp (void) {
		if (cleaningup)
			return;
		cleaningup = true;
		DASSERT(initCounter);
		if (!--initCounter) {
			Unparse_SingletonDestroy();
			CleanUp_DeltaMetaCompiler_Lib();
			if (debug)
				DeltaDebuggedVMFacade::CleanUp();
			DeltaPureVMFacade::CleanUp();
			UtilPackage::CleanUp();
			dclose();
		}
		cleaningup = false;
	}
}

/////////////////////////////////////////////////////////
