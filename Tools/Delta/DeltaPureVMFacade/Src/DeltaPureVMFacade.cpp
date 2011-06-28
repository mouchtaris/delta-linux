// DeltaPureVMFacade.cpp
// Facade to initialise / cleanup VM usage in pure mode.
// ScriptFigher Project.
// A. Savidis, July2008.
//

#include <assert.h>
#include "DeltaPureVMFacade.h"
#include "DeltaVirtualMachine.h"
#include "VMInit.h"
#include "uinit.h"
#include "usystem.h"
#include "DDebug.h"
#include "DeltaAssertExtensions.h"
#include "DeltaStdLib.h"
#include "MiscLib.h"
#include "RcLoaderAPI.h"
#include <string>
#include "DeltaDebugDynamicActivator.h"
#include "DeltaProfiler.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

#define	DDEBUG_LOGFILE	"derror.txt"

typedef DeltaPureVMFacade::CleanUpCallback CleanUpCallback;
typedef std::list< ucallbackwithclosure<CleanUpCallback> > CleanUpCallbackList;

static bool 				isInitialised		= false;
static bool 				cppAsserting		= false;
static CleanUpCallbackList*	cleanupCallbacks	= (CleanUpCallbackList*) 0;

////////////////////////////////////////////////////////////////////

POSSIBLY_UNUSED_STATIC_FUNCTION(void CPPAssertCallback (bool, const char*, uvoidvoid_f))
static void CPPAssertCallback (bool val, const char* expr, uvoidvoid_f cleaner) {

	if (!val) {
		
		if (cppAsserting)
			abort();	// Escape since the user-supplied cleaner caused another assertion!
		
		cppAsserting = true;
		(*cleaner)();

#ifdef	_DEBUG
		assert(false);
#else
		umodalmessage("Assertion failure", std::string(expr) + ":an assertion failed!");
		abort();
#endif
	}
}

////////////////////////////////////////////////////////////

static void DeltaAbortCallback (void) 
	{ uabort(); }

//*********************************

const std::string DeltaPureVMFacade::AssertMessage (
		const std::string&	expr, 
		util_ui32			line, 
		const char*			source, 
		const char*			id
	) {
	return	uconstructstr(
				"The assertion '%s' failed,\nline %d, source file '%s',\nvm with id '%s'!\nAbort execution?",
				expr.c_str(),
				line,
				source,
				id
			);
}

//*********************************

#ifdef	_MSC_VER
static LRESULT CALLBACK MessageProc(__in  int code, WPARAM wParam, __in LPARAM lParam) 
	{ return code == HC_ACTION ? 1 : CallNextHookEx(NULL, code, wParam, lParam); }
#endif

static util_ui32 DeltaAssertInterfaceCallback (DeltaVirtualMachine* vm, const std::string& expr, void* unused) {

	std::string	msg(
					DeltaPureVMFacade::AssertMessage(
						expr,
						DPTR(vm)->Line(), 
						DPTR(vm)->Source(), 
						DPTR(vm)->Id()
					)
				);

#ifdef _MSC_VER
	HHOOK hook = SetWindowsHookEx(WH_GETMESSAGE, &MessageProc, NULL, GetCurrentThreadId());
	int result =	MessageBox(
						GetForegroundWindow(),
						msg.c_str(),
						"Assertion failed!",
						MB_APPLMODAL | MB_YESNO
					);

	UnhookWindowsHookEx(hook);
	return result == IDYES ? ASSERT_ABORT : ASSERT_CONTINUE;
#else
	fprintf(stderr, "Assertion failed!\n%s\n1.Abort\t2.Continue\n", msg.c_str());
	util_ui32 i, j;
	do
		{ printf(">"); j = scanf("%u", &i); }
	while (j != 1 || (i != 1 && i != 2));
	return i == 1 ? ASSERT_ABORT : ASSERT_CONTINUE;
#endif
}

DeltaPureVMFacade::AssertInterface DeltaPureVMFacade::GetAssertInterface (void)
	{ return &DeltaAssertInterfaceCallback; }

////////////////////////////////////////////////////////////////////

static void InvokeCleanUpCallbacks (void) {
	std::for_each(
		DPTR(cleanupCallbacks)->begin(),
		DPTR(cleanupCallbacks)->end(),
		ucallbackwithclosure<CleanUpCallback>::invoke()
	);
}

//*********************************

void DeltaPureVMFacade::AddOnJustBeforeCleanUpCallback (CleanUpCallback f, void* closure) {
	DASSERT(cleanupCallbacks);
	DPTR(cleanupCallbacks)->push_back(ucallbackwithclosure<CleanUpCallback>(f, closure));
}

