// Execute.h
// Common macros relying on the VM which are used for instructions only.
// ScriptFighter Project.
// A. Savidis, November 2009 (moved here from the VM header).
//

#ifndef	EXECUTE_H
#define	EXECUTE_H

#include "DeltaVirtualMachine.h"

//---------------------------------------------------------------
// For operand translation inside instructions only.

#define	DELTA_TRANSLATE_OPERAND(arg, var, temp)										\
	DeltaValue temp;																\
	DeltaValue* var = DPTR(vm)->TranslateOperand(instr->arg, &temp);				\
	if (!var)																		\
		{ DASSERT(vm->HasProducedError()); return; }								\
	DASSERT(var == &temp || instr->arg.IsStorageType())

#define	DELTA_TRANSLATE_NON_STORAGE_OPERAND(arg, var, temp)							\
	DeltaValue temp;																\
	DeltaValue* var = DPTR(vm)->TranslateOperand(instr->arg, &temp);				\
	if (!var)																		\
		{ DASSERT(vm->HasProducedError()); return; }								\
	DASSERT(var == &temp)

#define	DELTA_TRANSLATE_STORAGE_OPERAND(arg, var)									\
	DeltaValue* var = DPTR(vm)->GetStorageObject(instr->arg);						\
	if (!var)																		\
		{ DASSERT(vm->HasProducedError()); return; }								\
	else DASSERT(DPTR(vm)->ValidateStackValuePtr(var))

///////////////////////////////////////////////////////
// Used to pop failing vm entries due to an error. Abstracts on both
// the normal VM call trace stack as well as the debugger backend stack.

#define	DELTA_DETERMINE_CALLSTACK_ENTRIES_TO_POP_ON_ERROR(							\
			vm_entry_type,															\
			get_curr_vm,															\
			position_at_top_call,													\
			begin_stack_iteration,													\
			stack_bottom_not_reached,												\
			stack_down,																\
			caller_vm																\
		)																			\
	position_at_top_call;															\
	std::list<vm_entry_type> failing, assumedNotFailing;							\
	failing.push_back(caller_vm);													\
	begin_stack_iteration;															\
	util_ui32 callsToPop = 1;														\
	while (stack_bottom_not_reached) {												\
		stack_down;																	\
		vm_entry_type currVM = get_curr_vm;											\
		if (std::find(failing.begin(), failing.end(), currVM) != failing.end()) {	\
			++callsToPop;															\
			if (assumedNotFailing.size()) {											\
				callsToPop += (util_ui32) assumedNotFailing.size();					\
				ulistcopy(assumedNotFailing, failing);								\
				assumedNotFailing.clear();											\
			}																		\
		}																			\
		else																		\
			assumedNotFailing.push_back(currVM);									\
	}

//---------------------------------------------------------------

#define _DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP(								\
		call,																		\
		vm,																			\
		validate_vm_serialno,														\
		validate_execution_loop_passport											\
	)																				\
	if (true) {																		\
		DELTA_RECORD_VM_SERIALNO_EX(vm);											\
		DELTA_RECORD_EXECUTION_LOOP_PASSPORT(vm);									\
		{ call; }																	\
		validate_vm_serialno;														\
		validate_execution_loop_passport;											\
	} else

#define	DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP(call, vm, ret)					\
	_DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP(									\
		call,																		\
		vm,																			\
		DELTA_VALIDATE_VM_SERIALNO_EX(vm, ret),										\
		DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT(vm, ret)								\
	)

#define DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_RETURN_VOID(call, vm)			\
	_DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP(									\
		call,																		\
		vm,																			\
		DELTA_VALIDATE_VM_SERIALNO_EX_RETURN_VOID(vm),								\
		DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT_RETURN_VOID(vm)						\
	)

#define _DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_ERROR_RETURN(					\
		call,																		\
		vm,																			\
		validate_vm_serialno,														\
		validate_execution_loop_passport,											\
		return_if_produced_error													\
	)																				\
	if (true) {																		\
		DELTA_RECORD_VM_SERIALNO_EX(vm);											\
		DELTA_RECORD_EXECUTION_LOOP_PASSPORT(vm);									\
		{ call; }																	\
		validate_vm_serialno;														\
		validate_execution_loop_passport;											\
		return_if_produced_error;													\
	} else

#define	DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_ERROR_RETURN(					\
			call, vm, zombi_vm_ret, ret												\
		)																			\
		_DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_ERROR_RETURN(					\
			call,																	\
			vm,																		\
			DELTA_VALIDATE_VM_SERIALNO_EX(vm, zombi_vm_ret),						\
			DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT(vm, ret),						\
			DELTA_RETURN_IF_PRODUCEDERROR(vm, ret)									\
		)

