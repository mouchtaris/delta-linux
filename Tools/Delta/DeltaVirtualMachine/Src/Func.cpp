// Func.cpp
// Execution functions for managing functions.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Re-factored July 2005 to include new method for operand translation.
// Extended July 2005 to support functor tables.
// Extended August 2005 to support functions holding their VM owner.
// Extended September 2010 with extra more fast opcodes.
//

#include "DDebug.h"
#include "uerrorclass.h"
#include "Func.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "DeltaString.h"
#include "DeltaRuntimeError.h"
#include "DeltaTable.h"
#include "DeltaObject.h"
#include "DeltaVMRegistry.h"
#include "DeltaExceptionHandling.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "DeltaLibFuncBinder.h"
#include "Execute.h"
#include "DeltaClosure.h"
#include "DeltaClosureHandling.h"

#define	IF_CALLEE_NOT_RUN_CHECK_ELSE(_callee_vm)							\
	if (DPTR(_callee_vm)->HasNotRunYet())									\
		DPTR(vm)->SetErrorCode(DELTA_VM_NOT_RUN_ERROR)->PrimaryError(		\
			"vm '%s' (src '%s') not yet run (mandatory after loading)!",	\
			DPTR(_callee_vm)->Id(),											\
			DPTR(_callee_vm)->Source()										\
		);																	\
	else

//------------------------------------------------------------------

#define	CALLFUNC_ARGS \
	DeltaValue* func, DeltaVirtualMachine*	vm

typedef void (*Callfunc_Func) (CALLFUNC_ARGS);

static Callfunc_Func callFuncs[] = {
	DELTAVALUE_OPERATION Callfunc_Error,			// Number
	DELTAVALUE_OPERATION Callfunc_String,			
	DELTAVALUE_OPERATION Callfunc_Table,
	DELTAVALUE_OPERATION Callfunc_ProgramFunc,
	DELTAVALUE_OPERATION Callfunc_Libfunc,
	DELTAVALUE_OPERATION Callfunc_Error,			// Bool
	DELTAVALUE_OPERATION Callfunc_ExternId,
	DELTAVALUE_OPERATION Callfunc_Error,			// Nil
	DELTAVALUE_OPERATION Callfunc_MethodFunc,
	DELTAVALUE_OPERATION Callfunc_Error				// Undefined
};

/////////////////////////////////////////////////////////////////

void DELTAVALUE_OPERATION Callfunc_Libfunc (CALLFUNC_ARGS) {

	IF_CALLEE_NOT_RUN_CHECK_ELSE(vm) {

		// Any extra arguments from the library function itself?
		if (const DeltaLibraryFuncArgsBinder* binder = func->GetLibraryFuncArgsBinder())
			binder->PushArgs(vm);
		DPTR(vm)->CallPreboundLibraryFunc(func->ToLibraryFunc(), DPTR(vm)->GetTotalActualArgs());	
	}
}

//************************
// Calling a string as a library function by late binding.
//
void DELTAVALUE_OPERATION  Callfunc_String (CALLFUNC_ARGS) {
	IF_CALLEE_NOT_RUN_CHECK_ELSE(vm)
		DPTR(vm)->CallLibraryFunc(
			DPTR(func->val.strObj)->Content().c_str(),
			vm->GetTotalActualArgs()
		);
}

//************************

#define	IF_CALLEE_PRODUCED_ERROR_CHECK_ELSE(_callee_vm, _callable)				\
	if (DPTR(_callee_vm)->HasProducedError())									\
		DPTR(vm)->PrimaryError(													\
			"calling a %s of vm '%s' which previously produced an error!",		\
			_callable, DPTR(_callee_vm)->Id()									\
		); 	else

//************************

