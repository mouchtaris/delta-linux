// PushArg.h
// Execution function for actual arguments push.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, July 2001.
//

#ifndef	PUSHARG_H
#define	PUSHARG_H

struct DeltaInstruction;
class DeltaVirtualMachine;

extern void Execute_PUSHARG (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_PUSHARG (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport);
extern void Execute_PUSHLATEBOUNDARG (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_PUSHLATEBOUNDARG (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport);

#endif	// Do not add stuff beyond this point.