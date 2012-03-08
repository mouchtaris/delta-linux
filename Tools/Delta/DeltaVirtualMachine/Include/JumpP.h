// Jump.h
// Private definitions for jump instructions.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, November 2009 (refactoring).
//

#ifndef	JUMPP_H
#define	JUMPP_H

#include "DeltaStdDefs.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"

//////////////////////////////////////////////////////////////////////
// This is a technique to have two compatible error managers
// that can be used in arithmetic expression evaluation,
// one reporting an error to a callback, and another to the
// running VM, so that the execution functions can be used
// for both debug expression evaluation and instruction execution.
//
class DeltaRelationalErrorHandler {
	public:
	virtual void OnInvalidOperation (const char* type1, const char* op, const char* type2) const = 0;
	DeltaRelationalErrorHandler (void) {}
	virtual ~DeltaRelationalErrorHandler(){}
};

typedef bool (*RelationalOp_Func) (
			DeltaValue*						obj1,
			DeltaValue*						obj2,
			DeltaRelOpType					relOp,
			DeltaRelationalErrorHandler*	handler,
			DeltaVirtualMachine*			vm,
			void**							cache
		);

//////////////////////////////////////////////////////////////////////

class DebugExprRelationalErrorHandler : public DeltaRelationalErrorHandler {

	private:
	void (*handler)(const char*);

	public:
	virtual void OnInvalidOperation (const char* type1, const char* op, const char* type2) const {
		(*handler)(
			uconstructstr(
				"Relational %s %s %s not allowed!", 
				type1, 
				op, 
				type2
			).c_str()
		);
	}

	DebugExprRelationalErrorHandler (void (*_handler)(const char*)) : handler(_handler) {}
	virtual ~DebugExprRelationalErrorHandler(){}
};

//////////////////////////////////////////////////////////////////////

class RelationalInstructionErrorHandler : public DeltaRelationalErrorHandler {
	
	private:
	DeltaVirtualMachine* vm;

	virtual void OnInvalidOperation (const char* type1, const char* op, const char* type2) const {
		DASSERT(vm);
		DPTR(vm)->SetErrorCode(DELTA_RELAT_OPERATOR_ERROR)->PrimaryError(
			"Relational %s %s %s not allowed",
			type1,
			op,
			type2
		);
	}
	
	public:
	void	Set (DeltaVirtualMachine* _vm) { vm = DNULLCHECK(_vm); }
	RelationalInstructionErrorHandler (DeltaVirtualMachine* _vm = (DeltaVirtualMachine*) 0) : vm (_vm){}
	virtual ~RelationalInstructionErrorHandler(){}
};

/////////////////////////////////////////////////////////////

#define	DELTA_INSTRUCTIONS_TAKE_THE_JUMP()											\
		if (true) {																	\
		DASSERT(instr->DELTA_OPERAND_JUMP_TARGET.GetType() == DeltaOperand_Label);	\
		util_ui32 label = instr->DELTA_OPERAND_JUMP_TARGET.GetValue();				\
		DPTR(vm)->pc = label;														\
		DASSERT(vm->IsValidTargetLabel(label));										\
		} else

/////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
