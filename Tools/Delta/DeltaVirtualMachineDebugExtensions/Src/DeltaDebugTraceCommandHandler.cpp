// DeltaDebugTraceCommandHandler.h
// Trace command management for the debugger.
// Debug support extensions.
// Script Fighter Project.
// A. Savidis, June 2006 (original).
// RF to separate class (July 2008).
// Added selective step-in and finnaly made the dispatcher (September 2008).
//

#include "DeltaDebugTraceCommandHandler.h"
#include "usystem.h"

/////////////////////////////////////////////////////////////////

DeltaDebugTraceCommandHandler::DeltaDebugTraceCommandHandler (void) {} 
DeltaDebugTraceCommandHandler::~DeltaDebugTraceCommandHandler() {}

DeltaDebugTraceCommandHandler::TraceFunc DeltaDebugTraceCommandHandler::traceFuncs[TOTAL_TRACE_COMMANDS] = {
	&DeltaDebugTraceCommandHandler::HandleStep,
	&DeltaDebugTraceCommandHandler::HandleStepIn,
	&DeltaDebugTraceCommandHandler::HandleSelectiveStepIn,
	&DeltaDebugTraceCommandHandler::HandleStepOut,
	&DeltaDebugTraceCommandHandler::HandleRunTo,
	&DeltaDebugTraceCommandHandler::HandleContinue
};

/////////////////////////////////////////////////////////////////

void DeltaDebugTraceCommandHandler::OnFunctionEnter (void) 
	{ debugCmmd.IncCallNesting(); }

void DeltaDebugTraceCommandHandler::OnFunctionExit (void)
	{ debugCmmd.DecCallNesting(); }

void DeltaDebugTraceCommandHandler::OnStopPoint (void)
	{ debugCmmd.Reset(); }

/////////////////////////////////////////////////////////////////

void DeltaDebugTraceCommandHandler::StartStep (DeltaVirtualMachine* vm) 
	{ debugCmmd.SetStep(DPTR(vm)); }

void DeltaDebugTraceCommandHandler::StartStepIn (DeltaVirtualMachine* vm) 
	{ debugCmmd.SetStepIn(DPTR(vm)); }

void DeltaDebugTraceCommandHandler::StartSelectiveStepIn (DeltaVirtualMachine* vm, DeltaCodeAddress pc) {
	if (pc == DPTR(vm)->PC())			// We are to the target instruction.
		debugCmmd.SetStepIn(DPTR(vm));
	else {
		DASSERT(pc > DPTR(vm)->PC());	// Should follow the current instruction.
		debugCmmd.SetSelectiveStepIn(DPTR(vm), pc);
	}
}

void DeltaDebugTraceCommandHandler::StartStepOut (DeltaVirtualMachine* vm)
	{ debugCmmd.SetStepOut(DPTR(vm)); }

void DeltaDebugTraceCommandHandler::StartRunTo (DeltaVirtualMachine* vm, util_ui16 line)
	{ debugCmmd.SetRunTo(DPTR(vm), line); }

void DeltaDebugTraceCommandHandler::StartContinue (DeltaVirtualMachine* vm)
	{ debugCmmd.SetContinue(DPTR(vm)); }

void DeltaDebugTraceCommandHandler::Reset (void)
	{ debugCmmd.Reset(); }

/////////////////////////////////////////////////////////////////
// TRACE DISPATCHER
/////////////////////////////////////////////////////////////////
//

#define	HANDLER_COMMON_STARTING_CODE()					\
	util_ui32 line = DPTR(vm)->GetInstructionLine();	\
	if (line == DBG_LINEVALUE_IS_INVALID_BPT)			\
		return;											\
	DeltaCodeAddress POSSIBLY_UNUSED_LOCAL(pc) = DPTR(vm)->PC()

//******************************

