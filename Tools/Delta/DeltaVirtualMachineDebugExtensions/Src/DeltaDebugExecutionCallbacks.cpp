// DeltaDebugExecutionCallbacks.cpp
// Implementation of the debug callbacks invoked by the 
// VM to support the debugger.
// Script Fighter Project.
// A. Savidis, June 2007.
// Extended August 2007, adding support for reporting
// most recent function return values after trace commands.
// Changed August 2008, added execution error callback for graceful
// clean-up of stacks.
//

#include "DeltaDebugExtensions.h"
#include "DeltaAssertExtensions.h"
#include "DeltaLibFuncBinder.h"
#include "DebugExprParserAPI.h"
#include "ContainerDelayedCollector.h"
#include "DeltaAssertExtensions.h"
#include "Execute.h"
#include "usystem.h"

#define	THREAD_SAFE_VOID_EXPR(expr)		\
	LOCK_VOID_EXPR(expr, linesMutex)

/////////////////////////////////////////////////////////////////////////
// Because access to line information is possible in parallel by the 
// debug server, in response to bpt messages from the debugger-client,
// all access to VM line information should be thread safe. This includes
// VM functions that may alter line information, aka VM loading. The pair
// of functions start / finished loading essentialy locks access to line
// information and 'has debug information' flags during loading time.
//

void DeltaDebugExtensions::OnStartLoading (void) 
	{ linesMutex.WaitToLock(); }

void DeltaDebugExtensions::OnFinishedLoading (void) 
	{ linesMutex.UnLock(); }


/////////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnStartingExecution (void) {

	DASSERT(!DPTR(vm)->GetUserFuncClosure());

	GetBreaker().ValidateAllBreakPoints();
	GetCallStack().Push(
		DPTR(vm),
		(DeltaClosure*) 0,
		DeltaDebugCallStackEntry_GlobalCode, 
		DELTA_CANTBE_A_FUNCTION_ADDRESS,
		DPTR(vm)->pc,
		DPTR(vm)->top,
		DPTR(vm)->topsp,
		GetBlocks().GetTopBlockIdStackIndex(),
		DELTA_GLOBALCODE_DEFLINE
	);
}

/////////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnEndingExecution (void) {
	DASSERT(GetCallStack().GetTotal());
	DASSERT(GetCallStack().IsTopCallAtGlobalCode());
	GetCallStack().Pop(); 
}

/////////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnBlockEnter (bool isMainBlock, util_ui16 blockId) {
	DASSERT(GetCallStack().GetTotal());
	GetCallStack().ChangeTopStackPointers(DPTR(vm)->top, DPTR(vm)->topsp);
}

void DeltaDebugExtensions::OnBlockExit (bool isMainBlock, util_ui16 blockId) {
	DASSERT(GetCallStack().GetTotal());
	GetCallStack().ChangeTopStackPointers(DPTR(vm)->top, DPTR(vm)->topsp);
}

/////////////////////////////////////////////////////////////////////////

bool DeltaDebugExtensions::OnAssertionFailure (const std::string& expr) {
	switch (DeltaAssertExtensions::AssertInterfaceResponse(vm, expr)) {
		case ASSERT_ABORT		: DeltaAssertExtensions::Abort(); return false;	// Abort
		case ASSERT_DEBUG		: OnArtificialBreakPoint();	return false;	// Retry
		case ASSERT_CONTINUE	: return true;	// Ignore
		default					: DASSERT(false); return false;
	}
}

//******************************

void DeltaDebugExtensions::OnStackUnWindingProgramFunction (void) 
	{}

void DeltaDebugExtensions::OnStackUnWindingLibraryFunction (void) 
	{}

void DeltaDebugExtensions::OnStackUnWindingGlobalCode (void) {
	DASSERT(GetCallStack().IsTopCallAtGlobalCode());
	GetCallStack().Pop();
}

//******************************

