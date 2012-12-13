// DeltaDebugExtensions.cpp
// Debugging support in the delta virtal machine as an add on
// library. A cross VM stack is maintained.
// Script Fighter Project.
// A. Savidis, December 1999 (original version).
// Revisions (mainly clean-up), June 2001.
// Refactoring as an orthogonal API (different lib version), June 2006.
// Refactoring to allow a cross-vm unified debug callstack, June 2006.
// Refactoring to move the extra functionality to an independent library,
// not needing separate lib versions with #ifdefs, June 2007.
// Changed July 2007, using source file name (instead of VM id) for 
// organizing break points.
// Changed December 2008, updated to use the non-singleton breakpoint holder.
// Changed February 2010, separated user vars inspector.
// Changed February 2010, separated debug context and break controller.
//

#include "DeltaDebugExtensions.h"
#include "DebugBreakPointHolder.h"
#include "DDebug.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DeltaVMRegistry.h"
#include "DeltaLibFuncBinder.h"
#include "DebugBreakPointHolder.h"
#include "DeltaRuntimeError.h"
#include "ustlhelpers.h"
#include "uthreadlib.h"
#include "DebugExprParserAPI.h"
#include "DeltaTable.h"
#include "DeltaDebugServerBreakPoints.h"
#include "Execute.h"
#include "DeltaDebugDynamicActivator.h"
#include "DeltaVMRegistry.h"
#include "DeltaClosureHandling.h"
#include <list>
#include <algorithm>

//////////////////////////////////////////////////////////////

#define	ASSERT_IN_BREAKPOINT()																	\
	DASSERT(DeltaDebugContextHandler::GetCurrentDebugContext());								\
	DASSERT(!DeltaDebugContextHandler::GetCurrentDebugContext()->GetVM()->HasProducedError());	\
	DASSERT(DeltaDebugExecutionBreakController::InsideBreakPoint())

//////////////////////////////////////////////////////////////

DeltaDebugRecentFunctionResults*	
						DeltaDebugExtensions::callResults		= (DeltaDebugRecentFunctionResults*) 0;

DeltaDebugMessengerProxy*	
						DeltaDebugExtensions::messenger			= (DeltaDebugMessengerProxy*) 0;

DeltaDebugTraceCommandHandler*	
						DeltaDebugExtensions::tracer			= (DeltaDebugTraceCommandHandler*) 0;

DeltaDebugCallStackInspector*	
						DeltaDebugExtensions::globalCallStack	= (DeltaDebugCallStackInspector*) 0;

DeltaDebugUserVarsInspector*		
						DeltaDebugExtensions::userVarsInspector	= (DeltaDebugUserVarsInspector*) 0;

//////////////////////////////////////////////////////////////

void DeltaDebugExtensions::SingletonCreate (void) {

	unew(callResults);
	unew(messenger);
	unew(tracer);
	unew(globalCallStack);
	unew(userVarsInspector);

	DeltaDebugContextHandler::Initialise(
		globalCallStack, 
		userVarsInspector, 
		messenger
	);

	DeltaDebugExecutionBreakController::Initialise(
		globalCallStack, 
		messenger, 
		callResults, 
		tracer, 
		&DeltaDebugExtensions::StartCallStackManagement
	);

	DeltaDebugDynamicActivator::GetSingleton().SetAfterLoadingCallback(&AfterDynamicLoadingCallback);
	DeltaDebugDynamicActivator::GetSingleton().SetBeforeUnloadingCallback(&BeforeDynamicUnloadingCallback);
}

//*****************************

void DeltaDebugExtensions::SingletonDestroy(void) {

	udelete(callResults);
	udelete(messenger);
	udelete(tracer);
	udelete(globalCallStack);
	udelete(userVarsInspector);

	DeltaDebugContextHandler::CleanUp();
	DeltaDebugExecutionBreakController::CleanUp();

	DeltaDebugDynamicActivator::GetSingleton().ResetAfterLoadingCallback();
	DeltaDebugDynamicActivator::GetSingleton().ResetBeforeUnloadingCallback();
}

