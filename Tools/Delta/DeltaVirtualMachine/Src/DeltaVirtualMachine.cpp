// DeltaVirtualMachine.cpp
// The Delta Virtual Machine implementation.
// Script Fighter Project.
// A. Savidis, October 1999 (original version).
// Latest refactoring, November 2009.
//

#include "DDebug.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DeltaVMRegistry.h"
#include "DeltaLibFuncBinder.h"
#include "Func.h"
#include "ContainerDelayedCollector.h"
#include "DeltaRuntimeError.h"
#include "DeltaExceptionHandling.h"
#include "DeltaClosureHandling.h"
#include "DeltaObject.h"
#include "uerrorclass.h"
#include "ugeometry.h"
#include "DeltaTableFactory.h"
#include "Execute.h"
#include "DeltaProfiler.h"
//	#define	DELTA_SUICIDAL_VM_ERROR
//	#define	DELTA_ZOMBIE_VM_ERROR
//	#define	DELTA_PROFILE_ACTIVATED

#ifdef	DELTA_PROFILE_ACTIVATED
#define	PROFILE_PROGRAM_FUNC(name)	DeltaProfiler::GetSingleton().EnterFunc(std::string(Source()) + "::" + name)
#define	PROFILE_LIBRARY_FUNC(addr)	DeltaProfiler::GetSingleton().EnterFunc(DeltaLibFuncBinder::GetNameByAddress(addr))
#define	PROFILE_EXIT_FUNC()			DeltaProfiler::GetSingleton().ExitFunc()
#define	PROFILE_MAIN_ENTER()		DELTA_RECORD_VM_SERIALNO(this); DeltaProfiler::GetSingleton().EnterFunc(std::string(Source()))
#define	PROFILE_MAIN_EXIT()			DELTA_VALIDATE_VM_SERIALNO(this, DELTA_RETURN_VOID); PROFILE_EXIT_FUNC()
#else
#define	PROFILE_PROGRAM_FUNC(addr)	
#define	PROFILE_LIBRARY_FUNC(addr)	
#define	PROFILE_EXIT_FUNC()	
#define	PROFILE_MAIN_ENTER()
#define	PROFILE_MAIN_EXIT()
#endif

//------------------------------------------------------------------

DeltaVirtualMachine*						
	DeltaVirtualMachine::callingVM				= (DeltaVirtualMachine*) 0;

ucallbackwithclosure<DeltaVirtualMachine::OnLoadCallback> 
	DeltaVirtualMachine::onLoadCallback;

DeltaVirtualMachine::StdUtilities*	
	DeltaVirtualMachine::stdUtilities			= (StdUtilities*) 0;

DeltaVirtualMachine::TableIteratorHandler*	
	DeltaVirtualMachine::tableIteratorHandler	= (TableIteratorHandler*) 0;

DeltaVirtualMachine::CallTraceStack*
	DeltaVirtualMachine::callTraceStack			= (CallTraceStack*) 0;

DeltaVirtualMachine*
	DeltaVirtualMachine::primaryFailing			= (DeltaVirtualMachine*) 0;

DeltaVirtualMachine::SnList*	
	DeltaVirtualMachine::vmNesting				= (SnList*) 0;

DeltaDebugExtensionsSuper::ConstructorFunc 
	DeltaDebugExtensionsSuper::ctor				= (ConstructorFunc) 0;

DeltaVirtualMachine::CallTrace*	
	DeltaVirtualMachine::topCallTrace			= (CallTrace*) 0;

util_ui32		DeltaVirtualMachine::callTraceStackTotal	=	0;
std::string*	DeltaVirtualMachine::stackTrace				= (std::string*) 0;
bool			DeltaVirtualMachine::somewhereWeHadAnError	= false;
bool			DeltaVirtualMachine::anyErrorInvalidatesAll = false;

DeltaDebugExtensionsSuper* DeltaDebugExtensionsSuper::Construct (DeltaVirtualMachine* vm) 
	{ DASSERT(ctor); return (*ctor)(vm); }

DeltaDebugExtensionsSuper::ConstructorFunc DeltaDebugExtensionsSuper::GetConstructor (void)
	{ return ctor; }

void DeltaDebugExtensionsSuper::SetConstructor (ConstructorFunc f) 
	{ DASSERT(f); ctor = f; }

DeltaDebugExtensionsSuper* DeltaVirtualMachine::NoDebugExtensionsConstructor (DeltaVirtualMachine* unused) 
	{ return (DeltaDebugExtensionsSuper*) 0; }

void DeltaVirtualMachine::SetDebugger (DeltaDebugExtensionsSuper* dbg) {
	if ((debugExtensions = dbg)) {
		currExecutor		= &DeltaVirtualMachine::ExecuteCurrInstruction;
		currExecutorType	= WithDebugger;
	}
	else {
		currExecutor		= &DeltaVirtualMachine::ExecuteCurrInstructionWithoutDebugger;
		currExecutorType	= WithoutDebugger;
	}
}

//------------------------------------------------------------------

DeltaVirtualMachine* DeltaVirtualMachine::GetPrimaryFailing (void) {
	DASSERT(!primaryFailing || VMRegistry().Exists(primaryFailing));
	return primaryFailing;
}

//////////////////////////////////

void DeltaVirtualMachine::ResetRunTimeErrors (void) {

	DASSERT(!GetPrimaryFailing());

	somewhereWeHadAnError = 0;
	const DeltaVMRegistry::VMList& vms = VMRegistry().GetAll();

	for (DeltaVMRegistry::VMList::const_iterator i = vms.begin(); i != vms.end(); ++i)
		if (DPTR(*i)->HasProducedError())
			DPTR(*i)->ResetError();
}

//////////////////////////////////

bool DeltaVirtualMachine::RunTimeErrorsExist (void)
	{ return somewhereWeHadAnError; }