void DeltaDebugTraceCommandHandler::HandleStep (DeltaVirtualMachine* vm, const LinesHolder& lines) {
	HANDLER_COMMON_STARTING_CODE();
	if (	(	DPTR(vm) == debugCmmd.GetPostedBy() &&			// Same VM,
				debugCmmd.GetCallNesting() == 0		&&			// at same function,
				lines.IsLeadingInstructionOfLine(pc, line)	)	// at different line.
				|| debugCmmd.GetCallNesting() < 0				// Tracing out.
		)	
			debugCmmd.SetCompleted();
}

//******************************

void DeltaDebugTraceCommandHandler::HandleStepIn (DeltaVirtualMachine* vm, const LinesHolder& lines) {
	HANDLER_COMMON_STARTING_CODE();
	if (	lines.IsLeadingInstructionOfLine(pc, line) ||		// Any change of line
			debugCmmd.GetCallNesting() < 0						// or tracing out to a call.
		)
			debugCmmd.SetCompleted();}

//******************************

void DeltaDebugTraceCommandHandler::HandleSelectiveStepIn (DeltaVirtualMachine* vm, const LinesHolder& lines) {
	HANDLER_COMMON_STARTING_CODE();
	if (pc == debugCmmd.GetSelectiveStepInTarget())				// Reached start of target call, so post a step in.
		debugCmmd.SetStepIn(DPTR(vm));
	else
	if (	(	debugCmmd.GetCallNesting() == 0 &&				// Any change of line at same nesting
				lines.IsLeadingInstructionOfLine(pc, line)	)
			||	debugCmmd.GetCallNesting() < 0					// or tracing out.											
		)
			debugCmmd.SetCompleted();							// Selective was not possible.
}

//******************************

void DeltaDebugTraceCommandHandler::HandleStepOut (DeltaVirtualMachine* vm, const LinesHolder& lines) {
	HANDLER_COMMON_STARTING_CODE();
	if (debugCmmd.GetCallNesting() < 0)							// Tracing out.
		debugCmmd.SetCompleted();		
}

//******************************

void DeltaDebugTraceCommandHandler::HandleRunTo (DeltaVirtualMachine* vm, const LinesHolder& lines) {
	HANDLER_COMMON_STARTING_CODE();
	if (	debugCmmd.GetRunToLine() == line	&&				// Target line reached,
			debugCmmd.GetPostedBy() == DPTR(vm)	&&				// at same VM,
			lines.IsLeadingInstructionOfLine(pc, line)			// at beginning of line.
		)	
			debugCmmd.SetCompleted();
}

//******************************

void DeltaDebugTraceCommandHandler::HandleContinue (DeltaVirtualMachine* vm, const LinesHolder& lines)
	{}

/////////////////////////////////////////////////////////////////

void DeltaDebugTraceCommandHandler::HandleAnyTraceCommand (
		DeltaVirtualMachine*					vm,
		const DeltaDebugLinesWithCodeHolder&	lines
	) {
	if (debugCmmd.HasCommand()) {
		DASSERT((util_ui32) debugCmmd.GetCommand() < uarraysize(traceFuncs));
		(this->*traceFuncs[debugCmmd.GetCommand()])(
			vm, 
			lines
		);
	}
}

/////////////////////////////////////////////////////////////////

bool DeltaDebugTraceCommandHandler::IsTraceCommandCompleted (void)
	{ return debugCmmd.IsCompleted(); }

/////////////////////////////////////////////////////////////////

void DeltaDebugTraceCommandHandler::BusyWaitForTraceCommand (util_ui32 sleepTime) {

	// Enter loop until a debug command is explicitly posted (by another thread).
	//
	while (debugCmmd.GetCommand() == None) {
		uprocesssleep(sleepTime);
		DASSERT(debugCmmd.GetCommand() >= Step && debugCmmd.GetCommand() <= None);
	}

	DASSERT(debugCmmd.GetCommand() != None);
}

/////////////////////////////////////////////////////////////////
