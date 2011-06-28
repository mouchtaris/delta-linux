// DeltaAssert.hh
// Assert in the Delta VM
// ScriptFighter Project.
// A. Savidis, June 2006.
//

#ifndef	DELTAASSERT_H
#define	DELTAASSERT_H

struct DeltaInstruction;
class DeltaVirtualMachine;
class DeltaObject;

extern void Execute_ASSERT (DeltaInstruction*, DeltaVirtualMachine*);

#endif	// Do not add stuff beyond this point.