void DeltaVirtualMachine::SetAnyErrorInvalidatesAll (bool val)
	{ anyErrorInvalidatesAll  = val; }

bool DeltaVirtualMachine::GetAnyErrorInvalidatesAll (void) 
	{ return anyErrorInvalidatesAll; }

//------------------------------------------------------------------

void DeltaVirtualMachine::Initialise (void) {
	somewhereWeHadAnError	= false;
	anyErrorInvalidatesAll	= false;
	unew(callTraceStack);
	unew(vmNesting);
	unew(stackTrace);
	callTraceStackTotal = 0;
	DPTR(callTraceStack)->resize(DELTA_CALLTRACE_STACK_SIZE);
	DeltaDebugExtensionsSuper::SetConstructor(&NoDebugExtensionsConstructor);
}

void DeltaVirtualMachine::CleanUp(void) 
	{ udelete(callTraceStack); udelete(stackTrace); udelete(vmNesting); }

//------------------------------------------------------------------
// New with stack size. also with an external id to recognise
// the source of error; can't check _id, since it may not be dynamic.
//
DeltaVirtualMachine::DeltaVirtualMachine (const char* _id, util_ui32 _stackSize) {

	id = (!_id || !*_id) ? DELTA_DEFAULT_VM_ID : _id;

	if (VMRegistry().Get(id.c_str())) {
		uerror::GetSingleton().postprimary("Can't create VM id '%s', already exists!", id.c_str());
		UERRORCLASS_POSTEXCEPTION;
	}

	pc						=	// Marks that VM is not run yet.
	currLine				=
	codeSize				=
	totalUserDefinedFuncs	= 
	totalActiveGlobals		=
	actualArgsPushed		= 0;

	unullify(code);
	unullify(userDefinedFuncs);
	unullify(currConstructedObject);
	unullify(currFunc);
	unullify(userFuncClosure);
	
	unullify(globalFunctionsTable);
	globalFunctionsTableSerialNo = 0;

	unullify(staticsTable);
	unullify(selfAsExternId);

	errorCause = NoneFailed;

	// Allocate stack memory, and set top at the beginning of
	// inverted stack.
	//
	if (_stackSize < DELTA_DEFAULT_STACK_MINSIZE)
		_stackSize = DELTA_DEFAULT_STACK_MINSIZE;
	
	stackSize	= _stackSize;
	stack		= DNEWARR(DeltaValue, stackSize);
	top			= DELTA_TOP_INITIAL_VALUE;
	topsp		= DELTA_TOPSP_BEFORE_RUN_VALUE;	// Guard value.

	for (util_ui16 i = 0; i<stackSize; ++i)	// Set residency of stack objects.
		stack[i].SetResidencyType(DeltaValue::Stacked);

	ResetTotalActualArgs();
	ResetUnwindingChecker();
	callingLibraryFunction	= 0;
	executionError			= 
	isRunning				= 
	currFuncReturnsValue	= 
	retain_pc				= false;
	retval					= &DELTA_RETVAL_VAR;	// Set the return value storage position.

	InstallOperandConverters();
	VMRegistry().Register(id.c_str(), this);
	SetDebugger(DeltaDebugExtensionsSuper::Construct(this));

	DASSERT(!selfAsExternId);
	selfAsExternId = DNULLCHECK(GetStdUtilities())->MakeExternId(this);
	DPTR(selfAsExternId)->IncRefCounter((DeltaValue*) 0);
}

//////////////////////////////////

void DeltaVirtualMachine::Clear (void) {

	UnreferenceStaticsTable();
	UnreferenceFunctionsTable();
	UndefineAllReferrers();

	GlobalGarbageCollection();
	unullify(selfAsExternId); 

	if (code) {
		DASSERT(codeSize);
		DDELARR(code);
		unullify(code);
	}

	if (userDefinedFuncs) {
		DDELARR(userDefinedFuncs);
		unullify(userDefinedFuncs);
	}

	funcByName.clear();
	funcByAddr.clear();

	if (stack) {
		DASSERT(stackSize);
		DDELARR(stack);
		unullify(stack);
	}

	objectConstructionStack.clear();
}

//////////////////////////////////

#ifdef	DELTA_SUICIDAL_VM_ERROR
#define	DELTA_ON_SUICIDAL_VM()	PossiblePrimaryError("destroying vm '%s' from its own instructions", Id())
#else	
#define	DELTA_ON_SUICIDAL_VM()
#endif

#ifdef	DELTA_ZOMBIE_VM_ERROR
#define	DELTA_ON_ZOMBIE_VM()	DPTR(topVM)->PossiblePrimaryError("the vm '%s' destroys its caller vm '%s'", topVM->Id(), Id())
#else	
#define	DELTA_ON_ZOMBIE_VM()
#endif

DeltaVirtualMachine::~DeltaVirtualMachine() {
	
#if	defined(DELTA_SUICIDIAL_VM_ERROR) || defined(DELTA_ZOMBIE_VM_ERROR)
	if (DeltaVirtualMachine* topVM = GetTopVMFromCallTraceStack()) {
		if (this == topVM)				// vm destroys itself.
			DELTA_ON_SUICIDAL_VM();
		else
		if (IsFoundInAnyCallTrace())	// top vm destroys 'this' being its caller.
			DELTA_ON_ZOMBIE_VM();
	}
#endif

	// Necessary pre-destruction actions to ensure every
	// data left due to execution must be cleared manually
	// since no normal 'return actions' will be performed.

	EXCEPTION_HANDLERS->RemoveAllCalls(this);
	CLOSURE_HANDLERS->RemoveAllCalls(this);

	ExpireAllExecutionLoopPassports();
	RemoveAllMyCallTracess();

	if (GetDebugger())
		GetDebuggerRef().OnVirtualMachineDestruction();

	// Clear actually.

	Clear();
	VMRegistry().Cancel(id.c_str());
	udeleteunlessnull(debugExtensions);
}

