// DeltaDebuggedVMFacade.cpp
// Facade for extra initialise / cleanup for VM usage in 
// debugged mode.
// ScriptFigher Project.
// A. Savidis, July2008.
//

#include "DeltaDebuggedVMFacade.h"
#include "DeltaPureVMFacade.h"
#include "DeltaDebugExtensionsInit.h"
#include "DebugServer.h"
#include "DeltaAssertExtensions.h"
#include "DeltaDebugServerBreakPoints.h"
#include "DDebug.h"
#include "udynamiclibloader.h"

#ifdef	_MSC_VER
#include <windows.h>
#endif

////////////////////////////////////////////////////////////

typedef DeltaPureVMDebuggerProxy::DebuggerInterface	Debugger;

class RealDebugger : public Debugger {
	public:
	virtual void 			CleanUp (void) const				
								{ DeltaDebuggedVMFacade::CleanUp(); }
	virtual void 			CloseClientConnection (void) const	
								{ DeltaDebuggedVMFacade::CloseClientConnection(); }
	virtual bool 			IsClientConnected (void) const		
								{ return DeltaDebuggedVMFacade::IsClientConnected(); }
	virtual ActivationType	GetActivationType (void) const {
								if (!IsClientConnected()) return None;
								return DeltaDebuggedVMFacade::ClientWasAttached() ? Attached : UponStart;
							}
	void					SetOnBreakPointCallbacks (uvoidvoid_f onEnter, uvoidvoid_f onLeave) const
								{ DeltaDebuggedVMFacade::SetOnBreakPointCallbacks(onEnter, onLeave); }
	void					SetOnStopDebuggingCallback (uvoidvoid_f onStop) const
								{ DeltaDebuggedVMFacade::SetOnStopDebuggingCallback(onStop); }
	void					SetOnStartCallback (uvoidvoid_f onGo) const
								{ DeltaDebuggedVMFacade::SetOnStartCallback(onGo); }

	RealDebugger (void){}
	virtual ~RealDebugger(){}
};


////////////////////////////////////////////////////////////

static RealDebugger 
				debugger;
static Debugger*
				oldDebugger				= (Debugger*) 0;

static bool		isInitialised			= false;

static DeltaVirtualMachine::ErrorWarningCallback 
				vmErrorCallback			= (DeltaVirtualMachine::ErrorWarningCallback) 0;

static DeltaVirtualMachine::ErrorWarningCallback 
				vmWarningCallback		= (DeltaVirtualMachine::ErrorWarningCallback) 0;

static DeltaAssertExtensions::AssertInterfaceCallback
				assertInterfaceCallback	= (DeltaAssertExtensions::AssertInterfaceCallback) 0;

static DeltaAssertExtensions::ErrorInterfaceCallback
				errorInterfaceCallback	= (DeltaAssertExtensions::ErrorInterfaceCallback) 0;

////////////////////////////////////////////////////////////
// The only difference to the default is that it includes
// retry as a viable option.
//
static util_ui32 DeltaAssertInterfaceCallback (DeltaVirtualMachine* vm, const std::string& expr, void* unused) {

	if (!DeltaDebugServer::IsClientConnected())
		return (*DeltaPureVMFacade::GetAssertInterface())(vm, expr, unused);

	std::string	msg(
					DeltaPureVMFacade::AssertMessage(
						expr,
						DPTR(vm)->Line(),
						DPTR(vm)->Source(),
						DPTR(vm)->Id()
					)
				);

#ifdef	_MSC_VER
		int result =	MessageBox(
							GetForegroundWindow(),
							msg.c_str(),
							"Assertion failed!",
							MB_APPLMODAL | MB_ABORTRETRYIGNORE
						);
		return	result == IDABORT  ? ASSERT_ABORT :
				result == IDRETRY  ? ASSERT_DEBUG : ASSERT_CONTINUE;
#else
	fprintf(stderr, "Assertion failed!\n%s\n1.Abort\t2.Debug\t3.Continue\n", msg.c_str());
	util_ui32 i, j;
	do
		{ printf(">"); j = scanf("%u", &i); }
	while (j != 1 || (i != 1 && i != 2 && i != 3));
	return i == 1 ? ASSERT_ABORT : i == 2? ASSERT_DEBUG : ASSERT_CONTINUE;
#endif

}

//////////////////////////////////////////

#ifdef	_MSC_VER
static LRESULT CALLBACK MessageProc(__in  int code, WPARAM wParam, __in LPARAM lParam) 
	{ return code == HC_ACTION ? 1 : CallNextHookEx(NULL, code, wParam, lParam); }
#endif

static bool DeltaErrorInterfaceCallback (const std::string& msg, void* unused) {

	DASSERT(DeltaDebugServer::IsClientConnected());

#ifdef	_MSC_VER
	HHOOK hook = SetWindowsHookEx(WH_GETMESSAGE, &MessageProc, NULL, GetCurrentThreadId());
	bool result =	MessageBox(
						GetForegroundWindow(),
						msg.c_str(),
						"Runtime error!",
						MB_APPLMODAL | MB_YESNO
					) == IDYES;

	UnhookWindowsHookEx(hook);
	return result;
#else
	fprintf(stderr, "Runtime error!\n%s\n1.Yes\t2.No\n", msg.c_str());
	util_ui32 i, j;
	do
		{ printf(">"); j = scanf("%u", &i); }
	while (j != 1 || (i != 1 && i != 2));
	return i == 1;
#endif

}

