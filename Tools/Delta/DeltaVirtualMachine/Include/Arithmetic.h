// Arithmetic.h
// Implementation of the execution arithmetic instructions.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, November 1999.
//

#ifndef	ARITHMETIC_H
#define	ARITHMETIC_H

#include "DeltaStdDefs.h"
#include "DeltaValue.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include <string>

/////////////////////////////////////////////////////////////
// Functions needed for the expression evaluator (debugger).
// Those functions use directly objects, hence, they need not be
// part of an instruction set. However, the VM is still used.
//
DVM_FUNC  void Execute_ADD (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm,
				void					(*callback)(const char*)
			);
DVM_FUNC  void Execute_SUB (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm,
				void					(*callback)(const char*)
			);
DVM_FUNC  void Execute_DIV (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm,
				void					(*callback)(const char*)
			);
DVM_FUNC  void Execute_MOD (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm,
				void					(*callback)(const char*)
			);
DVM_FUNC  void Execute_MUL (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm,
				void					(*callback)(const char*)
			);

/////////////////////////////////////////////////////////////
// Functions needed for the built-in arithmetic operator library functions.
//
DVM_FUNC  void ExecuteBuiltIn_ADD (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm
			);
DVM_FUNC  void ExecuteBuiltIn_SUB (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm
			);
DVM_FUNC  void ExecuteBuiltIn_DIV (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm
			);
DVM_FUNC  void ExecuteBuiltIn_MOD (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm
			);
DVM_FUNC  void ExecuteBuiltIn_MUL (
				DeltaValue*				arg1, 
				DeltaValue*				arg2, 
				DeltaValue*				result, 
				DeltaVirtualMachine*	vm
			);

/////////////////////////////////////////////////////////////
// Functions for the VM execution loop (not exported).
//
extern void Execute_ADD (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_DIV (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_MOD (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_MUL (DeltaInstruction*, DeltaVirtualMachine*);
extern void Execute_SUB (DeltaInstruction*, DeltaVirtualMachine*);

/////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