#define	DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_ERROR_RETURN_RETURN_VOID(		\
			call, vm																\
		)																			\
		_DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_ERROR_RETURN(					\
			call,																	\
			vm,																		\
			DELTA_VALIDATE_VM_SERIALNO_EX_RETURN_VOID(vm),							\
			DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT_RETURN_VOID(vm),					\
			DELTA_RETURN_IF_PRODUCEDERROR_RETURN_VOID(vm)							\
		)

#define	DELTA_CORE_EXECUTION_LOOP(sn, passport, call, vm, ret)						\
	if (true) {																		\
		{ call; }																	\
		if (	!sn													||				\
				*passport != DELTA_EXECUTION_LOOP_VALID_PASSPORT	||				\
				vm->HasProducedError())												\
			ret;																	\
	} else

//---------------------------------------------------------------

#define	DELTA_EXECUTION_LOOP_VALID_PASSPORT		0x23232323
#define	DELTA_EXECUTION_LOOP_EXPIRED_PASSPORT	0x0
#define	DELTA_EXECUTION_LOOP_MIN_PASSPORTS		1
#define	DELTA_OLD_PC_TO_ALWAYS_RETAIN_PC_VALUE	0xffffffff

#define	DELTA_TOPSP_BEFORE_RUN_VALUE			0xffffffff
#define	DELTA_TOPSP_WHEN_AT_GLOBAL_CODE(vm)		(DPTR(vm)->stackSize - 1)
#define	DELTA_TOP_INITIAL_VALUE					(stackSize - 1)
#define	DELTA_TOP_AFTERGLOBALS_VALUE			(stackSize - 1 - totalGlobals)
#define	DELTA_TOP_FORCE_COMPLETE_EXECUTION		DELTA_TOP_AFTERGLOBALS_VALUE
#define	DELTA_TOPSP_LIBFUNC						DELTA_TOPSP_BEFORE_RUN_VALUE
#define	DELTA_CALLTRACE_STACK_SIZE				256
#define	DELTA_CALLTRACE_TAG_MASK				0x80402010
#define	DELTA_CALLTRACE_TAG_ENCODE(tag)			(~((tag) ^ DELTA_CALLTRACE_TAG_MASK))
#define	DELTA_CALLTRACE_TAG_DECODE(tag)			((~(tag)) ^ DELTA_CALLTRACE_TAG_MASK)
#define	DELTA_TOPSP_GLOBALCODE_FUNCNAME			"at " DBG_GLOBAL_CODE_DESCRIPTION
#define	DELTA_PC_PROGRAM_END					GetCodeSize()
#define	DELTA_PC_PROGRAM_END_VM(vm)				DPTR(vm)->GetCodeSize()
#define	DELTA_PC_LAST_VM_INSTRUCTION(vm)		(DPTR(vm)->GetCodeSize() - 1)

//---------------------------------------------------------------

#define DELTA_INVOKE_FUNCTION_SET_TOTAL_ACTUALS_AND_CLOSURE(_vm, _n, _cl, _fi)	\
	if (true) {																	\
		DPTR(_vm)->actualArgsPushed = _n;										\
		DPTR(_vm)->SetUserFuncClosure(_cl, _fi);								\
		DELTA_RETURN_IF_PRODUCEDERROR(_vm, DELTA_RETURN_VOID);					\
	} else

#define	DELTA_INVOKE_FUNCTION_OR_METHOD(_callee_vm, _call)						\
	if (true) {																	\
	DELTA_RECORD_VM_SERIALNO(vm);												\
	DELTA_RECORD_VM_SERIALNO(_callee_vm);										\
	DELTA_RECORD_VM_ID(_callee_vm);												\
	DELTA_RECORD_EXECUTION_LOOP_PASSPORT(vm);									\
	DPTR(_callee_vm)->_call;													\
	DELTA_VALIDATE_VM_SERIALNO(vm, DELTA_RETURN_VOID);							\
	DELTA_IF_INVALID_VM_SERIALNO(_callee_vm) {									\
		DPTR(vm)->Warning(														\
			"while calling function of vm '%s' the callee vm was destroyed!",	\
			DELTA_GET_RECORDED_VM_ID(_callee_vm).c_str()						\
		);																		\
		DPTR(vm)->GetReturnValue().Undefine();									\
		return;																	\
	}																			\
	DELTA_ON_UNWINDING_RETURN_TEST(vm, UNWIND_EMPTY);							\
	if (DPTR(_callee_vm)->HasProducedError()) {									\
		if (!DPTR(vm)->HasProducedError())										\
			DPTR(vm)->PrimaryError(												\
				"calling a function of vm '%s' produced an error!",				\
				DPTR(_callee_vm)->Id()											\
			);																	\
		DPTR(vm)->GetReturnValue().Undefine();									\
	}																			\
	else {																		\
		DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT(vm, DELTA_RETURN_VOID);			\
		/* Propagate return value to the calling VM. */							\
		DPTR(vm)->SetReturnValue(DPTR(_callee_vm)->GetReturnValue());			\
		DPTR(_callee_vm)->GetReturnValue().Undefine();							\
	}																			\
	} else

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
