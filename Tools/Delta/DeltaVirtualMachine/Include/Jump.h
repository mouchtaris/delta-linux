// Jump.h
// Implementation of the execution logic for conditional JUMP
// jumps statements.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, November 1999.
//

#ifndef	JUMP_H
#define	JUMP_H

#include "DeltaStdDefs.h"
#include "DeltaValue.h"
#include "DeltaByteCodeTypes.h"

/////////////////////////////////////////////////////////////
// Functions needed for the expression evaluator (debugger).
// Those functions use directly values, hence, they need not be
// part of an instruction set. However, the VM is still used.
//
DVM_FUNC  	bool Execute_EQ (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine*, void (*callback)(const char*));
DVM_FUNC  	bool Execute_NE (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine*, void (*callback)(const char*));
DVM_FUNC  	bool Execute_GE (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine*, void (*callback)(const char*));
DVM_FUNC  	bool Execute_GT (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine*, void (*callback)(const char*));
DVM_FUNC  	bool Execute_LE (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine*, void (*callback)(const char*));
DVM_FUNC  	bool Execute_LT (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine*, void (*callback)(const char*));

/////////////////////////////////////////////////////////////
// Functions needed for the built-in relational operator library functions.
//
DVM_FUNC  	bool ExecuteBuiltIn_EQ (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine* vm);
DVM_FUNC  	bool ExecuteBuiltIn_NE (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine* vm);
DVM_FUNC  	bool ExecuteBuiltIn_GE (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine* vm);
DVM_FUNC  	bool ExecuteBuiltIn_GT (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine* vm);
DVM_FUNC  	bool ExecuteBuiltIn_LE (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine* vm);
DVM_FUNC  	bool ExecuteBuiltIn_LT (DeltaValue* arg1, DeltaValue* arg2, DeltaVirtualMachine* vm);

/////////////////////////////////////////////////////////////
// Functions for the VM execution loop (not exported).
//
extern		void Execute_JUMP	(DeltaInstruction*, DeltaVirtualMachine*);
extern 		void Execute_JEQ	(DeltaInstruction*, DeltaVirtualMachine*);
extern 		void Execute_JNE	(DeltaInstruction*, DeltaVirtualMachine*);
extern 		void Execute_JGE	(DeltaInstruction*, DeltaVirtualMachine*);
extern 		void Execute_JGT	(DeltaInstruction*, DeltaVirtualMachine*);
extern 		void Execute_JLE	(DeltaInstruction*, DeltaVirtualMachine*);
extern 		void Execute_JLT	(DeltaInstruction*, DeltaVirtualMachine*);
extern		void Execute_JTRUETEST (DeltaInstruction*, DeltaVirtualMachine*);
extern		void Execute_JFALSETEST (DeltaInstruction*, DeltaVirtualMachine*);

/////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
