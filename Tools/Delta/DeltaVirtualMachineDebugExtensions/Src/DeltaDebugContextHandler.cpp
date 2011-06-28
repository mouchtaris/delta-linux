// DeltaDebugContextHandler.cpp
// Handling current debug context during debugging (and when
// in breakpoint).
// ScriptFighter Project.
// A. Savidis, June 2006 (original).
// RF to a separate class (February 2010).
//

#include "DeltaDebugContextHandler.h"
#include "DeltaLibFuncBinder.h"
#include "Execute.h"

DeltaDebugCallStackInspector*	DeltaDebugContextHandler::callStack			
									= (DeltaDebugCallStackInspector*) 0;
DeltaDebugUserVarsInspector*	DeltaDebugContextHandler::userVars			
									= (DeltaDebugUserVarsInspector*) 0;
const DeltaDebugMessengerProxy*	DeltaDebugContextHandler::messenger			
									= (DeltaDebugMessengerProxy*) 0;
DeltaDebugContextHandler*		DeltaDebugContextHandler::currDebugContext	
									= (DeltaDebugContextHandler*) 0;

void DeltaDebugContextHandler::Initialise (
		DeltaDebugCallStackInspector*	_callStack, 
		DeltaDebugUserVarsInspector*	_userVars,
		const DeltaDebugMessengerProxy*	_messenger
	) {
	callStack	= _callStack;
	userVars	= _userVars;
	messenger	= _messenger;
}

void DeltaDebugContextHandler::CleanUp (void) {}

////////////////////////////////////////////////////////////////////////
// Sets the pointer to function debug information, if debug information
// is included and we are in a function. This is used for fast access on 
// local function debug information, without requiring iteration in the list 
// of functions, every time local data information is requested for the function 
// at the current activation record.
//
void DeltaDebugContextHandler::SetCurrentTracedFunction (void) {

	DeltaDebugCallStackFuncValue	func = GetCallStack().GetCurrFunction();
	DeltaDebugCallStackEntryType	type = GetCallStack().GetCurrCallType();

	// Reset, they stay like this if no debug info, or if at global code context.

	unullify(currTracedFuncDebugInfo);
	unullify(currLibFunc);
	GetVars().ResetCurrTracedFunction();

	if (type == DeltaDebugCallStackEntry_GlobalCode)			// Global code.
		GetVars().SetCurrTracedIsGlobalCode();
	else
	if (type == DeltaDebugCallStackEntry_LibraryFunc)			// Library function.
		GetVars().SetCurrTracedFunction(
			currLibFunc	= DNULLCHECK(DeltaLibFuncBinder::GetName(func))
		);
	else {
		DASSERT(type == DeltaDebugCallStackEntry_UserFunc);		// User-defined function.
		if (DPTR(vm)->HasDebugInfo()) {
			DASSERT(func != DELTA_CANTBE_A_FUNCTION_ADDRESS);
			currTracedFuncDebugInfo = DNULLCHECK(GetDebugInfoOfFunction((DeltaCodeAddress) func));
		}
		GetVars().SetCurrTracedFunction(
			currTracedFuncDebugInfo,
			DNULLCHECK(DPTR(vm)->GetFuncInfo((DeltaCodeAddress) func))
		);
	}
}

////////////////////////////////////////////////////////////////////////
// Get debug information of a called function.

const DeltaDebugFuncInfo* DeltaDebugContextHandler::GetDebugInfoOfFunction (DeltaCodeAddress addr) const {
	
	DASSERT(DPTR(vm)->HasDebugInfo());

	for (util_ui16 i = 0, n = GetDebugInfo().GetTotalFuncs(); i < n; ++i)
		if (GetDebugInfo().GetFunc(i).GetAddress() == addr)
			return &GetDebugInfo().GetFunc(i);

	DASSERT(false);
	return (DeltaDebugFuncInfo*) 0;
}

