// ForEach.cpp
// Execution functions regarding foreach stmt.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "JumpP.h"
#include "ForEach.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "uerrorclass.h"

#define	DELTA_FOREACH_ITERATOR			"iterator"
#define	DELTA_FOREACH_SETBEGIN			"setbegin"
#define	DELTA_FOREACH_CHECKEND			"checkend"
#define	DELTA_FOREACH_GETVAL			"getval"
#define	DELTA_FOREACH_GETINDEX			"getindex"
#define	DELTA_FOREACH_FWD				"fwd"

#define	ITERATOR_METHOD_VOID_RETURN		(DeltaValue*) 0
#define	ITERATOR_METHOD_VOID_ARGUMENTS	(DeltaValue*) 0

#define	CATEGORY_ITERATOR				"iterator"
#define	CATEGORY_CONTAINER				"container"

////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::SetTableIteratorHandler (TableIteratorHandler* handler)
	{ DASSERT(handler && !tableIteratorHandler); tableIteratorHandler = handler; }

void DeltaVirtualMachine::ResetTableIteratorHandler (void)
	{ DASSERT(tableIteratorHandler); unullify(tableIteratorHandler); }

DeltaVirtualMachine::TableIteratorHandler* DeltaVirtualMachine::GetTableIteratorHandler (void)
	{ return tableIteratorHandler; }

////////////////////////////////////////////////////////////////////