//-----------------------------------------------------------------
// A saved environment value must be always an unsigned value.
//
util_ui32 DeltaVirtualMachine::GetEnvironmentValue (util_i16 offset) const {
	DeltaNumberValueType	num = stack[topsp + offset].ToNumber();
	util_ui32				val = (util_ui32) num;
	DASSERT((DeltaNumberValueType) val == num);

	return val;
}

//********************************
// Returns all actualsd, including user supplied and those of the runtime system
// (i.e. arguments and, for methods only, self).
//
util_ui16 DeltaVirtualMachine::GetTotalActualsForCurrFunction (void) const {

	DASSERT(currFunc);	// Should be inside a function.
	return	GetEnvironmentValue(DELTA_TOPSP_TOTALARGS_OFFSET) +
			(currFunc->IsMethod() ? DELTA_METHOD_STD_ARGUMENTS : DELTA_PROGFUNC_STD_ARGUMENTS);
}

//********************************

DeltaTable* DeltaVirtualMachine::GetSelfForCurrMethod (void) {
	DASSERT(currFunc && currFunc->IsMethod());
	return stack[topsp + DELTA_TOPSP_SELF_OFFSET].ToTable();
}

//********************************

const DeltaStdFuncInfo*	DeltaVirtualMachine::GetFuncInfo (DeltaCodeAddress funcAddr) const {
	FuncByAddrMap::const_iterator i = funcByAddr.find(funcAddr);
	return i != funcByAddr.end() ? i->second : (DeltaStdFuncInfo*) 0;
}

const char*	DeltaVirtualMachine::GetFuncName (DeltaCodeAddress addr) const {
	const DeltaStdFuncInfo* funcInfo = GetFuncInfo(addr);
	return funcInfo ? funcInfo->GetName().c_str() : (const char*) 0;
}

//------------------------------------------------------------------
// Called from the VM only.
// If the library func is not found, this is a run-time
// error and the execution gets invalid. If after the call the
// VM is found to be destroyed, the library func call
// returns without executing normal return stmts (i.e. stack
// clean-up). The caller should take the extra overhead and
// re-check the VM.
//
void DeltaVirtualMachine::CallLibraryFunc (const char* name, util_ui16 totalActuals) {

	DASSERT(!HasProducedError());

	if (DeltaLibraryFunc func = DeltaLibFuncBinder::Get(name))
		CallPreboundLibraryFunc(func, totalActuals);
	else
		SetErrorCode(DELTA_UNRESOLVED_LIBFUNC_ERROR)->PrimaryError("Library function '%s' was not found", name);
}

//////////////////////////////////

void DeltaVirtualMachine::SetCreationInfoIfLibraryFuncCreatedAnExternId (DeltaLibraryFunc func) {
	const char* name = DeltaLibFuncBinder::GetNameByAddress(func);
	if (retval->Type() == DeltaValue_ExternId)
		retval->GetExternId()->GetCreationInfo().SetCreatorInformation(
			Source(),
			Line(),
			uconstructstr(
				"%s '%s'", 
				DeltaValue::GetTypeStr(DeltaValue_LibraryFunc),
				name ? name : "from a dll that was removed"
			)
		);
}

//////////////////////////////////

void DeltaVirtualMachine::CallPreboundLibraryFunc (DeltaLibraryFunc func, util_ui16 totalActuals) {

	DASSERT(func && !HasProducedError());
	
	if (!DeltaLibFuncBinder::IsValidAddress(func)) 
		{ PrimaryError("In calling library function: not found (was removed)!"); return; }

	if (!PushUserArgumentsOnly(DELTA_SAVED_ENVIRONMENT_SIZE))
		return;

	CallFuncPrepare(this, totalActuals, 0);

	++callingLibraryFunction;
	DoExternFuncEnterCode(func);			// Callee obligations.

	DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_ERROR_RETURN_RETURN_VOID(
		try			
			{ (*func)(this); } 
		catch(...)	{ 
			SetErrorCode(DELTA_LIBFUNC_NATIVE_EXCEPTION_ERROR)->PrimaryError(
				"Call to library function '%s' caused a native exception!",
				DeltaLibFuncBinder::GetNameByAddress(func)
			); return; 
		}, 
		this
	);

	DASSERT(callingLibraryFunction && !GetUserFuncClosure()); 
	--callingLibraryFunction;

	if (!EXCEPTION_HANDLERS->IsUnwinding())
		DoExternFuncReturnCode(func);		// Explicit return sequence (pop activation record).
	
	SetCreationInfoIfLibraryFuncCreatedAnExternId(func);	
}

//////////////////////////////////

void DeltaVirtualMachine::PrebindLibraryFunc (DeltaOperand& arg) {

	if (arg.GetType() == DeltaOperand_LibFunc) {
		const char*			id	= constArrays.GetExternFuncUsed(arg.GetValue());
		DeltaLibraryFunc	f	= DeltaLibFuncBinder::Get(id);
		if (!f) {	// Turn to a string constant.
			arg.SetType(DeltaOperand_ConstString);
			arg.SetValue((util_ui32) id);
		}
		else
			arg.SetValue((util_ui32) f); 
	}
}

//**********************

void DeltaVirtualMachine::PrebindLibraryFuncs (DeltaInstruction* instr) {
	PrebindLibraryFunc(instr->arg1);
	PrebindLibraryFunc(instr->arg2);
	PrebindLibraryFunc(instr->result);
}

//////////////////////////////////

void DeltaVirtualMachine::PrebindStringConst (DeltaOperand& arg) {
	if (arg.GetType() == DeltaOperand_ConstString)
		arg.SetValue((util_ui32) DNULLCHECK(constArrays.GetStringConst(arg.GetValue())));
}