//*****************************

DeltaDebugExtensions::DeltaDebugExtensions (DeltaVirtualMachine* _vm) :
		vm					( DNULLCHECK(_vm)							),
		contextHandler		( _vm										),
		breaker				( &contextHandler, &codeLines, linesMutex	)
		{}

//***************************

DeltaDebugExtensions::~DeltaDebugExtensions()
	{ LOCK_VOID_EXPR(GetLines().Clear(), linesMutex); }

//------------------------------------------------------------------
// CALLSTACK MANAGEMENT.

void DeltaDebugExtensions::StartCallStackManagement (void) {

	DASSERT(
		DeltaDebugExecutionBreakController::InsideBreakPoint()	&& 
		DeltaDebugExecutionBreakController::GetBreakPointVM()	&& 
		!DPTR(DeltaDebugExecutionBreakController::GetBreakPointVM())->HasProducedError()
	);

	// After we position at the top call and at the the debug context is
	// the VM of the top call. We set its block id stack also at the top.

	GetCallStack().VerifyCalls();
	GetCallStack().AtTopCall();
	DeltaVirtualMachine* vm = GetCallStack().GetTopVM();

	// Update the current source line in case the top context is 
	// global code (will be automatically done with every trace command).

	if (GetCallStack().IsAtGlobalCode())
		GetCallStack().ChangeTopGlobalCode(DPTR(vm)->Line(), DPTR(vm)->PC());

	GetDebugger(vm).GetBlocks().AtTopBlockIdStack();
	GetDebugger(vm).AtTopActivationRecord();
	GetDebugger(vm).SetAsCurrentDebugContext();
}

//*****************************

void DeltaDebugExtensions::AtTopActivationRecord (void) {
	AtCurrentActivationRecord();
	DASSERT(DPTR(vm)->top == GetVars().GetTop() && DPTR(vm)->topsp == GetVars().GetTopSp());
}

//*****************************

void DeltaDebugExtensions::AtCurrentActivationRecord (void) {
	util_ui32 top, topsp;
	GetCallStack().RetrieveCurrCallValues(&top, &topsp);
	GetVars().SetStackPointers(top, topsp, GetCallStack().GetCurrClosure());
	GetVars().SetCurrLine(
		GetCallStack().IsTopLevelReached() ? 
			GetCallStack().GetTopVM()->Line() :
			GetCallStack().GetCallLine(GetCallStack().GetCalleeOf(GetCallStack().GetCurr()))
	);
}

//*****************************

#define	STACK_UP_OR_DOWN_IMPL(																	\
			is_top_bottom,																		\
			call_stack_up_down,																	\
			blockid_stack_next_prev,															\
			get_caller_or_called																\
		)																						\
	ASSERT_IN_BREAKPOINT();																		\
	DASSERT(GetContext().IsCurrentDebugContext() && vm == GetCurrentDebugContext());			\
	GetCallStack().VerifyCalls();																\
	if (GetCallStack().is_top_bottom())															\
		return false;																			\
	else																						\
	if (GetCallStack().get_caller_or_called() != vm) {											\
		DeltaVirtualMachine* curr_vm = GetCallStack().get_caller_or_called();					\
		GetCallStack().call_stack_up_down();													\
		DASSERT(GetCallStack().IsCurrCallValid() && curr_vm == GetCallStack().GetCurrVM());		\
		GetDebugger(curr_vm).GetBlocks().AtBlockIdStack(GetCallStack().GetCurrBlocksIndex());	\
		GetDebugger(curr_vm).AtCurrentActivationRecord();										\
		GetDebugger(curr_vm).SetAsCurrentDebugContext();										\
		return true;																			\
	}																							\
	else {																						\
		GetCallStack().call_stack_up_down();													\
		DASSERT(GetCallStack().IsCurrCallValid());												\
		GetBlocks().blockid_stack_next_prev();													\
		AtCurrentActivationRecord();															\
		GetContext().SetCurrentTracedFunction();												\
		return true;																			\
	}

//*****************************

