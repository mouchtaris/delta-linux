// DeltaDebugExtensions.h
// Implementation of the common debug extensions API for the 
// Delta virtual machine.
// ScriptFighter Project.
// A. Savidis, see source file for revision history.
//

#ifndef	DELTADEBUGEXTENSIONS_H
#define	DELTADEBUGEXTENSIONS_H

#include "DeltaVirtualMachine.h"
#include "DeltaDebugExtensionsSuper.h"
#include "DeltaDebugExtensionsStdDefs.h"
#include "DebugBreakPointHolder.h"
#include "DeltaDebugRecentFunctionResults.h"
#include "DeltaDebugLinesWithCodeHolder.h"
#include "DeltaDebugMessengerProxy.h"
#include "DeltaDebugTraceCommandHandler.h"
#include "DeltaDebugCallStackInspector.h"
#include "DeltaDebugUserVarsInspector.h"
#include "DeltaDebugContextHandler.h"
#include "DeltaDebugExecutionBreakController.h"
#include "utypes.h"

#include <list>
#include <stack>
#include <functional>

/////////////////////////////////////////////////////////////////

class DeltaDebugExtensions : public DeltaDebugExtensionsSuper  {

	private:
	DeltaVirtualMachine*					vm;
	const DeltaDebugProgramInfo&			GetDebugInfo (void) const
												{ return DPTR(vm)->GetDebugInfo(); }
	// MAIN COMPONENTS.

	static DeltaDebugRecentFunctionResults*	callResults;
	umutex									linesMutex;
	DeltaDebugLinesWithCodeHolder			codeLines;
	static DeltaDebugMessengerProxy*		messenger;
	static DeltaDebugTraceCommandHandler*	tracer;
	static DeltaDebugCallStackInspector*	globalCallStack;
	static DeltaDebugUserVarsInspector*		userVarsInspector;
	DeltaDebugContextHandler				contextHandler;
	DeltaDebugExecutionBreakController		breaker;

	DeltaBlockIdStacks&						GetBlocks (void) 
												{ return DPTR(vm)->GetBlocks(); }
	const DeltaBlockIdStacks&				GetBlocks (void) const
												{ return DPTR(vm)->GetBlocks(); }
	static DeltaDebugRecentFunctionResults& GetCallResults (void)
												{ DASSERT(callResults); return *DPTR(callResults); }
	DeltaDebugLinesWithCodeHolder&			GetLines (void) 
												{ return codeLines; }
	static DeltaDebugTraceCommandHandler&	GetTracer (void)
												{ DASSERT(tracer); return *DPTR(tracer); }
	static DeltaDebugCallStackInspector&	GetCallStack (void)
												{ DASSERT(globalCallStack); return *DPTR(globalCallStack); }
	static DeltaDebugUserVarsInspector&		GetVars (void)
												{ DASSERT(userVarsInspector); return *DPTR(userVarsInspector); }
	DeltaDebugContextHandler&				GetContext (void)
												{ return contextHandler; }
	const DeltaDebugContextHandler&			GetContext (void) const
												{ return contextHandler; }
	DeltaDebugExecutionBreakController&		GetBreaker (void)
												{ return breaker; }
	const DeltaDebugExecutionBreakController&	
											GetBreaker (void) const
												{ return breaker; }

	// CALLSTACK MANAGEMENT.

	static void								StartCallStackManagement (void);
	void									AtTopActivationRecord (void);
	void									AtCurrentActivationRecord (void);


	// CURRENT DEBUG CONTEXT.

	static void 							SetCurrentDebugContext (DeltaDebugContextHandler* context);
	void									SetAsCurrentDebugContext (void)
												{ SetCurrentDebugContext(&GetContext()); }

	// DYNAMIC DEBUGGER ACTIVATION.
	void									OnArtificialBreakPoint (void);
	static void								AfterDynamicLoadingCallback (void* unused);
	static void								BeforeDynamicUnloadingCallback (void* unused);

	static void								ForEachVirtualMachineInstantiateDebugExtensions (void);
	static void								ForEachVirtualMachineDestroyDebugExtensions (void);
	static void								ReproduceCallStackFromVirtualMachineCallTraces (void);

	/////////////////////////////////////////////////////////////////

	public:

	// EXECUTION CALLBACKS.

	virtual void			OnStartingExecution (void);
	virtual void			OnEndingExecution (void);
	virtual void			OnBlockEnter (bool isMainBlock, util_ui16 blockId);
	virtual void			OnBlockExit (bool isMainBlock, util_ui16 blockId);
	virtual bool			OnAssertionFailure (const std::string& expr);
	virtual void			OnThrow (void);
	virtual void			OnStackUnWindingProgramFunction (void);
	virtual void			OnStackUnWindingLibraryFunction (void);
	virtual void			OnStackUnWindingGlobalCode (void);
	virtual void			OnCallingLibraryFunction (DeltaLibraryFunc f);
	virtual void			OnCallingProgramFunction (const std::string& name);
	virtual void			OnReturnLibraryFunction (DeltaLibraryFunc f);
	virtual void			OnReturnProgramFunction (const std::string& name);
	virtual void			OnInitLinesOfCodes (util_ui16 codeSize);
	virtual void			OnReadingLineWithCode (DeltaCodeAddress addr, util_ui16 line, bool explicitLeader);
	virtual void			OnExecuteHandleErrorBreak (void);
	virtual CheckBreakPointStatus		
							OnExecuteCheckBreakPoints (void);
	virtual void			OnVirtualMachineDestruction (void);
	virtual void			OnExecutionError (void);
	virtual void			OnStartLoading (void);
	virtual void			OnFinishedLoading (void);

