// DeltaDebugCallStackInspector.cpp
// Debugger's call stack.
// Debug support extensions.
// Script Fighter Project.
// A. Savidis, June 2006 (original).
// RF to separate class (July 2008).
//

#include "DeltaDebugCallStackInspector.h"
#include "DeltaVMRegistry.h"
#include "DeltaDebugExtensions.h"
#include "DDebug.h"
#include "ugeometry.h"

////////////////////////////////////////////////////////////////////

DeltaDebugCallStackInspector::DeltaDebugCallStackInspector (void) {
	callStackTop	= DELTA_DEBUG_CALLSTACK_EMPTY_INDEX;
	callStackCurr	= DELTA_DEBUG_CALLSTACK_EMPTY_INDEX;
	callStack.resize(DELTA_DEBUG_CALLSTACK_SIZE);
}

DeltaDebugCallStackInspector::~DeltaDebugCallStackInspector() 
	{}

bool DeltaDebugCallStackInspector::Entry::IsValid (void) const {
	return	(vm && top && topsp)							&&
			ValidatableHandler::Get(vm->GetSerialNo())		&&
			VMRegistry().Exists(vm)							&&
			(top < DPTR(vm)->GetStackSize())				&&
			(topsp < DPTR(vm)->GetStackSize())				&&
			type >= DeltaDebugCallStackEntry_LibraryFunc	&&
			type <= DeltaDebugCallStackEntry_GlobalCode;
}

////////////////////////////////////////////////////////////////////

void DeltaDebugCallStackInspector::RemoveAllCalls (DeltaVirtualMachine* vm) {

	if (GetTotal()) {

		DASSERT(callStackTop < (util_i32) callStack.size());

		std::list<Entry> validCalls;

		for (util_i32 i = 0; i <= callStackTop; ++i) {
			if (callStack[i].vm != vm)
				validCalls.push_back(callStack[i]);
			callStack[i].Reset();
		}
		
		callStack.clear();
		callStack.resize(umax((util_ui32) validCalls.size(), (util_ui32) DELTA_DEBUG_CALLSTACK_SIZE));

		callStackTop = DELTA_DEBUG_CALLSTACK_EMPTY_INDEX;
		for (std::list<Entry>::iterator i = validCalls.begin(); i != validCalls.end(); ++i)
			callStack[++callStackTop] = *i;
		callStackCurr = callStackTop;
	}
}

////////////////////////////////////////////////////////////////////

