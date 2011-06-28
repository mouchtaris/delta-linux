// ArgCheck.cpp
// Common argument checking for building library functions.
// Std lib for Delta.
// A. Savidis, Feburary 2000.
//

#include "DDebug.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "DeltaRuntimeError.h"
#include "ArgCheck.h"

//------------------------------------------------------------------

// Returns the stack first argument, which should be of type
// String, else, returns null.
//
DVM_FUNC DeltaValue* StringArgCheck (DeltaVirtualMachine* vm, const char* func) {

	if (!DPTR(vm)->TotalActualArgs()) {
		DPTR(vm)->PrimaryError("No argument passed in '%s' (String first arg expected)", 
			func
		);
		return (DeltaValue*) 0;
	}

	DeltaValue* obj = vm->GetActualArg(0);

	if (obj->Type() != DeltaValue_String) {
		DPTR(vm)->PrimaryError("String first arg expected ('%s' passed) in '%s'", 
			obj->TypeStr(),
			func
		);
		return (DeltaValue*) 0;
	}

	return obj;
}

//------------------------------------------------------------------

DVM_FUNC DeltaValue* NumberArgCheck (DeltaVirtualMachine* vm, const char* func) {

	if (!DPTR(vm)->TotalActualArgs()) {
		DPTR(vm)->PrimaryError("No argument passed in '%s' (Number first arg expected)", 
			func
		);
		return (DeltaValue*) 0;
	}

	DeltaValue* obj = vm->GetActualArg(0);
	if (obj->Type() != DeltaValue_Number) {
		DPTR(vm)->PrimaryError("Number first arg expected ('%s' passed) in '%s'", 
			obj->TypeStr(),
			func
		);
		return (DeltaValue*) 0;
	}

	return obj;
}

//------------------------------------------------------------------
// Returns the first argument, which should be of type
// ExternId (used for FILE* ptr), else, returns null.
//
DVM_FUNC  DeltaValue* ExternIdArgCheck (DeltaVirtualMachine* vm, const char* func) {

	if (!DPTR(vm)->TotalActualArgs()) {
		DPTR(vm)->PrimaryError("No argument passed in '%s' (ExternId first arg expected)", 
			func
		);
		return (DeltaValue*) 0;
	}

	DeltaValue* obj = vm->GetActualArg(0);
	if (obj->Type() != DeltaValue_ExternId) {
		DPTR(vm)->PrimaryError("ExternId first arg expected ('%s' passed) in '%s'", 
			obj->TypeStr(),
			func
		);
		return (DeltaValue*) 0;
	}

	return obj;
}

//------------------------------------------------------------------