void DeltaVirtualMachine::PrebindStringConsts (DeltaInstruction* instr) {
	PrebindStringConst(instr->arg1);
	PrebindStringConst(instr->arg2);
	PrebindStringConst(instr->result);
}

//**********************

void DeltaVirtualMachine::PrebindNumericConst (DeltaOperand& arg) {
	if (arg.GetType() == DeltaOperand_ConstNumber)
		arg.SetValue((util_ui32) DNULLCHECK(constArrays.GetNumericConstPtr(arg.GetValue())));
}

void DeltaVirtualMachine::PrebindNumericConsts (DeltaInstruction* instr) {
	PrebindNumericConst(instr->arg1);
	PrebindNumericConst(instr->arg2);
	PrebindNumericConst(instr->result);
}

/////////////////////////////////////////////////////////////

void DeltaVirtualMachine::PrebindFunctionOrMethod(DeltaOperand& arg) {
	if (arg.GetType() == DeltaOperand_ProgramFunc || arg.GetType() == DeltaOperand_MethodFunc)
		arg.SetValue((util_ui32) DNULLCHECK(GetFuncInfo((DeltaCodeAddress) arg.GetValue())));
}

//**********************

void DeltaVirtualMachine::PrebindFunctionsOrMethods (DeltaInstruction* instr) {
	PrebindFunctionOrMethod(instr->arg1);
	PrebindFunctionOrMethod(instr->arg2);
	PrebindFunctionOrMethod(instr->result);
}

/////////////////////////////////////////////////////////////

#define	DELTA_FUNC_ENTER_ACTIONS(_precheck, _preact, _exceptions_trace, _call_trace, _debugger_callback)	\
	topsp = top;																							\
	_precheck;																								\
	_preact;																								\
	SetActualsAndLocalsPointers();																			\
	ResetTotalActualArgs();																					\
	GetReturnValue().Undefine();																			\
	GetBlocks().PushBlockIdStack();																			\
	EXCEPTION_HANDLERS->PushFunc(this, _exceptions_trace);													\
	CLOSURE_HANDLERS->PushFunc(this, GetUserFuncClosure());													\
	PushCallTrace(_call_trace);																				\
	if (GetDebugger())																						\
		GetDebuggerRef()._debugger_callback;

#define	ENTER_LOCAL_FUNC_PRECHECK				DASSERT(code[pc].opcode == DeltaVM_FUNCENTER)
#define	ENTER_LOCAL_FUNC_PREACTIONS																			\
	if (true) {																								\
		DASSERT(!IsCurrFuncReturningValue());																\
		DASSERT(funcInfo == DNULLCHECK(GetFuncInfo(PC())));													\
		PushFunc(funcInfo);																					\
		DASSERT(funcInfo == currFunc && currFunc->GetAddress() == PC());									\
	} else

#define	ENTER_LOCAL_FUNC_EXCEPTIONS_TRACE		DeltaExceptionHandling::InDeltaFunction, pc
#define	ENTER_LOCAL_FUNC_CALL_TRACE				DNULLCHECK(currFunc), pc, top, topsp, GetBlocks().GetTopBlockIdStackIndex()
#define	ENTER_LOCAL_FUNC_DEBUGGER_CALLBACK		OnCallingProgramFunction(currFunc->GetName());

#define	ENTER_EXTERN_FUNC_PRECHECK				
#define	ENTER_EXTERN_FUNC_PREACTIONS			PushFunc(); ResetUserFuncClosure()
#define	ENTER_EXTERN_FUNC_EXCEPTIONS_TRACE		DeltaExceptionHandling::InLibraryFunction, func
#define	ENTER_EXTERN_FUNC_CALL_TRACE			UFUNC2VOID(func), pc, top, DELTA_TOPSP_LIBFUNC, GetBlocks().GetTopBlockIdStackIndex()
#define	ENTER_EXTERN_FUNC_DEBUGGER_CALLBACK		OnCallingLibraryFunction(func);

//*******************************

void DeltaVirtualMachine::DoLocalFuncEnterCode (const DeltaStdFuncInfo* funcInfo) {
	
	PROFILE_PROGRAM_FUNC(funcInfo->GetName());
	
	DELTA_FUNC_ENTER_ACTIONS(
		ENTER_LOCAL_FUNC_PRECHECK,
		ENTER_LOCAL_FUNC_PREACTIONS,
		ENTER_LOCAL_FUNC_EXCEPTIONS_TRACE,	
		ENTER_LOCAL_FUNC_CALL_TRACE,		
		ENTER_LOCAL_FUNC_DEBUGGER_CALLBACK
	);		
}

void DeltaVirtualMachine::DoExternFuncEnterCode (DeltaLibraryFunc func) {

	PROFILE_LIBRARY_FUNC(func);

	DELTA_FUNC_ENTER_ACTIONS(
		ENTER_EXTERN_FUNC_PRECHECK,
		ENTER_EXTERN_FUNC_PREACTIONS,
		ENTER_EXTERN_FUNC_EXCEPTIONS_TRACE,		
		ENTER_EXTERN_FUNC_CALL_TRACE,		
		ENTER_EXTERN_FUNC_DEBUGGER_CALLBACK	
	);		
}

//////////////////////////////////
//	1:	BX = TOPSP + 1.
//	2:	TOP = S[TOPSP + DELTA_SAVED_TOP_OFFSET].
//	3:	PC = S[TOPSP + DELTA_SAVED_PC_OFFSET].
//	4:	TOPSP = S[TOPSP + DELTA_SAVED_TOPSP_OFFSET].

