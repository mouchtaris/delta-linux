// DeltaDebugExtensions.cpp
// It should be noted that the VMs emulate independent processor
// machines. Hence, each one keeps its own call stack and performs activation
// record management. However, since we support in the stdlib cross VM calls,
// meaning we can SYNCHRONOUSLY call another VM's global functions. Such 
// calls to other VM's global functions may in turn stop execution 
// at some break point. In this case, the call stack images could be:
// VM1: |f1|f2|f3|extern : vmcall|
// VM2: |g1|g2|
//
// When requesting the call stack at the break point of VM2, we simply
// get |g1|g2|, since there is no way for VM2 to know that the call
// was originated from VM1, and to have access to VM1's activation stack.
// Even though this could be done, it would complicate unecessarily
// the implementation of the debugger, since we emphasize localised (to VM)
// debugging. The script developer may put proper break points at VM1
// so that upon returning from 'vmcall' the debugging of VM1 may continue.
// Script Fighter Project.

// A. Savidis, December 1999.
// Revisions (mainly clean-up), June 2001.
// Re-factoring as an orthogonal API, June 2006.
// Re-factoring to allow a cross-VM unified debug call-stack, June 2006.
//

#ifdef	DEBUGGER_EXTENSIONS

#include "DeltaDebugExtensions.h"
#include "debugbreakpointholder.h"
#include "DebugServer.h"

#include "DDebug.h"
#include "DeltaTypes.h"
#include "VMTypes.h"
#include "VirtualMachine.h"
#include "VMRegistry.h"
#include "deltalibfuncbinder.h"
#include "debugbreakpointholder.h"
#include "VMErrorMsg.h"
#include "ustlhelpers.h"
#include "ThreadLib.h"
#include "DebugExprParserAPI.h"

#include <list>
#include <algorithm>

//////////////////////////////////////////////////////////////
// Standard values returned upon unknown named debug items.
//
#define	UNKNOWN_FUNC	"<UnKnownFunc>"
#define	UNKNOWN_ARG		"<UnKnownActualArg>"
#define	UNKNOWN_LOCAL	"<UnKnownLocalVar>"
#define	UNKNOWN_GLOBAL	"<UnknownGlobalVar>"

//**********************************
// Automatic exit checks in case of runtime errors.
//
#define	EMPTY
#define	CHECK_EXECUTION_VALID(ret) \
	if (DPTR(DebugContextVM())->ExecutionCompleted()) \
		return ret;

//**********************************
// The mutex is shared by every VM, and is only destroyed when
// the last VM instance is destroyed, while created if no VM instance
// yet exists.
//
static util_ui16	mutexCounter = 0;
static Mutex*		breakPointMutex = (Mutex*) 0;

//**********************************

DeltaVirtualMachine*	DeltaDebugExtensions::breakVM = (DeltaVirtualMachine*) 0;
DeltaVirtualMachine*	DeltaDebugExtensions::debugContextVM = (DeltaVirtualMachine*) 0;
DeltaDebugExtensions::DebugStackEntry	
						DeltaDebugExtensions::callStack[DELTA_DEBUG_CALLSTACK_SIZE];
util_i32				DeltaDebugExtensions::callStackTop = -1;
util_i32				DeltaDebugExtensions::callStackCurr = -1;
bool					DeltaDebugExtensions::currActivRecValid = false;

void	(*DeltaDebugExtensions::onBreakEnter)(void)	= (void(*)(void)) 0;
void	(*DeltaDebugExtensions::onBreakLeave)(void)	= (void(*)(void)) 0;
void	(*DeltaDebugExtensions::onBreak)(void)		=  (void(*)(void)) 0;

//////////////////////////////////////////////////////////////

DeltaDebugExtensions::DeltaDebugExtensions (DeltaVirtualMachine* _vm) {

	DASSERT(_vm);
	vm = _vm;
	assertForcedBreak = false;

	// Initialise curr library function and debug
	// command.
	//
	currLibFunc		= (char*) 0;
	deferredDbgCmmd = DeltaDebugExecCommand_None;

	// Request the break-points.
	//
	std::list<DeltaBreakPoint>* myBpts = DebugBreakPointHolder::Get(DPTR(vm)->Id());
	if (myBpts)
		DebugBreakPointHolder::DeltaCopyBreakPoints(*myBpts, breakPoints);

	CreateBreakPointMutex();
}

//***************************

DeltaDebugExtensions::~DeltaDebugExtensions() {

	// Clear lines containing user code and break points.
	// Close the mutex.
	//
	linesWithCode.clear();
	breakPoints.clear();
	CloseBreakPointMutex();
}

//////////////////////////////////////////////////////////////
// The breakpoints need to be tested for validity before running,
// while proper messages need to be sent to the debug server if
// something is wrong.
//
void DeltaDebugExtensions::TestAllBreakPoints (void) {

	std::list<DeltaBreakPoint>::iterator bpt = breakPoints.begin();

	while (bpt != breakPoints.end()) {

		std::list<DeltaBreakPoint>::iterator subjectBpt = bpt;
		++bpt;

		if (!IsValidBreakPoint(subjectBpt->GetLine())) {

			util_ui32 nextLine = GetNextValidBreakPoint(subjectBpt->GetLine());	// Find next valid line.

			DeltaDebugServer::DoInfoInvalidBreakPoint(
				Debug_InfoInvalidBreakPoint,
				DPTR(vm)->Id(), 
				subjectBpt->GetLine(),
				nextLine,
				subjectBpt->GetConditionString()
			);

			if (nextLine)						// Found a valid next line ?
				subjectBpt->SetLine(nextLine);	// Change the original bpt line
			else
				breakPoints.erase(subjectBpt);	// Else remove it.
		}
	}
}

