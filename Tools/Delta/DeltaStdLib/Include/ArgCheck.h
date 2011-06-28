// ArgCheck.h
// Common argument checking for building library functions.
// Std lib for Delta.
// A. Savidis, Feburary 2000.
//

#ifndef	ARGCHECK_H
#define	ARGCHECK_H
#include "DeltaStdDefs.h"

class DeltaVirtualMachine;
class DeltaValue;

DVM_FUNC DeltaValue*	StringArgCheck (DeltaVirtualMachine* vm, const char* func);
DVM_FUNC DeltaValue*	NumberArgCheck (DeltaVirtualMachine* vm, const char* func);
DVM_FUNC DeltaValue*	ExternIdArgCheck (DeltaVirtualMachine* vm, const char* func);

#endif	// Do not add stuff beyond this point.