DeltaDebugExtensionsSuper::CheckBreakPointStatus DeltaDebugExtensions::OnExecuteCheckBreakPoints (void) {

	if (!GetMessenger().IsConnected())
		return FallThrough;

	// If a trace command is under processing, we have to
	// handle it since it may cause a break point.
	//
	THREAD_SAFE_VOID_EXPR(
		GetTracer().HandleAnyTraceCommand(
			DPTR(vm), 
			GetLines()	// Mutexed due to this parm.
		)
	);

	// If a line is 0, the src line number is not changed, 
	// and no breakpoint is checked. We don't break at 0 lines, since 
	// the execution will not remain valid there. Also, we do check
	// for a break point even if a previous debug command (from the
	// last break point) has not been served.
	//
	if (DPTR(vm)->GetInstructionLine()) {

		// Break points are considered for lines only if the current
		// PC is equal to the first instruction of that line or an assert failed.
		// Else, the break point is simply ignored (it is invalid). In case of
		// a forced assert break, we also assume a break-point.
		//
		if (GetBreaker().IsBreakPointTime()) {
			DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP(
				GetBreaker().BreakPointServiceLoop(), 
				vm, 
				ReturnFalse
			);
			if (DPTR(vm)->PC() == DELTA_PC_PROGRAM_END_VM(vm))
				return ReturnTrue;
		}
	}

	return FallThrough;
}

/////////////////////////////////////////////////////////////////////////
	
void DeltaDebugExtensions::OnArtificialBreakPoint (void) {
	THREAD_SAFE_VOID_EXPR(GetTracer().Reset());	// If a trace command was posted reset it.
	DeltaDebugExecutionBreakController::SetIsBreakFromTraceCommand(false);	// The break is not due to tracing.

	DASSERT(!DPTR(vm)->HasProducedError());		// When we handle the error no error action is yet taken.
	GetBreaker().BreakPointServiceLoop();
}

/////////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnExecuteHandleErrorBreak (void) {

	DASSERT(GetCallStack().GetTotal() && GetCallStack().GetTopVM() == vm);
	if (GetMessenger().IsConnected())					// Debugger is running.
		if (DeltaAssertExtensions::ErrorInterfaceResponse(UERROR_GETREPORT()))
			OnArtificialBreakPoint();
}

/////////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnThrow (void) {

	DASSERT(GetCallStack().GetTotal() && GetCallStack().GetTopVM() == vm);

	if (GetMessenger().IsConnected() && DeltaDebugExecutionBreakController::ShouldBreakOnThrownException()) {

		umodalmessage(
			"Runtime exception!", 
			"An exception is to be thrown by the running program and the \n"
			"execution will be broken at the corresponding statement."
		);
	
		OnArtificialBreakPoint();
	}
}

/////////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnVirtualMachineDestruction (void) {
	if (DPTR(vm)->HasRun() && !DPTR(vm)->HasExecutionCompleted()) {	// Still has entries in the call stack
		DASSERT(GetCallStack().GetTotal());							// At least one entry exists.
		GetCallStack().RemoveAllCalls(vm);							// No more entries in call stack.
	}
}

/////////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnExecutionError (void) {
	
	DASSERT(DPTR(vm)->HasProducedError());
	DASSERT(!DPTR(vm)->IsFoundInAnyCallTrace());

	DASSERT(GetCallStack().GetTotal() || DPTR(vm)->HasNotRunYet());

	if (!GetCallStack().GetTotal())
		return;

	DELTA_DETERMINE_CALLSTACK_ENTRIES_TO_POP_ON_ERROR(
		DeltaVirtualMachine*,
		GetCallStack().GetCurrVM(),
		GetCallStack().AtTopCall(),
		UEMPTY,
		!GetCallStack().IsBottomLevelReached(),
		GetCallStack().Down(),
		vm
	);

	// Pop (callstack, blocks, ret values and tracer), by invalidating.

	GetCallStack().AtTopCall();
	
	while (callsToPop--) {

		if (IsBreakFromTraceCommand()) {			
			GetCallResults().ExitingRecentlyCalledFunction();	// Default args signify clearing results.
			GetTracer().OnFunctionExit();
		}

		GetCallStack().Pop();
	}
}

