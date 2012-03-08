// PushArg.cpp
// Implementation of PUSHARG virtual machine instruction, pushing
// actual arguments to the run-time stack. This opcode in the Delta VM
// has been added to substitute the sequence of instructions
// 's[top] = arg, top = top-1' with a single fast instruction.
// ScriptFighter Project, optimiztion phase.
// A. Savidis, July 2001.
// Last revisions February 2006.
// Revised May 2009, allowed nil to be passed as an argument to functions
// and supported lists for argument unrolling.
//
#include "DDebug.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaRuntimeError.h"
#include "DeltaTable.h"
#include "DeltaArgumentsRecycler.h"
#include "DeltaTableFactory.h"
#include "DeltaValue.h"
#include "PushArg.h"
#include "Func.h"
#include "Table.h"
#include "Execute.h"

///////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::ArgumentsPreserver::Copy (void) {		// Auto invoked by the VM.
	if (!copied) {
		args	= DPTR(vm)->GetActualArguments();
		n		= DPTR(vm)->GetTotalActualArgs();
		DPTR(vm)->ResetArguments();
		copied	= true;
	}
}

//*******************************

void DeltaVirtualMachine::ArgumentsPreserver::Invalidate (void) {	// When the vm dies or when an error occurs.
	unullify(vm); 
	if (copied) {
		std::for_each(args.begin(), args.end(), &DeltaValueFactory::Delete);
		copied = false;
	}
}

//*******************************

void DeltaVirtualMachine::ArgumentsPreserver::Restore (void) {	// Invoked by the call method logic.
	if (vm && copied)
		DPTR(vm)->RestoreArguments(args, n);
}

//*******************************

DeltaVirtualMachine::ArgumentsPreserver::ArgumentsPreserver (DeltaVirtualMachine* _vm) : 
	n(0),
	vm(_vm),
	copied(false)
	{ DPTR(vm)->SetArgumentsPreserver(this); }					// Auto registration.

//*******************************

DeltaVirtualMachine::ArgumentsPreserver::~ArgumentsPreserver() 
	{ if (vm) DPTR(vm)->ResetArgumentsPreserver(this); }	// Auto cancellation.

///////////////////////////////////////////////////////////////////////

bool DeltaVirtualMachine::PushUserArgumentsOnly (void) 
	{ return PushUserArgumentsOnly(actualArguments); }

bool DeltaVirtualMachine::PushUserArgumentsOnly (std::list<DeltaValue*>& userArguments) {

	for (std::list<DeltaValue*>::iterator i = userArguments.begin(); i != userArguments.end(); ++i) {			
		stack[top].Assign(DPTR(*i));	// Push on the runtime stack.
		if (!top_minusminus())			// Stack overflow.
			return false;
		DeltaValueFactory::Delete(*i);	// Clear dynamic argument.
	}

	userArguments.clear();
	return true;
}

///////////////////////////////////////////////////////////////////////

bool DeltaVirtualMachine::PushUserArgumentsAndArgumentsVector (void) 
	{ return PushUserArgumentsAndArgumentsVector(actualArguments); }

bool DeltaVirtualMachine::PushUserArgumentsAndArgumentsVector (std::list<DeltaValue*>& userArguments) {
	
	DeltaValue				argumentsVal;
	CollectableContainer*	argumentsCont   = DNULLCHECK(GetStdUtilities())->MakeVector(&argumentsVal);
	uvector<DeltaValue>&	argumentsVector = GetStdUtilities()->GetVector(&argumentsVal);

	for (std::list<DeltaValue*>::iterator i = userArguments.begin(); i != userArguments.end(); ++i) {			
		stack[top].Assign(DPTR(*i));	// Push on the runtime stack.
		if (!top_minusminus())			// Stack overflow.
			return false;
		argumentsVector.push_front(**i);
		argumentsVector.front().SetResidencyType(DeltaValue::Contained, argumentsCont);
		DeltaValueFactory::Delete(*i);	// Clear dynamic argument.
	}

	userArguments.clear();
	stack[top].Assign(argumentsVal);
	return top_minusminus();
}


///////////////////////////////////////////////////////////////////////

DeltaValue* DeltaVirtualMachine::NewActualArgument (void) {

	InvokeArgumentsPreserverIfWasSet();	// Preserve arguments if requested.

	DeltaValue* obj = DeltaValueFactory::New();
	actualArguments.push_back(obj);
	++actualArgsPushed;

	DASSERT(actualArgsPushed == actualArguments.size());
	return obj;
}

///////////////////////////////////////////////////////////////////////
// In this case, all arguments are pushed (always in reverse order).
//
void DeltaVirtualMachine::PushArgumentList (const std::list<DeltaValue>& l) {
	for (std::list<DeltaValue>::const_reverse_iterator i = l.rbegin(); i != l.rend(); ++i)
		DPTR(NewActualArgument())->Assign(*i);
}