//////////////////////////////////////////
// Auto forwarding from the debug server to the debugger UI.
//
static void SendErrorToDebugger (const char* error) {
	if (DeltaDebugServer::IsClientConnected())
		DeltaDebugServer::DoInfoError(error);
	else
		fprintf(stderr, "%s", error);
}

static void SendWarningToDebugger (const char* warning) {
	if (DeltaDebugServer::IsClientConnected())
		DeltaDebugServer::DoInfoWarning(warning);
	else
		fprintf(stderr, "%s", warning);
}

///////////////////////////////////////////

DVMDEBUG_CFUNC void* DeltaDebuggedVMFacade_Initialise (void) {
	DeltaDebuggedVMFacade::Initialise(0, (char**) 0);
	return udynamiclibloader::New("init");
}

///////////////////////////////////////////

bool DeltaDebuggedVMFacade::Initialise (int argc, char** argv) {

	DASSERT(!IsInitialised() && DeltaPureVMFacade::IsInitialised());

	assertInterfaceCallback	= DeltaAssertExtensions::GetAssertInterfaceCallback();
	errorInterfaceCallback	= DeltaAssertExtensions::GetErrorInterfaceCallback();
	DeltaAssertExtensions::SetAssertInterfaceCallback(&DeltaAssertInterfaceCallback);
	DeltaAssertExtensions::SetErrorInterfaceCallback(&DeltaErrorInterfaceCallback);

	DeltaDebugServerBreakPoints::Create();
	VirtualMachineDebugExtensionsPackage::Initialise();
		
	vmErrorCallback		= DeltaVirtualMachine::GetErrorCallback();
	vmWarningCallback	= DeltaVirtualMachine::GetWarningCallback();
	DeltaVirtualMachine::SetErrorCallback(&SendErrorToDebugger);
	DeltaVirtualMachine::SetWarningCallback(&SendWarningToDebugger);

	oldDebugger = &DeltaPureVMDebuggerProxy::GetDebugger();
	DeltaPureVMDebuggerProxy::SetDebugger(&debugger);

	bool result = DeltaDebugServer::Initialise(DEBUG_SERVER_PORT, argc, argv);	// Always at the end.

	isInitialised = true;
	return result;
}

///////////////////////////////////////////

DVMDEBUG_CFUNC void* DeltaDebuggedVMFacade_CleanUp (void) {
	DeltaDebuggedVMFacade::CleanUp();
	return udynamiclibloader::New("cleanup");
}

void DeltaDebuggedVMFacade::CleanUp (void) {

	DASSERT(IsInitialised() && DeltaPureVMFacade::IsInitialised()); // Should destroy before pure.

	DeltaAssertExtensions::SetAssertInterfaceCallback(assertInterfaceCallback);
	DeltaAssertExtensions::SetErrorInterfaceCallback(errorInterfaceCallback);

	DeltaVirtualMachine::SetErrorCallback(vmErrorCallback);
	DeltaVirtualMachine::SetWarningCallback(vmWarningCallback);

	DeltaDebugServer::CleanUp();

	VirtualMachineDebugExtensionsPackage::CleanUp(); // Always after the debug server.
	DeltaDebugServerBreakPoints::Destroy();

	DeltaPureVMDebuggerProxy::SetDebugger(oldDebugger);

	isInitialised = false;
}

///////////////////////////////////////////

void DeltaDebuggedVMFacade::CloseClientConnection (void) 
	{ DeltaDebugServer::CloseClientConnection(); }

bool DeltaDebuggedVMFacade::IsClientConnected (void)
	{ return DeltaDebugServer::IsClientConnected(); }

bool DeltaDebuggedVMFacade::ClientWasAttached (void)
	{ return DeltaDebugServer::ClientWasAttached(); }

///////////////////////////////////////////

void DeltaDebuggedVMFacade::SetOnBreakPointCallbacks (const std::pair<uvoidvoid_f,uvoidvoid_f>& callbacks)
	{ SetOnBreakPointCallbacks(callbacks.first, callbacks.second); } 

void DeltaDebuggedVMFacade::SetOnBreakPointCallbacks (uvoidvoid_f onEnter, uvoidvoid_f onLeave) {
	DASSERT(IsInitialised());
	DeltaDebugExtensions::SetOnBreakEnter(onEnter);
	DeltaDebugExtensions::SetOnBreakLeave(onLeave);
}

void DeltaDebuggedVMFacade::SetOnStopDebuggingCallback (uvoidvoid_f onStop) {
	DASSERT(IsInitialised());
	DeltaDebugServer::SetExitServeFunction(onStop);
}

void DeltaDebuggedVMFacade::SetOnStartCallback (uvoidvoid_f onGo) {
	DASSERT(IsInitialised());
	DeltaDebugServer::SetStartServeFunction(onGo);
}

bool DeltaDebuggedVMFacade::IsInitialised (void) 
	{ return isInitialised; }

////////////////////////////////////////////////////////////
