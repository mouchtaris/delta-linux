// AssignOverloadable.cpp
// Execution functions for assignments.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Latest revision November 2007, type casting support.
// May 2009, repairing a problem due to changing ovweriting retval if an rvalue and
// trying to invoke a type casting or assignment overloaded operator.
//
#include "DDebug.h"
#include "Assign.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "DeltaString.h"
#include "DeltaRuntimeError.h"
#include "OperatorOverloading.h"
#include "DeltaExceptionHandling.h"
#include "Execute.h"

//	#define	DELTA_ENABLE_ASSIGNMENT_AND_RETURN_WARNINGS

/////////////////////////////////////////////////////////////////////

#ifdef	DELTA_ENABLE_ASSIGNMENT_AND_RETURN_WARNINGS

#define	IF_USING_UNDEFINED_RETURN_VALUE_WARNING(rv)			\
	if (rv->Type() == DeltaValue_Undefined)					\
		DPTR(vm)->Warning(									\
			"'%s' function return value taken",				\
			rv->TypeStr()									\
		);													\
	else

#define	ELSE_IF_ASSIGNING_UNDEFINED_VALUE_WARNING()			\
	if (rv->Type() == DeltaValue_Undefined)					\
		DPTR(vm)->Warning(									\
			"assigning an '%s' value!", rv->TypeStr()		\
	); else

#define	SELF_ASSIGNMENT_WARNING()							\
	DPTR(vm)->Warning(										\
		"assigning a variable of type '%s' with its self!",	\
		lv->TypeStr()										\
	)

#else

#define	IF_USING_UNDEFINED_RETURN_VALUE_WARNING(rv)
#define	ELSE_IF_ASSIGNING_UNDEFINED_VALUE_WARNING()
#define	SELF_ASSIGNMENT_WARNING()

#endif	// DELTA_ENABLE_ASSIGNMENT_AND_RETURN_WARNINGS

/////////////////////////////////////////////////////////////////////
// Helpers to make the code more readable.
//

#define	IS_RETVAL(x)								\
	((x) == retval)

#define	IS_TEMP_RESULT(arg)							\
	instr->result.IsTempVar()

#define	IF_SELF_ASSIGNMENT_IGNORE()					\
	if (rv == lv) {									\
		SELF_ASSIGNMENT_WARNING();					\
		return;										\
	} else

/////////////////////////////////////////////////////////////////////

void Execute_ASSIGN (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	
	DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_ASSIGN_OPERAND_LVALUE, lv);
	DELTA_TRANSLATE_OPERAND(DELTA_ASSIGN_OPERAND_RVALUE, rv, temp);

	DASSERT(
		rv == &temp || 
		(instr->DELTA_ASSIGN_OPERAND_RVALUE.IsStorageType() && vm->ValidateStackValuePtr(rv))
	);

	IF_SELF_ASSIGNMENT_IGNORE()
	ELSE_IF_ASSIGNING_UNDEFINED_VALUE_WARNING(); // General case where the rvalue is undefined.

	lv->AssignOverloadable(rv);
}

/////////////////////////////////////////////////////////////////////

void Execute_ASSIGNTEMPVAR (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	
	DASSERT(instr->DELTA_ASSIGN_OPERAND_LVALUE.IsTempVar());

	DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_ASSIGN_OPERAND_LVALUE, lv);
	DELTA_TRANSLATE_OPERAND(DELTA_ASSIGN_OPERAND_RVALUE, rv, temp);
	
	DASSERT(
		rv == &temp || 
		(instr->DELTA_ASSIGN_OPERAND_RVALUE.IsStorageType() && vm->ValidateStackValuePtr(rv))
	);

	lv->Undefine(); 
	lv->Assign(rv);
}

/////////////////////////////////////////////////////////////////////

void Execute_GETRETVAL (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	
	DASSERT(instr->DELTA_GETRETVAL_OPERAND_LVALUE.IsTempVar());

	DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_GETRETVAL_OPERAND_LVALUE, lv);
	DASSERT(IS_TEMP_RESULT(DELTA_GETRETVAL_OPERAND_LVALUE));		// Retval assigned always to temporaries (should never be optimized).

	DeltaValue* retval = &DPTR(vm)->GetReturnValue();
	IF_USING_UNDEFINED_RETURN_VALUE_WARNING(retval);				// About to get an undefined function result?

	lv->Undefine(); 
	lv->Assign(retval);
	retval->Undefine();												// After the func result is taken (used) we reset the retval.
}

/////////////////////////////////////////////////////////////////////