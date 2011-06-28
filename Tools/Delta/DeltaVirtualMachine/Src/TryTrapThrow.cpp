// DeltaExceptionHandling.cpp
// Script Fighter Project.
// A. Savidis, June-August (lazy imlpementation...) 2006.
//
#include "DeltaExceptionHandling.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaString.h"
#include "DeltaRuntimeError.h"
#include "DeltaVMRegistry.h"

//////////////////////////////////////////////////////////////
// [UNUSED, UNUSED, Result(Addr)]
//
void Execute_TRAPENABLE (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	
	DeltaValue  temp;
	DeltaCodeAddress addr = instr->DELTA_TRAPENABLE_OPERAND_ADDRESS.GetValue();
	DASSERT(vm->IsValidTargetLabel(addr));

	EXCEPTION_HANDLERS->TrapEnable(addr);
}

//////////////////////////////////////////////////////////////
// [UNUSED, UNUSED, Result(Addr)]
//
void Execute_TRAPDISABLE (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	
	DeltaCodeAddress addr = instr->DELTA_TRAPDISABLE_OPERAND_ADDRESS.GetValue();
	DASSERT(vm->IsValidTargetLabel(addr));

	EXCEPTION_HANDLERS->TrapDisable(addr);
}

//////////////////////////////////////////////////////////////
// [Arg1(Trap block id), UNUSED, Result(lvalue)]
//
void Execute_TRAP (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DASSERT(instr->DELTA_TRAP_OPERAND_BLOCKID.IsInternal());

	EXCEPTION_HANDLERS->Trap(
		vm, 
		DNULLCHECK(vm->GetStorageObject(instr->DELTA_TRAP_OPERAND_VARIABLE)), 
		(util_ui16) instr->DELTA_TRAP_OPERAND_BLOCKID.GetValue()
	);
}

//////////////////////////////////////////////////////////////
// [UNUSED, UNUSED, UNUSED, Result(Exception)]
//
void Execute_THROW (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DeltaValue temp;
	if (DeltaValue* exception = vm->TranslateOperand(instr->DELTA_THROW_OPERAND_EXCEPTION, &temp)) {

		DASSERT(exception == &temp || instr->DELTA_THROW_OPERAND_EXCEPTION.IsStorageType());

		if (DPTR(vm)->GetDebugger())
			DPTR(vm)->GetDebuggerRef().OnThrow();

		EXCEPTION_HANDLERS->Throw(vm, *exception);
	}
	else
		DASSERT(vm->HasProducedError());
}

//////////////////////////////////////////////////////////////