void DELTAVALUE_OPERATION Callfunc_ProgramFunc (CALLFUNC_ARGS) {

	const DeltaStdFuncInfo* funcInfo;
	DeltaVirtualMachine*	funcVM;
	DeltaClosure*			closure;
	func->ToProgramFunc(&funcInfo, &funcVM, &closure);

	IF_CALLEE_NOT_RUN_CHECK_ELSE(funcVM) {

		util_ui16 totalArgs = vm->GetTotalActualArgs();	// Save before pushing!
		DASSERT(totalArgs == vm->actualArguments.size());

		if (funcVM == vm) {	// Calling VM is function owner VM.

			DASSERT(!DPTR(vm)->HasProducedError());
	
			if (!DPTR(vm)->PushUserArgumentsAndArgumentsVector())	// Push agruments, since it is a normal program  function.
				{ DASSERT(DPTR(vm)->HasProducedError()); return; }

			DPTR(vm)->SetUserFuncClosure(closure, funcInfo);
			DELTA_RETURN_IF_PRODUCEDERROR(vm, DELTA_RETURN_VOID);
			DELTA_ON_UNWINDING_RETURN_TEST(vm,  DELTA_RETURN_VOID);
			DPTR(vm)->ExecuteCallProgramFunc(vm, funcInfo->GetAddress(), totalArgs, DELTA_PROGFUNC_STD_ARGUMENTS);
		}
		else // Calling VM is different from function owner VM.
		IF_CALLEE_PRODUCED_ERROR_CHECK_ELSE(funcVM, "function") {

			DASSERT(funcVM);

			// Pass calling arguments.
			bool result = DPTR(funcVM)->PushUserArgumentsAndArgumentsVector(DPTR(vm)->actualArguments);
			DPTR(vm)->ResetTotalActualArgs();
			if (!result)
				{ DASSERT(DPTR(funcVM)->HasProducedError()); return; }

			DELTA_INVOKE_FUNCTION_SET_TOTAL_ACTUALS_AND_CLOSURE(funcVM, totalArgs, closure, funcInfo);
			DELTA_INVOKE_FUNCTION_OR_METHOD(funcVM, ExtCallGlobalFuncCommit(funcInfo->GetAddress(), DELTA_PROGFUNC_STD_ARGUMENTS));
		}
	}
}

//************************

void DELTAVALUE_OPERATION Callfunc_MethodFunc (CALLFUNC_ARGS) {
 
	DeltaTable*					self;
	const DeltaStdFuncInfo*		funcInfo;
	DeltaVirtualMachine*		methodVM;
	DeltaClosure*				closure;

	func->ToMethodFunc(&funcInfo, &self, &methodVM, &closure);

	IF_CALLEE_NOT_RUN_CHECK_ELSE(methodVM) {

		util_ui16 totalArgs = vm->GetTotalActualArgs();

		if (methodVM == vm) {

			DASSERT(!DPTR(vm)->HasProducedError());
			if (!DPTR(vm)->PushUserArgumentsAndArgumentsVector())
				{ DASSERT(DPTR(vm)->HasProducedError()); return; }

			DPTR(vm)->PushSelfArgument(self);	// Not counted as part of the user arguments.
			DELTA_RETURN_IF_PRODUCEDERROR(vm, DELTA_RETURN_VOID);
			DELTA_ON_UNWINDING_RETURN_TEST(vm,  DELTA_RETURN_VOID);

			DPTR(vm)->SetUserFuncClosure(closure, funcInfo);
			DELTA_RETURN_IF_PRODUCEDERROR(vm, DELTA_RETURN_VOID);
			DELTA_ON_UNWINDING_RETURN_TEST(vm, DELTA_RETURN_VOID);

			DPTR(vm)->ExecuteCallProgramFunc(vm, funcInfo->GetAddress(), totalArgs, DELTA_METHOD_STD_ARGUMENTS);
		}
		else  // Calling VM is different from method owner VM.
		IF_CALLEE_PRODUCED_ERROR_CHECK_ELSE(methodVM, "method") {
			
			DASSERT(methodVM);

			// Pass calling arguments.
			bool result = DPTR(methodVM)->PushUserArgumentsAndArgumentsVector(DPTR(vm)->actualArguments);
			DPTR(vm)->ResetTotalActualArgs();
			if (!result)
				{ DASSERT(DPTR(methodVM)->HasProducedError()); return; }
			
			DPTR(methodVM)->PushSelfArgument(self);	// Not counted as part of the arguments.
			DELTA_RETURN_IF_PRODUCEDERROR(methodVM, DELTA_RETURN_VOID);
			DELTA_ON_UNWINDING_RETURN_TEST(methodVM,  DELTA_RETURN_VOID);

			DELTA_INVOKE_FUNCTION_SET_TOTAL_ACTUALS_AND_CLOSURE(methodVM, totalArgs, closure, funcInfo);
			DELTA_INVOKE_FUNCTION_OR_METHOD(methodVM, ExtCallGlobalFuncCommit(funcInfo->GetAddress(), DELTA_METHOD_STD_ARGUMENTS));
		}
	}
}

