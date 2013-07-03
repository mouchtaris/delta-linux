// Execute.cpp.
// Implementation of the ExecuteCurrInstruction member of the
// DeltaVirtualMachine class.
// ScriptFighter Project.
// A. Savidis, November 1999.
// Nunmerous refactorings afterwards. Main change
// is July 2007, putting debug extensions callbacks separately.
//

#include "DDebug.h"
#include "uerrorclass.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "Execute.h"
#include "DeltaTable.h"
#include "DeltaString.h"
#include "DeltaVMRegistry.h"
#include "Jump.h"
#include "Table.h"
#include "Assign.h"
#include "Func.h"
#include "Arithmetic.h"
#include "PushArg.h"
#include "FuncRet.h"
#include "ForEach.h"
#include "FuncEnter.h"
#include "Block.h"
#include "DeltaAssert.h"
#include "uvalidatable.h"
#include "DeltaRuntimeError.h"
#include "DeltaExceptionHandling.h"
#include "DeltaClosureHandling.h"
#include "DeltaDebugDynamicActivator.h"

// Fast calls causes all push instructions and the following
// call to be executed in a single round (not via subsequent
// instruction executions).

//	#define	USE_FAST_CALLS

//------------------------------------------------------------------

extern void Execute_TRAPENABLE (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_TRAPDISABLE (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_TRAP (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_THROW (DeltaInstruction* instr, DeltaVirtualMachine* vm);

void Execute_PROGRAMBEGIN (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	
	DPTR(vm)->StartUp();
	
	DPTR(vm)->GetBlocks().PushBlockIdStack();
	EXCEPTION_HANDLERS->PushFunc(vm, DeltaExceptionHandling::InDeltaGlobalCode);
	CLOSURE_HANDLERS->PushFunc(vm, DELTA_NIL_CLOSURE);

	DASSERT(DELTA_TOPSP_WHEN_AT_GLOBAL_CODE(vm) == DPTR(vm)->topsp);
	DPTR(vm)->PushCallTrace(
		(void*) 0, 
		DPTR(vm)->pc, 
		DPTR(vm)->top, 
		DPTR(vm)->topsp, 
		DPTR(vm)->GetBlocks().GetTopBlockIdStackIndex()
	);
	
	if (DPTR(vm)->GetDebugger())
		DPTR(vm)->GetDebuggerRef().OnStartingExecution();
}

////////////////////////////////////////////////////////////////////

void Execute_PROGRAMEND (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	// ATTENTION: Intentionally, we do not pop the block id stack, since
	// it concerns the global code whose variables should not be collected
	// upon block exit.

	DASSERT(DPTR(vm)->PCAtLastInstruction());

	EXCEPTION_HANDLERS->PopFunc();
	CLOSURE_HANDLERS->PopFunc(vm);

	DeltaVirtualMachine::PopCallTrace();
	if (DPTR(vm)->GetDebugger())
		DPTR(vm)->GetDebuggerRef().OnEndingExecution();
}

void Execute_NOP (DeltaInstruction* instr, DeltaVirtualMachine* vm) {}

//------------------------------------------------------------------

typedef void (*InstructionExecuteFunc) (DeltaInstruction*, DeltaVirtualMachine*);

static InstructionExecuteFunc executeFuncs[]= {
	Execute_ADD,
	Execute_OBJGET,
	Execute_OBJNEW,
	Execute_OBJNEWSET,
	Execute_OBJNEWEMPTY,
	Execute_OBJSET,
	Execute_ASSIGN,
	Execute_ASSIGNTEMPVAR,
	Execute_CALLFUNC,
	Execute_CALLPROGRAMFUNC,
	Execute_CALLLAMBDAPROGRAMFUNC,
	Execute_CALLLAMBDAMETHODFUNC,
	Execute_CALLEXTFUNC,
	Execute_CALLEXTPREBOUNDFUNC,
	Execute_DIV,
	Execute_JUMP,
	Execute_JEQ,
	Execute_JNE,
	Execute_JGE,
	Execute_JGT,
	Execute_JLE,
	Execute_JLT,
	Execute_JTRUETEST,
	Execute_JFALSETEST,
	Execute_MOD,
	Execute_MUL,
	Execute_SUB,
	Execute_PUSHARG,
	Execute_FUNCRET,
	Execute_FUNCENTER,
	Execute_PROGRAMBEGIN,
	Execute_OBJDONE,
	Execute_PUSHLATEBOUNDARG,
	Execute_BLOCKENTER,
	Execute_BLOCKEXIT,
	Execute_BOUNDEDOBJGET,
	Execute_BOUNDEDOBJSET,
	Execute_ASSERT,
	Execute_TRAPENABLE,
	Execute_TRAPDISABLE,
	Execute_TRAP,
	Execute_THROW,
	Execute_PROGRAMEND,
	Execute_OBJNEWATTR,
	Execute_OBJSETATTR,
	Execute_OBJGETATTR,
	Execute_CALLOBJGETMETHOD,
	Execute_CALLOBJBOUNDEDGETMETHOD,
	Execute_FOREACHBEGIN,
	Execute_FOREACHCHECKEND,
	Execute_FOREACHGETVAL,
	Execute_FOREACHGETINDEX,
	Execute_FOREACHFWD,
	Execute_FOREACHEND,
	Execute_GETRETVAL,
	Execute_RETURNVAL,
	Execute_RETURN,
	Execute_NOP
};

////////////////////////////////////////////////////////////////////

static InstructionExecuteFunc executeFuncsWithoutDebugger[]= {
	Execute_ADD,
	Execute_OBJGET,
	Execute_OBJNEW,
	Execute_OBJNEWSET,
	Execute_OBJNEWEMPTY,
	Execute_OBJSET,
	Execute_ASSIGN,
	Execute_ASSIGNTEMPVAR,
	Execute_CALLFUNC,
	Execute_CALLPROGRAMFUNC,
	Execute_CALLLAMBDAPROGRAMFUNC,
	Execute_CALLLAMBDAMETHODFUNC,
	Execute_CALLEXTFUNC,
	Execute_CALLEXTPREBOUNDFUNC,
	Execute_DIV,
	Execute_JUMP,
	Execute_JEQ,
	Execute_JNE,
	Execute_JGE,
	Execute_JGT,
	Execute_JLE,
	Execute_JLT,
	Execute_JTRUETEST,
	Execute_JFALSETEST,
	Execute_MOD,
	Execute_MUL,
	Execute_SUB,
	Execute_PUSHARG,
	Execute_FUNCRETWithoutDebugger,
	Execute_FUNCENTERWithoutDebugger,
	Execute_PROGRAMBEGIN,
	Execute_OBJDONE,
	Execute_PUSHLATEBOUNDARG,
	Execute_BLOCKENTERWithoutDebugger,
	Execute_BLOCKEXITWithoutDebugger,
	Execute_BOUNDEDOBJGET,
	Execute_BOUNDEDOBJSET,
	Execute_ASSERT,
	Execute_TRAPENABLE,
	Execute_TRAPDISABLE,
	Execute_TRAP,
	Execute_THROW,
	Execute_PROGRAMEND,
	Execute_OBJNEWATTR,
	Execute_OBJSETATTR,
	Execute_OBJGETATTR,
	Execute_CALLOBJGETMETHOD,
	Execute_CALLOBJBOUNDEDGETMETHOD,
	Execute_FOREACHBEGIN,
	Execute_FOREACHCHECKEND,
	Execute_FOREACHGETVAL,
	Execute_FOREACHGETINDEX,
	Execute_FOREACHFWD,
	Execute_FOREACHEND,
	Execute_GETRETVAL,
	Execute_RETURNVAL,
	Execute_RETURN,
	Execute_NOP
};

////////////////////////////////////////////////////////////////////
// Passports allow to permanently disable an earlier execution loop
// due to occurence of an error, even when the error flag is cleared
// before we return to it. Such execution loops are renedered ivalid 
// because of the error cleanup actions, so we need to ensure they 
// are gracefully ignored.

void DeltaVirtualMachine::EnterExecutionLoop (void) 
	{ executionLoopPassports.push_front(DELTA_EXECUTION_LOOP_VALID_PASSPORT); }

//********************************

inline bool UncorruptedPassportValue (util_ui32 passport)
	{ return passport == DELTA_EXECUTION_LOOP_VALID_PASSPORT || passport == DELTA_EXECUTION_LOOP_EXPIRED_PASSPORT; }

void DeltaVirtualMachine::ExitExecutionLoop (void) {
	DASSERT(executionLoopPassports.size() > DELTA_EXECUTION_LOOP_MIN_PASSPORTS);
	DASSERT(UncorruptedPassportValue(executionLoopPassports.front()));
	executionLoopPassports.pop_front();
}

//********************************

const util_ui32* DeltaVirtualMachine::GetCurrentExecutionLoopPassport (void) const {
	DASSERT(executionLoopPassports.size() >= DELTA_EXECUTION_LOOP_MIN_PASSPORTS);
	DASSERT(UncorruptedPassportValue(executionLoopPassports.front()));
	return &executionLoopPassports.front();
}

//********************************

bool DeltaVirtualMachine::AllPreceedingPassportsHaveExpired (const ExecutionLoopPassports::iterator& i) const {
	for (ExecutionLoopPassports::iterator j = i; j != executionLoopPassports.end(); ++j) {
		DASSERT(UncorruptedPassportValue(*j));
		if (*j == DELTA_EXECUTION_LOOP_VALID_PASSPORT)
			return false;
	}
	return true;
}

//********************************

void DeltaVirtualMachine::ExpireAllExecutionLoopPassports (void) {
	for (	ExecutionLoopPassports::iterator i = executionLoopPassports.begin();
			i != executionLoopPassports.end();
			++i		) {
			DASSERT(UncorruptedPassportValue(*i));
			if (*i == DELTA_EXECUTION_LOOP_VALID_PASSPORT)
				*i = DELTA_EXECUTION_LOOP_EXPIRED_PASSPORT;
			else
				{ DASSERT(AllPreceedingPassportsHaveExpired(++i)); return; }
	}
}

//********************************

bool DeltaVirtualMachine::IsValidExecutionLoopPassport (util_ui32 passport) {
	DASSERT(UncorruptedPassportValue(passport));
	return passport == DELTA_EXECUTION_LOOP_VALID_PASSPORT;
}

////////////////////////////////////////////////////////////////////

static const char* errorCauseStrings[] = { "caller", "current" };

#define	ERROR_CAUSE_STRING()				errorCauseStrings[(util_ui32) errorCause]
#define	IS_PRIMARY_FAILING_CAUSE(cause)		(cause == CurrentFailed)
#define	IS_SECONDARY_FAILING_CAUSE(cause)	(cause == CallerFailed)

//********************************

bool DeltaVirtualMachine::IsInvalidationCommitted (void) const
	{  return HasProducedError() && errorCause == NoneFailed; }

//********************************

void DeltaVirtualMachine::InvalidateExecution (ErrorCause cause) {

	DASSERT(IsValidErrorCause(cause));
	DASSERT(!HasProducedError() && IsErrorCauseReset());			// No error / error cause set.
	DASSERT(!IsInvalidationCommitted());							// No invalidation committed.
	DASSERT(IsPrimaryFailing() == IS_PRIMARY_FAILING_CAUSE(cause));	// Primary failing sanity.

	if (IsPrimaryFailing())  {		
		
		// No debugger backend and supports dynamic activation.
		if (!GetDebugger() && DeltaDebugDynamicActivator().GetSingleton().IsApplicable())
			DeltaDebugDynamicActivator().GetSingleton().LoadOnlyUponUserRequest();

		// Backend, at this point, may have been activated statically or dynamically.
		if (GetDebugger())
			GetDebuggerRef().OnExecuteHandleErrorBreak();
	}

	SetExecutionError();
	SetErrorCause(cause);
	CommitInvalidation();		

	DASSERT(IsErrorCauseReset());
}

//********************************

void DeltaVirtualMachine::CommitInvalidation (void) {

	DASSERT(!IsInvalidationCommitted());
	DASSERT(IsValidErrorCause(errorCause));
	DASSERT(((util_ui32) errorCause) < uarraysize(errorCauseStrings));
	DASSERT(IsPrimaryFailing() == IS_PRIMARY_FAILING_CAUSE(errorCause));

	// Just post an extra error message.
	Error("interrupting execution due to an error in %s vm!", ERROR_CAUSE_STRING());

	// Only the primary failing vm performs global invalidation iff it was not earlier
	// pruned off the calltrace stack.
	if (IsPrimaryFailing() && IsFoundInAnyCallTrace()) {
		InvalidateEverySecondaryFailingAndPerformClearActions();
		if (GetDebugger())
			GetDebuggerRef().OnExecutionError();
	}

	DASSERT(!IsFoundInAnyCallTrace());
	
	if (IsPrimaryFailing())
		ResetPrimaryFailing();
	ResetErrorCause();						// Error committed.
	ExpireAllExecutionLoopPassports();		// Disable opened execution loops.
	ForceCompleteExecutionByError();		// Further disable execution and GC.	
}

//********************************

bool DeltaVirtualMachine::HasProducedError (void) const
	{ return executionError; }

////////////////////////////////////////////////////////////////////
// The src line is updated accordingly. We check for stack overflow,
// and invalid PC. If PC remains unchanged, proceed to next instruction.
// A function table is used to speed-up execution. If from a break point
// the current VM gets destroyed, false is returned to stop execution.
//

#ifdef _DEBUG
static util_ui32 instrCounter = 0;
#define	DELTA_INC_INSTRUCTION_COUNTER()	++instrCounter
#else
#define	DELTA_INC_INSTRUCTION_COUNTER()
#endif

//------------------------------------------------------------------

#define	EXECUTE_CASE(opcode)	\
	case DeltaVM_##opcode : Execute_##opcode(curr, this); break;

#define	EXECUTE_CALLMETHOD_CASES()																					\
	case DeltaVM_CALLOBJGETMETHOD			: Execute_CALLOBJGETMETHOD(curr, this, sn, passport);			break;	\
	case DeltaVM_CALLOBJBOUNDEDGETMETHOD	: Execute_CALLOBJBOUNDEDGETMETHOD(curr, this, sn, passport);	break;

#ifdef	USE_FAST_CALLS
#define	EXECUTE_PUSHARG_CASES()																						\
	case DeltaVM_PUSHARG					: Execute_PUSHARG(curr, this, sn, passport);					break;	\
	case DeltaVM_PUSHLATEBOUNDARG			: Execute_PUSHLATEBOUNDARG(curr, this, sn, passport);			break;
#else
#define	EXECUTE_PUSHARG_CASES()																						\
	case DeltaVM_PUSHARG					: Execute_PUSHARG(curr, this);									break;	\
	case DeltaVM_PUSHLATEBOUNDARG			: Execute_PUSHLATEBOUNDARG(curr, this	);						break;
#endif

#define	EXECUTE_BLOCK_CASES()																						\
	case DeltaVM_BLOCKENTER					: Execute_BLOCKENTER(curr, this);								break;	\
	case DeltaVM_BLOCKEXIT					: Execute_BLOCKEXIT(curr, this);								break;

#define	EXECUTE_BLOCK_CASES_WITHOUT_DEBUGGER()																		\
	case DeltaVM_BLOCKENTER					: Execute_BLOCKENTERWithoutDebugger(curr, this);				break;	\
	case DeltaVM_BLOCKEXIT					: Execute_BLOCKEXITWithoutDebugger(curr, this);					break;

#define	EXECUTE_FUNC_CASES()																						\
	case DeltaVM_FUNCENTER					: Execute_FUNCENTER(curr, this);								break;	\
	case DeltaVM_FUNCRET					: Execute_FUNCRET(curr, this);									break;

#define	EXECUTE_FUNC_CASES_WITHOUT_DEBUGGER()																		\
	case DeltaVM_FUNCRET					: Execute_FUNCRETWithoutDebugger(curr, this);					break;	\
	case DeltaVM_FUNCENTER					: Execute_FUNCENTERWithoutDebugger(curr, this);					break;

#define	EXECUTE_SWITCH(opcode)						\
	switch (opcode) {								\
	EXECUTE_CASE(ADD)								\
	EXECUTE_CASE(OBJGET)							\
	EXECUTE_CASE(OBJNEW)							\
	EXECUTE_CASE(OBJNEWSET)							\
	EXECUTE_CASE(OBJNEWEMPTY)						\
	EXECUTE_CASE(OBJSET)							\
	EXECUTE_CASE(ASSIGN)							\
	EXECUTE_CASE(ASSIGNTEMPVAR)						\
	EXECUTE_CASE(CALLFUNC)							\
	EXECUTE_CASE(CALLPROGRAMFUNC)					\
	EXECUTE_CASE(CALLLAMBDAPROGRAMFUNC)				\
	EXECUTE_CASE(CALLLAMBDAMETHODFUNC)				\
	EXECUTE_CASE(CALLEXTFUNC)						\
	EXECUTE_CASE(CALLEXTPREBOUNDFUNC)				\
	EXECUTE_CASE(DIV)								\
	EXECUTE_CASE(JUMP)								\
	EXECUTE_CASE(JEQ)								\
	EXECUTE_CASE(JNE)								\
	EXECUTE_CASE(JGE)								\
	EXECUTE_CASE(JGT)								\
	EXECUTE_CASE(JLE)								\
	EXECUTE_CASE(JLT)								\
	EXECUTE_CASE(JTRUETEST)							\
	EXECUTE_CASE(JFALSETEST)						\
	EXECUTE_CASE(MOD)								\
	EXECUTE_CASE(MUL)								\
	EXECUTE_CASE(SUB)								\
	EXECUTE_PUSHARG_CASES()							\
	EXECUTE_FUNC_CASES()							\
	EXECUTE_CASE(PROGRAMBEGIN)						\
	EXECUTE_CASE(OBJDONE)							\
	EXECUTE_BLOCK_CASES()							\
	EXECUTE_CASE(BOUNDEDOBJGET)						\
	EXECUTE_CASE(BOUNDEDOBJSET)						\
	EXECUTE_CASE(ASSERT)							\
	EXECUTE_CASE(TRAPENABLE)						\
	EXECUTE_CASE(TRAPDISABLE)						\
	EXECUTE_CASE(TRAP)								\
	EXECUTE_CASE(THROW)								\
	EXECUTE_CASE(PROGRAMEND)						\
	EXECUTE_CASE(OBJNEWATTR)						\
	EXECUTE_CASE(OBJSETATTR)						\
	EXECUTE_CASE(OBJGETATTR)						\
	EXECUTE_CALLMETHOD_CASES()						\
	EXECUTE_CASE(FOREACHBEGIN)						\
	EXECUTE_CASE(FOREACHCHECKEND)					\
	EXECUTE_CASE(FOREACHGETVAL)						\
	EXECUTE_CASE(FOREACHGETINDEX)					\
	EXECUTE_CASE(FOREACHFWD)						\
	EXECUTE_CASE(FOREACHEND)						\
	EXECUTE_CASE(GETRETVAL)							\
	EXECUTE_CASE(RETURNVAL)							\
	EXECUTE_CASE(RETURN)							\
	EXECUTE_CASE(NOP)								\
	default : DASSERT(false);						\
	}

#define	EXECUTE_SWITCH_WITHOUT_DEBUGGER(opcode)		\
	switch (opcode) {								\
	EXECUTE_CASE(ADD)								\
	EXECUTE_CASE(OBJGET)							\
	EXECUTE_CASE(OBJNEW)							\
	EXECUTE_CASE(OBJNEWSET)							\
	EXECUTE_CASE(OBJNEWEMPTY)						\
	EXECUTE_CASE(OBJSET)							\
	EXECUTE_CASE(ASSIGN)							\
	EXECUTE_CASE(ASSIGNTEMPVAR)						\
	EXECUTE_CASE(CALLFUNC)							\
	EXECUTE_CASE(CALLPROGRAMFUNC)					\
	EXECUTE_CASE(CALLLAMBDAPROGRAMFUNC)				\
	EXECUTE_CASE(CALLLAMBDAMETHODFUNC)				\
	EXECUTE_CASE(CALLEXTFUNC)						\
	EXECUTE_CASE(CALLEXTPREBOUNDFUNC)				\
	EXECUTE_CASE(DIV)								\
	EXECUTE_CASE(JUMP)								\
	EXECUTE_CASE(JEQ)								\
	EXECUTE_CASE(JNE)								\
	EXECUTE_CASE(JGE)								\
	EXECUTE_CASE(JGT)								\
	EXECUTE_CASE(JLE)								\
	EXECUTE_CASE(JLT)								\
	EXECUTE_CASE(JTRUETEST)							\
	EXECUTE_CASE(JFALSETEST)						\
	EXECUTE_CASE(MOD)								\
	EXECUTE_CASE(MUL)								\
	EXECUTE_CASE(SUB)								\
	EXECUTE_PUSHARG_CASES()							\
	EXECUTE_FUNC_CASES_WITHOUT_DEBUGGER()			\
	EXECUTE_CASE(PROGRAMBEGIN)						\
	EXECUTE_CASE(OBJDONE)							\
	EXECUTE_BLOCK_CASES_WITHOUT_DEBUGGER()			\
	EXECUTE_CASE(BOUNDEDOBJGET)						\
	EXECUTE_CASE(BOUNDEDOBJSET)						\
	EXECUTE_CASE(ASSERT)							\
	EXECUTE_CASE(TRAPENABLE)						\
	EXECUTE_CASE(TRAPDISABLE)						\
	EXECUTE_CASE(TRAP)								\
	EXECUTE_CASE(THROW)								\
	EXECUTE_CASE(PROGRAMEND)						\
	EXECUTE_CASE(OBJNEWATTR)						\
	EXECUTE_CASE(OBJSETATTR)						\
	EXECUTE_CASE(OBJGETATTR)						\
	EXECUTE_CALLMETHOD_CASES()						\
	EXECUTE_CASE(FOREACHBEGIN)						\
	EXECUTE_CASE(FOREACHCHECKEND)					\
	EXECUTE_CASE(FOREACHGETVAL)						\
	EXECUTE_CASE(FOREACHGETINDEX)					\
	EXECUTE_CASE(FOREACHFWD)						\
	EXECUTE_CASE(FOREACHEND)						\
	EXECUTE_CASE(GETRETVAL)							\
	EXECUTE_CASE(RETURNVAL)							\
	EXECUTE_CASE(RETURN)							\
	EXECUTE_CASE(NOP)								\
	default : DASSERT(false);						\
	}
//------------------------------------------------------------------

#define	EXECUTE_BY_DISPATCH_WITHOUT_DEBUGGER	(*executeFuncsWithoutDebugger[curr->GetOpCode()])(curr, this)
#define	EXECUTE_BY_DISPATCH						(*executeFuncs[curr->GetOpCode()])(curr, this)
#define	EXECUTE_BY_SWITCH						EXECUTE_SWITCH(curr->GetOpCode())
#define	EXECUTE_BY_SWITCH_WITHOUT_DEBUGGER		EXECUTE_SWITCH_WITHOUT_DEBUGGER(curr->GetOpCode())

#define	EXECUTE_INSTRUCTION_IMPL(_debug_unloading_check, _breakpoint_check, _execute)				\
	DASSERT(!retain_pc && !PCIsInvalid() && !HasProducedError());									\
	DeltaInstruction* curr = code + pc;																\
	if (curr->line)	{																				\
		currLine = curr->line;																		\
		if (!GetCurrFuncInfo())																		\
			UpdateCallLineAndPCOfTopCallTraceBeingAtGlobalCode();									\
	}																								\
	_debug_unloading_check;																			\
	_breakpoint_check;																				\
	old_pc = pc;																					\
	DELTA_INC_INSTRUCTION_COUNTER();																\
	DELTA_CORE_EXECUTION_LOOP(sn, passport, _execute, this, throw ExecutionTerminationException());	\
	/* Error can be propagated from a callee VM. Behaves as if current failed ( */					\
	/* (necessary to propagate the error). */														\
	DASSERT(!HasProducedError());																	\
	if (RunTimeErrorsExist() && GetAnyErrorInvalidatesAll())	{									\
		PossiblePrimaryError("failed artificially due to errors in other vms");						\
		throw ExecutionTerminationException();														\
	}																								\
	if (retain_pc)																					\
		ResetRetainPC();																			\
	else																							\
	if (pc == old_pc)	/* If the PC has not changed, go to the next instruction. */				\
		++pc;																						\

////////////////////////////////////////////////////////////////////

#define	DEBUGGER_UNLOADING_CHECK																	\
	if (DeltaDebugDynamicActivator().GetSingleton().IsUnloadPending())								\
		DeltaDebugDynamicActivator().GetSingleton().Unload()

#define	DEBUGGER_BREAKPOINT_CHECK(_next_instr)															\
	switch (GetDebuggerRef().OnExecuteCheckBreakPoints()) {												\
		case		DeltaDebugExtensionsSuper::ReturnFalse	:	throw ExecutionTerminationException();	\
		case		DeltaDebugExtensionsSuper::ReturnTrue	:	_next_instr;							\
		case		DeltaDebugExtensionsSuper::FallThrough	:	break;									\
		default: DASSERT(false);																		\
	}

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::ExecuteCurrInstruction (util_ui32 sn, const util_ui32* passport) {
	DASSERT(GetDebugger()); util_ui32 old_pc;
	EXECUTE_INSTRUCTION_IMPL(DEBUGGER_UNLOADING_CHECK, DEBUGGER_BREAKPOINT_CHECK(return), EXECUTE_BY_SWITCH);
}

void DeltaVirtualMachine::ExecuteCurrInstructionWithoutDebugger (util_ui32 sn, const util_ui32* passport) {
	DASSERT(!GetDebugger()); util_ui32 old_pc;
	EXECUTE_INSTRUCTION_IMPL(UEMPTY, UEMPTY, EXECUTE_BY_SWITCH_WITHOUT_DEBUGGER);
}

//------------------------------------------------------------------

#define	NOT_PROGRAM_END()				(pc != DELTA_PC_PROGRAM_END)
#define	PERFORM_UNWINDING_CHECKS()		(this->*currUnwindingChecker)()

void DeltaVirtualMachine::Continue (void) {

	DASSERT(HasRun() && !EXCEPTION_HANDLERS->IsUnwinding() && NOT_PROGRAM_END());

	DeltaVirtualMachine* saved_vm = GetCallingVM();
	SetCallingVM(this);	
	DPTR(vmNesting)->push_front(GetSerialNo());
	EnterExecutionLoop();
	EXCEPTION_HANDLERS->PushExecutionLoop(this);
	ResetUnwindingChecker();	// Always reset before normal execution.

	DELTA_RECORD_VM_SERIALNO_EX(this);
	DELTA_RECORD_EXECUTION_LOOP_PASSPORT(this);

	// We use a try / catch block since it is faster than testing
	// boolean return flags on the executor and the unwinding checker.

	util_ui32 old_pc = 0;

	#define	sn			DELTA_GET_RECORDED_VM_SERIALNO(this)
	#define	passport	DELTA_GET_RECORDED_EXECUTION_LOOP_PASSPORT(this)

	try {
		do {
			switch (currExecutorType) {
				case WithDebugger : {
					DASSERT(GetDebugger());
					EXECUTE_INSTRUCTION_IMPL(DEBUGGER_UNLOADING_CHECK, DEBUGGER_BREAKPOINT_CHECK(continue), EXECUTE_BY_SWITCH); 
					break; 
				}
				case WithoutDebugger: {
					DASSERT(!GetDebugger());
					EXECUTE_INSTRUCTION_IMPL(UEMPTY, UEMPTY, EXECUTE_BY_SWITCH_WITHOUT_DEBUGGER);
					break; 
				}
				default: DASSERT(false);
			}
			PERFORM_UNWINDING_CHECKS(); 
			DASSERT(GetCallingVM() == this); 
		}
		while (NOT_PROGRAM_END());	
	}
	catch (ExecutionTerminationException) 
		{}

	#undef	sn
	#undef	passport

	SetCallingVM(saved_vm);
	DPTR(vmNesting)->pop_front();

	DELTA_VALIDATE_VM_SERIALNO_EX(this, DELTA_RETURN_VOID);
	ExitExecutionLoop();
	EXCEPTION_HANDLERS->PopExecutionLoop(this);
}

//------------------------------------------------------------------