//------------------------------------------------------------------
// PUBLIC API.
// MANAGEMENT OF BREAK POINTS (KEEPS UP TO DATE THE BREAK POINT HOLDER
// TOO).
// MANAGEMENT OF ACTIVATION RECORDS, WITHOUT NAME INFORMATION. DOES NOT 
// NEED DEBUG INFORMATION TO BE INCLUDED.
//

void DeltaDebugExtensions::CreateBreakPointMutex (void) {

	if (!breakPointMutex) {	// Not created yet.
		DASSERT(!mutexCounter);
		breakPointMutex = DNEW(Mutex);
	}

	++mutexCounter;
}

void DeltaDebugExtensions::CloseBreakPointMutex (void) {

	DASSERT(breakPointMutex);
	DASSERT(mutexCounter);

	if (!--mutexCounter) {
		DDELETE(breakPointMutex);
		breakPointMutex = (Mutex*) 0;
	}
}

////////////////////////////////////////////////////////////////////
// The debug information is set so that the activation record
// variables are positioned at the top record of its respective VM.
//
void DeltaDebugExtensions::PositionAtTopActivationRecord (void) {
	activRecTopsp	= DPTR(vm)->topsp;
	activRecTop		= DPTR(vm)->top;
}

//*****************************
// When processing of activation records is about to begin,
// we check if the debug call-stack is empty. If yes, then the
// current activation record is invalid.
//
void DeltaDebugExtensions::StartCallStackManagement (void) {

	CHECK_EXECUTION_VALID(EMPTY);
	DASSERT(BreakPointVM() == DebugContextVM());	// We only start on break-point.
	VerifyCallStack();

	if (callStackTop == -1) // Empty
		currActivRecValid = false;
	else {

		// Position very distinct VM at its top activation
		// record for debugging. No problem if we pass from the
		// VM multiple times.
		//
		for (util_ui8 i = 0; i <= callStackTop; ++i)
			DPTR(callStack[i].vm)->GetDebugger().PositionAtTopActivationRecord();

		callStackCurr		= callStackTop;
		currActivRecValid	= true;

		DASSERT(callStack[callStackCurr].vm);

		// But the debug context is the current VM, currently selected
		// as the VM of the top call.
		//
		SetDebugContextVM(callStack[callStackCurr].vm);
	}
}

////////////////////////////////////////////////////////////////////
// Try to locate the previous activation record. If a call
// has been made (i.e. not at global execution scope),
// this should be located by getting the presious 'top' and 'topsp'
// as they have been stored in the stack prior to call. If we have
// reached global execution scope, then callStackCurr should be -1.
// The crossing of VM boundaries.
//
bool DeltaDebugExtensions::PreviousActivRec (void) {	// DOWN

	CHECK_EXECUTION_VALID(false);	// If execution invalid, fail.
	DASSERT(IsValidDebugContext());
	VerifyCallStack();

	if (!currActivRecValid || GlobalLevelReached())		
		return false;
	else 
	if (CallingVM() &&  CallingVM() != vm) {		// The calling VM is not the same.

		DeltaVirtualMachine* calling = CallingVM();	// Store, because it may change by going down.
		CallStackDown();							// Only the debug stack. We retain its previous context.
		DASSERT(callStackTop != -1);

		SetDebugContextVM(calling);

		return true;
	}
	else {

		// Else, we are at a call for the same VM, hence, we can go back one
		// activation record, extracting previous TOP and TOPSP values.
		//
		DeltaNumberValueType num;
		DPTR(vm)->stack[activRecTopsp + DELTA_TOPSP_OLDTOP_OFFSET].ToNumber(&num);
		activRecTop = (util_ui32) num;

		DPTR(vm)->stack[activRecTopsp + DELTA_TOPSP_OLDTOPSP_OFFSET].ToNumber(&num);
		activRecTopsp = (util_ui32) num;

		// If not at a global level, we set the current function.
		//
		if (CallStackDown() != -1)
			SetCurrFunction(CallStackGet());

		return true;
	}
}

