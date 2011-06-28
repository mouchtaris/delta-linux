// DeltaDebugContextHandler.h
// Handling current debug context during debugging (and when
// in breakpoint).
// ScriptFighter Project.
// A. Savidis, June 2006 (original).
// RF to a separate class (February 2010).
//

#ifndef	DELTADEBUGCONTEXTHANDLER_H
#define	DELTADEBUGCONTEXTHANDLER_H

#include "DeltaStdDefs.h"
#include "DeltaDebugExtensionsStdDefs.h"
#include "DeltaVirtualMachine.h"
#include "DeltaDebugCallStackInspector.h"
#include "DeltaDebugUserVarsInspector.h"
#include "DeltaDebugMessengerProxy.h"
#include "DeltaFunc.h"
#include "DDebug.h"
#include "utypes.h"

/////////////////////////////////////////////////////////////////

class DeltaDebugContextHandler {

	DFRIENDDESTRUCTOR()

	private:
	DeltaVirtualMachine*			vm;
	const DeltaDebugProgramInfo&	debugInfo;
	const DeltaDebugFuncInfo*		currTracedFuncDebugInfo;
	const char*						currLibFunc;

	static DeltaDebugContextHandler*
									currDebugContext;
	static DeltaDebugCallStackInspector*	
									callStack;
	static DeltaDebugUserVarsInspector*	
									userVars;
	static const DeltaDebugMessengerProxy*	
									messenger;

	static DeltaDebugCallStackInspector&
									GetCallStack (void) 
										{ return *DPTR(callStack); }
	static DeltaDebugUserVarsInspector&	
									GetVars (void)
										{ return *DPTR(userVars); }
	static const DeltaDebugMessengerProxy&	
									GetMessenger (void)
										{ return *DPTR(messenger); }

	const DeltaDebugProgramInfo&	GetDebugInfo (void) const
										{ return debugInfo; }

	bool							InFunction (void) const
										{ return currTracedFuncDebugInfo || currLibFunc; }
	const DeltaDebugFuncInfo*		GetDebugInfoOfFunction (DeltaCodeAddress addr) const;
	const std::string				GetCurrFunction (util_ui16* defLine, util_ui16* callLine, util_ui16* scope) const;
	bool							MatchesCallLineWithCallInstruction (util_ui32 callLine) const;	

	/////////////////////////////////////////////////////////////////

	public:	
	void							SetCurrentTracedFunction (void);
	void							SendCurrentContext (void) const;
	bool							IsTracingAtProgramFunction (void) const
										{ return GetCallStack().IsAtFunctionCall() && currTracedFuncDebugInfo; }
	const DeltaDebugFuncInfo*		GetTracedProgramFunction (void) const // Null if in lib function or at global code
										{ return currTracedFuncDebugInfo; }
	DeltaVirtualMachine*			GetVM (void) const
										{ return vm; }
	bool							IsCurrentDebugContext (void) const
										{ return currDebugContext == this; }
	static DeltaDebugContextHandler*
									GetCurrentDebugContext (void) 
										{ return currDebugContext; }
	static void						SetCurrentDebugContext (DeltaDebugContextHandler* context) 
										{ currDebugContext = context; }
	static void						ResetCurrentDebugContext (void) 
										{ unullify(currDebugContext); }

	bool							IsDynamicCode (void) const
										{ return debugInfo.IsDynamicCode (); }
	const std::string				GetDynamicCode (void) const
										{ return debugInfo.GetDynamicCode(); }
	const std::string				GetDynamicCodeVirtualPath (void) const
										{ return std::string(DBG_DYNAMIC_SOURCE) + DPTR(vm)->Id(); }
	const std::string				GetSourcePath (void) const
										{ return IsDynamicCode() ? GetDynamicCodeVirtualPath().c_str() : DPTR(vm)->Source(); }

	static void						Initialise (
										DeltaDebugCallStackInspector*	_callStack, 
										DeltaDebugUserVarsInspector*	_userVars,
										const DeltaDebugMessengerProxy*	_messenger
									);
	static void						CleanUp (void);

	DeltaDebugContextHandler (DeltaVirtualMachine* _vm) :
		vm						( DNULLCHECK(_vm)			),
		debugInfo				( DPTR(vm)->GetDebugInfo()	),
		currTracedFuncDebugInfo	( (DeltaDebugFuncInfo*) 0	),
		currLibFunc				( (char*) 0					)
		{}

	~DeltaDebugContextHandler()
		{}
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
