// DeltaPureVMFacade.h
// Facade to initialise / cleanup VM usage in pure mode.
// ScriptFigher Project.
// A. Savidis, July2008.
//

#ifndef	DELTAPUREVMFACADE_H
#define	DELTAPUREVMFACADE_H

#include <string>
#include "utypes.h"
#include "ucallbacks.h"
#include "DeltaStdDefs.h"
#include "DeltaAssertExtensions.h"

////////////////////////////////////////////////////////////////////

class DVM_CLASS DeltaPureVMFacade {
	public:
	typedef void				(*CleanUpCallback)(void* closure);
	typedef void				(*PrintCallback)(const char*);
	typedef DeltaAssertExtensions::AssertInterfaceCallback AssertInterface;

	static void					Initialise (bool errorInvalidatesAll = true);
	static bool					IsInitialised (void);	// For cross dependency preconditions.
	static void					WriteProfileReport (const std::string& path);	// Always before clean up.
	static void					CleanUp (void);

	static void					SetByteCodeLoadingPath (const std::string& path, bool prioritised = false);
	static void					SetDllImportPath (const std::string& path, bool prioritised = false);

	static void					AddOnJustBeforeCleanUpCallback (CleanUpCallback f, void* closure = (void*) 0);
	static void					RemoveOnJustBeforeCleanUpCallback (CleanUpCallback f, void* closure = (void*) 0);
	static void					SetPrintCallback (PrintCallback f);

	static const std::string	AssertMessage (
									const std::string&	expr,
									util_ui32			line, 
									const char* 		source, 
									const char* 		id
								);
	static AssertInterface		GetAssertInterface (void);
};

////////////////////////////////////////////////////////////////////
// We can use this proxy in place of the debugger facade in case we need to
// handle debugger behavior from DLLs that do not depend on the debugger backend. 
// It is safe since the debugger facade is always initialised prior to host
// libraries loaded as dlls and it sets its specific debugger proxy as needed.
//
class DVM_CLASS DeltaPureVMDebuggerProxy {

	public:
	class DebuggerInterface {
		public:
		enum ActivationType { Attached = 0, UponStart = 1, None = 2 };
		virtual void 			CleanUp (void) const = 0;
		virtual void 			CloseClientConnection (void) const = 0;
		virtual bool 			IsClientConnected (void) const = 0;
		virtual void 			SetOnBreakPointCallbacks (uvoidvoid_f onEnter, uvoidvoid_f onLeave) const = 0;
		virtual void 			SetOnStopDebuggingCallback (uvoidvoid_f onStop) const = 0;
		virtual void 			SetOnStartCallback (uvoidvoid_f onGo) const = 0;
		virtual ActivationType	GetActivationType (void) const = 0;
		DebuggerInterface (void){}
		virtual ~DebuggerInterface(){}
	};

	static DebuggerInterface&	GetDebugger (void);
	static void					SetDebugger (DebuggerInterface* debugger);
};

////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.