#define	DELTA_FUNC_RETURN_ACTIONS(_pop_actions, _debugger_callback)			\
	DASSERT(!HasProducedError() && top == topsp);							\
	DeltaValue* s_topsp = stack + topsp;									\
	bx = topsp + 1;															\
	util_ui32 next_top;														\
	ucastassign(next_top, s_topsp[DELTA_SAVED_TOP_OFFSET].ToNumber());		\
	CLOSURE_HANDLERS->PopFunc(												\
		this,																\
		s_topsp + 1,														\
		next_top - topsp 													\
	);																		\
	top = next_top;															\
	ucastassign(pc, s_topsp[DELTA_SAVED_PC_OFFSET].ToNumber());				\
	DASSERT(pc <= DELTA_PC_PROGRAM_END);									\
	ucastassign(topsp, s_topsp[DELTA_SAVED_TOPSP_OFFSET].ToNumber());		\
	SetActualsAndLocalsPointers();											\
	CollectGarbage();														\
	_pop_actions;															\
	GetBlocks().PopBlockIdStack();											\
	EXCEPTION_HANDLERS->PopFunc();											\
	ResetUserFuncClosure();													\
	PopCallTrace();															\
	if (GetDebugger())														\
		GetDebuggerRef()._debugger_callback;								\
	PROFILE_EXIT_FUNC();

#define	RETURN_EXTERN_FUNC_POP_ACTIONS										\
	DASSERT(!currFunc);														\
	PopFunc();

#define	RETURN_EXTERN_FUNC_DEBUGGER_CALLBACK		OnReturnLibraryFunction(func)

#define	RETURN_LOCAL_FUNC_POP_ACTIONS										\
	DASSERT(currFunc);														\
	std::string funcName(DNULLCHECK(currFunc)->GetName());					\
	PopFunc();																\
	if (!IsCurrFuncReturningValue())										\
		retval->Undefine();													\
	else																	\
		ResetCurrFuncReturnsValue();


#define	RETURN_LOCAL_FUNC_DEBUGGER_CALLBACK			OnReturnProgramFunction(funcName);							

//*******************************

void DeltaVirtualMachine::PushFunc (const DeltaStdFuncInfo* f) {
	funcStack.push(f);
	currFunc = f;
	DASSERT(!f || uptrinside(f, userDefinedFuncs, userDefinedFuncs + totalUserDefinedFuncs - 1));
}

void DeltaVirtualMachine::PopFunc (void) {
	DASSERT(!funcStack.empty());
	funcStack.pop();
	currFunc = funcStack.empty() ? (DeltaStdFuncInfo*) 0 : funcStack.top();
}

//*******************************

void DeltaVirtualMachine::DoExternFuncReturnCode (DeltaLibraryFunc func) {
	DELTA_FUNC_RETURN_ACTIONS(
		RETURN_EXTERN_FUNC_POP_ACTIONS, 
		RETURN_EXTERN_FUNC_DEBUGGER_CALLBACK
	);
}

void DeltaVirtualMachine::DoLocalFuncReturnCode (void) {
	DELTA_FUNC_RETURN_ACTIONS(
		RETURN_LOCAL_FUNC_POP_ACTIONS, 
		RETURN_LOCAL_FUNC_DEBUGGER_CALLBACK
	); 
}

/////////////////////////////////////////////////////////////

void DeltaVirtualMachine::SetActualsAndLocalsPointers (void) {
	actuals = stack + topsp + DELTA_SAVED_ENVIRONMENT_SIZE + 1;
	locals	= stack + topsp;
}

/////////////////////////////////////////////////////////////
// Initialy called, after loading.

void DeltaVirtualMachine::SetGlobalsAndStackPointers (void) {
	top		= DELTA_TOP_INITIAL_VALUE;					// Beginning of inverted stack.
	topsp	= DELTA_TOPSP_WHEN_AT_GLOBAL_CODE(this);	// Initial value.
	DASSERT(top == topsp);								// Equal at this point.
	globals	= stack + top;								// Globals start there.
}

//------------------------------------------------------------------
// LOCAL FUNCTION TABLE.

bool DeltaVirtualMachine::GlobalFuncExists (const char* name) 
	{ return funcByName.find(name) != funcByName.end(); }

bool DeltaVirtualMachine::GlobalFuncExists (DeltaCodeAddress funcAddr) 
	{ return funcByAddr.find(funcAddr) != funcByAddr.end(); }

//////////////////////////////////

#define	DELTA_EXTERNAL_CALL_GLOBAL_FUNC(addr, numArgs)		\
	if (!HasProducedError()) {								\
		EnterExecutionLoop();								\
		if (ExtCallGlobalFuncCommit(addr, numArgs))			\
			ExitExecutionLoop();							\
	} else
	
//////////////////////////////////
// In such a call we continue execution for this function. 
// The call ensures a return after the function is called.
// To certify that, we store the PC with a value being program end.
// Then, we restore the original PC value upon return.
// It returns false only when the vm has died.
//
bool DeltaVirtualMachine::ExtCallGlobalFuncCommit (DeltaCodeAddress funcAddr, util_ui16 stdCallArguments) {

	util_ui32 saved_pc	= pc;
	pc = DELTA_PC_PROGRAM_END;

	// This only sets PC to the target function,
	// preparing the call. The Continue() that follows is necessary.

	ExecuteCallProgramFunc(this, funcAddr, actualArgsPushed, stdCallArguments);
	ResetTotalActualArgs();

	DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_ERROR_RETURN(
		Continue(), 
		this, 
		false, // if vm died
		true
	);

	pc = saved_pc;
	return true;
}

//////////////////////////////////
// This is an externally originated member call.

