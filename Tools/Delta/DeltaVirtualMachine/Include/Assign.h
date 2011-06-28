// Assign.h
// Execution functions for assignments.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef	ASSIGN_H
#define	ASSIGN_H

struct DeltaInstruction;
class DeltaVirtualMachine;

extern void Execute_ASSIGN (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_ASSIGNTEMPVAR (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_RETURNVAL (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_GETRETVAL (DeltaInstruction*, DeltaVirtualMachine*);

#endif	// Do not add stuff beyond this point.