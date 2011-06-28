// FuncEnter.cpp
// ScriptFighter Project, optimisation phase.
// Function enter sequence implementation as a single
// run-time instruction. This substitutes the code-generation
// function start sequence in the old ScriptFighter version.
// A. Savidis, July 2001.
//

#include "DDebug.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "DeltaString.h"
#include "FuncEnter.h"
#include "DeltaRuntimeError.h"

/////////////////////////////////////////////////////////////////

#define	FUNCENTER_ACTIONS(_call)																\
	DASSERT(																					\
		instr->DELTA_FUNCENTER_OPERAND_TOTALLOCALS.IsInternal()	&&								\
		instr->DELTA_FUNCENTER_OPERAND_BLOCKID.IsInternal()	&&									\
		(util_ui16) instr->DELTA_FUNCENTER_OPERAND_BLOCKID.GetValue() == DELTA_MAIN_BLOCK_VALUE	\
	);																							\
	DPTR(vm)->DoLocalFuncEnterCode(																\
		(const DeltaStdFuncInfo*) instr->DELTA_FUNCENTER_OPERAND_FUNCINFO.GetValue()			\
	);																							\
	DPTR(vm)->_call(																			\
		(util_ui16) instr->DELTA_FUNCENTER_OPERAND_BLOCKID.GetValue(),							\
		(util_ui16) instr->DELTA_FUNCENTER_OPERAND_TOTALLOCALS.GetValue(),						\
		false																					\
	);

/////////////////////////////////////////////////////////////////

void Execute_FUNCENTER (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ FUNCENTER_ACTIONS(BlockEnterImpl); }

void Execute_FUNCENTERWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ FUNCENTER_ACTIONS(BlockEnterImplWithoutDebugger); }

/////////////////////////////////////////////////////////////////