void DeltaDebugCallStackInspector::AtTopCall (void) {
	callStackCurr = callStackTop;
	DASSERT(IsCurrCallValid());
	DASSERT(callStack[callStackCurr].IsValid());
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugCallStackInspector::IsAtFunctionCall (void) const 
	{ return callStackTop > DELTA_DEBUG_CALLSTACK_EMPTY_INDEX &&  !IsAtGlobalCode();  }

//*******************************

bool DeltaDebugCallStackInspector::IsAtGlobalCode (void) const {
	DASSERT(IsCurrCallValid());
	return callStack[callStackCurr].type == DeltaDebugCallStackEntry_GlobalCode;
}

//*******************************

bool DeltaDebugCallStackInspector::IsTopCallAtGlobalCode (void) const {
	DASSERT(GetTotal() && callStack[callStackTop].IsValid());
	return callStack[callStackTop].type == DeltaDebugCallStackEntry_GlobalCode;
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugCallStackInspector::IsAtGlobalCodeWithNoPreviousCallOnTheStack (void) const
	{ return IsAtGlobalCode() && GetTotal() == 1; }

bool DeltaDebugCallStackInspector::IsBottomLevelReached (void) const {
	DASSERT(IsCurrCallValid());
	return !callStackCurr;
}

bool DeltaDebugCallStackInspector::IsTopLevelReached (void) const {
	DASSERT(IsCurrCallValid());
	return callStackCurr == callStackTop;
}

////////////////////////////////////////////////////////////////////
// Verifies that the entries and variables for the global call-stack
// are valid. For error defense.
//
void DeltaDebugCallStackInspector::VerifyCalls (void) const {

	//	callStackCurr != DELTA_DEBUG_CALLSTACK_EMPTY_INDEX => 
	//	callStackTop != DELTA_DEBUG_CALLSTACK_EMPTY_INDEX
	//
	DASSERT(
		callStackCurr == DELTA_DEBUG_CALLSTACK_EMPTY_INDEX || 
		callStackTop != DELTA_DEBUG_CALLSTACK_EMPTY_INDEX
	);
	DASSERT(callStackTop < (util_i32) callStack.size());

	for (util_ui32 i = 0; i < (util_ui32) callStack.size(); ++i)
		DASSERT(callStack[i].IsValid() == (i <= (util_ui32) callStackTop));
}

////////////////////////////////////////////////////////////////////
// Returns the VM encompassing the call site for the current context.
// It is the context below the current (if any).
//
DeltaVirtualMachine* DeltaDebugCallStackInspector::GetCurrCallerVM (void) {
	DASSERT(!IsBottomLevelReached()); 
	DASSERT(callStack[callStackCurr - 1].IsValid());
	return DPTR(callStack[callStackCurr - 1].vm);
}

//*******************************
// Returns the VM called by the the current context.
// It is the context above the current.
//
DeltaVirtualMachine* DeltaDebugCallStackInspector::GetCurrCalleeVM (void) {
	DASSERT(!IsTopLevelReached());
	DASSERT(callStack[callStackCurr + 1].IsValid());
	return DPTR(callStack[callStackCurr + 1].vm);
}

//*******************************

DeltaVirtualMachine* DeltaDebugCallStackInspector::GetCurrVM (void) {
	DASSERT(IsCurrCallValid());
	return DPTR( callStack[callStackCurr].vm);
}

//*******************************
// Gets the VM of a call record.
//
DeltaVirtualMachine* DeltaDebugCallStackInspector::GetVM (util_ui16 call) {
	DASSERT(call >= 0 && call < GetTotal());
	DASSERT(callStack[call].IsValid());
	return DPTR(callStack[call].vm);
}

DeltaVirtualMachine* DeltaDebugCallStackInspector::GetTopVM (void)
	{ return GetVM(GetTotal() - 1); }

////////////////////////////////////////////////////////////////////

void DeltaDebugCallStackInspector::Down (void) {
	DASSERT(IsCurrCallValid() && callStackCurr);
	--callStackCurr;
}

void DeltaDebugCallStackInspector::Up (void) {
	DASSERT(IsCurrCallValid() && callStackCurr < callStackTop);
	++callStackCurr;
}

util_ui16 DeltaDebugCallStackInspector::GetTotal (void) const
	{ return callStackTop + 1; }

util_ui16 DeltaDebugCallStackInspector::GetCurr (void) const {
	DASSERT(IsCurrCallValid());
	return callStackCurr;
}

util_ui16 DeltaDebugCallStackInspector::GetCalleeOf (util_ui16 call) const {
	DASSERT(call >= 0 && call < callStackTop);
	return call + 1;
}

util_ui16 DeltaDebugCallStackInspector::GetCallerOf (util_ui16 call) const {
	DASSERT(call > 0 && call <= callStackTop);
	return call - 1;
}

bool DeltaDebugCallStackInspector::IsCurrCallValid (void) const {
	return	GetTotal() &&
			callStackCurr > DELTA_DEBUG_CALLSTACK_EMPTY_INDEX &&
			callStackCurr < GetTotal() &&			
			callStack[callStackCurr].IsValid();
}

////////////////////////////////////////////////////////////////////

const DeltaClosure* DeltaDebugCallStackInspector::GetCurrClosure (void) const {
	DASSERT(IsCurrCallValid());
	return callStack[callStackCurr].closure;
}

DeltaDebugCallStackFuncValue DeltaDebugCallStackInspector::GetCurrFunction (void) const {
	DASSERT(IsCurrCallValid());
	return callStack[callStackCurr].func;
}

DeltaDebugCallStackEntryType DeltaDebugCallStackInspector::GetCurrCallType (void) const {
	DASSERT(IsCurrCallValid());
	return callStack[callStackCurr].type;
}

util_ui32 DeltaDebugCallStackInspector::GetCurrBlocksIndex (void) const {
	DASSERT(IsCurrCallValid());
	return callStack[callStackCurr].blocksIndex;
}

////////////////////////////////////////////////////////////////////

util_ui32 DeltaDebugCallStackInspector::GetCurrCallIndex (void) const 
	{ DASSERT(IsCurrCallValid()); return callStackCurr; }

util_ui16 DeltaDebugCallStackInspector::GetCallLine (util_ui16 call) {
	DASSERT(call >= 0 && call < GetTotal());
	DASSERT(callStack[call].IsValid());
	return callStack[call].callLine;
}

util_ui16 DeltaDebugCallStackInspector::GetCurrCallLine (void) const 
	{ DASSERT(IsCurrCallValid()); return callStack[callStackCurr].callLine; }

////////////////////////////////////////////////////////////////////

void DeltaDebugCallStackInspector::Push (
		DeltaVirtualMachine*			vm,
		const DeltaClosure*				closure,
		DeltaDebugCallStackEntryType	type,
		FuncValue						func, 
		DeltaCodeAddress				pc,
		util_ui32						top, 
		util_ui32						topsp,
		util_ui32						blocksIndex,
		util_ui16						callLine
	) {

	DASSERT(callStackTop < (util_i32) callStack.size());

	if (util_ui32(++callStackTop) == callStack.size())
		callStack.resize(2 * callStack.size());

	new (&callStack[callStackTop])	Entry(
										vm, 
										closure,
										type,
										func, 
										pc,
										top, 
										topsp,
										blocksIndex,
										callLine
									);	
}

////////////////////////////////////////////////////////////////////

void DeltaDebugCallStackInspector::Pop (void) {
	DASSERT(GetTotal() && callStack[callStackTop].IsValid());
	callStack[callStackTop--].Reset();
}

////////////////////////////////////////////////////////////////////

void DeltaDebugCallStackInspector::ChangeTopGlobalCode (util_ui32 newLine, DeltaCodeAddress pc) {
	DASSERT(
		GetTotal()												&& 
		callStack[callStackTop].IsValid()						&& 
		pc														&& 
		pc < DPTR(callStack[callStackTop].vm)->GetCodeSize()	&&
		callStack[callStackTop].type == DeltaDebugCallStackEntry_GlobalCode
	);

	callStack[callStackTop].callLine	= newLine;
	callStack[callStackTop].pc			= pc;
}

////////////////////////////////////////////////////////////////////

void DeltaDebugCallStackInspector::ChangeTopStackPointers (util_ui32 top, util_ui32 topsp) {
	DASSERT(
		GetTotal()												&& 
		callStack[callStackTop].IsValid()						&& 
		top <= topsp											&&
		callStack[callStackTop].topsp == topsp					&&
		(callStack[callStackTop].type == DeltaDebugCallStackEntry_UserFunc || callStack[callStackTop].type == DeltaDebugCallStackEntry_GlobalCode)
	);
	callStack[callStackTop].top	= top;
}

////////////////////////////////////////////////////////////////////

void DeltaDebugCallStackInspector::RetrieveCurrCallValues (util_ui32* top, util_ui32* topsp) const {
	DASSERT(IsCurrCallValid());
	*top	= callStack[callStackCurr].top;
	*topsp	= callStack[callStackCurr].topsp;
}

void DeltaDebugCallStackInspector::RetrieveCallValues (util_ui16 call, util_ui32* top, util_ui32* topsp) const {
	DASSERT(call >= 0 && call < GetTotal() && callStack[call].IsValid());
	*top	= callStack[call].top;
	*topsp	= callStack[call].topsp;
}

DeltaCodeAddress DeltaDebugCallStackInspector::RetrieveCurrCallPC (void) const
	{ DASSERT(IsCurrCallValid()); return callStack[callStackCurr].pc; }

DeltaCodeAddress DeltaDebugCallStackInspector::RetrieveCallPC (util_ui16 call) const
	{ DASSERT(call >= 0 && call < GetTotal() && callStack[call].IsValid()); return callStack[call].pc; }

////////////////////////////////////////////////////////////////////