bool DeltaDebugExtensions::StackDown (void) 
	{ STACK_UP_OR_DOWN_IMPL(IsBottomLevelReached, Down, AtPreviousBlockIdStack, GetCurrCallerVM); }

//*****************************

bool DeltaDebugExtensions::StackUp (void)
	{ STACK_UP_OR_DOWN_IMPL(IsTopLevelReached, Up, AtNextBlockIdStack, GetCurrCalleeVM); }

//------------------------------------------------------------------
// USER VARS INSPECTION.

util_ui16 DeltaDebugExtensions::TotalGlobalData (void) const {
	ASSERT_IN_BREAKPOINT();
	return GetVars().TotalGlobalData();
}

//***************************

DeltaValue* DeltaDebugExtensions::GetGlobalVar (util_ui16 offset) {
	ASSERT_IN_BREAKPOINT();
	return GetVars().GetGlobalVar(offset);
}

//***************************

util_ui16 DeltaDebugExtensions::TotalActualArgs (void) const {
	ASSERT_IN_BREAKPOINT();
	return GetVars().TotalActualArgs();
}

//***************************

util_ui16 DeltaDebugExtensions::TotalUserActualArgs (void) const {
	ASSERT_IN_BREAKPOINT();
	return GetVars().TotalUserActualArgs();
}

//***************************

util_ui16 DeltaDebugExtensions::TotalLocalData (void) const {
	ASSERT_IN_BREAKPOINT();
	return GetVars().TotalLocalData();
}

//***************************

DeltaValue* DeltaDebugExtensions::GetActualArg (util_ui16 argNo) {
	ASSERT_IN_BREAKPOINT();
	return GetVars().GetActualArg(argNo);
}

//***************************

DeltaValue* DeltaDebugExtensions::GetLocalVar (util_ui16 varNo) {
	ASSERT_IN_BREAKPOINT();
	return GetVars().GetLocalVar(varNo);
}

//***************************

DeltaValue* DeltaDebugExtensions::GetActualArg (util_ui16 argNo, std::string& putName) {
	ASSERT_IN_BREAKPOINT();
	return GetVars().GetActualArg(argNo, putName);
}

//***************************

DeltaValue* DeltaDebugExtensions::GetActualArg (const std::string& name) {
	ASSERT_IN_BREAKPOINT();
	return GetVars().GetActualArg(name);
}

//***************************

DeltaValue* DeltaDebugExtensions::GetLocalVar (const std::string& name) {
	ASSERT_IN_BREAKPOINT();
	return GetVars().GetLocalVar(name);
}

//***************************

DeltaValue* DeltaDebugExtensions::GetGlobalVar (const std::string& name, bool isStatic) {
	ASSERT_IN_BREAKPOINT();
	return GetVars().GetGlobalVar(GetDebugInfo().GetGlobals(), name, isStatic);
}

//***************************

DeltaValue* DeltaDebugExtensions::GetClosureVar (const std::string& name) {
	ASSERT_IN_BREAKPOINT();
	return GetVars().GetClosureVar(name);
}

//***************************

void DeltaDebugExtensions::GetVariablesAtCurrentContext (
		std::list< std::pair<std::string, std::string> >& vars
	) const {
	ASSERT_IN_BREAKPOINT();
	return  GetVars().GetVariablesAtCurrentContext(GetDebugInfo().GetGlobals(), vars);
}

//------------------------------------------------------------------
// CURRENT DEBUG CONTEXT.

// Tries to locate upwards the first visible (either enclosed in current
// or enclosing the current) function with the inquired name.

