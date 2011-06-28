// DeltaDebugExecutionBreakController.cpp
// Controls if execution is to be broken.
// ScriptFighter Project.
// A. Savidis, June 2006 (original).
// RF to a separate class (February 2010).
//

#include "DeltaDebugExecutionBreakController.h"
#include "DebugExprParserAPI.h"
#include "DeltaDebugServerBreakPoints.h"

////////////////////////////////////////////////////////////////////////

#define	BREAKPOINT_BUSYWAIT_SLEEP 500

#define	THREAD_SAFE_CODE_FOLLOWS()			\
	LOCK_BLOCK(linesMutex)

#define	GLOBAL_THREAD_SAFE_CODE_FOLLOWS()	\
	LOCK_BLOCK(*breakExecutionMutex)

#define	THREAD_SAFE_EXPR_VALUE(expr)		\
	LOCK_EXPR_VALUE(expr, linesMutex)

////////////////////////////////////////////////////////////////////////

umutex*								DeltaDebugExecutionBreakController::breakExecutionMutex
										= (umutex*) 0;
bool								DeltaDebugExecutionBreakController::inBreakPoint
										= false;
bool								DeltaDebugExecutionBreakController::isBreakFromTraceCommand
										= false;
bool								DeltaDebugExecutionBreakController::breakExecutionRequested
										= false;
bool								DeltaDebugExecutionBreakController::breakOnThrownException
										= false;
const DeltaDebugContextHandler*		DeltaDebugExecutionBreakController::breakContext
										= (DeltaDebugContextHandler*) 0;
void								(*DeltaDebugExecutionBreakController::startCallStackMgmt)(void)
										= (void(*)(void)) 0;
void								(*DeltaDebugExecutionBreakController::onBreakEnter)(void)
										= (void(*)(void)) 0;
void								(*DeltaDebugExecutionBreakController::onBreakLeave)(void)
										= (void(*)(void)) 0;
const DeltaDebugCallStackInspector*	DeltaDebugExecutionBreakController::callStack
										= (DeltaDebugCallStackInspector*) 0;
const DeltaDebugMessengerProxy*		DeltaDebugExecutionBreakController::messenger
										= (DeltaDebugMessengerProxy*) 0;
DeltaDebugRecentFunctionResults*	DeltaDebugExecutionBreakController::callResults
										= (DeltaDebugRecentFunctionResults*) 0;
DeltaDebugTraceCommandHandler*		DeltaDebugExecutionBreakController::tracer
										= (DeltaDebugTraceCommandHandler*) 0;

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::Initialise (
		const DeltaDebugCallStackInspector*	_callStack,
		const DeltaDebugMessengerProxy*		_messenger,
		DeltaDebugRecentFunctionResults*	_callResults,
		DeltaDebugTraceCommandHandler*		_tracer,
		void								(*_startCallStackMgmt)(void)
	) {
	DASSERT(!callStack && !messenger && !callResults && !tracer && !startCallStackMgmt);
	callStack			= DNULLCHECK(_callStack);
	messenger			= DNULLCHECK(_messenger);
	callResults			= DNULLCHECK(_callResults);
	tracer				= DNULLCHECK(_tracer);
	startCallStackMgmt	= DNULLCHECK(_startCallStackMgmt);
	unew(breakExecutionMutex);
}

void DeltaDebugExecutionBreakController::CleanUp (void)
	{ udelete(breakExecutionMutex); }

////////////////////////////////////////////////////////////////////////

DeltaDebugExecutionBreakController::DeltaDebugExecutionBreakController (
		const DeltaDebugContextHandler*			_myContext,
		const DeltaDebugLinesWithCodeHolder*	_lines,
		umutex&									_linesMutex
	) :
	vm					( _myContext->GetVM()	),
	linesMutex			( _linesMutex			),
	myContext			( _myContext			),
	lines				( _lines				)
	{}
	
DeltaDebugExecutionBreakController::~DeltaDebugExecutionBreakController()
	{}

////////////////////////////////////////////////////////////////////////
// The breakpoints need to be tested for validity before running,
// while proper messages need to be sent to the debug server if
// something is wrong.

