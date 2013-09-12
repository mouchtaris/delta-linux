// DeltaExceptionHandling.cpp
// Exception handling instructions.
// Script Fighter Project.
// A. Savidis, August 2006.
// Extended May 2009 to support native code trap and throw
// (cross-language exception handling).
//
// The method adopts book-keeping of a parallel lightweight call stack
// since the unwinding policy differentiates per call context (delta 
// func, delta global code, lib func and native code). The amortised
// overhead over vm function calls is actually very small.
//
#include "DeltaExceptionHandling.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaString.h"
#include "DeltaRuntimeError.h"
#include "DeltaVMRegistry.h"
#include "Execute.h"

DeltaExceptionHandling*	DeltaExceptionHandling::singletonPtr = (DeltaExceptionHandling*) 0;

USINGLETON_APISTYLE_IMPL_PUBLICSTDMETHODS(DeltaExceptionHandling)
USINGLETON_APISTYLE_IMPL_GETTER(DeltaExceptionHandling)

#define	RETURN_CONTINUE_UNWINDING	return true
#define	RETURN_STOP_UNWINDING		return false

//////////////////////////////////////////////////////////////

DeltaExceptionHandling::DeltaExceptionHandling (void) : 
	currList						((TrapBlockList*) 0),
	currVM							((DeltaVirtualMachine*) 0),
	expectedNativeTrapDisableCalls	(0),
	isUnwinding						(false),
	shouldUnwind					(false),
	isUnwindingFromNativeCode		(false),
	postingUnhandledExceptionError	(false)
	{}