bool DeltaDebugExtensions::GetScopeClosestFunction (
		const std::string&	name, 
		DeltaCodeAddress*	addr, 
		util_ui16*			line, 
		util_ui16*			scope
	) const {

	if (GetCallStack().GetCurrCallType() == DeltaDebugCallStackEntry_LibraryFunc)
		return false;

	DeltaVirtualMachine*	currVM	= (DeltaVirtualMachine*) 0;	// VM owning the function not necessarily the caller.
	DeltaCodeAddress		curr_pc = 0;						// PC at call site

	if (GetCallStack().IsTopLevelReached()) {		// If top record then it is the current instruction.
		currVM  = GetCallStack().GetTopVM();
		curr_pc = DPTR(currVM)->PC();
	}
	else {
		currVM  = GetCallStack().GetCurrVM();
		curr_pc = GetCallStack().RetrieveCurrCallPC();
	}
	
	DASSERT(currVM && curr_pc && curr_pc < DPTR(currVM)->GetCodeSize());
	const DeltaInstruction* instr = &DPTR(currVM)->GetInstruction(curr_pc);
	
	for (util_ui32 i = curr_pc; i; --i, instr = &DPTR(currVM)->GetInstruction(i)) {

		if (instr->GetOpCode() == DeltaVM_FUNCRET) {		// An enclosed function, go directly to its 1st instruction.
			DASSERT(
				instr->arg1.GetType() == DeltaOperand_ProgramFunc || 
				instr->arg1.GetType() == DeltaOperand_MethodFunc
			);
			DeltaStdFuncInfo* func = (DeltaStdFuncInfo*) instr->arg1.GetValue();
			DASSERT(DPTR(currVM)->IsValidFuncInfo(func));

			util_ui32 j = func->GetAddress();				// Function address.
			DASSERT(j < i);
			instr = &DPTR(currVM)->GetInstruction(i = j);	// Reposition at instruction.
			DASSERT(instr->GetOpCode() == DeltaVM_FUNCENTER);
		}

		if (instr->GetOpCode() == DeltaVM_FUNCENTER) {		// Either an enclosed or enclosing function.

			DeltaStdFuncInfo* func = (DeltaStdFuncInfo*) instr->arg1.GetValue();
			DASSERT(i == func->GetAddress());
			
			if (name == func->GetName()) {				// We found it!
					
				const DeltaDebugFuncInfo& info = GetDebugInfo().GetFunc(func->GetSerial());

				*addr	= info.GetAddress();
				*scope	= info.GetScope();
				*line	= info.GetLine();

				return true;
			}
		}
	}
	
	return false;	// Not found.
}

//***************************

void DeltaDebugExtensions::GetAllEnclosingFunctions (
		std::list<std::string>*	names // inside->out
	) const {

	if (GetCallStack().GetCurrCallType() == DeltaDebugCallStackEntry_UserFunc) {

		DeltaVirtualMachine*		currVM		= GetCallStack().GetCurrVM();
		const DeltaDebugFuncInfo*	funcInfo	= DNULLCHECK(GetContext().GetTracedProgramFunction());

		while (true) {
				names->push_back(funcInfo->GetFullPath());
				if (funcInfo->GetOwnerAddress() != DELTA_CANTBE_A_FUNCTION_ADDRESS)
					funcInfo = DPTR(currVM)->GetDebugInfo().GetFuncByAddr(funcInfo->GetOwnerAddress());
				else
					return;
		}
	}
}

//***************************

void DeltaDebugExtensions::SetCurrentDebugContext (DeltaDebugContextHandler* context) {

	if (!DNULLCHECK(context)->IsCurrentDebugContext()) {

		DeltaDebugContextHandler::SetCurrentDebugContext(context);
		DeltaVirtualMachine* vm	 = context->GetVM();
		
		GetVars().SetDataPointers(
			DPTR(vm)->stack,
			DPTR(vm)->stackSize,
			DPTR(vm)->globals, 
			DPTR(vm)->totalActiveGlobals,
			GetDebugger(DPTR(vm)).GetBlocks()
		);

		context->SetCurrentTracedFunction();
	}
}

//***************************

DeltaVirtualMachine* DeltaDebugExtensions::GetCurrentDebugContext (void) {
	DeltaDebugContextHandler* context = DeltaDebugContextHandler::GetCurrentDebugContext();
	return context ? context->GetVM() : (DeltaVirtualMachine*) 0;
}

bool DeltaDebugExtensions::IsTracingAtProgramFunction (void) const 
	{ ASSERT_IN_BREAKPOINT(); return GetContext().IsTracingAtProgramFunction(); }