void DeltaVirtualMachine::ExtCallMethodFunc (DeltaCodeAddress funcAddr, DeltaTable* self) {

	DASSERT(!HasProducedError() && self->IsOfClass(DELTA_OBJECT_CLASS_ID));

	std::map<DeltaCodeAddress, DeltaStdFuncInfo*>::iterator func = funcByAddr.find(funcAddr);
	DASSERT(func != funcByAddr.end());

	if (PushUserArgumentsAndArgumentsVector(1 + DELTA_SAVED_ENVIRONMENT_SIZE)) {
		PushSelfArgument(self);
		DELTA_EXTERNAL_CALL_GLOBAL_FUNC(funcAddr, DELTA_METHOD_STD_ARGUMENTS);
	}
}

//////////////////////////////////
// Only global script functions may be called through the
// embedding API. This version calls a func on the basis
// of its address. No need to externally validate an address.
// This call is more fast than the one supplying a function name.

void DeltaVirtualMachine::ExtCallGlobalFunc (DeltaCodeAddress funcAddr) {

	DASSERT(!HasProducedError());
	std::map<DeltaCodeAddress, DeltaStdFuncInfo*>::iterator func = funcByAddr.find(funcAddr);
	DASSERT(func != funcByAddr.end());

	PushUserArgumentsAndArgumentsVector();
	DELTA_EXTERNAL_CALL_GLOBAL_FUNC(funcAddr, DELTA_PROGFUNC_STD_ARGUMENTS);
}

//////////////////////////////////

DeltaCodeAddress DeltaVirtualMachine::GlobalFuncAddress (const char* name) {
	FuncByNameMap::iterator i = funcByName.find(name);
	if (i != funcByName.end() && i->second->IsNamedFunction() && i->second->IsExported())
		return i->second->GetAddress();
	else
		return DELTA_CANTBE_A_FUNCTION_ADDRESS;
}

//////////////////////////////////

void DeltaVirtualMachine::GetGlobalFunctions (std::list<std::pair<std::string, DeltaValue> >* funcs) {

	for (FuncByNameMap::iterator i = funcByName.begin(); i != funcByName.end(); ++i) {

		DASSERT(i->first == i->second->GetName());
		DeltaStdFuncInfo* func = i->second;

		if (func->IsNamedFunction() && func->IsExported()) 
			funcs->push_back(
				std::pair<std::string, DeltaValue>(
					func->GetName(), 
					DeltaValue(func->GetAddress(), this)
				)
			);
	}
} 

//////////////////////////////////

void DeltaVirtualMachine::GetGlobalFunctionsTable (DeltaValue* funcs) {

	// We create or recreate the table if not yet created or collected, respectively.

	if (!globalFunctionsTable || !ValidatableHandler::Validate(globalFunctionsTable, globalFunctionsTableSerialNo)) {
		globalFunctionsTable = DeltaObject::NativeCodeHelpers::NewObject();
		globalFunctionsTableSerialNo = DPTR(globalFunctionsTable)->GetSerialNo();
	}

	funcs->FromTable(globalFunctionsTable);

	for (FuncByNameMap::iterator i = funcByName.begin(); i != funcByName.end(); ++i) {

		DASSERT(i->first == i->second->GetName());
		DeltaStdFuncInfo* func = i->second;

		if (func->IsNamedFunction() && func->IsExported())
			DPTR(globalFunctionsTable)->Set(
				DeltaValue(func->GetName()), 
				DeltaValue(func->GetAddress(), this)
			);
	}
}

//////////////////////////////////

void DeltaVirtualMachine::UndefineAllReferrers (void) {

	if (selfAsExternId) {

		DELTA_COLLECTOR_ENTER();

		// During collection of referrers, it is possible that other referrers in the bag
		// are collected, hence, they will become undefined. This is why we need to explicitly
		// test with the original referrers bag.

		while (!DPTR(selfAsExternId)->GetReferrers().empty())
			DPTR(selfAsExternId)->GetReferrers().begin()->first->Undefine();

		DPTR(selfAsExternId)->DecRefCounter((DeltaValue*) 0);

		DELTA_COLLECTOR_EXIT();
	}
}

//////////////////////////////////

void DeltaVirtualMachine::ProduceStaticsList (void) {

	DASSERT(staticsList.empty());

	if (HasDebugInfo()) {

		const std::list<DeltaDebugLocal>& statics =
			debugInfo.
			GetGlobals().
			GetBlock(DELTA_STATICS_OWNING_BLOCKID).
			GetLocals();
		
		util_ui16 shift = (util_ui16) strlen(DELTA_STATICVAR_NAME_PREFIX);

		for (	std::list<DeltaDebugLocal>::const_iterator i = statics.begin();
				i != statics.end();
				++i	)

				if (ustrprefix(DELTA_STATICVAR_NAME_PREFIX, i->GetName()))
					staticsList.push_back(
						std::pair<std::string, util_ui16>(
							i->GetName().substr(shift),
							i->GetOffset()
						)
					);
	}
} 

//**********************

DeltaTable*	DeltaVirtualMachine::GetStaticsTable (void) {

	if (HasDebugInfo()) {

		if (!staticsTable)
			staticsTable = DeltaObject::NativeCodeHelpers::NewObject();

		for (StaticsList::iterator i = staticsList.begin(); i != staticsList.end(); ++i)
			staticsTable->Set(
				DeltaValue(i->first),
				*GetGlobal(i->second)
			);

		return staticsTable;
	}
	else
		return (DeltaTable*) 0;
}

//**********************
	
void DeltaVirtualMachine::ClearStaticsTable (void) {
	if (staticsTable) {
		staticsList.clear();
		DPTR(staticsTable)->Clear();
	}
}

void DeltaVirtualMachine::UnreferenceStaticsTable (void) {
	staticsList.clear();
	if (staticsTable) {
		DASSERT(HasDebugInfo());
		DeltaObject::NativeCodeHelpers::GiveUp(staticsTable);
		unullify(staticsTable); 
	}
}

//////////////////////////////////