DeltaExceptionHandling::~DeltaExceptionHandling(){
	DASSERT(Invariant());
	std::for_each(
		trapBlockStack.begin(),
		trapBlockStack.end(),
		uptrdestructorfunctor<TrapBlockList*>()
	);
	trapBlockStack.clear();
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::SetException (const DeltaValue& e) {
	DASSERT(Invariant()); 
	exceptionValue.Undefine();
	exceptionValue.Assign(e); 
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::Throw (DeltaVirtualMachine* vm, const DeltaValue& exception) {

	DASSERT(Invariant()); 
	DASSERT(!IsUnwinding());

	if (DPTR(vm)->GetDebugger())
		DPTR(vm)->GetDebuggerRef().OnThrow();

	if (!IsThereAnyTrapBlock()) {
		postingUnhandledExceptionError = true;
		DPTR(vm)->PrimaryError(
			"'throw' stmt generates unhandled exception '%s'!",
			exception.ConvertToStringBySkippingUserCode().c_str()
		);
		postingUnhandledExceptionError = false;
	}
	else {
		SetException(exception);
		SetUnwinding(true);
		SetUnwindingCheckers();
		Unwind(vm);
	}
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::PopExecutionLoop (DeltaVirtualMachine* vm) {

	if (!DPTR(vm)->HasProducedError()) {

		CallContext& call(callStack.front());
		DASSERT(call.first == vm && call.second == InExecutionLoop);

		PopFunc();
	}
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::PushFunc (DeltaVirtualMachine* vm, Context context) {

	DASSERT(Invariant() && (context == InDeltaGlobalCode || context == InNativeCode || context == InExecutionLoop));
	DASSERT(vm || context == InNativeCode);
	DASSERT(context != InNativeCode || callStack.empty() || callStack.front().second != InNativeCode);

	trapBlockStack.push_front(currList = DNEW(TrapBlockList));
	callStack.push_front(
		CallContext(
			currVM = vm, 
			currContext = context, 
			ContextInfo()
		)
	);
	unullify(currFunc);

	DASSERT(IsValidCallContext(callStack.front()));
	DASSERT(Invariant()); 
}

//****************************

void DeltaExceptionHandling::PushFunc (DeltaVirtualMachine* vm, Context context, util_ui32 pc) {

	DASSERT(Invariant() && vm && pc && context == InDeltaFunction);

	trapBlockStack.push_front(currList = DNEW(TrapBlockList));
	callStack.push_front(CallContext(currVM = vm, currContext = context, ContextInfo(pc)));
	unullify(currFunc);

	DASSERT(IsValidCallContext(callStack.front()));
	DASSERT(Invariant()); 
}

//****************************

void DeltaExceptionHandling::PushFunc (DeltaVirtualMachine* vm, Context context, DeltaLibraryFunc func) {

	DASSERT(Invariant() && vm && func && context == InLibraryFunction);
	trapBlockStack.push_front(currList = DNEW(TrapBlockList));
	callStack.push_front(CallContext(currVM = vm, currContext = context, ContextInfo(currFunc = func)));

	DASSERT(IsValidCallContext(callStack.front()));
	DASSERT(Invariant()); 
}

//////////////////////////////////////////////////////////////

bool DeltaExceptionHandling::IsValidCallContext (const CallContext& c) const {
	return	(	c.second == InExecutionLoop		&& 
				c.first && c.third.IsEmpty()	)								||

			(	c.second == InNativeCode		&& 
				!c.first && c.third.IsEmpty()	)								||

			(	(IsDeltaCodeContext(c.second) || c.second == InLibraryFunction) && 
				c.first															&& 
				(!c.third.IsEmpty() || c.second == InDeltaGlobalCode)	);
}

//////////////////////////////////////////////////////////////
// When a vm gets destroyed while being executing, some calls may
// remain on the exception handling stack that need to be removed.
//
void DeltaExceptionHandling::RemoveAllCalls (DeltaVirtualMachine* vm) {

	DASSERT(Invariant() && vm && !IsUnwinding());
	TrapBlockStack::iterator	i_traps	= trapBlockStack.begin();	
	CallStack::iterator			i_calls = callStack.begin();
	
	while (i_calls != callStack.end()) {

		DASSERT(i_traps != trapBlockStack.end());
		DASSERT(IsValidCallContext(*i_calls));

		if (i_calls->first == vm) {
			udelete(*i_traps);
			trapBlockStack.erase(i_traps++);
			callStack.erase(i_calls++);
		}
		else
			++i_traps, ++i_calls;
	}
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::SetUnwindingCheckers (void) const {
	DASSERT(Invariant());
	for (CallStack::const_iterator i = callStack.begin(); i != callStack.end(); ++i) {
		DASSERT(IsValidCallContext(*i));
		if (DeltaVirtualMachine* vm = i->first)
			DPTR(vm)->SetUnwindingChecker();
	}
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::PopCallsAffectedByError (DeltaVirtualMachine* vm) {

	DASSERT(!IsUnwinding());	// Can't generate an error during unwinding.
	DASSERT(Invariant() && vm && DPTR(vm)->HasProducedError());

	DELTA_DETERMINE_CALLSTACK_ENTRIES_TO_POP_ON_ERROR(
		DeltaVirtualMachine*,
		i->first,
		CallStack::iterator i = callStack.begin(),
		UEMPTY,
		&*i != &callStack.back(),	// Not last (bottom of stack)
		++i,
		vm
	);

	while (callsToPop--) {
		if (callStack.front().second == InNativeCode) {
			DASSERT(expectedNativeTrapDisableCalls);
			--expectedNativeTrapDisableCalls;
		}
		PopFunc();
	}	
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::PopFunc (void) {

	DASSERT(!Empty());
	DASSERT(Invariant());

	DDELETE(currList);
	trapBlockStack.pop_front();
	callStack.pop_front();

	if (!trapBlockStack.empty()) {	// only at global code this is empty

		DASSERT(IsValidCallContext(callStack.front()));

		currList	= trapBlockStack.front();
		currContext = callStack.front().second;
		currVM		= callStack.front().first;

		if (currContext == InLibraryFunction)
			currFunc = callStack.front().third.func;
		else
			unullify(currFunc);
	}
	else {	// gracefull program end only
		DASSERT(callStack.empty());
		unullify(currVM);
		unullify(currList);
		unullify(currFunc);
		currContext	= InDeltaGlobalCode;
	}

	DASSERT(Invariant()); 
}
	
//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::TrapEnable (DeltaCodeAddress addr) {	// TRAPENABLE
	DASSERT(Invariant() && currList);
	DASSERT(IsDeltaCodeContext(callStack.front().second));
	DASSERT(
		std::find(
			DPTR(currList)->begin(),
			DPTR(currList)->end(),
			addr
		) == DPTR(currList)->end()
	);
	DPTR(currList)->push_back(addr);
}
		
//////////////////////////////////////////////////////////////
// Necessary in break / continue which can skip the trap disable
// instruction.

void DeltaExceptionHandling::DoMultipleTrapDisable (util_ui16 n) {
	while (n--)
		TrapDisable(DPTR(currList)->back());
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::TrapDisable (DeltaCodeAddress addr) {	// TRAPDISABLE

	DASSERT(Invariant() && currList);
	DASSERT(IsDeltaCodeContext(callStack.front().second));
	DASSERT(DPTR(currList)->back() == addr);

	DPTR(currList)->pop_back();

	DASSERT(
		std::find(
			DPTR(currList)->begin(),
			DPTR(currList)->end(),
			addr
		) == DPTR(currList)->end()
	);
	DASSERT(Invariant()); 
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::Trap (DeltaVirtualMachine* vm, DeltaValue* lvalue, util_ui16 blockId) {

	DASSERT(Invariant()); 

	// Perform artifical block exits until the trap block.
	DPTR(vm)->DoExitBlocksUntil(blockId);

	// Assign the exception value.
	
	lvalue->Assign(GetException());
	ResetException();

	// Disable the current trap block. This is done
	// automatically to avoid the need of an explicit 
	// trap disable instuction.

	TrapDisable(DPTR(vm)->PC());

	DASSERT(Invariant()); 
}

//////////////////////////////////////////////////////////////

bool DeltaExceptionHandling::NativeTry (void) {
	PushFunc((DeltaVirtualMachine*) 0, InNativeCode); 
	++expectedNativeTrapDisableCalls;
	return true;
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::NativeTrapDisable (void) {

	DASSERT(Invariant()); 
	DASSERT(!IsUnwinding());

	if (expectedNativeTrapDisableCalls) {
		DASSERT(
			!callStack.empty()							&&
			!callStack.front().first					&& 	// no vm in native code
			callStack.front().second == InNativeCode	&&
			!callStack.front().third	// no pc or func
		);

		PopFunc();
		--expectedNativeTrapDisableCalls;
	}
}

//////////////////////////////////////////////////////////////

bool DeltaExceptionHandling::NativeTrap (DeltaValue* lvalue) {

	DASSERT(Invariant()); 
	DASSERT(IsUnwinding());

	UnwindNativeCode();

	lvalue->Assign(GetException());
	ResetException();
	SetUnwinding(false);	// Stop unwinding.

	return true;
}

//////////////////////////////////////////////////////////////

void DeltaExceptionHandling::NativeThrow (const std::string& from, util_ui32 line, const DeltaValue& exception) {

	DASSERT(!IsUnwinding());
	DASSERT(!callStack.empty() && callStack.front().second != InNativeCode);
	DASSERT(DeltaVirtualMachine::GetCallingVM()); // Must have enclosing Delta code to post a native throw.

	if (!IsThereAnyTrapBlock())
		DPTR(DeltaVirtualMachine::GetCallingVM())->PrimaryError(
			"native throw from '%s' line %u generates unhandled exception '%s'!",
			from.c_str(),
			line,
			exception.ConvertToStringBySkippingUserCode().c_str()
		);
	else {
		SetException(exception);
		SetUnwinding(true);
		SetUnwindingCheckers();
		shouldUnwind = true;	// Next time in an vm execution loop we will unwind.
	}
}

//////////////////////////////////////////////////////////////
// Should be called after every instruction fetch-execute cycle.
// If false is returned, the execution cycle must be exited.
//
bool DeltaExceptionHandling::PerformUnwindingTests (DeltaVirtualMachine* vm) {
	
	DASSERT(Invariant()); 

	if (!IsUnwinding())
		{ DPTR(vm)->ResetUnwindingChecker(); return true; }
	else {

		if (IsUnwinding() && ShouldUnwind())
			Unwind(vm);

		if (IsUnwinding() && !ShouldUnwind()) {				// Signifies request to break execution loop.
			SwitchShouldUnwind();							// Next time we do unwind.
			return false;
		}
		else
			return true;
	}
}

//////////////////////////////////////////////////////////////
// Returns true if there is a trap block that can handle a raised
// exception when stack unwinding is initiated.

bool DeltaExceptionHandling::IsThereAnyTrapBlock (void) const {

	DASSERT(!IsUnwinding());

	TrapBlockStack::const_reverse_iterator	i = trapBlockStack.rbegin();
	CallStack::const_reverse_iterator		j = callStack.rbegin(); 

	while (i != trapBlockStack.rend() && j != callStack.rend()) {
		DASSERT(IsValidCallContext(*j));
		if ((IsDeltaCodeContext(j->second) && !(*i)->empty()) || IsNativeTryBlockContext(j->second))
			return true;
		++i, ++j;
	}
	DASSERT(i == trapBlockStack.rend() && j == callStack.rend()); 
	return false;
}

//////////////////////////////////////////////////////////////

bool DeltaExceptionHandling::HasTrapBlockAtThisCall (void) const
	{ return !DPTR(currList)->empty(); }

void DeltaExceptionHandling::Trap (void) {
	DPTR(currVM)->pc = *(DPTR(currList)->rbegin());	// Assign trap address to PC.
	DPTR(currVM)->ResetUnwindingChecker();			// Must be done explicitly, as it was reset.
	SetUnwinding(false);
}

//////////////////////////////////////////////////////////////

bool DeltaExceptionHandling::UnwindDeltaFunction (void) {

	DASSERT(!Empty() && currVM);
	DeltaVirtualMachine* prevVM = currVM;

	DPTR(currVM)->DoExitAllBlocksOfCurrentContext();

	if (DPTR(currVM)->GetDebugger())
		DPTR(currVM)->GetDebuggerRef().OnStackUnWindingProgramFunction();

	DPTR(currVM)->DoLocalFuncReturnCode();		// Will lead to a PopFunc() and possible change of curr vm.

	// Between cross-vm calls or nested loops; we need to exit
	// the execution loop
	if (IsExecutionLoop(currContext))
		RETURN_STOP_UNWINDING;
	else {
		DASSERT(IsDeltaCodeContext(currContext) && prevVM == currVM);
		RETURN_CONTINUE_UNWINDING;
	}
}

//////////////////////////////////////////////////////////////

bool DeltaExceptionHandling::UnwindLibraryFunction (void) {

	DASSERT(!Empty() && currVM && !shouldUnwind);

	if (DPTR(currVM)->GetDebugger())
		DPTR(currVM)->GetDebuggerRef().OnStackUnWindingLibraryFunction();

	// If this is not the bottom call we have to exit to 
	// cause unwinding (return) of the C++ vm calls.

	DASSERT(currFunc);
	DPTR(currVM)->DoExternFuncReturnCode((DeltaLibraryFunc) currFunc);	// Will lead to a PopFunc().

	shouldUnwind = isUnwindingFromNativeCode;	// Should unwind the next time unwinding checks are performed?
	RETURN_STOP_UNWINDING;						// Break unwinding loop.
}

//////////////////////////////////////////////////////////////

bool DeltaExceptionHandling::UnwindGlobalCode (void) {

	DPTR(currVM)->DoExitAllBlocksOfCurrentContext();

	if (DPTR(currVM)->GetDebugger())
		DPTR(currVM)->GetDebuggerRef().OnStackUnWindingGlobalCode();

	// When unwinding a global context there is no way to 
	// continue its execution thus PROGRAMEND is not manually invoked. 
	// This is why we need to manually invoke PopFunc() below.

	PopFunc();

	// We may either come from a lib function (like vmrun) or an 
	// explicit vm invocation from within native code.

	DASSERT(currContext == InLibraryFunction || currContext == InNativeCode);
	RETURN_CONTINUE_UNWINDING;		// Never cause breaking of the unwinding loop.
}

//////////////////////////////////////////////////////////////
// Always manually invoked inside NativeTrap.

bool DeltaExceptionHandling::UnwindNativeCode (void) {
	
	DASSERT(
		!callStack.empty()							&&
		!callStack.front().first					&& 	// no vm in native code
		callStack.front().second == InNativeCode	&&
		!callStack.front().third					&&	// no pc or func
		shouldUnwind
	);	

	PopFunc();
	--expectedNativeTrapDisableCalls;

	shouldUnwind = false;		// If native code on top of the lib func.
	RETURN_STOP_UNWINDING;		// Always breaking the unwinding loop.
}

//////////////////////////////////////////////////////////////
// MAIN STACK UNWINDING ALGORITHM FOR Delta.
// Unwinding should handle to be carried out across
// C++ calls of Delta library functions or cross vm function
// calls, while gracefully cleaning-up the stack and continuing
// unwinding.

void DeltaExceptionHandling::Unwind (DeltaVirtualMachine* caller) {

	DASSERT(
		Invariant()			&& 
		IsUnwinding()		&& 
		!callStack.empty()	&&
		(callStack.front().second != InExecutionLoop &&
		 callStack.front().second != InNativeCode)
	);

	shouldUnwind				= false;
	isUnwindingFromNativeCode	= !IsDeltaCodeContext(callStack.front().second);
	caller->pc					= DELTA_PC_PROGRAM_END_VM(caller);

	while (true) {

		// Unwound everything and found no handler is impossible
		// since this is checked when an exception is thrown.
		DASSERT(!callStack.empty());

		if (HasTrapBlockAtThisCall()) 
			{ Trap(); return; }	// Exception handling point reached. 
		else
		switch (currContext) {
			case InDeltaFunction	:	if (!UnwindDeltaFunction())		return;		continue;
			case InLibraryFunction	:	if (!UnwindLibraryFunction())	return;		continue;
			case InDeltaGlobalCode	:	if (!UnwindGlobalCode())		return;		continue;
			case InExecutionLoop	:	return;

			case InNativeCode		:	DASSERT(false);	// should never reveal native code while unwinding
			default					:	DASSERT(false);
		}
	}
}

//////////////////////////////////////////////////////////////