const DeltaDebugFuncInfo* DeltaDebugExtensions::GetTracedProgramFunction (void) const 
	{ ASSERT_IN_BREAKPOINT(); return GetContext().GetTracedProgramFunction(); }

const DeltaClosure* DeltaDebugExtensions::GetTracedProgramFunctionClosure (void) const
	{ ASSERT_IN_BREAKPOINT(); return GetCallStack().GetCurrClosure(); }

void DeltaDebugExtensions::SendCurrentContext (void) const 
	{ ASSERT_IN_BREAKPOINT(); GetContext().SendCurrentContext(); }

//------------------------------------------------------------------
// TRACE CONTROLLER.

void DeltaDebugExtensions::Step (void) {

	ASSERT_IN_BREAKPOINT();

	GetTracer().StartStep(DPTR(vm));
	DeltaDebugExecutionBreakController::SetIsBreakFromTraceCommand(true);
}

//***************************

void DeltaDebugExtensions::StepIn (void) {

	ASSERT_IN_BREAKPOINT();

	GetTracer().StartStepIn(DPTR(vm));
	DeltaDebugExecutionBreakController::SetIsBreakFromTraceCommand(true);
}

//***************************

void DeltaDebugExtensions::SelectiveStepIn (util_ui32 callOrder) {

	ASSERT_IN_BREAKPOINT();

	bool done = false;

	if (DPTR(vm)->HasDebugInfo() && GetDebugInfo().GetCalls().Has(DPTR(vm)->Line())) {

		const DeltaUnparsedCallsInStmt& stmt = GetDebugInfo().GetCalls().Get(DPTR(vm)->Line());

		std::list<DeltaUnparsedCall> calls;
		stmt.GetCallsAfterInstruction(DPTR(vm)->PC(), calls);

		if (callOrder < calls.size()) {
			GetTracer().StartSelectiveStepIn(DPTR(vm), ulistgetbyindex(calls, callOrder).GetLabel());
			DeltaDebugExecutionBreakController::SetIsBreakFromTraceCommand(true);
			done = true;
		}
	}

	if (!done)
		StepIn();
}

//***************************

void DeltaDebugExtensions::RunTo (util_ui16 line) {

	ASSERT_IN_BREAKPOINT();
	DASSERT(IsValidBreakPointLine(line));

	GetTracer().StartRunTo(DPTR(vm), line);
	DeltaDebugExecutionBreakController::SetIsBreakFromTraceCommand(true);
}

//***************************

void DeltaDebugExtensions::StepOut (void) {

	ASSERT_IN_BREAKPOINT();

	GetTracer().StartStepOut(DPTR(vm));
	DeltaDebugExecutionBreakController::SetIsBreakFromTraceCommand(true);
}

//***************************

void DeltaDebugExtensions::Continue (void) {

	ASSERT_IN_BREAKPOINT();

	GetTracer().StartContinue(DPTR(vm));
	DeltaDebugExecutionBreakController::SetIsBreakFromTraceCommand(false);
}

//***************************

void DeltaDebugExtensions::GetAllPossibleCalls (
		std::list< std::pair<std::string, std::string> >& result
	) {

	if (DPTR(vm)->HasDebugInfo() && GetDebugInfo().GetCalls().Has(DPTR(vm)->Line())) {

		const DeltaUnparsedCallsInStmt& stmt = GetDebugInfo().GetCalls().Get(DPTR(vm)->Line());

		std::list<DeltaUnparsedCall> calls;
		stmt.GetCallsAfterInstruction(DPTR(vm)->PC(), calls);

		if (calls.size() <= 1)	// For selective step-in we need at least 2 calls.
			return;
		
		for (std::list<DeltaUnparsedCall>::iterator i = calls.begin(); i != calls.end(); ++i)
			result.push_back(
				std::pair<std::string, std::string>(
					i->GetFunction(),
					i->GetArguments()
				)
			);
	}
}

//------------------------------------------------------------------
// DYNAMIC DEBUGGER ACTIVATION.

