// TranslateOperand.cpp
// Function to convert from an instruction operand
// to a runtime Delta object.
// The Delta Virtual Machine implementation.
// Script Fighter Project.
// A. Savidis, July 2005 (on major re-factoring).
// Updated September 2010 to support closure vars.
//

#include "DDebug.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaValue.h"
#include "DeltaVirtualMachine.h"
#include "DeltaObject.h"
#include "DeltaClosureHandling.h"
#include "Execute.h"

//-----------------------------------------------------------------------

DeltaValue* DeltaVirtualMachine::GetGlobal (util_ui16 offset) {
	// S[MAX - 1 - OFFSET]
	DASSERT(offset < totalActiveGlobals);
	return globals - offset;
}

//********************************

DeltaValue* DeltaVirtualMachine::GetLocal (util_ui16 offset) {
	// S[TOPSP - OFFSET]
	DASSERT(offset < TotalLocals() && locals == stack + topsp);
	return locals - offset;
}

//********************************

DeltaValue* DeltaVirtualMachine::GetFormal (util_ui16 offset) {

	// S[TOPSP +  ENVSIZE + 1 + OFFSET]
	DASSERT(currFunc && offset < currFunc->GetTotalFormals());
	DASSERT(actuals == (stack + topsp + DELTA_SAVED_ENVIRONMENT_SIZE + 1));

	if (GetTotalActualsForCurrFunction() <= offset) {	// No value for formal arg is supplied in the call.
		PrimaryError(
			"in function '%s' while accessing formal '%s' (no actual value was supplied)!",
			currFunc->GetName().c_str(),
			currFunc->GetFormalName(offset).c_str()
		);		
		return (DeltaValue*) 0;
	}
	else
		return actuals + offset;
}

//////////////////////////////////////////////////////////////////////////

DeltaValue* DeltaVirtualMachine::FromOperandToStorage (const DeltaOperand& src, DeltaValue* dest) {
	DASSERT(src.IsStorageType());
	return (this->*storageConverterFuncs[src.GetType() - DELTA_STORAGE_TYPE_FIRST])(src);
}

//********************************

DeltaValue* DeltaVirtualMachine::FromLocalToStorage (const DeltaOperand& src) 
	{ return GetLocal(src.GetValue()); }

DeltaValue* DeltaVirtualMachine::FromFormalToStorage (const DeltaOperand& src) 
	{ return GetFormal(src.GetValue()); }

DeltaValue* DeltaVirtualMachine::FromGlobalToStorage (const DeltaOperand& src) 
	{ return GetGlobal(src.GetValue()); }

DeltaValue* DeltaVirtualMachine::FromClosureVarToStorage (const DeltaOperand& src) 
	{ return DPTR(DNULLCHECK(CLOSURE_HANDLERS->GetTopClosure()))->GetVar(src.GetValue()); }

//-----------------------------------------------------------------------

#define	FROM_OPERAND_TO_FUNCTION_OR_METHOD(type, opt_self)									\
	const DeltaStdFuncInfo* func = (const DeltaStdFuncInfo*) src.GetValue();				\
	DASSERT(dest && IsValidFuncInfo(func) && func->GetAddress() < DELTA_PC_PROGRAM_END);	\
	DASSERT(code[func->GetAddress()].GetOpCode() == DeltaVM_FUNCENTER);						\
	DeltaClosure* closure = CLOSURE_HANDLERS->MakeClosureIfNeeded(this, func);				\
	if (!closure && HasProducedError())	/* Null closure is legal once no error occured */	\
		return (DeltaValue*) 0;																\
	else  {																					\
		dest->From##type(																	\
			func,																			\
			opt_self																		\
			this,																			\
			closure																			\
		);																					\
		return dest;																		\
	}

#define	NO_SELF_ARGUMENT	
#define	HAS_SELF_ARGUMENT	DNULLCHECK(GetCurrConstructedObject()),

//********************************

DeltaValue* DeltaVirtualMachine::FromOperandToProgramFunc (const DeltaOperand& src, DeltaValue* dest) 
	{ FROM_OPERAND_TO_FUNCTION_OR_METHOD(ProgramFunc, NO_SELF_ARGUMENT); }

DeltaValue* DeltaVirtualMachine::FromOperandToMethodFunc (const DeltaOperand& src, DeltaValue* dest) 
	{ FROM_OPERAND_TO_FUNCTION_OR_METHOD(MethodFunc, HAS_SELF_ARGUMENT); }

//********************************
 // Lib funcs are prebound.
DeltaValue* DeltaVirtualMachine::FromOperandToLibFunc (const DeltaOperand& src, DeltaValue* dest) 
	{ DNULLCHECK(dest)->FromLibraryFunc((DeltaLibraryFunc) src.GetValue()); return dest; }

//********************************

DeltaValue* DeltaVirtualMachine::FromOperandToConstNumber (const DeltaOperand& src, DeltaValue* dest) 
	{ DNULLCHECK(dest)->FromNumber(*((DeltaNumberValueType*) src.GetValue())); return dest; }

//********************************
// Strings are prebound.
DeltaValue* DeltaVirtualMachine::FromOperandToConstString (const DeltaOperand& src, DeltaValue* dest) 
	{ DNULLCHECK(dest)->FromString((const char*) src.GetValue()); return dest; }

//********************************

DeltaValue* DeltaVirtualMachine::FromOperandToConstBool (const DeltaOperand& src, DeltaValue* dest) 
	{ DNULLCHECK(dest)->FromBool(src.GetValue() != 0); return dest; }

//********************************

