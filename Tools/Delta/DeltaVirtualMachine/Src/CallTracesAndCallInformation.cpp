// CallTracesAndCallInformation.cpp.cpp
// Call traces management in the Delta Virtual Machine.
// Script Fighter Project.
// A. Savidis, May 2008 (original version).
// RF, moved to a separate file, March 2010.
//

#include "DDebug.h"
#include "ugeometry.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaExceptionHandling.h"
#include "DeltaClosureHandling.h"
#include "Execute.h"

//------------------------------------------------------------------
// CALL INFORMATION.

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::RemoveAllMyCallTracess (void) {

	if (HasRun() && !HasExecutionCompleted()) {	// There is still some trace.

		DASSERT(IsFoundInAnyCallTrace());

		CallTraceStack* copy = DNEW(CallTraceStack);
		DPTR(copy)->resize(callTraceStackTotal);

		util_ui32 newTotal = 0;
		for (util_ui32 i = 0; i < callTraceStackTotal; ++i)
			if ((*DPTR(callTraceStack))[i].vm != this)
				(*DPTR(copy))[newTotal++] = (*DPTR(callTraceStack))[i];

		// Set new pruned call trace stack.
		DDELETE(callTraceStack);
		callTraceStackTotal = newTotal;
		callTraceStack		= copy;
	}
	else
		DASSERT(!IsFoundInAnyCallTrace());
}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::PushCallTrace (const void* info, DeltaCodeAddress _pc, util_ui32 _top, util_ui32 _topsp, util_ui32 _blocksIndex) {

	if (callTraceStackTotal == DPTR(callTraceStack)->size())
		DPTR(callTraceStack)->resize(2 * DPTR(callTraceStack)->size());

	topCallTrace = &(*DPTR(callTraceStack))[callTraceStackTotal++];
	topCallTrace->Set(this, info, _pc, _top, _topsp, _blocksIndex, Line());
	DASSERT(topCallTrace->IsValid());
}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::PopCallTrace (void) {
	DASSERT(callTraceStackTotal);
	(*DPTR(callTraceStack))[--callTraceStackTotal].Reset();
	topCallTrace = callTraceStackTotal ? &(*DPTR(callTraceStack))[callTraceStackTotal - 1] : (CallTrace*) 0;
}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::InvalidateEverySecondaryFailingAndPerformClearActions (void){

	DASSERT(callTraceStackTotal);
	DASSERT(IsPrimaryFailing());
	DASSERT(GetTopVMFromCallTraceStack() == this);

	DELTA_DETERMINE_CALLSTACK_ENTRIES_TO_POP_ON_ERROR(
		DeltaVirtualMachine*,
		(*DPTR(callTraceStack))[i].vm,
		UEMPTY,
		util_ui32 i = callTraceStackTotal - 1,
		i,
		--i,
		this
	);

	// Clear call traces, block id stacks and vm func stacks.
	while (callsToPop--) {
		DASSERT(callTraceStackTotal);
		CallTrace& call = (*DPTR(callTraceStack))[callTraceStackTotal - 1];
		if (DeltaVirtualMachine* vm = DELTAVM_VGET_INST(call.vmSerialNo))
			if (DPTR(vm)->GetBlocks().GetTotalBlockIdStacks() > DELTA_BLOCKIDSTACKS_RESERVED) {
				DPTR(vm)->GetBlocks().PopBlockIdStack();
				DPTR(vm)->PopFunc();
			}
		PopCallTrace();
	}

	// Prune the affected execptions handler and closure creation entries.
	EXCEPTION_HANDLERS->PopCallsAffectedByError(this);
	CLOSURE_HANDLERS->PopCallsAffectedByError(this);

	// Explicitly invalidate secondary failing vms.
	for (std::list<DeltaVirtualMachine*>::iterator i = failing.begin(); i != failing.end(); ++i)
		if (!DPTR(*i)->HasProducedError())
			DPTR(*i)->InvalidateExecution(DeltaVirtualMachine::CallerFailed);
}

////////////////////////////////////////////////////////////////////

bool DeltaVirtualMachine::IsFoundInAnyCallTrace (void) const {
	for (util_ui32 i = callTraceStackTotal; i; --i)
		if (this == (*DPTR(callTraceStack))[i - 1].vm)
			return true;
	return false;
}

////////////////////////////////////////////////////////////////////

util_ui32 DeltaVirtualMachine::GetTotalCallTraces (void)
	{ return callTraceStackTotal; }

////////////////////////////////////////////////////////////////////

DeltaVirtualMachine::CallTrace&	DeltaVirtualMachine::GetCallTrace (util_ui32 i)
	{ DASSERT(i < callTraceStackTotal); return (*DPTR(callTraceStack))[i]; }

DeltaVirtualMachine::CallTrace&	DeltaVirtualMachine::GetTopCallTrace (void)
	{ DASSERT(callTraceStackTotal); return GetCallTrace(callTraceStackTotal - 1); }

DeltaVirtualMachine::CallTrace&	DeltaVirtualMachine::GetCallerOfTopCallTrace (void)
	{ DASSERT(callTraceStackTotal >= 2); return GetCallTrace(callTraceStackTotal - 2); }

////////////////////////////////////////////////////////////////////

bool DeltaVirtualMachine::CallTrace::IsCallAtGlobalCode (void) const
	{ return topsp == DELTA_TOPSP_WHEN_AT_GLOBAL_CODE(vm); }

bool DeltaVirtualMachine::CallTrace::IsCallAtLibraryFunc (void) const
	{ return topsp == DELTA_TOPSP_LIBFUNC; }