void DeltaVirtualMachine::PushArgumentVector (const uvector<DeltaValue>& args) {
	util_ui32 n = args.size();
	while (n) 
		DPTR(NewActualArgument())->Assign(args[--n]);
}

///////////////////////////////////////////////////////////////////////
// In this case, the argument must be an object of type table having
// numerically indexed items, with successive indices. No overloading
// (dot or attributes or content assignment) applies.
//

#define	CHECK_GET_ITEM(key)															\
	if (!DPTR(table)->BasicGetBySkippingUserCode(index, &content)) {				\
		SetErrorCode(DELTA_ILL_FORMED_TABLE_IN_DYNAMIC_ARGS_ERROR)->				\
		PrimaryError("in dynamic arguments |table|, getting '%s'!", key);			\
		return;																		\
	} else

//**********************************

#define	CHECK_ITEM_TYPE(var, key)													\
	if (content.Type() != DeltaValue_Number) {										\
		SetErrorCode(DELTA_ILL_FORMED_TABLE_IN_DYNAMIC_ARGS_ERROR)->				\
		PrimaryError(																\
			"in dynamic arguments |table| number '%s' field expected (%s found)!",	\
			key, content.TypeStr()													\
		);																			\
		return;																		\
	} else if (true) {																\
		double x = content.ToNumber();												\
		var = (util_ui32) x;														\
		if ( x < 0 || ((double) var) != x) {										\
			SetErrorCode(DELTA_ILL_FORMED_TABLE_IN_DYNAMIC_ARGS_ERROR)->			\
			PrimaryError(															\
				"in dynamic arguments |table|, field '%s' not an unsigned number!",	\
				key, content.TypeStr()												\
			);																		\
			return;																	\
		}																			\
	} else


//**********************************

#define	CHECK_TOTAL(var)																\
	if (var > DELTA_TABLEARGUMENTS_MAXTOTAL ) {											\
		SetErrorCode(DELTA_ILL_FORMED_TABLE_IN_DYNAMIC_ARGS_ERROR)->					\
		PrimaryError(																	\
			"in dynamic arguments |table|, field '%s' (value %u) exceeds bound %u!",	\
			DELTA_TABLEARGUMENTS_TOTAL, var, DELTA_TABLEARGUMENTS_MAXTOTAL				\
		);																				\
		return;																			\
	} else
			
//**********************************

#define	HANDLE_ARGUMENTS_VARIABLE(var, key, val)	\
	index.FromString(key);							\
	CHECK_GET_ITEM(key);							\
	if (content.Type() != DeltaValue_Nil)			\
		CHECK_ITEM_TYPE(var, key);					\
	else											\
		var = val

//**********************************

void DeltaVirtualMachine::PushArgumentTable (DeltaTable* table) {

	util_ui32 total = 0, start = 0, end = 0;
	DeltaValue index, content;

	HANDLE_ARGUMENTS_VARIABLE(
		total, 
		DELTA_TABLEARGUMENTS_TOTAL, 
		DPTR(table)->Total()
	);

	CHECK_TOTAL(total);

	HANDLE_ARGUMENTS_VARIABLE(
		start, 
		DELTA_TABLEARGUMENTS_START, 
		0
	);

	for (end = start + total; end-- != start; ) {			// Always in reverse order.

		index.FromNumber((DeltaNumberValueType) end);

		CHECK_GET_ITEM(										// Checks only execution error.
			uconstructstr("element [%u]!", (util_ui32) end).c_str()
		);

		DPTR(NewActualArgument())->Assign(content);			// Nil allowed.
	}
}

///////////////////////////////////////////////////////////////////////

#define	PUSH_FAST_CASE(_opcode)											\
	case DeltaVM_##_opcode:	 {											\
		Execute_##_opcode(instr, vm, sn, passport); return;				\
	}

#define	CALL_FAST_CASE(_opcode, _extra_args)							\
	case DeltaVM_##_opcode: {											\
		util_ui32 old_pc = vm->pc;										\
		Execute_##_opcode(instr, vm _extra_args);						\
		if (sn && vm->pc == old_pc)										\
			++vm->pc;													\
		return;															\
	}

#define	CALL_METHOD_EXTRA	, sn, passport

