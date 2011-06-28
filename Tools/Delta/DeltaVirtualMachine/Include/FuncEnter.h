// FuncEnter.h
// Execution function for function enter opcode.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, July 2001.
//

#ifndef	FUNCENTER_H
#define	FUNCENTER_H

struct DeltaInstruction;
class DeltaVirtualMachine;

extern void Execute_FUNCENTER (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_FUNCENTERWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm);

#endif	// Do not add stuff beyond this point.