bool DeltaVirtualMachine::CallTrace::IsCallAtProgramOrMethodFunc (void) const
	{ return !IsCallAtGlobalCode() && !IsCallAtLibraryFunc(); }

////////////////////////////////////////////////////////////////////

DeltaVirtualMachine* DeltaVirtualMachine::GetTopVMFromCallTraceStack (void) {
	if (callTraceStackTotal)
		return (*DPTR(callTraceStack))[callTraceStackTotal - 1].vm;
	else
		return (DeltaVirtualMachine*) 0;
}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::UpdateCallLineAndPCOfTopCallTraceBeingAtGlobalCode (void) {
	DASSERT(!GetCurrFuncInfo() && topCallTrace); 
	topCallTrace->callLine	= Line(); 
	topCallTrace->pc		= PC();
}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::UpdateTopCallTrace (util_ui32 _top, util_ui32 _topsp) {
	DASSERT(callTraceStackTotal);
	CallTrace& call = (*DPTR(callTraceStack))[callTraceStackTotal - 1];
	call.top = _top;
	if (call.topsp ==  DELTA_TOPSP_WHEN_AT_GLOBAL_CODE(call.vm))
		DASSERT(call.topsp == _topsp);
	else
		call.topsp = _topsp;
}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::MakeCallInformationFromCallTrace (CallTrace& call, CallInformation* info, util_ui32 tag) {

	DASSERT(call.IsValid());

	DASSERT(DELTAVM_VGET_INST(call.vmSerialNo));	// No call traces of destroyed vms are possible.
	info->vm	= call.vm;
	info->tag	= tag;

	if (call.IsCallAtGlobalCode()) {
		info->func.Undefine();
		info->name		= DELTA_TOPSP_GLOBALCODE_FUNCNAME;
		info->defLine	= call.callLine;
	} 
	else 
	if (call.IsCallAtLibraryFunc()) {
		DASSERT(call.func);
		info->func.FromLibraryFunc( UVOID2FUNC(DeltaLibraryFunc, call.func) );
		info->name		= DeltaLibFuncBinder::GetNameByAddress( UVOID2FUNC(DeltaLibraryFunc, call.func) );
		info->defLine	= DELTA_CANTBE_A_SOURCE_LINE;
	}
	else {
		DeltaVirtualMachine*	vm			= info->vm;
		const DeltaStdFuncInfo*	funcInfo	= (DeltaStdFuncInfo*) call.func;
		DeltaCodeAddress		addr		= funcInfo->GetAddress();

		DASSERT(DPTR(vm)->IsValidFuncInfo(funcInfo));

		info->name		= funcInfo->GetName();
		info->defLine	= DPTR(vm)->code[addr + DELTA_FUNCENTER_USER_INSTR_OFFSET].line;

		if (!funcInfo->IsMethod())
			info->func.FromProgramFunc(addr, vm);
		else {				// Need to extra self from the stack.
			DeltaValue* self = DPTR(vm)->stack + (call.topsp + DELTA_TOPSP_SELF_OFFSET);

			DASSERT(DPTR(vm)->ValidateStackValuePtr(self));
			DASSERT(self->Type() == DeltaValue_Table);

			info->func.FromMethodFunc(addr, self->ToTable(), vm);
		}
	}
}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::GetCurrentCall (CallInformation* curr) {
	DASSERT(callTraceStack && callTraceStackTotal);
	util_ui32 i = callTraceStackTotal - 1;
	MakeCallInformationFromCallTrace(
		(*DPTR(callTraceStack))[i],
		curr,  
		DELTA_CALLTRACE_TAG_ENCODE(i)
	);
}

////////////////////////////////////////////////////////////////////
	
bool DeltaVirtualMachine::IsValidCallTag (util_ui32 tag) 
	{ return DELTA_CALLTRACE_TAG_DECODE(tag) < callTraceStackTotal; }

////////////////////////////////////////////////////////////////////

bool DeltaVirtualMachine::GetNextCall (util_ui32 tag, CallInformation* info) {

	tag = DELTA_CALLTRACE_TAG_DECODE(tag);
	DASSERT(tag < callTraceStackTotal);

	if (!tag)	// Bottom call.
		return false;
	else {
		--tag;	// Move to next call.
		MakeCallInformationFromCallTrace(
			(*DPTR(callTraceStack))[tag],
			info,
			DELTA_CALLTRACE_TAG_ENCODE(tag)
		);
		return true;
	}
}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::GetCall (util_ui32 i, CallInformation* info) {
	DASSERT(callTraceStack && i < callTraceStackTotal);
	MakeCallInformationFromCallTrace(
		(*DPTR(callTraceStack))[i],
		info,  
		DELTA_CALLTRACE_TAG_ENCODE(i)
	);
}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::CreateStackTrace (util_ui32 maxTraces) {

	CallInformation info;
	GetCurrentCall(&info);
	maxTraces = maxTraces ? umin(callTraceStackTotal, maxTraces) : callTraceStackTotal;
	util_ui32 n = callTraceStackTotal;
	DASSERT(n && maxTraces);
	
	DPTR(stackTrace)->clear();

	while (maxTraces-- && n--) {

		DPTR(stackTrace)->append(
			uconstructstr(
				"[%d]:%s::%s%s\n",
				n,
				DPTR(info.vm)->Id(),
				info.name.c_str(),
				info.name != DELTA_TOPSP_GLOBALCODE_FUNCNAME ? "()" : ""
			)
		);

		if (!GetNextCall(info.tag, &info))
			DASSERT(!n);
	}

	if (n)
		DPTR(stackTrace)->append(uconstructstr("<Total %u calls hidden>", n));
}

//------------------------------------------------------------------
