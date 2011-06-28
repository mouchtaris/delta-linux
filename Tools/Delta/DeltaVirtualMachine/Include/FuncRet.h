// FuncRet.h
// Execution function for function return opcode.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, July 2001.
//

#ifndef	FUNCRET_H
#define	FUNCRET_H

struct DeltaInstruction;
class DeltaVirtualMachine;

extern void Execute_FUNCRET (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_FUNCRETWithoutDebugger (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_RETURN (DeltaInstruction*, DeltaVirtualMachine*);

#endif	// Do not add stuff beyond this point.