void DeltaVirtualMachine::UnreferenceFunctionsTable (void) {
	if (globalFunctionsTable) {
		DeltaObject::NativeCodeHelpers::GiveUp(globalFunctionsTable);
		unullify(globalFunctionsTable);
	}
}

//////////////////////////////////
// The embedding API version which calls through a name.
//
void DeltaVirtualMachine::ExtCallGlobalFunc (const char* name) {

	DASSERT(!HasProducedError());

	FuncByNameMap::iterator i = funcByName.find(name);
	DASSERT(i != funcByName.end());

	PushUserArgumentsAndArgumentsVector();
	ExtCallGlobalFuncCommit(i->second->GetAddress(), DELTA_PROGFUNC_STD_ARGUMENTS);
}

//////////////////////////////////
// The instruction sequence for call function preparation when
// user defined / library functions are called by the VM at run-time 
// is the following:
//
//	1:	PUSHARG #Arguments
//	2:	PUSHARG PC + DELTA_FUNCCALL_PC_OFFSET
//	3:	PUSHARG TOP + #Arguments + DELTA_PREVIOUS_TOP_EXTRA_OFFSET
//	4:	PUSHARG TOPSP
//

#define	MINUSMINUS_TOP()		\
	if (true) {					\
		DASSERT(top); 			\
		--top;					\
		--s_top;				\
	} else

void DeltaVirtualMachine::CallFuncPrepare (DeltaVirtualMachine* caller, util_ui32 totalActuals, util_ui32 stdCallArguments) {

	DASSERT(!HasProducedError() && pc <= DELTA_PC_PROGRAM_END);
	DASSERT(top >= DELTA_SAVED_ENVIRONMENT_SIZE);	// we should have detected overflow earlier

	DeltaValue* s_top = stack + top;	// For fast access.

	//	1:	PUSHARG #Arguments
	//
	s_top->FromNumber((DeltaNumberValueType) totalActuals);
	MINUSMINUS_TOP();

	//	2:	PUSHARG PC + DELTA_FUNCCALL_PC_OFFSET
	//		If the caller is a different vm then we simply store the same PC.
	//		Else it may be the case that the call is coming from
	//		C++ code in the form of a forced local function call. In this
	//		case, if it happens to be at the end of program execution,
	//		we do not store pc + 1, but the program end pc.
	//
	s_top->FromNumber(
		(DeltaNumberValueType) (pc + (this != caller || DELTA_PC_PROGRAM_END == pc ? 0 : DELTA_FUNCCALL_PC_OFFSET))
	);
	MINUSMINUS_TOP();

	//	3:	PUSHARG TOP + #Arguments + DELTA_PREVIOUS_TOP_EXTRA_OFFSET
	//
	s_top->FromNumber(
		(DeltaNumberValueType) top + totalActuals + stdCallArguments + DELTA_PREVIOUS_TOP_EXTRA_OFFSET
	);
	MINUSMINUS_TOP();

	//	4:	PUSHARG TOPSP
	//
	s_top->FromNumber((DeltaNumberValueType) topsp);
	DASSERT(top);
	--top;	// Only top, no s_top needed.
}

//////////////////////////////////
//	Calling a user defined function requires:

//	1:	Prepare the call.
//	2:	PC = Address(F).
//
void DeltaVirtualMachine::ExecuteCallProgramFunc (
		DeltaVirtualMachine*	caller,	
		DeltaCodeAddress		addr, 
		util_ui16				totalActuals, 
		util_ui16				stdCallArguments
	) {

	//	1:	Prepare call.
	CallFuncPrepare(caller, totalActuals, stdCallArguments);

	//	2:	PC = Address(F).
	pc = addr;
	DASSERT(code[pc].GetOpCode() == DeltaVM_FUNCENTER); // Next instruction is function enter.
}

//------------------------------------------------------------------
// ARGUMENTS AND RETURN VALUE MANAGEMENT.

void DeltaVirtualMachine::PushActualArg (const DeltaValue& obj) 
	{ PushActualArg(&obj); }

//////////////////////////////////

void DeltaVirtualMachine::PushActualArg (const DeltaValue* obj) {

	DASSERT(!HasProducedError());
	DeltaValue* arg = NewActualArgument();
	arg->Assign(obj);
}

//////////////////////////////////
// Returns number of actual args passed to a library function call. 
// The lib func may be called indirectly as it can be stored in a prorgam 
// var (callfunc possible)or object slot (getmethod versions possible).
//
util_ui16 DeltaVirtualMachine::TotalActualArgs (void) {

	DASSERT(!HasProducedError());
	DASSERT(callingLibraryFunction && (code[pc].IsCall() || PCAtNormalProgramEnd()));

	return (util_ui16) GetEnvironmentValue(DELTA_TOPSP_TOTALARGS_OFFSET);
}

//////////////////////////////////

DeltaValue* DeltaVirtualMachine::GetActualArg (util_ui16 argNo) {

	DASSERT(!HasProducedError());	
	DASSERT(callingLibraryFunction && (code[pc].IsCall() || PCAtNormalProgramEnd()));
	DASSERT(argNo < ((util_ui16) GetEnvironmentValue(DELTA_TOPSP_TOTALARGS_OFFSET)));

	return actuals + argNo;
}

//------------------------------------------------------------------
// GARBAGE COLLECTION, VALIDATION.

// Garbage collection is done from S[Number(BX)]->S[TOP].
// This is the normal clean-up sequence upon function return.
//
void DeltaVirtualMachine::CollectGarbage (void) {
	
	DASSERT(bx < top || (bx == top && HasProducedError() && top == DELTA_TOP_FORCE_COMPLETE_EXECUTION));
	DeltaValue* stop = stack + bx;

	DELTA_COLLECTOR_ENTER();

	for (util_ui32 i = top - bx + 1; i; --i, ++stop)
		stop->CollectGarbage();

	DELTA_COLLECTOR_EXIT();
}