void DeltaDebugExtensions::AfterDynamicLoadingCallback (void* unused) {
	ForEachVirtualMachineInstantiateDebugExtensions();
	ReproduceCallStackFromVirtualMachineCallTraces();
}

//***************************

void DeltaDebugExtensions::BeforeDynamicUnloadingCallback (void* unused) 
	{ ForEachVirtualMachineDestroyDebugExtensions(); }

//***************************

void DeltaDebugExtensions::ForEachVirtualMachineInstantiateDebugExtensions (void) {

	typedef DeltaVMRegistry::VMList VMList;
	const VMList& vms = VMRegistry().GetAll();

	for (VMList::const_iterator i = vms.begin(); i != vms.end(); ++i) {

		DeltaVirtualMachine* vm = *i;
		DASSERT(!DPTR(vm)->GetDebugger());

		DeltaDebugExtensions* debugger = DNEWCLASS(DeltaDebugExtensions, (vm));
		DPTR(vm)->SetDebugger(debugger);

		// Need to also initialise lines with code.

		DeltaDebugLinesWithCodeHolder& lines = DPTR(debugger)->GetLines();
		lines.Initialize(vm->GetCodeSize());

		DeltaDebugExtensionsSuper::LoopLeadingLines loopLines;
		for (util_ui32 i = 0, n = DPTR(vm)->GetCodeSize(); i < n; ++i)
			loopLines.Preprocess(DPTR(vm)->GetInstruction(i), i);
		loopLines.Add(*debugger);
	}
}

//***************************

void DeltaDebugExtensions::ForEachVirtualMachineDestroyDebugExtensions (void) {

	typedef DeltaVMRegistry::VMList VMList;
	const VMList& vms = VMRegistry().GetAll();

	for (VMList::const_iterator i = vms.begin(); i != vms.end(); ++i) {
		DeltaVirtualMachine* vm = *i;
		DASSERT(DPTR(vm)->GetDebugger());
		DDELETE(DPTR(vm)->GetDebugger());
		DPTR(vm)->SetDebugger((DeltaDebugExtensionsSuper*) 0);
	}
}

//***************************

void DeltaDebugExtensions::ReproduceCallStackFromVirtualMachineCallTraces (void) {

	for (util_ui32 i = 0, n = DeltaVirtualMachine::GetTotalCallTraces(); i < n; ++i) {

		DeltaVirtualMachine::CallTrace& call		= DeltaVirtualMachine::GetCallTrace(i);
		DeltaVirtualMachine*			vm			= call.vm;
		DeltaDebugCallStackFuncValue	func		= DELTA_CANTBE_A_FUNCTION_ADDRESS;
		DeltaDebugCallStackEntryType	type		= DeltaDebugCallStackEntry_Undef;

		if (call.topsp == DELTA_TOPSP_WHEN_AT_GLOBAL_CODE(vm))
			{ DASSERT(!call.func); type = DeltaDebugCallStackEntry_GlobalCode; }
		else
		if (call.topsp == DELTA_TOPSP_LIBFUNC) {
			DASSERT(call.func);	
			type = DeltaDebugCallStackEntry_LibraryFunc;
			func = DeltaLibFuncBinder::GetSerialNo(UVOID2FUNC(DeltaLibraryFunc, call.func));

			// We do not check here if func != DELTA_LIBFUNC_INTERNAL_SERIALNO
			// since the library (dll) may have been unloaded.
		}
		else {
			DASSERT(call.func && DPTR(call.vm)->IsValidFuncInfo((DeltaStdFuncInfo*) call.func));
			DeltaStdFuncInfo* funcInfo = (DeltaStdFuncInfo*) call.func;
			func = funcInfo->GetAddress();
			type = DeltaDebugCallStackEntry_UserFunc;
		}

		GetCallStack().Push(
			DPTR(vm),
			CLOSURE_HANDLERS->GetClosure(i),
			type,
			func, 
			call.pc,
			call.top,
			call.topsp == DELTA_TOPSP_LIBFUNC? call.top : call.topsp,
			call.blockIdStackIndex,
			call.callLine
		);
	}
}

//------------------------------------------------------------------