#define	CALL_FAST_IMPL()												\
	if (true) {															\
		DASSERT(!DPTR(vm)->HasProducedError());							\
		instr = instr + 1;												\
		++(vm->pc);														\
		switch (instr->GetOpCode()) {									\
			PUSH_FAST_CASE(PUSHARG);									\
			PUSH_FAST_CASE(PUSHLATEBOUNDARG);							\
			CALL_FAST_CASE(CALLFUNC, UEMPTY);							\
			CALL_FAST_CASE(CALLEXTFUNC, UEMPTY);						\
			CALL_FAST_CASE(CALLEXTPREBOUNDFUNC, UEMPTY);				\
			CALL_FAST_CASE(CALLPROGRAMFUNC, UEMPTY);					\
			CALL_FAST_CASE(CALLLAMBDAPROGRAMFUNC, UEMPTY);				\
			CALL_FAST_CASE(CALLLAMBDAMETHODFUNC, UEMPTY);				\
			CALL_FAST_CASE(CALLOBJGETMETHOD, CALL_METHOD_EXTRA);		\
			CALL_FAST_CASE(CALLOBJBOUNDEDGETMETHOD, CALL_METHOD_EXTRA);	\
			default: DASSERT(false);									\
		}																\
	} else

///////////////////////////////////////////////////////////////////////

#define	PUSHLATEBOUNDARG_IMPL()																\
	DASSERT(instr->arg1.IsStorageType());													\
	if (DeltaValue* val = DPTR(vm)->GetStorageObject(instr->arg1))							\
		if (val->Type() == DeltaValue_Table)												\
			DPTR(vm)->PushArgumentTable(val->ToTable());									\
		else																				\
		if (DNULLCHECK(DPTR(vm)->GetStdUtilities())->IsList(val))							\
			DPTR(vm)->PushArgumentList(DPTR(vm)->GetStdUtilities()->GetList(val));			\
		else																				\
		if (DNULLCHECK(DPTR(vm)->GetStdUtilities())->IsVector(val))							\
			DPTR(vm)->PushArgumentVector(DPTR(vm)->GetStdUtilities()->GetVector(val));		\
		else {																				\
			DPTR(vm)->SetErrorCode(DELTA_INVALID_VALUE_IN_DYNAMIC_ARGS_ERROR)->				\
			PrimaryError(																	\
				"in dynamic arguments |expr|:"												\
				"table / object / list / vector expected but '%s' passed",					\
				val->TypeStr()																\
			);																				\
			return;																			\
		}																					\
	else																					\
		{ DASSERT(DPTR(vm)->HasProducedError()); return; }

///////////////////////////////////////////////////////////////////////

void Execute_PUSHLATEBOUNDARG (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ PUSHLATEBOUNDARG_IMPL(); }

void Execute_PUSHLATEBOUNDARG (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport) 
	{ PUSHLATEBOUNDARG_IMPL(); CALL_FAST_IMPL(); }

///////////////////////////////////////////////////////////////////////

#define	DECLARE_HANDLE_VARS											\
	DeltaValue*	arg;												\
	DeltaValue*	obj;

// If the operand translation does not use the supplied 'arg',
// we have a storage argument that needs to be assigned to the
// to the argument placeholder explicitly.

#define	HANDLE_ARG(_arg)											\
	if (instr->_arg.GetType() != DeltaOperand_NotUsed) {			\
		arg = DPTR(vm)->NewActualArgument();						\
		obj = DPTR(vm)->TranslateOperand(instr->_arg, arg);			\
		if (!obj)													\
			{ DASSERT(DPTR(vm)->HasProducedError()); return; }		\
		else														\
		if (obj != arg) {											\
			DASSERT(instr->_arg.IsStorageType());					\
			arg->Assign(obj);										\
		}															\
	}

///////////////////////////////////////////////////////////////////////
// There might be multiple arguments to be pushed onto the stack in a single
// PUSHARG instruction.

#define	PUSHARG_IMPL()												\
	DASSERT(!DPTR(vm)->HasProducedError());							\
	DASSERT(instr->arg1.GetType() != DeltaOperand_NotUsed);			\
	DECLARE_HANDLE_VARS;											\
	HANDLE_ARG(arg1);												\
	HANDLE_ARG(arg2);												\
	HANDLE_ARG(result);

///////////////////////////////////////////////////////////////////////

void Execute_PUSHARG (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ PUSHARG_IMPL(); }

void Execute_PUSHARG (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport) 
	{ PUSHARG_IMPL(); CALL_FAST_IMPL(); }

///////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::SetStdUtilities (StdUtilities* utils)
	{ DASSERT(utils && !stdUtilities); stdUtilities = utils; }

void DeltaVirtualMachine::ResetStdUtilities (void)
	{ DASSERT(stdUtilities); unullify(stdUtilities); }

DeltaVirtualMachine::StdUtilities* DeltaVirtualMachine::GetStdUtilities (void)
	{ return stdUtilities; }

///////////////////////////////////////////////////////////////////////
