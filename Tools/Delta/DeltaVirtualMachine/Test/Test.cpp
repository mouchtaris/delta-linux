// Virtual machine test.
// A. Savidis, revised for June 2002.
// Revised June 2008 using new facades.
//
#include "DDebug.h"
#include "VMCompLib.h"
#include "DeltaVirtualMachine.h"
#include "uerrorclass.h"
#include "DeltaPureVMFacade.h"
#include "DeltaExceptionHandling.h"
#include "DeltaDebugDynamicActivator.h"
#include "DeltaCompilerInit.h"
#include "DeltaMetaCompiler.h"
#include "BuildDependencies.h"
#include "MetaCompilerLib.h"
#include "Unparse.h"
#include "usystem.h"
#include "ufiles.h"

#define	TEST_FILE argv[1]

static void OnWarning (const char* warning) {
	fprintf(stderr, "%s", warning);
	fflush(stderr);
}

static void onerror (const char* error) {
	fprintf(stderr, "%s", error);
	fflush(stderr);
}

#ifdef	_DEBUG
#define	DEBUGGER_COMMON		"DebuggerCommonD.dll"
#define	DEBUGGER_BACKEND	"DebuggerBackendD.dll"
#else
#define	DEBUGGER_COMMON		"DebuggerCommon.dll"
#define	DEBUGGER_BACKEND	"DebuggerBackend.dll"
#endif

EMPTY_COMPILERIFACE_INIT_IMPL(NoCompilerInit)

///////////////////////////////////////////

int main (int argc, char* argv[]) {

	if (argc != 2)
		return 1;

	DeltaPureVMFacade::Initialise();
	dseterrorcallback(onerror);
	
	INSTALL_DEFAULT_COMPILERIFACE(DeltaMetaCompiler, NoCompilerInit);
	INSTALL_DEFAULT_BUILDDEPENDENCIESIFACE(DeltaBuildDependencies);

	DeltaDebugDynamicActivator::GetSingleton().AddLibrary(
		DEBUGGER_COMMON, DELTA_DEBUG_DYNAMIC_ACTIVATOR_NO_INIT, DELTA_DEBUG_DYNAMIC_ACTIVATOR_NO_CLEANUP
	);
	DeltaDebugDynamicActivator::GetSingleton().AddLibrary(
		DEBUGGER_BACKEND, "DeltaDebuggedVMFacade_Initialise", "DeltaDebuggedVMFacade_CleanUp"
	);

	DeltaVirtualMachine::SetWarningCallback(OnWarning);
	DeltaVirtualMachine* vm = DNEWCLASS(DeltaVirtualMachine, ("test"));
	util_ui32 serialNo = vm->GetSerialNo();

	Unparse_SingletonCreate();
	Install_DeltaMetaCompiler_Lib();

	if (DPTR(vm)->Load(TEST_FILE))
		DPTR(vm)->Run();

	if (DeltaVirtualMachine::RunTimeErrorsExist()) {
		printf("\n%s\n", UERROR_GETREPORT().c_str());
		UERROR_CLEAR();
	}

	if (ValidatableHandler::Validate(vm, serialNo))
		DDELETE(vm);

	Unparse_SingletonDestroy();
	CleanUp_DeltaMetaCompiler_Lib();

	DeltaPureVMFacade::WriteProfileReport("ProfilerReport.txt");
	DeltaPureVMFacade::CleanUp();

	return 0;
}