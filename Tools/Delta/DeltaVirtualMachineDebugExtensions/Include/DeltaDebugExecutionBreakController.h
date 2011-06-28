// DeltaDebugExecutionBreakController.h
// Controls if execution is to be broken.
// ScriptFighter Project.
// A. Savidis, June 2006 (original).
// RF to a separate class (February 2010).
//

#ifndef	DELTADEBUGEXECUTIONBREAKCONTROLLER_H
#define	DELTADEBUGEXECUTIONBREAKCONTROLLER_H

#include "DeltaStdDefs.h"
#include "DebugBreakPointHolder.h"
#include "DeltaDebugExtensionsStdDefs.h"
#include "DeltaVirtualMachine.h"
#include "DeltaDebugCallStackInspector.h"
#include "DeltaDebugUserVarsInspector.h"
#include "DeltaDebugMessengerProxy.h"
#include "DeltaDebugRecentFunctionResults.h"
#include "DeltaDebugTraceCommandHandler.h"
#include "DeltaDebugContextHandler.h"
#include "DeltaDebugLinesWithCodeHolder.h"
#include "DeltaFunc.h"
#include "DDebug.h"
#include "utypes.h"

/////////////////////////////////////////////////////////////////

class DeltaDebugExecutionBreakController {

	DFRIENDDESTRUCTOR()

	private:

	DeltaVirtualMachine*	vm;							// From debug context.
	DeltaBreakPoint			currBreakPoint;				// Copy of the current user break point (when applicable).
	static umutex*			breakExecutionMutex;		// May be set while running.
	static bool				inBreakPoint;				// For every type of breakpoint.
	static bool				isBreakFromTraceCommand;	// Artificial stop when trace command completes.
	static bool				breakExecutionRequested;	// User break.
	static bool				breakOnThrownException;		// If should break when an exception is thrown.
	static const DeltaDebugContextHandler*
							breakContext;				// Where the break occurred.
	static void				(*onBreakEnter)(void);
	static void				(*onBreakLeave)(void);

	///****************************
	// External components and vars.

	mutable umutex&			linesMutex;
	static void				(*startCallStackMgmt)(void);

	const DeltaDebugContextHandler*	
							myContext;
	const DeltaDebugLinesWithCodeHolder*	
							lines;
	static const DeltaDebugCallStackInspector*	
							 callStack;
	static const DeltaDebugMessengerProxy*	
							 messenger;
	static DeltaDebugRecentFunctionResults*	
							callResults;
	static DeltaDebugTraceCommandHandler*
							tracer;

	static const DeltaDebugCallStackInspector&
							GetCallStack (void) 
								{ return *DPTR(callStack); }
	static const DeltaDebugMessengerProxy&	
							GetMessenger (void)
								{ return *DPTR(messenger); }
	static DeltaDebugRecentFunctionResults& 
							GetCallResults (void)
								{ return *DPTR(callResults); }
	static DeltaDebugTraceCommandHandler& 
							GetTracer (void)
								{ return *DPTR(tracer); }
	const DeltaDebugLinesWithCodeHolder&	
							GetLines (void) const
								{ return *DNULLCHECK(lines); }
	const DeltaDebugContextHandler&	
							GetContext (void) const
								{ return *DNULLCHECK(myContext); }

	///****************************

	static void 			SetBreakPointContext (const DeltaDebugContextHandler* _context) 
								{ breakContext = _context;  }
	static void 			ResetBreakPointContext(void) 
								{ unullify(breakContext); }
	static void				ResetBreakExecutionRequested (void);
	void					ResetBreakPointEnvironmentVariables (void);

	static bool				IsBreakExecutionRequested (void);
	static void				SetInsideBreakPoint (bool val);
	void					SetBreakPointEnvironmentVariables (void);
	void					NotifyEnterBreakPoint (void);
	void					NotifyExitBreakPoint (void);

	bool					WeShouldBreakFromTraceCommand (void);
	bool					WeShouldBreakDueToUserRequest (void);
	bool					WeShouldBreakAtLine (util_ui16 line);
	bool					EvaluateCondition (util_ui16 bptLine, const std::string& cond);

	void					SendBreakPointInformation (void);

	/////////////////////////////////////////////////////////////////

	public:
	static void				Initialise (
								const DeltaDebugCallStackInspector*	_callStack,
								const DeltaDebugMessengerProxy*		_messenger,
								DeltaDebugRecentFunctionResults*	_callResults,
								DeltaDebugTraceCommandHandler*		_tracer,
								void								(*_startCallStackMgmt)(void)
							);
	static void				CleanUp (void);

	static void				SetOnBreakEnter (void (*f)(void))	
								{ onBreakEnter = f; }
	static void				SetOnBreakLeave (void (*f)(void))	
								{ onBreakLeave = f; }

	void					ValidateAllBreakPoints (void);
	static DeltaVirtualMachine*		
							GetBreakPointVM (void) 
								{ return breakContext ? breakContext->GetVM() : (DeltaVirtualMachine*) 0; }
	static const DeltaDebugContextHandler*
							GetBreakPointContext (void) 
								{ return breakContext; }
	const DeltaBreakPoint&	GetCurrentBreakPoint (void) const
								{ DASSERT(breakContext == myContext); return currBreakPoint; }
	static bool				IsBreakFromTraceCommand (void)
								{ return isBreakFromTraceCommand; }
	static void				SetIsBreakFromTraceCommand (bool val);
	static bool				InsideBreakPoint (void);

	bool					IsBreakPointTime (void);
	bool					IsValidBreakPointLine (util_ui16 line) const;
	util_ui16				GetNextValidBreakPointLine (util_ui16 line) const; // Returns 0 if none found.

	static bool				ShouldBreakOnThrownException (void) 
								{ return breakOnThrownException; }
	static void				SetBreakOnThrownException (bool val)
								{ breakOnThrownException = val; }
	static void				BreakExecution (void);
	void					BreakPointServiceLoop (void);

	DeltaDebugExecutionBreakController (
		const DeltaDebugContextHandler*			_myContext,
		const DeltaDebugLinesWithCodeHolder*	_lines,
		umutex&									_linesMutex
	);
	~DeltaDebugExecutionBreakController();
};

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