////////////////////////////////////////////////////////////////////
// If current call is the to of the debug stack,
// then the top-most activation record is reached. Else, since we 
// do not save information on the stack to move upwards in
// activation records, we have to start from TOPSP
// until its saved value is the same as activRecTopsp.
// The crossing of VM boundaries.
//
bool DeltaDebugExtensions::NextActivRec (void) {	// UP

	CHECK_EXECUTION_VALID(false);
	DASSERT(IsValidDebugContext());
	VerifyCallStack();

	if (!currActivRecValid || callStackCurr == callStackTop)
		return false;

	if (CalledVM() != vm) {		// The calling VM is not the same.

		DeltaVirtualMachine* called = CalledVM();	// Store, because it may change by going up.
		CallStackUp();								// Only the debug stack. We retain its next context.
		SetDebugContextVM(called);

		return true;
	}

	// Start from current top of stack.
	//
	util_ui32				currTopsp = DPTR(vm)->topsp;
	util_ui32				currTop = DPTR(vm)->top;
	util_ui32				totalActivRecs = CallStackTotal();
	DeltaNumberValueType	num;

	// Iterate until we reach an activation record, whose
	// saved topsp / top are the same as the saved values in the current 
	// debug activation record. Then, this activation record is
	// required one. If we make more than callStackTop iterations, i.e.,
	// more than the existing activation records, this is a fatal error.
	//
	while (1) {

		DPTR(vm)->stack[currTopsp + DELTA_TOPSP_OLDTOPSP_OFFSET].ToNumber(&num);
		--totalActivRecs;

		// If previous activation record from this is current, then
		// the current is where we want to go. Else, if we have more
		// activation records, so we go to the previous one.
		//
		if (((util_ui32) num) == activRecTopsp) {

			activRecTopsp	= currTopsp;
			activRecTop		= currTop;
			break;
		}
		else
		if (totalActivRecs) {

			// Move one activation record down.
			//
			DPTR(vm)->stack[currTopsp + DELTA_TOPSP_OLDTOP_OFFSET].ToNumber(&num);
			currTop = (util_ui32) num;

			DPTR(vm)->stack[currTopsp + DELTA_TOPSP_OLDTOPSP_OFFSET].ToNumber(&num);
			currTopsp = (util_ui32) num;
		}
		else
			DASSERT(!"Could not locate current activation record!");
	}

	// Update currently called function in address
	// call stack and set current function.
	//
	CallStackUp();
	SetCurrFunction(CallStackGet());

	return true;
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugExtensions::InsideFunctionCall (void) const {
	return callStackTop != -1;
}

////////////////////////////////////////////////////////////////////
// In case the current activation record is invalid,
// global execution context is assumed.
//
bool DeltaDebugExtensions::GlobalLevelReached (void) const {

	CHECK_EXECUTION_VALID(false);
	return !currActivRecValid || callStackCurr == -1;
}

////////////////////////////////////////////////////////////////////
// Just the total number of cells used for global variables, not
// the total number of global variables.
//
util_ui16 DeltaDebugExtensions::TotalGlobalData (void) const {
	return (util_ui32) DPTR(vm)->totalGlobals;
}

////////////////////////////////////////////////////////////////////
// That simly allows to read global cells. Only when the algorithm
// for assigining global memory cells to global variables is known,
// the association to a single global program variable may be known.
//
DeltaObject* DeltaDebugExtensions::GetGlobalVar (util_ui16 offset) {
	DASSERT(offset < DPTR(vm)->totalGlobals);
	return DPTR(vm)->stack + (DPTR(vm)->stackSize - 1 - offset);
}

////////////////////////////////////////////////////////////////////
// Returns the total number of actual args passed in the call of
// the current activation record (n_user) INCLUDING the standard arguments
// passed by the language (n_all). This means in debug mode we provide
// all arguments, not just the user-specific ones.
//
util_ui16 DeltaDebugExtensions::TotalActualArgs (void) {

	CHECK_EXECUTION_VALID(0);

	if (!currActivRecValid || GlobalLevelReached())
		return 0;
	else {

		DeltaNumberValueType argN_offset;
		DPTR(vm)->stack[activRecTopsp + DELTA_TOPSP_OLDTOP_OFFSET].ToNumber(&argN_offset);

		util_ui32 arg0_offset = activRecTopsp + DELTA_TOPSP_FIRSTARG_OFFSET;

		util_ui16 n_all = (((util_ui32) argN_offset) - arg0_offset) + 1;

		DeltaNumberValueType num;
		DPTR(vm)->stack[activRecTopsp + DELTA_TOPSP_TOTALARGS_OFFSET].ToNumber(&num);
		util_ui16 n_user = (util_ui16) num;

		// In lib functions we do not have extra arguments, so
		// n_all = n_user. Else, we have 1 (arguments) or 2 (self)
		// extra arguments.
		//
		DASSERT(
			(!currLibFunc || n_all == n_user) &&
			(!!currLibFunc || 
			 (n_all > n_user &&
			 (n_all - n_user) <= DELTA_CALL_MAX_STD_ARGUMENTS)
			 )
		);

		return n_all;
	}
}

////////////////////////////////////////////////////////////////////
// This returns only the arguments supplied by the user
// in a function call, excluding 'self'and 'arguments'.
//
util_ui16 DeltaDebugExtensions::TotalUserActualArgs (void) {

	CHECK_EXECUTION_VALID(0);

	if (!currActivRecValid || GlobalLevelReached())
		return 0;
	else {

		DeltaNumberValueType num;
		DPTR(vm)->stack[activRecTopsp + DELTA_TOPSP_TOTALARGS_OFFSET].ToNumber(&num);
		return (util_ui16) num;
	}
}

////////////////////////////////////////////////////////////////////
// Returns the local data size of the function called in the
// current activation record. This is always the difference
// (DPTR(vm)->topsp - DPTR(vm)->top).
//
util_ui16 DeltaDebugExtensions::TotalLocalData (void) {

	CHECK_EXECUTION_VALID(0);

	if (!currActivRecValid || GlobalLevelReached())
		return 0;
	else {
		DASSERT(activRecTopsp >= activRecTop);
		return ((util_ui16) (activRecTopsp - activRecTop));
	}
}

////////////////////////////////////////////////////////////////////
// Get an actual argument by its relative order from the current
// activation record.
//
DeltaObject* DeltaDebugExtensions::GetActualArg (util_ui16 argNo) {

	CHECK_EXECUTION_VALID((DeltaObject*) 0);
	DASSERT(argNo < TotalActualArgs());

	if (!currActivRecValid || GlobalLevelReached())
		return (DeltaObject*) 0;
	else
		return DPTR(vm)->stack + (activRecTopsp + DELTA_TOPSP_FIRSTARG_OFFSET + argNo);
}

////////////////////////////////////////////////////////////////////
// Get a local var from the current activation record, by its relative
// order in the function definition. Notice that a local variable storage
// may syntactically map to many locally defined variables having non-overlapping
// lifetimes. Hence, viewing local data this way should be done together by analyzing
// the debug information in the text-generated file and always knowing the 
// active blocks.
//
DeltaObject* DeltaDebugExtensions::GetLocalVar (util_ui16 varNo) {

	CHECK_EXECUTION_VALID((DeltaObject*) 0);
	DASSERT(varNo < TotalLocalData());

	if (!currActivRecValid || GlobalLevelReached())
		return (DeltaObject*) 0;
	else
		return DPTR(vm)->stack + (activRecTopsp - varNo);
}

//------------------------------------------------------------------
// PRIVATE API.
// CALLSTACK MANAGEMENT. DOES NOT NEED DEBUG INFORMATION TO BE
// INCLUDED.
//

// Sets the pointer to function debug information, if debug information
// is included. This is used for fast access on local function debug
// information, without requiring iteration in the list of functions,
// each time local data information is requested for the function called
// in the current activation record.
//
void DeltaDebugExtensions::SetCurrFunction (DeltaDebugStackIndex funcIndex) {

	// Lets check if we have a library function.
	// In this case, we set debug info to null, and set the library
	// function name.
	//
	if (DELTA_ISLIBFUNC_INDEX(funcIndex)) {
		currFuncDebugInfo = (DeltaDebugFuncInfo*) 0;
		currLibFunc = const_cast<char*>(DeltaLibFuncBinder::GetName(DELTA_GETLIBFUNC_INDEX(funcIndex)));
		return;
	}

	currLibFunc = (char*) 0;

	// Else, we have a function address inside the index.
	// Locate the debug info for this function from its address.
	//
	DeltaCodeAddress  addr = DELTA_GETINSTR_INDEX(funcIndex);
	for (util_ui32 i=0; i < DPTR(vm)->debugInfo.totalFuncs; ++i)
		if (DPTR(vm)->debugInfo.funcs[i].GetAddress() == addr) {
			currFuncDebugInfo = DPTR(vm)->debugInfo.funcs + i;
			return;
		}

	DASSERT(!"Function not found in debug information!");
}

////////////////////////////////////////////////////////////////////
// Push a function address in the function address call stack.
// This stack is kept explicitly, since we do not store within
// the activation records the address of the current function
// prior to a call.
//
void DeltaDebugExtensions::PushFunc (DeltaDebugStackIndex funcIndex) {

	// To exceed the function address call stack means we have
	// more than 1024 activation records on the DPTR(vm)->stack. This may
	// happen in case of some recursive functions, but we have to track
	// it down and avoid it (it is too slow for games).
	//
	if (callStackTop >= DELTA_DEBUG_CALLSTACK_SIZE-1) {

		DeltaRunTimeError(
			"%s: Line %d: Call stack exceeded (total %d)",
			DPTR(vm)->Id(),
			DPTR(vm)->Line(),
			CallStackTotal()
		);

		DPTR(vm)->InvalidateExecution();
		return;
	}

	// Push current function on the shared debug call stack.
	// Also update curr and top.
	//
	callStack[callStackCurr = ++callStackTop].Set(vm, funcIndex);
}

////////////////////////////////////////////////////////////////////
// If popping currently processed activation record,
// set it as invalid.
//
void DeltaDebugExtensions::PopFunc (void) {

	DASSERT(callStackTop != -1);
	callStack[callStackTop].Reset();

	if (callStackCurr == callStackTop) {
		--callStackCurr;
		currActivRecValid = false;
	}

	--callStackTop;
}

/////////////////////////////////

util_ui16 DeltaDebugExtensions::CallStackTotal (void) {
	return callStackTop + 1;
}

////////////////////////////////////////////////////////////////////

util_i16 DeltaDebugExtensions::CallStackDown (void) {
	DASSERT(callStackCurr >= 0);
	return --callStackCurr;
}

////////////////////////////////////////////////////////////////////

util_i16 DeltaDebugExtensions::CallStackUp (void) {
	DASSERT(callStackCurr < callStackTop);
	return ++callStackCurr;
}

////////////////////////////////////////////////////////////////////

DeltaDebugStackIndex DeltaDebugExtensions::CallStackGet (void) {
	return callStack[callStackCurr].func;
}

////////////////////////////////////////////////////////////////////
// Verifies that the entries and variables for the call-stack
// are valid.
//
void DeltaDebugExtensions::VerifyCallStack (void) {

	// callStackCurr != -1 => callStackTop != -1
	//
	DASSERT(callStackCurr == -1 || callStackTop != -1);
	DASSERT(callStackTop < DELTA_DEBUG_CALLSTACK_SIZE);

	for (util_i32 i = 0; i < DELTA_DEBUG_CALLSTACK_SIZE; ++i)
		DASSERT(!callStack[i].IsValid() == i > callStackTop);
}

//------------------------------------------------------------------
// PUBLIC API.
// MANAGEMENT OF ACTIVATION RECORDS, WITH NAME INFORMATION.
// IT REQUIRES DEBUG INFORMATION TO BE INCLUDED.
//

static char libFuncNamedDecorated[DELTA_MAX_NAME_LEN+2];

inline const char* DECORATE_LIBFUNC_NAME (const char* funcName) {
	DASSERT(strlen(funcName) < DELTA_MAX_NAME_LEN);
	sprintf(libFuncNamedDecorated, "<%s>", funcName);
	return libFuncNamedDecorated;
}

////////////////////////////////////////////////////////////////////
// Get line and name information of function called at current
// activation record.
//
const char* DeltaDebugExtensions::CurrFunction (
					util_ui16* defLine, 
					util_ui16* callLine, 
					util_ui16* scope
				) {

	CHECK_EXECUTION_VALID((char*) 0);

	*defLine	= 0;
	*callLine	= 0;
	*scope		= 0;

	// Null name means we are at global level.
	//
	if (!currActivRecValid || GlobalLevelReached())
		return (char*) 0;

	// The DPTR(vm)->stack stored PC-1 is the instruction number of the
	// present function call. We extract the line of this instruction
	// directly from program code. This instruction will be
	// the last program instruction if we have a local function called
	// from C++ when the execution was interrupted.
	//
	DeltaNumberValueType num;
	DPTR(vm)->stack[activRecTopsp + DELTA_TOPSP_PC_OFFSET].ToNumber(&num);
	*callLine = DPTR(vm)->code[(util_ui16) num - 1].line;	// Since we store PC+1.

	if (!DPTR(vm)->DebugInfoIncluded())
		return UNKNOWN_FUNC;

	// When having debug info, but this is null it means we have an extern
	// function. We return a specific id for extern functions.
	//
	if (!currFuncDebugInfo && currLibFunc)
		return DECORATE_LIBFUNC_NAME(currLibFunc);

	DASSERT(currFuncDebugInfo);

	*defLine	= currFuncDebugInfo->GetLine();
	*scope		= currFuncDebugInfo->GetScope();

	return currFuncDebugInfo->GetName();
}

////////////////////////////////////////////////////////////////////
// Get an actual argument from current activation record, and put name
// information at 'putName'.
//
DeltaObject* DeltaDebugExtensions::GetActualArg (util_ui16 argNo, std::string& putName) {
	
	DASSERT(argNo < TotalActualArgs());
	DeltaObject* obj = GetActualArg(argNo);
	DASSERT(obj);

	if (!DPTR(vm)->DebugInfoIncluded())
		putName = currFuncDebugInfo->GetFormal(argNo);
	else
		putName = UNKNOWN_ARG;
	
	return obj;
}

////////////////////////////////////////////////////////////////////
// Get an actual argument by name. This requires debug information to resolve
// the name to a formal argument index, and use this to extract the object
// instance. Notice that not all actual arguments may be matched to formal 
// argument names, since, upon call, more actuals than formals may be supplied.
//
DeltaObject* DeltaDebugExtensions::GetActualArg (const char* name) {

	CHECK_EXECUTION_VALID((DeltaObject*) 0);

	if (InLocalFunctionWithDebugInformation()) {
		util_ui16 argNo;
		if (!currFuncDebugInfo->GetFormal(name, &argNo))
			return (DeltaObject*) 0;
		else
			return GetActualArg(argNo);
	}
	else
		return (DeltaObject*) 0;
}

////////////////////////////////////////////////////////////////////
// Get the offset of a local variable using the block id stack.
// The block id stack concerns all calls at the call-stack. We know
// we finish with the current call when the main-block (0) is met.
//
bool	DeltaDebugExtensions::GetLocalVarOffset (
				DeltaVars&		vars, 
				const char*		name, 
				util_ui16*		offset,
				util_ui16		blockId
			) {

	if (blockId != IGNORE_BLOCK_VALUE && 
		(blockIdStack.empty() || blockId > (*blockIdStack.begin())))
		return false;	// Not yet an active block.
	else {

		// We only search in the top current function, or else,
		// the whole list of blocks back to the main block. We do
		// not check across activation records.
		//
		bool	mainBlockMet = false;

		for (	std::list<util_ui16>::iterator i = blockIdStack.begin();
				!mainBlockMet;
				++i	) {

				DASSERT(i != blockIdStack.end());

				if (*i == blockId || blockId == IGNORE_BLOCK_VALUE) {

					if (vars.GetVar(name, *i, offset))
						return true;

					// Request for specific block, but not found means
					// failure.
					//
					if (blockId != IGNORE_BLOCK_VALUE)
						return false;
				}

				if (!*i)
					mainBlockMet = true;
		}
	
		DASSERT(mainBlockMet);
		return false;
	}
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::PopBlocksOfCurrentContext (void) {

	while (1) {

		DASSERT(!blockIdStack.empty());

		util_ui16 i = *(blockIdStack.begin());
		blockIdStack.pop_front();

		if (!i)	// Main block popped.
			return;
	}
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugExtensions::GetGlobalVarOffset (
				DeltaVars&		vars, 
				const char*		name, 
				util_ui16*		offset,
				util_ui16		blockId
			) {

	if (blockId != IGNORE_BLOCK_VALUE && 
		(blockIdStack.empty() || blockId > (*blockIdStack.begin())))
		return false;	// Not yet an active block.
	else {
		
		std::list<util_ui16> globalBlocks;
		std::list<util_ui16>::reverse_iterator j = blockIdStack.rbegin();

		// Get only the block list at global scope.
		// This is taken in reverse order, e.g. LOCAL|2|1|0|1|0|GLOBAL.
		// Since we visit |0|1|, we have to push front to gain |1|0|.
		//
		do 
			globalBlocks.push_front(*j);
		while (++j != blockIdStack.rend() && *j);
		
		for (	std::list<util_ui16>::iterator i = globalBlocks.begin(); 
				i != globalBlocks.end(); 
				++i	)
			if (*i == blockId || blockId == IGNORE_BLOCK_VALUE)
				if (vars.GetVar(name, *i, offset))
					return true;
				else
				if (blockId != IGNORE_BLOCK_VALUE)
					return false;
	
		return false;
	}
}

////////////////////////////////////////////////////////////////////
// Static variables are ensured to have unique names.
//
bool	DeltaDebugExtensions::GetStaticVarOffset (
				DeltaVars&		vars, 
				const char*		name, 
				util_ui16*		offset
			) {

	return vars.GetVar(name, 0, offset);	// Always at block 0.
}

////////////////////////////////////////////////////////////////////
// Get a local variable on a specified block. We firstly check if this block
// has been enter (i.e. active). Then we iterate active blocks trying to
// find an active block with this id (it may be that its activation passed).
//
DeltaObject* DeltaDebugExtensions::GetLocalVar (const char* name, util_ui16 blockId) {

	CHECK_EXECUTION_VALID((DeltaObject*) 0);

	if (!InLocalFunctionWithDebugInformation())
		return (DeltaObject*) 0;
	else {
		util_ui16 offset;
		if (GetLocalVarOffset(currFuncDebugInfo->Locals(), name, &offset, blockId))
			return DPTR(vm)->stack + (activRecTopsp - ((util_i16) offset));
		else
			return (DeltaObject*) 0;
	}
}

////////////////////////////////////////////////////////////////////
// In this case we try to find the first local variable having this name
// starting from the DPTR(vm)->top active (i.e. syntactically inner) block.
//
DeltaObject* DeltaDebugExtensions::GetLocalVar (const char* name) {

	CHECK_EXECUTION_VALID((DeltaObject*) 0);

	if (!InLocalFunctionWithDebugInformation())
		return (DeltaObject*) 0;
	else {
		util_ui16 offset;
		if (GetLocalVarOffset(currFuncDebugInfo->Locals(), name, &offset, IGNORE_BLOCK_VALUE))
			return DPTR(vm)->stack + (activRecTopsp - ((util_i16) offset));
		else
			return (DeltaObject*) 0;
	}
}

////////////////////////////////////////////////////////////////////
// The same as with local variables.
//
DeltaObject* DeltaDebugExtensions::GetGlobalVar (const char* name, util_ui16 blockId, bool isStatic) {

	CHECK_EXECUTION_VALID((DeltaObject*) 0);

	if (!DPTR(vm)->DebugInfoIncluded())
		return (DeltaObject*) 0;
	else {
		util_ui16 offset;
		if (isStatic)
			if (GetStaticVarOffset(DPTR(vm)->debugInfo.Globals(), name, &offset))
				return GetGlobalVar(offset);
			else
				return (DeltaObject*) 0;
		else
			if (GetGlobalVarOffset(DPTR(vm)->debugInfo.Globals(), name, &offset, blockId))
				return GetGlobalVar(offset);
			else
				return (DeltaObject*) 0;
	}
}

////////////////////////////////////////////////////////////////////
// The same as with local variables.
//
DeltaObject* DeltaDebugExtensions::GetGlobalVar (const char* name, bool isStatic) {
	return GetGlobalVar(name, IGNORE_BLOCK_VALUE, isStatic);
}

//------------------------------------------------------------------
// Execution control API.
//

// Prepares for execution. Resets execution parameters, clears call
// DPTR(vm)->stack and sets PC at the first instruction.
//
void DeltaDebugExtensions::Start (void) {
	DPTR(vm)->PrepareForExecution();
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::Step (void) {

	CHECK_EXECUTION_VALID(EMPTY);

	debugCmmdData.SetStep(DPTR(vm)->Line());
	deferredDbgCmmd = DeltaDebugExecCommand_Step;
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::StepIn (void) {

	CHECK_EXECUTION_VALID(EMPTY);

	debugCmmdData.SetStepIn(DPTR(vm)->Line());
	deferredDbgCmmd = DeltaDebugExecCommand_StepIn;
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::RunTo (util_ui16 line) {

	CHECK_EXECUTION_VALID(EMPTY);

	if (!IsValidBreakPoint(line)) {

		DeltaRunTimeMsg(
			"Warning: %s: Line %d: Invalid RunTo at '%d' (no code) ", 
			DPTR(vm)->Id(), 
			DPTR(vm)->Line(), 
			line
		);
		deferredDbgCmmd = DeltaDebugExecCommand_Continue;	// To unblock the Bpt serve loop.
	}
	else	{
		debugCmmdData.SetRunTo(line);
		deferredDbgCmmd = DeltaDebugExecCommand_RunTo;
	}
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::StepOut (void) {

	CHECK_EXECUTION_VALID(EMPTY);
	if (callStackTop != -1) {
		DASSERT(callStackTop >= 0);
		debugCmmdData.SetStepOut();
		deferredDbgCmmd = DeltaDebugExecCommand_StepOut;
	}
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::Continue (void) {

	CHECK_EXECUTION_VALID(EMPTY);
	deferredDbgCmmd = DeltaDebugExecCommand_Continue;
}

/////////////////////////////////
// The debug command data has enough information for
// the type of debug request and its criterion to judge if the
// stop-point for the execution has been reached. For instance,
// a step-out request will record the type of the request and
// the point in which it was poisted, while when served
// it will judged if the point of stepping-out has been reached.
//
void DeltaDebugExtensions::HandleDebugCommand (void) {

	util_ui32 pc = DPTR(vm)->PC();

	switch (debugCmmdData.cmmd) {

		case DeltaDebugExecCommand_Step	: { 

			if (DPTR(vm)->code[pc].opcode == DeltaVM_FUNCENTER)
				++debugCmmdData.data.step.callNesting;
			else
			if (DPTR(vm)->code[pc].opcode == DeltaVM_FUNCRET)
				if (debugCmmdData.data.step.callNesting)
					--debugCmmdData.data.step.callNesting;
			
			if (	DPTR(vm)->code[pc].line										&&  // A non hidden line.
					debugCmmdData.data.step.line != DPTR(vm)->code[pc].line		&&	// Reached a different line.
					!debugCmmdData.data.step.callNesting						&&  // Still in same function or previous one.
					GetFirstInstructionAddress(DPTR(vm)->code[pc].line)				// And this is a valid break point position.
				)	
					debugCmmdData.done = true;

			break;
		}

		//*******************************

		case DeltaDebugExecCommand_StepIn	: { 

			if (	DPTR(vm)->code[pc].line										&&  // A non hidden line
					debugCmmdData.data.stepIn.line != DPTR(vm)->code[pc].line	&&	// Reached a different line.
					GetFirstInstructionAddress(DPTR(vm)->code[pc].line)				// And this is a valid break point position.
				)	
					debugCmmdData.done = true;

			break;
		}	

		//*******************************

		case DeltaDebugExecCommand_RunTo	: { 

			if (	DPTR(vm)->code[pc].line										&&  // A non hidden line
					debugCmmdData.data.runTo.line == DPTR(vm)->code[pc].line	&&	// Reached the target line.
					GetFirstInstructionAddress(DPTR(vm)->code[pc].line)				// And this is a valid break point position.
				)	
					debugCmmdData.done = true;

			break;
		}	

		//*******************************

		case DeltaDebugExecCommand_StepOut : { 

			if (DPTR(vm)->code[pc].opcode == DeltaVM_FUNCENTER)
				++debugCmmdData.data.stepOut.callNesting;
			else
			if (DPTR(vm)->code[pc].opcode == DeltaVM_FUNCRET)
				if (debugCmmdData.data.stepOut.callNesting)
					--debugCmmdData.data.stepOut.callNesting;

			if (	DPTR(vm)->code[pc].line							&&		// A non hidden line
					!debugCmmdData.data.stepOut.callNesting			&&		// Still in same function or previous one.
					GetFirstInstructionAddress(DPTR(vm)->code[pc].line)		// And this is a valid break point position.
				)	
	
					debugCmmdData.done = true;
			
			break;
		}

		//*******************************

		default: DASSERT(false);
	}
}

////////////////////////////////////////////////////////////////////
// Returns true if at the current line we have to stop because
// it is a break point. This happens when the debug command is done,
// the current line is an enabled breakpoint, and the first instruction
// of the current line is actually the current one (at PC).
//
bool DeltaDebugExtensions::IsBreakPointTime (void) {
	return	debugCmmdData.IsCommandCompleted() ||
			GetBreakPointState(DPTR(vm)->Line()) &&
			GetFirstInstructionAddress(DPTR(vm)->Line()) == DPTR(vm)->PC();
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugExtensions::HasBreakPoint (util_ui16 line) {

	DPTR(breakPointMutex)->WaitToLock();

	bool result = DebugBreakPointHolder::DeltaFindBreakPoint(breakPoints, line) != breakPoints.end();
	
	DPTR(breakPointMutex)->UnLock();
	return result;
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugExtensions::SetBreakPoint (util_ui16 line, const std::string& cond) {

	DPTR(breakPointMutex)->WaitToLock();

	if (!IsValidBreakPoint(line)) {

		DeltaRunTimeMsg(
			"Warning: %s: Line %d: Invalid bpt at '%d' if(%s), no code", 
			DPTR(vm)->Id(), 
			DPTR(vm)->Line(),
			line,
			cond == "" ? "true" : cond.c_str()
		);

		return false;
	}

	// Conditions are evaluated exactly at the point of code
	// where the bpt is supposed to stop, not when set.
	//
	if (DebugBreakPointHolder::DeltaFindBreakPoint(breakPoints, line) == breakPoints.end()) {
		breakPoints.push_back(DeltaBreakPoint(line, cond.c_str()));
		DebugBreakPointHolder::Add(DPTR(vm)->Id(), line);
	}
	
	DPTR(breakPointMutex)->UnLock();
	return true;
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::ClearBreakPoint (util_ui16 line) {

	DPTR(breakPointMutex)->WaitToLock();

	std::list<DeltaBreakPoint>::iterator pos = DebugBreakPointHolder::DeltaFindBreakPoint(breakPoints, line);

	if (pos != breakPoints.end()) {
		breakPoints.erase(pos);
		DebugBreakPointHolder::Remove(DPTR(vm)->Id(), line);
	}
	else
		DASSERT(false);	// This should not happen.

	DPTR(breakPointMutex)->UnLock();
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugExtensions::GetBreakPointState (util_ui16 line) {

	DPTR(breakPointMutex)->WaitToLock();

	std::list<DeltaBreakPoint>::iterator pos = DebugBreakPointHolder::DeltaFindBreakPoint(breakPoints, line);
	bool result = pos != breakPoints.end() ? pos->IsEnabled() : false;

	// If we have a condition, we evaluate it immediately.
	// The bpt will be considered enabled if the condition is true too.
	// To apply evaluation we have to set temporarily the respective VM
	// for which bpt is checked as the break and debug context VM.
	//
	if (result && pos->HasCondition()) {
		
		DASSERT(!DebugContextVM());			// We should not have a debug context.
		DASSERT(!BreakPointVM());			// Neither a break point VM.

		SetDebugContextVM(vm);				// We set a debug context temporarily.
		SetBreakPointVM(vm);				// And the debug VM too.

		std::string vmId(vm->Id());			// We gain it, since we will set VM as null later.

		if (InsideFunctionCall())			// If in a funciton, we initiate call stack management.
			StartCallStackManagement();

		std::string error;					// Potential evaluation error message is put here.
		DeltaObject* val =	DebugExprEvaluator::EvaluateEx(
								vm, 
								pos->GetCondition(),
								error
							);
		
		ResetDebugContextVM();		// Reset after evaluation debug context.
		ResetBreakPointVM();		// And break VM too.

		if (!val) {					// Condition evaluation error!

			DASSERT(DebugExprEvaluator::HasError());

			DeltaDebugServer::DoInfoInvalidBreakPoint(
				Debug_InfoInvalidBreakPoint,
				vmId.c_str(), 
				pos->GetLine(),
				pos->GetLine(),
				uconstructstr("%s:%s", pos->GetCondition(), error.c_str()).c_str()
			);

			result = false;
			pos->Disable();	// Instantly disable, since it had erroneous condition.
		}
		else {
			DASSERT(!DebugExprEvaluator::HasError());
			result = val->TrueTest();
			DDELETE(val);
		}
	}

	if (result)
		new(&currBpt) DeltaBreakPoint(*pos);

	DPTR(breakPointMutex)->UnLock();
	return result;
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugExtensions::SetBreakPointState (util_ui16 line, bool state) {

	DPTR(breakPointMutex)->WaitToLock();;

	std::list<DeltaBreakPoint>::iterator pos = DebugBreakPointHolder::DeltaFindBreakPoint(breakPoints, line);

	bool result = true;
	if (pos!=breakPoints.end())
		if (state)
			pos->Enable();
		else
			pos->Disable();
	else {
		DeltaRunTimeMsg(
			"Warning: %s: Line %d: Breakpoint %d not found", 
			DPTR(vm)->Id(), 
			DPTR(vm)->Line(),
			line
		);
		result = false;
	}
	
	DPTR(breakPointMutex)->UnLock();
	return result;
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugExtensions::EnableBreakPoint (util_ui16 line) {

	DebugBreakPointHolder::Enable(DPTR(vm)->Id(), line);
	return SetBreakPointState(line, true);
}

////////////////////////////////////////////////////////////////////

bool DeltaDebugExtensions::DisableBreakPoint (util_ui16 line) {
	DebugBreakPointHolder::Disable(DPTR(vm)->Id(), line);
	return SetBreakPointState(line, false);
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::ClearAllBreakPoints (void) {

	DPTR(breakPointMutex)->WaitToLock();;

	breakPoints.clear();
	DebugBreakPointHolder::RemoveAll(DPTR(vm)->Id());

	DPTR(breakPointMutex)->UnLock();
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::EnableAllBreakPoints (void) {

	DPTR(breakPointMutex)->WaitToLock();;

	std::for_each(
		breakPoints.begin(), 
		breakPoints.end(),
		DeltaBreakPoint::ActFunctor(&DeltaBreakPoint::Enable)
	);

	DebugBreakPointHolder::EnableAll(DPTR(vm)->Id());

	DPTR(breakPointMutex)->UnLock();
}

////////////////////////////////////////////////////////////////////

void DeltaDebugExtensions::DisableAllBreakPoints (void) {

	DPTR(breakPointMutex)->WaitToLock();;

	std::for_each(
		breakPoints.begin(), 
		breakPoints.end(),
		DeltaBreakPoint::ActFunctor(&DeltaBreakPoint::Disable)
	);

	DebugBreakPointHolder::DisableAll(DPTR(vm)->Id());

	DPTR(breakPointMutex)->UnLock();
}

////////////////////////////////////////////////////////////////////

// PRIVATE.
// Call the callback for breakpoints.
//
void DeltaDebugExtensions::BreakPointServe (void) {

	SetBreakPointVM(vm);
	SetDebugContextVM(vm);

	if (onBreakEnter)
		(*onBreakEnter)();

	if (onBreak)
		(*onBreak)();

	// Enter loop until a debug command is explicitly posted
	// to the VM instance.
	//
	while (deferredDbgCmmd == DeltaDebugExecCommand_None)
		;	// Busy loop.

	if (onBreakLeave)
		(*onBreakLeave)();

	ResetBreakPointVM();
	ResetDebugContextVM();
}

////////////////////////////////////////////////////////////////////

// If line already registered as a line with legal code, then
// exit, else add it.
//
void DeltaDebugExtensions::AddLineWithCode (util_ui16 line, DeltaCodeAddress pc) {

	std::list<DeltaLineWithCode>::iterator pos = GetLineWithCode(line);

	// If already inside, and pc is smaller, change it.
	// Else simply ignore it and keep inside only the old entry.
	//
	if (pos != linesWithCode.end()) {
		if (pc < pos->pc)
			pos->pc = pc;
		return;
	}

	DPTR(breakPointMutex)->WaitToLock();;

	linesWithCode.push_back(DeltaLineWithCode(line, pc));

	DPTR(breakPointMutex)->UnLock();
}

////////////////////////////////////////////////////////////////////
// PRIVATE.
//
std::list<DeltaLineWithCode>::iterator DeltaDebugExtensions::GetLineWithCode (util_ui16 line) {

	DPTR(breakPointMutex)->WaitToLock();;

	std::list<DeltaLineWithCode>::iterator pos;

	pos =	std::find_if(
				linesWithCode.begin(),
				linesWithCode.end(),
				DeltaLineWithCode::MatchPred(line)
			);

	DPTR(breakPointMutex)->UnLock();

	return pos;
}

////////////////////////////////////////////////////////////////////
// PRIVATE.
//
DeltaCodeAddress DeltaDebugExtensions::GetFirstInstructionAddress (util_ui16 line) {

	std::list<DeltaLineWithCode>::iterator pos = GetLineWithCode(line);
	return pos != linesWithCode.end() ? pos->pc : 0;
}

////////////////////////////////////////////////////////////////////
// PRIVATE.
//
void DeltaDebugExtensions::ClearListOfLinesWithCode (void) {

	DPTR(breakPointMutex)->WaitToLock();;

	linesWithCode.clear();

	DPTR(breakPointMutex)->UnLock();
}

////////////////////////////////////////////////////////////////////
// PRIVATE.
// Check if a line of code is valid as a break-point. If not, then
// execution will never reach this point.
//
bool DeltaDebugExtensions::IsValidBreakPoint (util_ui16 line) {

	std::list<DeltaLineWithCode>::iterator pos = GetLineWithCode(line);
	return pos != linesWithCode.end();
}

////////////////////////////////////////////////////////////////////

util_ui16 DeltaDebugExtensions::GetNextValidBreakPoint (util_ui16 line) {

	if (!DPTR(vm)->DebugInfoIncluded())
		return 0;

	return DPTR(vm)->debugInfo.NextLine(line);
}

//------------------------------------------------------------------

#endif	// DEBUGGER_EXTENSIONS