void DeltaDebugExecutionBreakController::ValidateAllBreakPoints (void) {

	std::string source(GetContext().GetSourcePath());

	if (DeltaDebugServerBreakPoints::Get().HasAnyBreakPoints(source)) {

		const std::list<DeltaBreakPoint> bpts(DeltaDebugServerBreakPoints::Get().Get(source));
		
		for (	std::list<DeltaBreakPoint>::const_iterator i = bpts.begin();
				i != bpts.end();
				++i	) {

				if (!IsValidBreakPointLine(i->GetLine())) {

					util_ui32 newLine = GetNextValidBreakPointLine(i->GetLine());

					GetMessenger().NotifyInvalidBreakPoint(
						source.c_str(), 
						i->GetLine(),
						newLine,
						i->GetCondition()
					);

					if (newLine)
						DeltaDebugServerBreakPoints::Get().Change(
							source.c_str(), 
							i->GetLine(), 
							newLine
						);
					else
						DeltaDebugServerBreakPoints::Get().Remove(
							source.c_str(), 
							i->GetLine()
						);
			}
		}

		if (GetContext().IsDynamicCode())
			GetMessenger().NotifyDynamicCode(
				GetContext().GetDynamicCode().c_str()
			);
	}
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::SendBreakPointInformation  (void) {

	GetMessenger().NotifyStopPoint(
		GetContext().GetSourcePath().c_str(),
		DPTR(vm)->Line(),
		GetCallStack().IsAtGlobalCodeWithNoPreviousCallOnTheStack(),
		IsBreakFromTraceCommand() ? "" : GetCurrentBreakPoint().GetCondition()
	);
	GetContext().SendCurrentContext();
	GetMessenger().NotifyMostRecentFuncResults(GetCallResults().GetResults());
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::SetIsBreakFromTraceCommand (bool val) {
	if (!(isBreakFromTraceCommand = val)) 
		GetCallResults().Clear();
}

////////////////////////////////////////////////////////////////////////
// At least one instruction block starts with the line.

bool DeltaDebugExecutionBreakController::IsValidBreakPointLine (util_ui16 line) const
	{ return THREAD_SAFE_EXPR_VALUE(GetLines().IsLineWithCode(line)); }

////////////////////////////////////////////////////////////////////////

util_ui16 DeltaDebugExecutionBreakController::GetNextValidBreakPointLine (util_ui16 line) const {

	THREAD_SAFE_CODE_FOLLOWS();
	if (!DPTR(vm)->HasDebugInfo() || !(line = DPTR(vm)->GetDebugInfo().GetNextLine(line)))
		return DBG_LINEVALUE_IS_INVALID_BPT;
	else 		
		return line;
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::BreakExecution (void) {
	GLOBAL_THREAD_SAFE_CODE_FOLLOWS();
	if (!InsideBreakPoint())
		breakExecutionRequested = true;
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::ResetBreakExecutionRequested (void) {
	GLOBAL_THREAD_SAFE_CODE_FOLLOWS();
	breakExecutionRequested = false;
}

////////////////////////////////////////////////////////////////////////

bool DeltaDebugExecutionBreakController::IsBreakExecutionRequested (void) {
	GLOBAL_THREAD_SAFE_CODE_FOLLOWS();
	return breakExecutionRequested;
}

////////////////////////////////////////////////////////////////////////

bool DeltaDebugExecutionBreakController::InsideBreakPoint (void) 
	{ return inBreakPoint; }

void DeltaDebugExecutionBreakController::SetInsideBreakPoint (bool val)
	{ inBreakPoint = val; }

////////////////////////////////////////////////////////////////////////

bool DeltaDebugExecutionBreakController::IsBreakPointTime (void) {
	DASSERT(GetMessenger().IsConnected());
	return	WeShouldBreakDueToUserRequest()	||
			WeShouldBreakFromTraceCommand() ||
			WeShouldBreakAtLine(DPTR(vm)->Line());
}

////////////////////////////////////////////////////////////////////////

bool DeltaDebugExecutionBreakController::WeShouldBreakFromTraceCommand (void) {
	if (GetTracer().IsTraceCommandCompleted()) {
		DASSERT(IsBreakFromTraceCommand());
		return true;
	}
	else 
		return false;
}

////////////////////////////////////////////////////////////////////////

bool DeltaDebugExecutionBreakController::WeShouldBreakDueToUserRequest (void) {
	if (IsBreakExecutionRequested() && 
		THREAD_SAFE_EXPR_VALUE(GetLines().IsLeadingInstructionOfLine(DPTR(vm)->PC(), DPTR(vm)->Line()))) {
		SetIsBreakFromTraceCommand(false);
		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////

bool DeltaDebugExecutionBreakController::WeShouldBreakAtLine (util_ui16 line) {

	if (!THREAD_SAFE_EXPR_VALUE(GetLines().IsLeadingInstructionOfLine(DPTR(vm)->PC(), line)))
		return false;

	std::string source(GetContext().GetSourcePath());

	if (!DeltaDebugServerBreakPoints::Get().HasBreakPoint(source, line))
		return false;

	const DeltaBreakPoint& bpt = DeltaDebugServerBreakPoints::Get().Get(source, line);
	if (!bpt.IsEnabled())
		return false;

	// If we have a condition, we evaluate it immediately.
	// The bpt will be considered enabled if the condition is true too.
	//
	if (bpt.HasCondition() && !EvaluateCondition(line, bpt.GetCondition()))
		return false;

	SetIsBreakFromTraceCommand(false);

	new (&currBreakPoint) DeltaBreakPoint(bpt);
	return true;
}

////////////////////////////////////////////////////////////////////////

bool DeltaDebugExecutionBreakController::EvaluateCondition (util_ui16 line, const std::string& cond) {

	DASSERT(!DeltaDebugContextHandler::GetCurrentDebugContext() && !GetBreakPointContext());	
	
	SetBreakPointEnvironmentVariables();	// Temporarily, to allow condition evaluation.

	std::string error;
	DeltaValue* val = DebugExprEvaluator::Evaluate(vm, cond.c_str(), error);

	ResetBreakPointEnvironmentVariables();	// Reset now.

	if (!val) { // Condition evaluation error.

		DASSERT(DebugExprEvaluator::HasError());
		std::string source(GetContext().GetSourcePath());

		GetMessenger().NotifyInvalidCondition (
			source.c_str(), 
			line, 
			line,
			cond.c_str()
		);

		DeltaDebugServerBreakPoints::Get().Disable(source.c_str(), line); 
		return false;
	}
	else {
		DASSERT(!DebugExprEvaluator::HasError());
		bool result = val->TrueTest();
		DDELETE(val);
		return result;
	}
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::NotifyEnterBreakPoint (void) {

	GetTracer().OnStopPoint();
	SendBreakPointInformation();
	if (onBreakEnter)
		(*onBreakEnter)();
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::NotifyExitBreakPoint (void) {
	if (onBreakLeave)
		(*onBreakLeave)();
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::SetBreakPointEnvironmentVariables (void) {

	DASSERT(!InsideBreakPoint());

	SetInsideBreakPoint(true);
	SetBreakPointContext(&GetContext());

	ResetBreakExecutionRequested();

	(*DNULLCHECK(startCallStackMgmt))();

	DASSERT(DNULLCHECK(GetBreakPointContext()) == DeltaDebugContextHandler::GetCurrentDebugContext());

	GetCallResults().EnteringNewStopPoint();
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::ResetBreakPointEnvironmentVariables (void) { 
	
	SetInsideBreakPoint(false); 
	ResetBreakPointContext();

	DeltaDebugContextHandler::ResetCurrentDebugContext();

	DASSERT(!GetBreakPointContext() && !DeltaDebugContextHandler::GetCurrentDebugContext());
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugExecutionBreakController::BreakPointServiceLoop (void) {

	SetBreakPointEnvironmentVariables();
	NotifyEnterBreakPoint();

	GetTracer().BusyWaitForTraceCommand(BREAKPOINT_BUSYWAIT_SLEEP);

	NotifyExitBreakPoint();
	ResetBreakPointEnvironmentVariables();
}

////////////////////////////////////////////////////////////////////////
