// DeltaDebuggedVMFacade.h
// Facade for extra initialise / cleanup for VM usage in 
// debugged mode.
// ScriptFigher Project.
// A. Savidis, July2008.
//

#ifndef	DELTADEBUGGEDVMFACADE_H
#define	DELTADEBUGGEDVMFACADE_H

#include "utypes.h"
#include "DeltaStdDefs.h"

////////////////////////////////////////////////////////////////////

class  DVMDEBUG_CLASS DeltaDebuggedVMFacade {
	public:
	static bool			Initialise (util_ui32 negotiationPort = 0);
	static bool			IsInitialised (void);		// For cross dependency preconditions.

	static void			CleanUp (void);
	static void			CloseClientConnection (void);
	static bool			IsClientConnected (void);
	static bool			ClientWasAttached (void);	// If true, attached, else activated on start up.
	static util_ui32	NegotiationPortFromArguments (int argc, char **argv);

	// In DLLs these callbacks are taken from the pure facade.
	static void			SetOnBreakPointCallbacks (const std::pair<uvoidvoid_f,uvoidvoid_f>& callbacks);
	static void			SetOnBreakPointCallbacks (uvoidvoid_f onEnter, uvoidvoid_f onLeave);
	static void			SetOnStopDebuggingCallback (uvoidvoid_f onStop);
	static void			SetOnStartCallback (uvoidvoid_f onGo);
};

////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.