//************************
// Call applied on userdata (if any).

void DELTAVALUE_OPERATION Callfunc_ExternId (CALLFUNC_ARGS) {

	if (DeltaTable*	functor	= DPTR(func)->GetExternIdUserData()) {

		DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_RETURN_VOID(
			Callfunc_Table(functor, vm), 
			vm
		);

		if (DPTR(vm)->HasProducedError())
			DPTR(vm)->Error(
				"call of %s '%s' as a function (on userdata) failed!",
				func->TypeStr(),
				DPTR(func->GetExternId())->GetTypeStr().c_str()
			);
	}
	else
		DPTR(vm)->SetErrorCode(DELTA_EXTERNID_MISSING_USERDATA_ERROR)->PrimaryError(
			"illegal call of %s '%s' as a function (no userdata)!",
			func->TypeStr(),
			DPTR(func->GetExternId())->GetTypeStr().c_str()
		);
}

//************************
// Calling a table as a functor object. The table should have a functor, that
// is then called again as an object.
//
void DELTAVALUE_OPERATION Callfunc_Table (CALLFUNC_ARGS)
	{ Callfunc_Table(func->ToTable(), vm); }

static DeltaTable* calledFunctor = (DeltaTable*) 0;

void DELTAVALUE_OPERATION Callfunc_Table (DeltaTable* table, DeltaVirtualMachine* vm) {

	DASSERT(table);

	if (!DPTR(table)->IsOverloadingEnabled())
		DPTR(vm)->SetErrorCode(DELTA_FUNCTOR_OVERLOADING_DISABLED_ERROR)->PrimaryError(
			"in calling %s as function overloading is disabled!", 
			table->GetExtClassString()
		);
	else {
		// The lookup of the functor may hit on an overloaded dot
		// operator (another function). Thus we need to preserve the arguments.

		DeltaVirtualMachine::ArgumentsPreserver preserver(vm);
		DeltaValue functor;

		bool succeeded = DPTR(table)->GetFunctor(&functor) ;
		if (EXCEPTION_HANDLERS->IsUnwinding())
			return;

		if (succeeded) {
			if (calledFunctor == table)
				DPTR(vm)->SetErrorCode(DELTA_FUNCTOR_CYCLIC_REFERENCE_ERROR)->PrimaryError(
					"in calling %s as function detected endless recursion!", 
					table->GetExtClassString()
				);
			else {
				calledFunctor = table;
				preserver.Restore();				// Restore original arguments.
				DPTR(vm)->CallFunction(&functor);	// Recursive call.
			}
			unullify(calledFunctor);
		}
		else
			DPTR(vm)->SetErrorCode(DELTA_FUNCTOR_SLOT_MISSING_ERROR)->PrimaryError(
				"in calling %s as function, no '()' functor member!",
				table->GetExtClassString()
			);
	}
}

//************************

void DELTAVALUE_OPERATION Callfunc_Error (CALLFUNC_ARGS) {
	DPTR(vm)->SetErrorCode(DELTA_NOT_CALLABLE_ERROR)->PrimaryError(
		"illegal value of type '%s' called as a function!",
		func->TypeStr()
	);
}

