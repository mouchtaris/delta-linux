// FuncRet.cpp
// ScriptFighter Project, optimisation phase.
// Function return sequence implementation as a single
// run-time instruction. This substitutes the code-generation
// return sequence in the old ScriptFighter version.
// A. Savidis, July 2001.
//

#include "DDebug.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "DeltaString.h"
#include "FuncRet.h"
#include "DeltaRuntimeError.h"
#include "Execute.h"

//	#define	DELTA_ENABLE_ASSIGNMENT_AND_RETURN_WARNINGS

/////////////////////////////////////////////////////////////////////

#ifdef	DELTA_ENABLE_ASSIGNMENT_AND_RETURN_WARNINGS
#define	IF_RETURNING_UNDEFINED_VALUE_WARNING()				\
	if (rv->Type() == DeltaValue_Undefined)					\
	DPTR(vm)->Warning(										\
		"function '%s' returns an undefined value!",		\
		DPTR(vm)->GetCurrFuncInfo()->GetName().c_str()		\
	);														\
	else
#else

#define	IF_RETURNING_UNDEFINED_VALUE_WARNING()
#endif	// DELTA_ENABLE_ASSIGNMENT_AND_RETURN_WARNINGS

/////////////////////////////////////////////////////////////////////

void Execute_RETURNVAL (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DELTA_TRANSLATE_OPERAND(DELTA_RETURNVAL_OPERAND_RVALUE, rv, temp);
	DASSERT(
		rv == &temp || 
		(instr->DELTA_RETURNVAL_OPERAND_RVALUE.IsStorageType() && vm->ValidateStackValuePtr(rv))
	);

	DASSERT(DPTR(vm)->GetCurrFuncInfo());			// Always inside a function (it is a return <expr>).
	DPTR(vm)->SetCurrFuncReturnsValue();			// Make the vm aware the curr function returns a value.
	IF_RETURNING_UNDEFINED_VALUE_WARNING();			// About to return (rvalue) an undefined value?
	DPTR(vm)->SetReturnValue(rv);					// Set value.

	DPTR(vm)->DoExitBlock(DPTR(vm)->GetBlocks().ExitAllBlocksOfCurrentCall());
	DPTR(vm)->DoLocalFuncReturnCode();				// And perform function exit actions.
}

/////////////////////////////////////////////////////////////////////

void Execute_RETURN (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	DPTR(vm)->DoExitBlock(DPTR(vm)->GetBlocks().ExitAllBlocksOfCurrentCall());
	DPTR(vm)->DoLocalFuncReturnCode();
}

/////////////////////////////////////////////////////////////////

extern bool vm_retain_pc;

#define	FUNCRET_ACTIONS(_call)																	\
	DASSERT(																					\
		instr->DELTA_FUNCENTER_OPERAND_TOTALLOCALS.IsInternal()	&&								\
		instr->DELTA_FUNCENTER_OPERAND_BLOCKID.IsInternal()	&&									\
		(util_ui16) instr->DELTA_FUNCENTER_OPERAND_BLOCKID.GetValue() == DELTA_MAIN_BLOCK_VALUE	\
	);																							\
	DPTR(vm)->_call(																			\
		(util_ui16) instr->DELTA_FUNCENTER_OPERAND_BLOCKID.GetValue(),							\
		(util_ui16) instr->DELTA_FUNCENTER_OPERAND_TOTALLOCALS.GetValue(),						\
		false																					\
	);																							\
	DPTR(vm)->DoLocalFuncReturnCode();															\
	DPTR(vm)->SetRetainPC();

/////////////////////////////////////////////////////////////////

void Execute_FUNCRET (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ FUNCRET_ACTIONS(BlockExitImpl); }

void Execute_FUNCRETWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ FUNCRET_ACTIONS(BlockExitImplWithoutDebugger); }

/////////////////////////////////////////////////////////////////