//*********************************

void DeltaPureVMFacade::RemoveOnJustBeforeCleanUpCallback (CleanUpCallback f, void* closure) {
	DASSERT(cleanupCallbacks);
	CleanUpCallbackList::iterator i =	std::find(
											DPTR(cleanupCallbacks)->begin(),
											DPTR(cleanupCallbacks)->end(),
											ucallbackwithclosure<CleanUpCallback>(f, closure)
										);
	DASSERT(i != DPTR(cleanupCallbacks)->end());
	DPTR(cleanupCallbacks)->erase(i);
}

////////////////////////////////////////////////////////////////////

POSSIBLY_UNUSED_STATIC_FUNCTION(void ErrorDefenseCallback (const char*))
static void ErrorDefenseCallback (const char* msg) {
	FILE* fp = fopen(DDEBUG_LOGFILE, "at");
	fprintf(fp, "%s\n" , msg);
	fclose(fp);
}

////////////////////////////////////////////////////////////////////

void DeltaPureVMFacade::Initialise (bool errorInvalidatesAll) {

	DASSERT(!IsInitialised());

	dinit(ErrorDefenseCallback);
	dsetassertfunc(CPPAssertCallback);

	UtilPackage::Initialise();
	VirtualMachinePackage::Initialise();
	ResourceLoader::SingletonCreate();
	Install_StdLib();

	DeltaVirtualMachine::SetAnyErrorInvalidatesAll(errorInvalidatesAll);
	DeltaAssertExtensions::SetAbortCallback(DeltaAbortCallback);
	DeltaAssertExtensions::SetAssertInterfaceCallback(DeltaAssertInterfaceCallback);

	isInitialised = true;
	unew(cleanupCallbacks);
}

bool DeltaPureVMFacade::IsInitialised (void) 
	{ return isInitialised; }

void DeltaPureVMFacade::SetByteCodeLoadingPath(const std::string& path, bool prioritised) 
	{ DASSERT(IsInitialised()); SetByteCodeLoadingPath_StdLib(path, prioritised); }

void DeltaPureVMFacade::SetDllImportPath (const std::string& path, bool prioritised)
	{ DASSERT(IsInitialised()); SetDllImportPath_StdLib(path, prioritised); }

////////////////////////////////////////////////////////////////////

void DeltaPureVMFacade::WriteProfileReport (const std::string& path) 
	{ DeltaProfiler::GetSingleton().WriteReport(path); }

////////////////////////////////////////////////////////////////////

void DeltaPureVMFacade::CleanUp (void) {

	DASSERT(isInitialised);

	if (DeltaDebugDynamicActivator::GetSingleton().HasLoaded())
		DeltaDebugDynamicActivator::GetSingleton().Unload();

	InvokeCleanUpCallbacks();
	udelete(cleanupCallbacks);

	isInitialised = false;

	CleanUp_StdLib();
	VirtualMachinePackage::CleanUp();

	UtilPackage::CleanUp();
	ResourceLoader::SingletonDestroy();
	
	dclose();
}

////////////////////////////////////////////////////////////////////

void DeltaPureVMFacade::SetPrintCallback (void (*f)(const char*)) {
	DASSERT(IsInitialised());
	DeltaSet_print_OutputFunc(f);
}

//------------------------------------------------------------------

typedef DeltaPureVMDebuggerProxy::DebuggerInterface	Debugger;

class NoDebugger : public Debugger {
	public:
	virtual void 				CleanUp (void) const {}
	virtual void 				CloseClientConnection (void) const{}
	virtual bool 				IsClientConnected (void) const { return false; }
	virtual void 				SetOnBreakPointCallbacks (uvoidvoid_f onEnter, uvoidvoid_f onLeave) const {}
	virtual void 				SetOnStopDebuggingCallback (uvoidvoid_f onStop) const {}
	virtual void 				SetOnStartCallback (uvoidvoid_f onGo) const {}
	virtual ActivationType		GetActivationType (void) const { return None; }

	NoDebugger (void){}
	virtual ~NoDebugger(){}
};

static NoDebugger	noDebugger;
static Debugger*	currDebugger	=	&noDebugger;

////////////////////////////////////////////////////////////////////

DeltaPureVMDebuggerProxy::DebuggerInterface& DeltaPureVMDebuggerProxy::GetDebugger (void)
	{ return *currDebugger; }

void DeltaPureVMDebuggerProxy::SetDebugger (DebuggerInterface* debugger)
	{ DASSERT(debugger); currDebugger = debugger; }

//------------------------------------------------------------------
