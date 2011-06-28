// Block.h
// Block entry / exit management.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, February 2005.
//

#ifndef	BLOCK_H
#define	BLOCK_H

struct DeltaInstruction;
class DeltaVirtualMachine;

extern void Execute_BLOCKENTER (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_BLOCKENTERWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_BLOCKEXIT (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void Execute_BLOCKEXITWithoutDebugger (DeltaInstruction* instr, DeltaVirtualMachine* vm);

#endif	// Do not add stuff beyond this point.