////////////////////////////////////////////////////////////////////////
// The stack saved PC - 1 is the instruction number (call site)
// calling the current function. This instruction will be
// the last program instruction if we have a local function called
// from C++ code (like overloaded operators, for example). Otherwise,
// such a CALL instruction may either have 0 line or it should
// match the call line.
//
bool DeltaDebugContextHandler::MatchesCallLineWithCallInstruction (util_ui32 callLine) const {

	DASSERT(GetCallStack().IsAtFunctionCall());

	// In this case it is an embedding call, thus no check of call line
	// is viable.
	if (GetCallStack().IsBottomLevelReached())
		return true;

	// In case the caller is a different vm then its pc is not stored 
	// in the callee stack due to the call. Thus we cannot verify the 
	// caller line from the pc stored the callee stack.

	if (GetCallStack().GetCurrCallerVM() != vm)
		return true;
	else {
		util_ui32 pc = GetVars().GetStoredPC();
		if (pc == DELTA_PC_PROGRAM_END_VM(vm))
			return true;
		else {
			DASSERT(pc < DELTA_PC_PROGRAM_END_VM(vm));
			const DeltaInstruction& instr = DPTR(vm)->GetInstruction(pc - 1);
			return instr.IsCall() && (instr.line == DELTA_CANTBE_A_SOURCE_LINE || instr.line == callLine);
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Get information for the current function.

const std::string DeltaDebugContextHandler::GetCurrFunction (
		util_ui16*	defLine, 
		util_ui16*	callLine, 
		util_ui16*	scope
	) const {

	DASSERT(GetCallStack().IsAtFunctionCall());

	*defLine	= DBG_NO_LINE_INFO;
	*callLine	= DBG_NO_LINE_INFO;
	*scope		= 0;
	*callLine	= GetCallStack().GetCurrCallLine();
	
	DASSERT(MatchesCallLineWithCallInstruction(*callLine));

	if (!DPTR(vm)->HasDebugInfo()) {							// Then get it directly from the VM.
		DeltaVirtualMachine::CallInformation info;
		DeltaVirtualMachine::GetCall(GetCallStack().GetCurrCallIndex(), &info);
		*defLine = info.defLine;
		return info.name;
	}
	else {
		DASSERT(InFunction());

		if (!GetTracedProgramFunction())
			{ DASSERT(currLibFunc); return currLibFunc; }
		else {
			*defLine	= GetTracedProgramFunction()->GetLine();
			*scope		= GetTracedProgramFunction()->GetScope();
			return GetTracedProgramFunction()->GetName();
		}
	}
}

////////////////////////////////////////////////////////////////////////

void DeltaDebugContextHandler::SendCurrentContext (void) const {

	util_ui16 defLine	= DBG_NO_LINE_INFO;
	util_ui16 callLine	= DBG_NO_LINE_INFO;
	util_ui16 scope		= 0;

	std::string source(DPTR(vm)->Source());

	// This is a virtual file name for dynamic code (don't bother with
	// the duplicate presence of vmId).
	//
	if (GetDebugInfo().IsDynamicCode()) 
		source.append(DPTR(vm)->Id());

	std::string func = source +  "::" + DPTR(vm)->Id() + "::";
	std::string call;

	if (GetCallStack().IsAtFunctionCall()) {

		func.append(GetCurrFunction(&defLine, &callLine, &scope));

		// Now construct the call string, by concatenating
		// together the actual arguments.

		call = "(";

		util_ui16	n		= GetVars().TotalActualArgs();
		bool		first	= true;

		for (util_ui16 i = n - GetVars().TotalUserActualArgs(); i < n; ++i	) {
			if (!first)
				call.append(", ");
			else
				first = false;
			DeltaValue* obj = GetVars().GetActualArg(i);
			DASSERT(obj);
			call.append(obj->ConvertToStringBySkippingUserCode());
		}

		call.append(")");
	}
	else {
		DASSERT(GetCallStack().IsAtGlobalCode());
		func.append("at " DBG_GLOBAL_CODE_DESCRIPTION);
		call		= "()";
		callLine	= GetCallStack().GetCurrCallLine();
		defLine		= DELTA_GLOBALCODE_DEFLINE; 
	}

	GetMessenger().NotifySendCurrFunction(
		func.c_str(),
		defLine,
		callLine,
		scope,
		call.c_str()
	);
}

////////////////////////////////////////////////////////////////////////