DeltaValue* DeltaVirtualMachine::FromOperandToConstNil (const DeltaOperand& src, DeltaValue* dest) 
	{ DNULLCHECK(dest)->FromNil(); return dest; }

//********************************

DeltaValue* DeltaVirtualMachine::FromOperandToLabel (const DeltaOperand& src, DeltaValue* dest) 
	{ DASSERT(false); return (DeltaValue*) 0; } // Labels are used as operands directly without translation.

//********************************

DeltaValue* DeltaVirtualMachine::FromOperandToLambda (const DeltaOperand& unused, DeltaValue* dest) {

	const DeltaStdFuncInfo* f = GetCurrFuncInfo();

	DASSERT(
		f										&& 
		IsValidFuncInfo(f)						&& 
		(f->IsMethod() || f->HasClosureVars())	&& 
		f->GetAddress() < DELTA_PC_PROGRAM_END
	);

	if (f->IsMethod())
		DNULLCHECK(dest)->FromMethodFunc(
			f->GetAddress(), 
			GetSelfForCurrMethod(), 
			this,
			const_cast<DeltaClosure*>(CLOSURE_HANDLERS->GetTopClosure())
		);
	else
		DNULLCHECK(dest)->FromProgramFunc(
			f->GetAddress(), 
			this,
			const_cast<DeltaClosure*>(DNULLCHECK(CLOSURE_HANDLERS->GetTopClosure()))
		);
	return dest;
}

//********************************
// In object constructors 'new self' binds to the currently constructed object.
DeltaValue* DeltaVirtualMachine::FromOperandToNewSelf (const DeltaOperand& src, DeltaValue* dest) 
	{ DNULLCHECK(dest)->FromTable(DNULLCHECK(GetCurrConstructedObject())); return dest; }

//********************************

DeltaValue* DeltaVirtualMachine::FromOperandToNotUsed (const DeltaOperand& src, DeltaValue* dest) 
	{ DASSERT(false); return (DeltaValue*) 0; }
	
//////////////////////////////////////////////////////////////////////////

void DeltaVirtualMachine::InstallOperandConverters (void) {

	operandConverterFuncs[DeltaOperand_Label]			= &DeltaVirtualMachine::FromOperandToLabel;		
	operandConverterFuncs[DeltaOperand_Local]			= &DeltaVirtualMachine::FromOperandToStorage;		
	operandConverterFuncs[DeltaOperand_Global]			= &DeltaVirtualMachine::FromOperandToStorage;
	operandConverterFuncs[DeltaOperand_Formal]			= &DeltaVirtualMachine::FromOperandToStorage;		
	operandConverterFuncs[DeltaOperand_ClosureVar]		= &DeltaVirtualMachine::FromOperandToStorage;		
	operandConverterFuncs[DeltaOperand_ProgramFunc]		= &DeltaVirtualMachine::FromOperandToProgramFunc;	
	operandConverterFuncs[DeltaOperand_LibFunc]			= &DeltaVirtualMachine::FromOperandToLibFunc;	
	operandConverterFuncs[DeltaOperand_MethodFunc]		= &DeltaVirtualMachine::FromOperandToMethodFunc;	
	operandConverterFuncs[DeltaOperand_ConstString] 	= &DeltaVirtualMachine::FromOperandToConstString;
	operandConverterFuncs[DeltaOperand_ConstNumber] 	= &DeltaVirtualMachine::FromOperandToConstNumber;
	operandConverterFuncs[DeltaOperand_ConstBool]		= &DeltaVirtualMachine::FromOperandToConstBool;	
	operandConverterFuncs[DeltaOperand_ConstNil]		= &DeltaVirtualMachine::FromOperandToConstNil;	
	operandConverterFuncs[DeltaOperand_Lambda]			= &DeltaVirtualMachine::FromOperandToLambda;
	operandConverterFuncs[DeltaOperand_NewSelf]			= &DeltaVirtualMachine::FromOperandToNewSelf;
	operandConverterFuncs[DeltaOperand_NotUsed]			= &DeltaVirtualMachine::FromOperandToNotUsed;
	DASSERT(uarraysize(operandConverterFuncs) > DeltaOperand_NotUsed);

	storageConverterFuncs[DeltaOperand_Local - DELTA_STORAGE_TYPE_FIRST]		= &DeltaVirtualMachine::FromLocalToStorage;
	storageConverterFuncs[DeltaOperand_Formal - DELTA_STORAGE_TYPE_FIRST]		= &DeltaVirtualMachine::FromFormalToStorage;
	storageConverterFuncs[DeltaOperand_Global - DELTA_STORAGE_TYPE_FIRST]		= &DeltaVirtualMachine::FromGlobalToStorage;
	storageConverterFuncs[DeltaOperand_ClosureVar - DELTA_STORAGE_TYPE_FIRST]	= &DeltaVirtualMachine::FromClosureVarToStorage;

	DASSERT(uarraysize(storageConverterFuncs) > (DeltaOperand_ClosureVar - DELTA_STORAGE_TYPE_FIRST));
}

//********************************

DeltaValue* DeltaVirtualMachine::GetStorageObject (const DeltaOperand& src) 
	{ DASSERT(src.IsStorageType()); return FromOperandToStorage(src, (DeltaValue*) 0); }

//////////////////////////////////////////////////////////////////////////

DeltaValue* DeltaVirtualMachine::TranslateOperand (const DeltaOperand& src, DeltaValue* dest) {
	DASSERT(src.GetType() < DELTA_TOTAL_OPERAND_TYPES);
	return (this->*operandConverterFuncs[src.GetType()])(src, dest);
}

//////////////////////////////////////////////////////////////////////////
