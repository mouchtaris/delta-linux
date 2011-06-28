// Func.h
// Execution functions for functions.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef	FUNC_H
#define	FUNC_H

struct DeltaInstruction;
class DeltaVirtualMachine;

extern void Execute_CALLFUNC				(DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_CALLEXTFUNC				(DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_CALLEXTPREBOUNDFUNC		(DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_CALLPROGRAMFUNC			(DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_CALLLAMBDAPROGRAMFUNC	(DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_CALLLAMBDAMETHODFUNC	(DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_NOP						(DeltaInstruction*, DeltaVirtualMachine*);

#endif	// Do not add stuff beyond this point.