//------------------------------------------------------------------
// CALLEXTFUNC is used to call quickly a function when the name has been
// already statically resolved at compile time (as an Ident or as string const).
// Hence, we simply call the external function. For optimisation
// reasons, we do not check if the argument is an extern function,
// since, the only way the compiler generates such an instruction
// is to resolve an extern function name.
//
void Execute_CALLEXTFUNC (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	
	switch (instr->DELTA_CALLEXTFUNC_OPERAND_FUNC.GetType()) {

		case DeltaOperand_LibFunc: {
			DPTR(vm)->CallPreboundLibraryFunc(
				(DeltaLibraryFunc) instr->DELTA_CALLEXTFUNC_OPERAND_FUNC.GetValue(),
				vm->GetTotalActualArgs()
			);
			instr->SetOpCode(DeltaVM_CALLEXTPREBOUNDFUNC);
			break;
		}

		case DeltaOperand_ConstString: {
			const char* name = (const char*) instr->DELTA_CALLEXTFUNC_OPERAND_FUNC.GetValue();
			if (DeltaLibraryFunc func = DeltaLibFuncBinder::Get(name)) {
				// Change to a direct libfunc call.
				instr->SetOpCode(DeltaVM_CALLEXTPREBOUNDFUNC);
				instr->DELTA_CALLEXTFUNC_OPERAND_FUNC.SetValue((util_ui32) func);
				instr->DELTA_CALLEXTFUNC_OPERAND_FUNC.SetType(DeltaOperand_LibFunc);
				DPTR(vm)->CallPreboundLibraryFunc(func, vm->GetTotalActualArgs());
			}
			 else 
				DPTR(vm)->SetErrorCode(DELTA_UNRESOLVED_LIBFUNC_ERROR)->PrimaryError(
					"Library function '%s' was not found", 
					name
				);
			break;
		}

		default: DASSERT(false);
	}
}

/////////////////////////////////////////////////////////////////////
// Optimised at runtime always.

void Execute_CALLEXTPREBOUNDFUNC (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	DPTR(vm)->CallPreboundLibraryFunc(
		(DeltaLibraryFunc) instr->DELTA_CALLEXTFUNC_OPERAND_FUNC.GetValue(),
		DPTR(vm)->GetTotalActualArgs()
	); 
}

//------------------------------------------------------------------

void DeltaVirtualMachine::CallFunction (DeltaValue* functor) {
	DASSERT(((util_ui32) functor->Type()) < uarraysize(callFuncs));
	(*callFuncs[functor->Type()])(functor, this);
}

void DeltaVirtualMachine::ExtCallFunction (DeltaValue* functor) {

	DASSERT(!HasProducedError());	
	DASSERT(!EXCEPTION_HANDLERS->IsUnwinding());

	EnterExecutionLoop();
	if (ExtCallFunctionCommit(functor))
		ExitExecutionLoop();
}

/////////////////////////////////////////////////////////////////////

bool DeltaVirtualMachine::ExtCallFunctionCommit (DeltaValue* functor) {

	// Should always assign to this guard value (end pc),
	// else there is no way for the Continue() below
	// to execute selectively only the function body (i.e.,
	// it would restore a valid PC on func ret and 
	// would continue execution, which is not what we want).

	util_ui32 saved_pc		= pc;
	util_ui32 saved_top		= top;
	util_ui32 saved_topsp	= topsp;
	util_ui32 saved_line	= currLine;

	pc = DELTA_PC_PROGRAM_END;

	DELTA_RECORD_VM_SERIALNO_EX(this);
	DELTA_RECORD_EXECUTION_LOOP_PASSPORT(this);

	CallFunction(functor);

	DELTA_VALIDATE_VM_SERIALNO_EX(this, false);
	DELTA_RETURN_IF_PRODUCEDERROR(this, true);
	DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT(this, true);
//	DELTA_ON_UNWINDING_RETURN_TEST(this, true);

	if (pc != DELTA_PC_PROGRAM_END) {	// When calling a library function the saved (prog end) pc is restored.

		DELTA_RECORD_VM_SERIALNO_EX(this);
		DELTA_RECORD_EXECUTION_LOOP_PASSPORT(this);

		ResetTotalActualArgs();
		Continue();

		DELTA_VALIDATE_VM_SERIALNO_EX(this, false);
		DELTA_RETURN_IF_PRODUCEDERROR(this, true);
		DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT(this, true);
//		DELTA_ON_UNWINDING_RETURN_TEST(this, true);
	}

	// We should ensure that the stack pointers remain intact and that
	// we return to the saved PC exactly.

	DASSERT(
		PCAtNormalProgramEnd()	&& 
		top == saved_top		&& 
		topsp == saved_topsp
	);

	pc			= saved_pc;
	currLine	= saved_line;

	return true;
}

