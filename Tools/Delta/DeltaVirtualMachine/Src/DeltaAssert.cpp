// DeltaAssert.cpp.
// Assert functionality, revised from original version.
// Now it is a built-in command.
// ScriptFighter Project.
// A. Savidis, June 2006.
//

#include "DDebug.h"
#include "Assign.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "DeltaString.h"
#include "DeltaRuntimeError.h"
#include "DeltaAssert.h"
#include "DeltaAssertExtensions.h"
#include "DeltaDebugDynamicActivator.h"

///////////////////////////////////////////////////////////

void Execute_ASSERT (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	
	DeltaValue		temp;
	DeltaValue*	cond = vm->TranslateOperand(instr->result, &temp);
	if (!cond)
		{ DASSERT(vm->HasProducedError()); return; }
	DASSERT(cond == &temp || instr->result.IsStorageType());

	if (!cond->TrueTest()) {	// Assertion failed.
		
		// No debugger backend and supports dynamic activation.
		if (!DPTR(vm)->GetDebugger() && DeltaDebugDynamicActivator().GetSingleton().IsApplicable())
			DeltaDebugDynamicActivator().GetSingleton().LoadOnlyUponUserRequest();

		const char* expr = (const char*) instr->arg1.GetValue();
		if (DPTR(vm)->GetDebugger() && DPTR(vm)->GetDebuggerRef().OnAssertionFailure(expr))	// Continue
			return;

		if (DeltaAssertExtensions::AssertInterfaceResponse(vm, expr) == ASSERT_ABORT)
			DeltaAssertExtensions::Abort();
	}
}

///////////////////////////////////////////////////////////