static bool InvokeObjectMethod (
		DeltaVirtualMachine*	vm,
		const DeltaValue*		caller,	// The original caller object when seeking for a method.
		const char*				method, 
		const char*				category,
		DeltaTable*				owner,
		const char*				typeStr,
		DeltaValue*				result, 
		DeltaValue*				arg = ITERATOR_METHOD_VOID_ARGUMENTS
	) {

	DeltaValue f;
	if (!DPTR(owner)->Get(DeltaValue(method), &f)) {
		DPTR(vm)->PossiblePrimaryError(
			"in 'foreach %s' while trying to seek '%s' slot in '%s' 0x%x!",
			category,
			method,
			typeStr,
			owner->GetSerialNo()
		);
		return false;
	}

	if (f.IsNil()) {
		DPTR(vm)->PrimaryError(
			"in 'foreach %s' could not find '%s' in '%s' 0x%x!",
			category,
			method,
			typeStr,
			owner->GetSerialNo()
		);
		return false;
	}
	
	bool succeeded;
	DeltaValue actualCaller;
	if (DeltaLibraryFuncArgsBinder::IsLibraryMethodMatchingCaller(*caller, f, &actualCaller))
		succeeded = arg ? f(actualCaller, *arg, result) : f(*caller, result);
	else
		succeeded = arg ? f(*arg, result) : f(result);

	if (!succeeded) {
		DPTR(vm)->PossiblePrimaryError(
			"in 'foreach %s' while calling '%s'!", 
			category, 
			method
		);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////

static bool InvokeExternIdMethod (
		DeltaVirtualMachine*	vm,
		const char*				method, 
		const char*				category,
		DeltaValue*				owner, 
		DeltaValue*				result, 
		DeltaValue*				arg = (DeltaValue*) 0
	) {

	DASSERT(owner->Type() == DeltaValue_ExternId);

	DeltaTable* userData = DNULLCHECK(owner->GetExternIdUserData());
	return	InvokeObjectMethod(
				vm,
				owner,
				method,
				category,
				userData,
				(owner->GetExternIdTypeString() + "(method table)").c_str(),
				result,
				arg
			);
}

////////////////////////////////////////////////////////////////////

typedef DeltaVirtualMachine::TableIteratorHandler TableIteratorHandler;

static void ForeachBeginOnContainerObject (
				DeltaVirtualMachine*	vm,
				TableIteratorHandler*	handler,
				DeltaValue*				cont, 
				DeltaValue*				iter
			);

static void ForeachBeginOnContainerExternId (
				DeltaVirtualMachine*	vm, 
				DeltaValue*				cont, 
				DeltaValue*				iter
			);

static void ForeachSetIteratorAtBegin (
				DeltaVirtualMachine*	vm,
				DeltaValue* 			iter, 
				DeltaValue* 			cont
			);

static void ForeachSetAtBeginOnIteratorObject (
				DeltaVirtualMachine*	vm,
				DeltaValue* 			iter, 
				DeltaValue* 			cont
			);

static void ForeachSetAtBeginOnIteratorExternId (
				DeltaVirtualMachine*	vm,
				DeltaValue* 			iter, 
				DeltaValue* 			cont
			);

////////////////////////////////////////////////////////////////////

static void ForeachBeginOnContainerObject (
		DeltaVirtualMachine*	vm,
		TableIteratorHandler*	handler,
		DeltaValue*				cont, 
		DeltaValue*				iter
	) {
	
	DeltaTable* t = cont->ToTable();
	DeltaValue f;

	if (!DPTR(t)->Get(DELTA_FOREACH_ITERATOR, &f)) {	// Failure in get
		DPTR(vm)->PossiblePrimaryError(
			"in 'foreach' while getting '%s' slot from '%s' 0x%x!", 
			DELTA_FOREACH_ITERATOR,
			t->GetExtClassString(),
			t->GetSerialNo()
		);
		return;
	}

	if (f.IsNil())	{	// No iterator method in object, so we do built-in table iteration.
		DNULLCHECK(handler)->Make(iter);
		ForeachSetIteratorAtBegin(vm, iter, cont);
	}
	else {
		bool succeeded;
		DeltaValue actualCaller;
		if (DeltaLibraryFuncArgsBinder::IsLibraryMethodMatchingCaller(*cont, f, &actualCaller))
			succeeded = f(actualCaller, iter);
		else
			succeeded = f(iter);

		if (!succeeded)	// Continue with user-defined iterators.
			DPTR(vm)->PossiblePrimaryError(
				"in 'foreach' while invoking '%s' slot of '%s' 0x%x!",
				DELTA_FOREACH_ITERATOR,
				t->GetExtClassString(),
				t->GetSerialNo()
			);
		else
			ForeachSetIteratorAtBegin(vm, iter, cont);
	}
}

////////////////////////////////////////////////////////////////////

static void ForeachBeginOnContainerExternId (
		DeltaVirtualMachine*	vm, 
		DeltaValue*				cont, 
		DeltaValue*				iter
	) {
	DASSERT(cont->Type() == DeltaValue_ExternId);
	if (InvokeExternIdMethod(vm, DELTA_FOREACH_ITERATOR, CATEGORY_CONTAINER, cont, iter)) // Iterator constructor.
		ForeachSetIteratorAtBegin(vm, iter, cont);	
}

////////////////////////////////////////////////////////////////////

static void ForeachSetIteratorAtBegin (DeltaVirtualMachine*	vm, DeltaValue* iter, DeltaValue* cont) {
	switch (iter->Type()) {
		case DeltaValue_Table:
			ForeachSetAtBeginOnIteratorObject(vm, iter, cont); 
			return;
		case DeltaValue_ExternId:
			ForeachSetAtBeginOnIteratorExternId(vm, iter, cont); 
			return;
		default: 
			DPTR(vm)->PrimaryError(
				"in 'foreach' invalid iterator type '%s' in '%s'!", 
				iter->TypeStr(),
				DELTA_FOREACH_SETBEGIN
			);
	}
}

////////////////////////////////////////////////////////////////////

static void ForeachSetAtBeginOnIteratorObject (DeltaVirtualMachine* vm, DeltaValue* iter, DeltaValue* cont) {
	InvokeObjectMethod(
		vm,
		iter,
		DELTA_FOREACH_SETBEGIN,
		CATEGORY_ITERATOR,
		iter->ToTable(),
		iter->TypeStr(),
		ITERATOR_METHOD_VOID_RETURN,
		cont
	);
}

static void ForeachSetAtBeginOnIteratorExternId (DeltaVirtualMachine* vm, DeltaValue* iter, DeltaValue* cont) {
	InvokeExternIdMethod(
		vm, 
		DELTA_FOREACH_SETBEGIN, 
		CATEGORY_ITERATOR,
		iter, 
		ITERATOR_METHOD_VOID_RETURN, 
		cont
	);
}

////////////////////////////////////////////////////////////////////

void Execute_FOREACHBEGIN (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DASSERT(
		instr->DELTA_FOREACHBEGIN_OPERAND_CONTAINER.IsStorageType() && 
		instr->DELTA_FOREACHBEGIN_OPERAND_ITERATOR.IsStorageType()
	);

	DeltaValue* cont = DPTR(vm)->GetStorageObject(instr->DELTA_FOREACHBEGIN_OPERAND_CONTAINER);
	DASSERT(cont != &DPTR(vm)->GetReturnValue());

	DeltaValue* iter = DPTR(vm)->GetStorageObject(instr->DELTA_FOREACHBEGIN_OPERAND_ITERATOR);
	DASSERT(instr->DELTA_FOREACHBEGIN_OPERAND_ITERATOR.IsTempVar());

	iter->Undefine(); // Clear now any remaining content (since it is a temp variable).

	if (!cont || !iter)
		{ DASSERT(vm->HasProducedError()); return; }

	switch (cont->Type()) {
		case DeltaValue_Table:		
			ForeachBeginOnContainerObject(
				vm, 
				DPTR(vm)->GetTableIteratorHandler(), 
				cont, 
				iter
			);	return;
		case DeltaValue_ExternId:	
			ForeachBeginOnContainerExternId(
				vm, 
				cont, 
				iter
			);	return;
		default:
			DPTR(vm)->PrimaryError(
				 "invalid container type '%s' in 'foreach'!", 
				 cont->TypeStr()
			);	return;
	}
}

////////////////////////////////////////////////////////////////////

void Execute_FOREACHCHECKEND (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DASSERT(
		instr->DELTA_FOREACHCHECKEND_OPERAND_CONTAINER.IsStorageType() && 
		instr->DELTA_FOREACHCHECKEND_OPERAND_ITERATOR.IsStorageType()
	);
	DASSERT(instr->DELTA_OPERAND_JUMP_TARGET.GetType() == DeltaOperand_Label);

	DeltaValue* cont = DPTR(vm)->GetStorageObject(instr->DELTA_FOREACHCHECKEND_OPERAND_CONTAINER);
	DeltaValue* iter = DPTR(vm)->GetStorageObject(instr->DELTA_FOREACHCHECKEND_OPERAND_ITERATOR);
	if (!cont || !iter)
		{ DASSERT(vm->HasProducedError()); return; }

	DeltaValue atEnd;
	bool succeeded = false;

	switch (iter->Type()) {
		case DeltaValue_Table:		
			succeeded = InvokeObjectMethod(
							vm,
							iter,
							DELTA_FOREACH_CHECKEND,
							CATEGORY_ITERATOR,
							iter->ToTable(),
							iter->TypeStr(),
							&atEnd,
							cont
						); break;

		case DeltaValue_ExternId:	
			succeeded = InvokeExternIdMethod(
							vm,
							DELTA_FOREACH_CHECKEND,
							CATEGORY_ITERATOR,
							iter,							
							&atEnd,
							cont
						); break;
		default: DASSERT(false);
	}

	if (succeeded) {
		if (atEnd.Type() == DeltaValue_Bool) {
			if (atEnd.ToBool())
				{ DELTA_INSTRUCTIONS_TAKE_THE_JUMP(); }
		}
		else
			DPTR(vm)->PrimaryError(
				 "invalid return type '%s' of iterator '%s' in 'foreach'!", 
				 atEnd.TypeStr(),
				 DELTA_FOREACH_CHECKEND
			);
	}
}

////////////////////////////////////////////////////////////////////

#define	GET_VAL_OR_INDEX_IMPL(_iter_operand, _val_or_index_operand, _iter_method_name)				\
	DASSERT(instr->_iter_operand.IsStorageType() && instr->_val_or_index_operand.IsStorageType());	\
	DeltaValue* iter	= DPTR(vm)->GetStorageObject(instr->_iter_operand);							\
	DeltaValue* result	= DPTR(vm)->GetStorageObject(instr->_val_or_index_operand);					\
	if (!iter || !result)																			\
		{ DASSERT(vm->HasProducedError()); return; }												\
	switch (iter->Type()) {																			\
		case DeltaValue_Table:																		\
			InvokeObjectMethod(																		\
				vm,																					\
				iter,																				\
				_iter_method_name,																	\
				CATEGORY_ITERATOR,																	\
				iter->ToTable(),																	\
				iter->TypeStr(),																	\
				result																				\
			); return;																				\
		case DeltaValue_ExternId:																	\
			InvokeExternIdMethod(																	\
				vm,																					\
				_iter_method_name,																	\
				CATEGORY_ITERATOR,																	\
				iter,																				\
				result																				\
			); return;																				\
		default: DASSERT(false);																	\
	}

void Execute_FOREACHGETVAL (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	GET_VAL_OR_INDEX_IMPL(
		DELTA_FOREACHGETVAL_OPERAND_ITERATOR,
		DELTA_FOREACHGETVAL_OPERAND_VARIABLE,
		DELTA_FOREACH_GETVAL
	);
}

void Execute_FOREACHGETINDEX (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	GET_VAL_OR_INDEX_IMPL(
		DELTA_FOREACHGETVAL_OPERAND_ITERATOR,
		DELTA_FOREACHGETINDEX_OPERAND_VARIABLE,
		DELTA_FOREACH_GETINDEX
	);
}

////////////////////////////////////////////////////////////////////

void Execute_FOREACHFWD (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DASSERT(instr->DELTA_FOREACHFWD_OPERAND_ITERATOR.IsStorageType());
	DASSERT(instr->DELTA_OPERAND_JUMP_TARGET.GetType() == DeltaOperand_Label);

	DeltaValue* iter = DPTR(vm)->GetStorageObject(instr->DELTA_FOREACHFWD_OPERAND_ITERATOR);
	if (!iter)
		{ DASSERT(vm->HasProducedError()); return; }

	bool succeeded = false;
	switch (iter->Type()) {
		case DeltaValue_Table:		
			succeeded = InvokeObjectMethod(
							vm,
							iter,
							DELTA_FOREACH_FWD,
							CATEGORY_ITERATOR,
							iter->ToTable(),
							iter->TypeStr(),
							ITERATOR_METHOD_VOID_RETURN
						); break;

		case DeltaValue_ExternId:	
			succeeded = InvokeExternIdMethod(
							vm,
							DELTA_FOREACH_FWD,
							CATEGORY_ITERATOR,
							iter,
							ITERATOR_METHOD_VOID_RETURN
						); break;
		default: DASSERT(false);
	}

	if (succeeded)
		{ DELTA_INSTRUCTIONS_TAKE_THE_JUMP(); }	// Loop iteration jump.
}

////////////////////////////////////////////////////////////////////

void Execute_FOREACHEND (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DASSERT(instr->DELTA_FOREACHEND_OPERAND_ITERATOR.IsStorageType());
	DeltaValue* iter = DPTR(vm)->GetStorageObject(instr->DELTA_FOREACHEND_OPERAND_ITERATOR);
	if (!iter)
		{ DASSERT(vm->HasProducedError()); return; }
	else
		iter->Undefine();
}

////////////////////////////////////////////////////////////////////