	static DeltaDebugExtensionsSuper*	
							Constructor (DeltaVirtualMachine* vm)
								{ return DNEWCLASS(DeltaDebugExtensions, (vm)); }
	static DeltaDebugExtensions&	
							GetDebugger (DeltaVirtualMachine* vm)
								{ return *DPTR((DeltaDebugExtensions*) DPTR(vm)->GetDebugger()); }
	static DeltaDebugMessengerProxy&	
							GetMessenger (void)
								{ DASSERT(messenger); return *DPTR(messenger); }
	// STACK INSPECTION.

	bool					StackDown (void);
	bool					StackUp (void);

	// USER VARS INSPECTION.

	util_ui16				TotalActualArgs (void) const;
	util_ui16				TotalUserActualArgs (void) const;
	util_ui16				TotalLocalData (void) const;
	DeltaValue*				GetActualArg (util_ui16 argNo);
	DeltaValue*				GetLocalVar (util_ui16 varNo);
	util_ui16				TotalGlobalData (void) const;
	DeltaValue*				GetGlobalVar (util_ui16 offset);
	DeltaValue*				GetActualArg (util_ui16 argNo, std::string& putName);
	DeltaValue*				GetActualArg (const std::string& name);
	DeltaValue*				GetLocalVar (const std::string& name);
	DeltaValue*				GetGlobalVar (const std::string& name, bool isStatic = false);
	DeltaValue*				GetClosureVar (const std::string& name);
	void					GetVariablesAtCurrentContext (	// Only for non undefined vars.
								std::list< std::pair<std::string, std::string> >& vars
							) const;
	
	// TRACE CONTROL.

	void					Step (void);
	void					StepIn (void);
	void					SelectiveStepIn (util_ui32 callOrder);
	void					GetAllPossibleCalls (		// For selective step in
								std::list< std::pair<std::string, std::string> >& result
							);
	void					StepOut (void);
	void					Continue (void);
	void					RunTo (util_ui16 line);
	void					Stop (void);				// Stop execution and exit from bpt state

	// CURRENT DEBUG CONTEXT.

	static DeltaVirtualMachine*	
							GetCurrentDebugContext (void);
	bool					IsTracingAtProgramFunction (void) const;
	const DeltaDebugFuncInfo*
							GetTracedProgramFunction (void) const;
	const DeltaClosure*		GetTracedProgramFunctionClosure (void) const;
	void					SendCurrentContext (void) const;

	bool					GetScopeClosestFunction (
								const std::string&	name, 
								DeltaCodeAddress*	addr, 
								util_ui16*			defLine, 
								util_ui16*			scope
							) const;

	void					GetAllEnclosingFunctions (
								std::list<std::string>*	names // inside->out
							) const;

	bool					IsDynamicCode (void) const
								{ return GetContext().IsDynamicCode (); }
	const std::string		GetDynamicCode (void) const
								{ return GetContext().GetDynamicCode(); }
	const std::string		GetDynamicCodeVirtualPath (void) const
								{ return GetContext().GetDynamicCodeVirtualPath(); }
	const std::string		GetSourcePath (void) const
								{ return GetContext().GetSourcePath(); }

	// EXECUTION BREAK CONTROLLER.

	static DeltaVirtualMachine*		
							BreakPointVM (void) 
								{ return DeltaDebugExecutionBreakController::GetBreakPointVM(); }
	static bool				IsBreakFromTraceCommand (void)
								{ return DeltaDebugExecutionBreakController::IsBreakFromTraceCommand(); }
	static void				SetOnBreakEnter (void (*f)(void))
								{ DeltaDebugExecutionBreakController::SetOnBreakEnter(f); }
	static void				SetOnBreakLeave (void (*f)(void))	
								{ DeltaDebugExecutionBreakController::SetOnBreakLeave(f); }
	const DeltaBreakPoint&	GetCurrentBreakPoint (void) const
								{ return GetBreaker().GetCurrentBreakPoint(); }
	bool					IsValidBreakPointLine (util_ui16 line) const
									{ return GetBreaker().IsValidBreakPointLine(line); }
	util_ui16				GetNextValidBreakPointLine (util_ui16 line) const // Returns 0 if none found.
								{ return GetBreaker().GetNextValidBreakPointLine(line); }
	static void				BreakExecution (void)
								{ DeltaDebugExecutionBreakController::BreakExecution(); }
	static void				SetBreakOnThrownException (bool val)
								{ DeltaDebugExecutionBreakController::SetBreakOnThrownException(val); }	
	// INITIALISATION.

	static void				Install (void) 
									{ DeltaDebugExtensionsSuper::SetConstructor(Constructor); }
	static void				SingletonCreate (void);
	static void				SingletonDestroy(void);

	DeltaDebugExtensions (DeltaVirtualMachine* _vm);
	~DeltaDebugExtensions();
};

/////////////////////////////////////////////////////////////////

inline DeltaDebugExtensions& GetDebugger (DeltaVirtualMachine* vm) {
	DASSERT(DPTR(vm)->GetDebugger());
	return *DPTR((DeltaDebugExtensions*) DPTR(vm)->GetDebugger());
}

/////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