/////////////////////////////////////////////////////////////////////

void Execute_CALLFUNC (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DeltaValue  temp;
	DeltaValue* func = vm->TranslateOperand(instr->DELTA_CALLFUNC_OPERAND_FUNC, &temp);
	if (!func)
		{ DASSERT(vm->HasProducedError()); return; }

	DASSERT(func == &temp || instr->DELTA_CALLFUNC_OPERAND_FUNC.IsStorageType());
	DASSERT(func);

	DPTR(vm)->CallFunction(func);
}

//------------------------------------------------------------------
// TODO: the unwinding call when the activatin record is incomplete may cause errors!

#define	DELTA_FAST_PROGRAM_CALL(_std_args, _extra_push, _getfunc, _precheck, _closure)					\
	const DeltaStdFuncInfo* func = _getfunc;															\
	DASSERT(DPTR(vm)->IsValidFuncInfo(func));															\
	_precheck;																							\
	util_ui16 totalArgs = DPTR(vm)->GetTotalActualArgs();	/* Save before pushing! */					\
	DASSERT(totalArgs == (util_ui32) DPTR(vm)->actualArguments.size());									\
	/* Push agruments, since it is a normal program  function. */										\
	DPTR(vm)->PushUserArgumentsAndArgumentsVector();													\
	DELTA_RETURN_IF_PRODUCEDERROR(vm, DELTA_RETURN_VOID);												\
	DELTA_ON_UNWINDING_RETURN_TEST(vm, DELTA_RETURN_VOID);												\
	_extra_push;																						\
	DPTR(vm)->SetUserFuncClosure(_closure, func);														\
	DASSERT(!DPTR(vm)->HasProducedError());	/* No closure error possible as we handle it. */			\
	DPTR(vm)->ExecuteCallProgramFunc(																	\
		vm,																								\
		func->GetAddress(),																				\
		totalArgs,																						\
		_std_args																						\
	);

/////////////////////////////////////////////////////////////////////
// Calling directly a program function of the current vm.

void Execute_CALLPROGRAMFUNC (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	DELTA_FAST_PROGRAM_CALL(
		DELTA_PROGFUNC_STD_ARGUMENTS, 
		UEMPTY, 
		(const DeltaStdFuncInfo*) instr->DELTA_CALLFUNC_OPERAND_FUNC.GetValue(), 
		DASSERT(DPTR(vm)->GetCurrFuncInfo() != func),
		CLOSURE_HANDLERS->MakeClosureIfNeeded(vm, func)
	);
}

/////////////////////////////////////////////////////////////////////
// Recursive calls

void Execute_CALLLAMBDAPROGRAMFUNC (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	DELTA_FAST_PROGRAM_CALL(
		DELTA_PROGFUNC_STD_ARGUMENTS, 
		UEMPTY, 
		DNULLCHECK(DPTR(vm)->GetCurrFuncInfo()), 
		UEMPTY,
		const_cast<DeltaClosure*>(CLOSURE_HANDLERS->GetTopClosure())
	);
}

#define	PUSH_SELF														\
	DPTR(vm)->PushSelfArgument(DPTR(vm)->GetSelfForCurrMethod());		\
	DELTA_RETURN_IF_PRODUCEDERROR(vm, DELTA_RETURN_VOID);				\
	DELTA_ON_UNWINDING_RETURN_TEST(vm, DELTA_RETURN_VOID);

void Execute_CALLLAMBDAMETHODFUNC (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	DELTA_FAST_PROGRAM_CALL(
		DELTA_METHOD_STD_ARGUMENTS, 
		PUSH_SELF, 
		DNULLCHECK(DPTR(vm)->GetCurrFuncInfo()), 
		UEMPTY,
		const_cast<DeltaClosure*>(CLOSURE_HANDLERS->GetTopClosure())
	);
}

//------------------------------------------------------------------