//////////////////////////////////
// Here, the garbage collection is done globally clearing 
// from TOP to the beginning of the stack. This is necessary 
// when new code is loaded. Also, this process is put
// explicitly in-between the delayed collection method.
//
void DeltaVirtualMachine::GlobalGarbageCollection (void) {

	DELTA_COLLECTOR_ENTER();

	DeltaValue* stop = stack + top + 1;

	for (util_ui32 i = stackSize - top - 1; i; --i, ++stop)
		stop->CollectGarbage();

	DELTA_COLLECTOR_EXIT();
}

//////////////////////////////////
// An object is valid if it is an alive stack
// object inside interval [TOP+1, SIZE - 1].
//
DeltaValue* DeltaVirtualMachine::ValidateStackValuePtr (DeltaValue* obj) {
	return	uptrinside(
				obj, 
				stack + top + 1, 
				stack + stackSize  - 1
			) ? obj : ValidateClosureValuePtr(obj);
}

//////////////////////////////////

DeltaValue* DeltaVirtualMachine::ValidateClosureValuePtr (DeltaValue* val) {
	if (const DeltaClosure* closure = CLOSURE_HANDLERS->GetTopClosure())
		return DPTR(closure)->IsInside(val) ? val : (DeltaValue*) 0;
	else
		return (DeltaValue*) 0;
}

//////////////////////////////////
// An invalid address is an error of the host library using
// the VM.
//
DeltaCodeAddress DeltaVirtualMachine::ValidateFuncAddress (DeltaCodeAddress funcAddr, bool isMethod) {
	std::map<DeltaCodeAddress, DeltaStdFuncInfo*>::iterator func = funcByAddr.find(funcAddr);
	if (func != funcByAddr.end() && func->second->IsMethod() == isMethod)
		return funcAddr;
	else
		return DELTA_CANTBE_A_FUNCTION_ADDRESS;
}

//------------------------------------------------------------------

void DeltaVirtualMachine::StartUp (void) 
	{ SetGlobalsAndStackPointers(); }

bool DeltaVirtualMachine::PCAtLastInstruction (void) const
	{ return pc == DELTA_PC_PROGRAM_END - 1; }

bool DeltaVirtualMachine::PCIsInvalid (void) const
	{ return pc > codeSize; }

bool DeltaVirtualMachine::PCAtNormalProgramEnd (void) const 
	{ return pc == DELTA_PC_PROGRAM_END; }

bool DeltaVirtualMachine::AreRegistersResetDueToError (void) const {
	return (top == DELTA_TOP_AFTERGLOBALS_VALUE || top == DELTA_TOP_INITIAL_VALUE) &&
			topsp == DELTA_TOPSP_BEFORE_RUN_VALUE &&
			PCAtNormalProgramEnd();
}

//////////////////////////////////

void DeltaVirtualMachine::PrepareForExecution (void) {
	
	// Initialise execution parameters and set running.
	//
	pc			= 0;
	currLine	= 0;
	top			= DELTA_TOP_INITIAL_VALUE;
	isRunning	= true;
	retain_pc	= false;
	
	GetReturnValue().Undefine();
}

//////////////////////////////////

void DeltaVirtualMachine::Run (void) {
	if (HasNotRunYet()) {
		PROFILE_MAIN_ENTER();
		PrepareForExecution();
		EnterExecutionLoop();	// Resident execution loop.
		Continue();
		PROFILE_MAIN_EXIT();
	}
}

//////////////////////////////////

util_ui32 DeltaVirtualMachine::TotalExecutionLoops (void)	
	{ return (util_ui32) DPTR(vmNesting)->size(); }	

//////////////////////////////////

bool DeltaVirtualMachine::HasAnyExecutionLoopFailed (void) {
	for (SnList::iterator i = DPTR(vmNesting)->begin(); i != DPTR(vmNesting)->end(); ++i) {
		DeltaVirtualMachine* vm = DELTAVM_VGET_INST(*i);
		if (!vm || DPTR(vm)->HasProducedError())
			return true;
	}
	return false;
}

//////////////////////////////////

void DeltaVirtualMachine::UnwindingChecker (void)
	{ if (!EXCEPTION_HANDLERS->PerformUnwindingTests(this)) throw ExecutionTerminationException();  }

//////////////////////////////////

bool DeltaVirtualMachine::HasExecutionCompleted (void) const 
	{ return HasProducedError() || PCAtNormalProgramEnd(); }

//////////////////////////////////

void DeltaVirtualMachine::ForceCompleteExecutionByError (void) {

	DASSERT(HasProducedError() && IsErrorCauseReset());
	DASSERT(!IsFoundInAnyCallTrace());
	DASSERT(funcStack.empty());

	if (HasRun()) {

		ResetArguments();
		ResetUserFuncClosure();
		InvalidateArgumentsPreservers();
		callingLibraryFunction	= 0;

		pc		= DELTA_PC_PROGRAM_END;					// So we don't execute further.
		topsp	= DELTA_TOPSP_BEFORE_RUN_VALUE;			// Initial value before run.

		if (top < DELTA_TOP_AFTERGLOBALS_VALUE)	{		// top has moved after globals
			bx	= top + 1;								// We will collect everything,
			top	= DELTA_TOP_FORCE_COMPLETE_EXECUTION;	// except global data (no rerun needed, globals retained).
			CollectGarbage();
		}
		else
			DASSERT(top == DELTA_TOP_AFTERGLOBALS_VALUE || top == DELTA_TOP_INITIAL_VALUE);

		// Postconditions
		DASSERT(AreRegistersResetDueToError());
		DASSERT(IsInvalidationCommitted());		
	}
}

//------------------------------------------------------------------