/////////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnInitLinesOfCodes (util_ui16 _codeSize) 
	{ GetLines().Initialize(_codeSize); }

/////////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnReadingLineWithCode (DeltaCodeAddress addr, util_ui16 line, bool explicitLeader) 
	{ GetLines().AddLineWithCode(line, addr, explicitLeader); }

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::OnCallingLibraryFunction (DeltaLibraryFunc func) {

	DASSERT(!DPTR(vm)->GetUserFuncClosure());

	GetCallStack().Push(
		DPTR(vm),
		(DeltaClosure*) 0,
		DeltaDebugCallStackEntry_LibraryFunc,
		DeltaLibFuncBinder::GetSerialNo(func),
		DPTR(vm)->pc,
		DPTR(vm)->top, 
		DPTR(vm)->topsp,
		GetBlocks().GetTopBlockIdStackIndex(),
		DPTR(vm)->Line()
	);
	
	if (IsBreakFromTraceCommand()) {
		GetCallResults().EnteringRecentlyCalledFunction(
			DeltaLibFuncBinder::GetNameByAddress(func),
			vm
		);
		GetTracer().OnFunctionEnter();
	}
}

//******************************

void DeltaDebugExtensions::OnCallingProgramFunction (const std::string& name) {

	// The call trace is always pushed before the debugger backend
	// is notified to push the function too.
	DASSERT(
		DeltaVirtualMachine::GetTopCallTrace().MatchesRegisters(
			vm->pc,
			vm->top,
			vm->topsp
		)
	);

	if (GetCallStack().GetTotal() && GetCallStack().IsTopCallAtGlobalCode()) {

		DeltaVirtualMachine*			topVM	  = GetCallStack().GetTopVM();
		DeltaVirtualMachine::CallTrace& topCaller = DeltaVirtualMachine::GetCallerOfTopCallTrace();

		DASSERT(topCaller.IsCallAtGlobalCode());
		DASSERT(topCaller.vm == topVM);
		DASSERT(topVM != vm || topCaller.callLine == DPTR(topVM)->Line());

		GetCallStack().ChangeTopGlobalCode(topCaller.callLine, topCaller.pc);
	}

	// Get the top VM currently. It is always the caller so the call line is more
	// precise when taken from it.

	DeltaVirtualMachine* callerVM = GetCallStack().GetTotal() ? GetCallStack().GetTopVM() : vm;

	GetCallStack().Push(
		DPTR(vm),
		DPTR(vm)->GetUserFuncClosure(),
		DeltaDebugCallStackEntry_UserFunc,
		DPTR(vm)->PC(), 
		DPTR(vm)->PC(),
		DPTR(vm)->top, 
		DPTR(vm)->topsp,
		GetBlocks().GetTopBlockIdStackIndex(),
		DPTR(callerVM)->Line()
	);
	
	if (IsBreakFromTraceCommand()) {
		GetCallResults().EnteringRecentlyCalledFunction(name.c_str(), vm);
		GetTracer().OnFunctionEnter();
	}

}

//******************************

void DeltaDebugExtensions::OnReturnProgramFunction (const std::string& name){

	GetCallStack().Pop(); 

	if (IsBreakFromTraceCommand()) {
		GetCallResults().ExitingRecentlyCalledFunction(name, vm);
		GetTracer().OnFunctionExit();
	}
}

//******************************

void DeltaDebugExtensions::OnReturnLibraryFunction (DeltaLibraryFunc f){

	GetCallStack().Pop(); 

	if (IsBreakFromTraceCommand()) {
		GetCallResults().ExitingRecentlyCalledFunction(
			DNULLCHECK(DeltaLibFuncBinder::GetNameByAddress(f)),
			vm
		);
		GetTracer().OnFunctionExit();
	}
}

////////////////////////////////////////////////////////////////////
