// ForEach.h
// Execution functions regarding foreach instruction.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, July 2009.
//

#ifndef	FOREACH_H
#define	FOREACH_H

struct DeltaInstruction;
class DeltaVirtualMachine;

extern void Execute_FOREACHBEGIN (DeltaInstruction* instr, DeltaVirtualMachine* vm);	
extern void Execute_FOREACHCHECKEND (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_FOREACHGETVAL (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_FOREACHGETINDEX (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_FOREACHFWD (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_FOREACHEND (DeltaInstruction* instr, DeltaVirtualMachine* vm);

#endif	// Do not add stuff beyond this point.