// Table.hh
// Execution functions for managing tables.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
//

#ifndef	TABLE_HH
#define	TABLE_HH

struct DeltaInstruction;
class DeltaVirtualMachine;

///////////////////////////////////////////////////////////
// The following has been implemented for use with the debugger
// expression evaluator for tables.
//
extern void Execute_OBJGET (	
				DeltaValue*			table,
				DeltaValue*			index,
				DeltaValue*			result,
				void					(*callback)(char*)
			);

///////////////////////////////////////////////////////////

extern void Execute_OBJGET (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_BOUNDEDOBJGET (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_OBJNEW (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_OBJNEWSET (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_OBJNEWEMPTY (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_OBJDONE (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_OBJSET (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_BOUNDEDOBJSET (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_OBJNEWATTR (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_OBJSETATTR (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_OBJGETATTR (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_CALLOBJGETMETHOD (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_CALLOBJBOUNDEDGETMETHOD (DeltaInstruction*, DeltaVirtualMachine*);

#endif	// Do not add stuff beyond this point.