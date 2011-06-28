// OperatorOverloading.cpp
// Dynamic operator overloading implementation.
// ScriptFighter Project.
// A. Savidis, July 2005.
// Added type casting explicitly here (from inline code).
// Extended June 2009, unified type casting and assignment and
// provided a simple version not requiring the VM.
//

#include "OperatorOverloading.h"
#include "ustrings.h"
#include "DeltaObject.h"
#include "DeltaExceptionHandling.h"
#include <string>

//-------------------------------------------------------------------

#define	OPERATOR_SUFFIX	"_"
#define	TOTAL_VALID_ARITH_OPERATORS		uarraysize(arithOperators)
#define	TOTAL_VALID_RELAT_OPERATORS		uarraysize(relatOperators)

static const char* arithOperators[] = {
	"+", "-", "*", "/", "%",			// ARITHMETIC
};

static const char* relatOperators[] = {
	"==", "!=", ">", ">=", "<", "<="	// RELATIONAL
};

//-------------------------------------------------------------------

OperatorOverloading::GlobalOperators*	OperatorOverloading::global = (GlobalOperators*) 0;

bool OperatorOverloading::IsSingletonCreated (void) 
	{ return global; }

void OperatorOverloading::SingletonCreate (void) {

	unew(global);

	DASSERT(TOTAL_DELTAOBJECT_ARITHOP_TYPES == uarraysize(Global().arithmetic));
	DASSERT(TOTAL_DELTAOBJECT_RELOP_TYPES	== uarraysize(Global().relational));

	for (util_ui8 i = 0; i < TOTAL_DELTAOBJECT_ARITHOP_TYPES; ++i)
		Global().arithmetic[i].Undefine();

	for (util_ui8 i = 0; i < TOTAL_DELTAOBJECT_RELOP_TYPES; ++i)
		Global().relational[i].Undefine();

	Global().assignment.Undefine();
	Global().typeCasting.Undefine();
}

void OperatorOverloading::SingletonDestroy (void) 
	{ udelete(global); }

//-------------------------------------------------------------------
// Produces the appropriate operator string depending on whether
// the table is the first or the second argument. This applies
// to binay operators.
//

const std::string OperatorOverloading::GetOperatorString (
		const char*	op,
		bool		callerIsSecondArg
	) 
	{ return uconstructstr("%s%s", op, callerIsSecondArg ? OPERATOR_SUFFIX : ""); }

/////////////////////////////////////////////////////////////
// Not finding the operator member conditionally issues 
// a runtime error. In case "." is overloaded, but
// the built-in operators need to be enabled, make their lookup
// returning Nil which is interpreteted as not overloaded.
//
bool OperatorOverloading::GetOperatorMember (
		DeltaValue*				caller,
		const char*				opStr,
		DeltaValue*				func,
		DeltaVirtualMachine*	vm,
		bool					issueError
	) {

	DASSERT(!issueError || vm);
	DeltaValue index;
	index.FromString(opStr);

	DeltaTable* t;
	if (caller->Type() == DeltaValue_Table)
		t = caller->ToTable();
	else
	if (!(t = caller->GetExternIdUserData())) {
		if (issueError)
			DPTR(vm)->PrimaryError(
				"In %s '%s' 0x%x no 'userdata' to get operator slot '%s'!",
				caller->TypeStr(),
				caller->GetExternIdTypeString().c_str(),
				caller->ToExternIdSerialNo(),
				opStr
			);
		return false;
	}

	if (!DPTR(t)->IsOverloadingEnabled()) {
		if (issueError)
			DPTR(vm)->PrimaryError(
				"In %s 0x%x (trying operator '%s') overloading is disabled!",
				DPTR(t)->GetExtClassString(),
				DPTR(t)->GetSerialNo(),
				opStr
			);
		return false;
	}
	else
	if (!DPTR(t)->Get(index, func) || func->Type() == DeltaValue_Nil) {
		if (issueError)
			DPTR(vm)->PrimaryError(
				"In %s 0x%x operator slot '%s' was not found!",
				DPTR(t)->GetExtClassString(),
				DPTR(t)->GetSerialNo(),
				opStr
			);
		return false;
	} 
	else
		return true;
}

/////////////////////////////////////////////////////////////

bool OperatorOverloading::PassArguments (
		DeltaValue*				first,
		DeltaValue*				second,
		DeltaVirtualMachine*	vm
	) {
	DPTR(vm)->PushActualArg(second);
	DPTR(vm)->PushActualArg(first);
	return true;
}

/////////////////////////////////////////////////////////////
// Returns true if a functions is found and called,
// after the call, else posts and error and returns false.
// The case covers functors as overloaders too.
//
bool OperatorOverloading::CallOperatorFunction (
		DeltaValue&				func,
		DeltaVirtualMachine*	vm
	) {

	if (func.IsCallable()) {	
		DPTR(vm)->ExtCallFunction(&func);
		return !DPTR(vm)->HasProducedError();
	}
	else {
		DPTR(vm)->PrimaryError(
			"Operator value of type '%s' is not callable!", 
			func.TypeStr()
		);
		return false;
	}
}

//-------------------------------------------------------------------

#define	TYPECASTING_ASSIGN_INVOKE(ret)										\
	if (true) {																\
		const DeltaValue* args[] = { &rv_copy, lv };						\
		DeltaValue result;													\
		if (!func(args, 2, &result))										\
			return ret##_ExecutionError;									\
		else {																\
			DELTA_ON_UNWINDING_RETURN_TEST_NO_VM(ret##_Exception);			\
			const_cast<DeltaValue*>(rv)->Assign(rv_copy);					\
			if (result.Type() == DeltaValue_Undefined)						\
				return ret##_OverloadedButRequestedRetry;					\
			else {															\
				lv->Assign(result);											\
				return ret##_OverloadedAndSucceeded;						\
			}																\
		}																	\
	} else

//////////////////////////////////////////////////////////////////////

#define	TYPECASTING_ASSIGN(caller, op, ret)									\
	DeltaValue func, rv_copy(rv);											\
	if (GetOperatorMember(const_cast<DeltaValue*>(caller), op, &func)) {	\
		DELTA_ON_UNWINDING_RETURN_TEST_NO_VM(ret##_Exception);				\
		TYPECASTING_ASSIGN_INVOKE(ret);										\
	}																		\
	else																	\
		return ret##_NotOverloaded;

#define	GLOBAL_TYPECASTING_ASSIGN(opslot, ret)								\
	DeltaValue& func = Global().opslot;										\
	if (func.Type() == DeltaValue_Undefined)								\
		return ret##_NotOverloaded;											\
	else {																	\
		DeltaValue rv_copy(rv);												\
		TYPECASTING_ASSIGN_INVOKE(ret);										\
	}

//////////////////////////////////////////////////////////////////////

OperatorOverloading::TypeCastingResult OperatorOverloading::HandleTypeCasting (
		DeltaValue*			lv,
		const DeltaValue*	rv
	) {
	switch (HandleLocalTypeCasting(lv, rv)) {
		case TypeCasting_OverloadedAndSucceeded :	return TypeCasting_OverloadedAndSucceeded;
		case TypeCasting_ExecutionError:			return TypeCasting_ExecutionError;
		case TypeCasting_Exception:					return TypeCasting_Exception;
		case TypeCasting_NotOverloaded:
		case TypeCasting_OverloadedButRequestedRetry:	
			if (HasGlobalTypeCasting())
				return HandleGlobalTypeCasting(lv, rv);
			else
				return TypeCasting_NotOverloaded;
		default: DASSERT(false); return TypeCasting_ExecutionError;
	}
}

//*********************************

OperatorOverloading::ArithAssignResult OperatorOverloading::HandleAssign(
		DeltaValue*			lv,
		const DeltaValue*	rv
	) { 
	switch (HandleLocalAssign(lv, rv)) {
		case ArithAssign_OverloadedAndSucceeded :	return ArithAssign_OverloadedAndSucceeded;
		case ArithAssign_ExecutionError:			return ArithAssign_ExecutionError;
		case ArithAssign_Exception:					return ArithAssign_Exception;
		case ArithAssign_NotOverloaded:
		case TypeCasting_OverloadedButRequestedRetry:	
			if (HasGlobalAssign())
				return HandleGlobalAssign(lv, rv);
			else
				return ArithAssign_NotOverloaded;
		default: DASSERT(false); return ArithAssign_ExecutionError;
	}
}

//////////////////////////////////////////////////////////////////////

OperatorOverloading::TypeCastingResult OperatorOverloading::HandleLocalTypeCasting (
		DeltaValue*			lv,
		const DeltaValue*	rv
	) { TYPECASTING_ASSIGN(rv, DELTA_OPERATOR_OBJECT_TYPECAST, TypeCasting); }

//*********************************

OperatorOverloading::ArithAssignResult OperatorOverloading::HandleLocalAssign(
		DeltaValue*			lv,
		const DeltaValue*	rv
	) { TYPECASTING_ASSIGN(lv, DELTA_OPERATOR_OBJECT_ASSIGN, ArithAssign); }


//////////////////////////////////////////////////////////////////////

bool OperatorOverloading::HasGlobalAssign (void)
	{ return Global().assignment.Type() != DeltaValue_Undefined; }

bool OperatorOverloading::HasGlobalTypeCasting (void)
	{ return Global().typeCasting.Type() != DeltaValue_Undefined; }

//*********************************

OperatorOverloading::ArithAssignResult OperatorOverloading::HandleGlobalAssign (
		DeltaValue*				lv,
		const DeltaValue*		rv
	) { GLOBAL_TYPECASTING_ASSIGN(assignment, ArithAssign); }

OperatorOverloading::TypeCastingResult OperatorOverloading::HandleGlobalTypeCasting (
		DeltaValue*			lv,
		const DeltaValue*		rv
	) { GLOBAL_TYPECASTING_ASSIGN(typeCasting, TypeCasting); }

//*********************************

void OperatorOverloading::SetGlobalTypeCasting (const DeltaValue* func) {
	DASSERT(IsSingletonCreated());
	Global().typeCasting.Assign(func);
}

const DeltaValue& OperatorOverloading::GetGlobalTypeCasting (void) {
	DASSERT(IsSingletonCreated());
	return Global().typeCasting;
}

//*********************************

void OperatorOverloading::SetGlobalAssign (const DeltaValue* func) {
	DASSERT(IsSingletonCreated());
	Global().assignment.Assign(func);
}

const DeltaValue& OperatorOverloading::GetGlobalAssign (void) {
	DASSERT(IsSingletonCreated());
	return Global().assignment;
}

//-------------------------------------------------------------------

#define	PASS_OPERATOR_ARGUMENTS(error)																	\
		if (!(callerIsSecondArg ? PassArguments(arg, caller, vm) : PassArguments(caller, arg, vm)))		\
			return error

/////////////////////////////////////////////////////////////////////

#define	ARITHMETIC_INVOCATION_PRECHECKS()													\
		if (DPTR(vm)->HasProducedError())													\
			return ArithAssign_ExecutionError;												\
		DASSERT(																			\
			ustrpos(																		\
				arithOperators, TOTAL_VALID_ARITH_OPERATORS, op								\
			) != TOTAL_VALID_ARITH_OPERATORS												\
		)
	
/////////////////////////////////////////////////////////////////////

#define	ARITHMETIC_INVOKE_OPERATOR()														\
		if (CallOperatorFunction(func, vm)) {												\
			DELTA_ON_UNWINDING_RETURN_TEST(vm, ArithAssign_Exception);						\
			result->Assign(DPTR(vm)->GetReturnValue());										\
			return	result->Type() != DeltaValue_Undefined ?								\
						ArithAssign_OverloadedAndSucceeded:									\
						ArithAssign_OverloadedButRequestedRetry;							\
		}																					\
		else {																				\
			result->FromNumber(0);															\
			return ArithAssign_ExecutionError;												\
		}

/////////////////////////////////////////////////////////////////////

OperatorOverloading::ArithAssignResult OperatorOverloading::HandleArithmetic (
		const char*				op,
		bool					callerIsSecondArg,
		DeltaValue*				caller,
		DeltaValue*				arg,
		DeltaValue*				result,
		DeltaVirtualMachine*	vm
	) {

	ARITHMETIC_INVOCATION_PRECHECKS();

	DeltaValue func;
	if (GetOperatorMember(caller, GetOperatorString(op, callerIsSecondArg).c_str(), &func, vm)) {
		DELTA_ON_UNWINDING_RETURN_TEST(vm, ArithAssign_Exception);
		PASS_OPERATOR_ARGUMENTS(ArithAssign_ExecutionError);
		ARITHMETIC_INVOKE_OPERATOR();
	}
	else
		return ArithAssign_NotOverloaded;
}

/////////////////////////////////////////////////////////////////////

bool OperatorOverloading::HasGlobalArithmetic (DeltaArithOpType op)
	{ return Global().arithmetic[op].Type() !=  DeltaValue_Undefined; }

OperatorOverloading::ArithAssignResult OperatorOverloading::HandleGlobalArithmetic (
		DeltaArithOpType		op,
		DeltaValue*				left,
		DeltaValue*				right,
		DeltaValue*				result,
		DeltaVirtualMachine*	vm 
	) {

	DASSERT(op >= 0 && op < TOTAL_DELTAOBJECT_ARITHOP_TYPES);

	DeltaValue& func = Global().arithmetic[op];
	if (func.Type() == DeltaValue_Undefined) 
		return ArithAssign_NotOverloaded;
	else {
		PassArguments(left, right, vm);		
		ARITHMETIC_INVOKE_OPERATOR();
	}
}

/////////////////////////////////////////////////////////////////////

void OperatorOverloading::SetGlobalArithmetic (
		DeltaArithOpType	op,
		const DeltaValue*	func
	) {
	DASSERT(IsSingletonCreated());
	DASSERT(op >= 0 && op < TOTAL_DELTAOBJECT_ARITHOP_TYPES);
	Global().arithmetic[op].Assign(func);
}

const DeltaValue& OperatorOverloading::GetGlobalArithmetic (DeltaArithOpType op) {
	DASSERT(IsSingletonCreated());
	DASSERT(op >= 0 && op < TOTAL_DELTAOBJECT_ARITHOP_TYPES);
	return Global().arithmetic[(util_ui8) op];
}

/////////////////////////////////////////////////////////////////////

DeltaArithOpType OperatorOverloading::GetArithmeticOp (const char* opStr) {
	DASSERT(ustrpos(arithOperators, TOTAL_VALID_ARITH_OPERATORS, "+") == DeltaArithOp_ADD);
	DASSERT(ustrpos(arithOperators, TOTAL_VALID_ARITH_OPERATORS, "-") == DeltaArithOp_SUB);
	DASSERT(ustrpos(arithOperators, TOTAL_VALID_ARITH_OPERATORS, "*") == DeltaArithOp_MUL);
	DASSERT(ustrpos(arithOperators, TOTAL_VALID_ARITH_OPERATORS, "/") == DeltaArithOp_DIV);
	DASSERT(ustrpos(arithOperators, TOTAL_VALID_ARITH_OPERATORS, "%") == DeltaArithOp_MOD);
	util_ui16 i = ustrpos(arithOperators, TOTAL_VALID_ARITH_OPERATORS, opStr);
	return i == TOTAL_VALID_ARITH_OPERATORS ? DeltaArithOp_INVALID : (DeltaArithOpType) i;
}

//-------------------------------------------------------------------

#define	RELATIONAL_INVOCATION_PRECHECKS(op)													\
		if (DPTR(vm)->HasProducedError())													\
			return Relational_ExecutionError;												\
		DASSERT(																			\
			ustrpos(																		\
				relatOperators, TOTAL_VALID_RELAT_OPERATORS, op								\
			) != TOTAL_VALID_RELAT_OPERATORS												\
		)
	
/////////////////////////////////////////////////////////////////////

#define	RELATIONAL_INVOKE_OPERATOR(op)														\
		if (CallOperatorFunction(func, vm)) {												\
			DELTA_ON_UNWINDING_RETURN_TEST(vm, Relational_Exception);						\
			DeltaValueType type = DPTR(vm)->GetReturnValue().Type();						\
			if (type == DeltaValue_Undefined)												\
				return Relational_OverloadedButRequestedRetry;								\
			else																			\
			if (type != DeltaValue_Bool) {													\
				DPTR(vm)->PrimaryError(														\
					"Overloaded operator '%s' returned '%s' value (boolean expected)!",		\
					op, DPTR(vm)->GetReturnValue().TypeStr()								\
				);																			\
				return Relational_ExecutionError;											\
			}																				\
			else {																			\
				bool result;																\
				DPTR(vm)->GetReturnValue().ToBool(&result);									\
				return result ? Relational_True : Relational_False;							\
			}																				\
		}																					\
		else																				\
			return Relational_ExecutionError

/////////////////////////////////////////////////////////////////////

OperatorOverloading::RelationalResult OperatorOverloading::HandleRelational (
		const char*				op,
		bool					callerIsSecondArg,
		DeltaValue*				caller,	
		DeltaValue*				arg,
		DeltaVirtualMachine*	vm
	) {

	RELATIONAL_INVOCATION_PRECHECKS(op);
	
	std::string opStr = GetOperatorString(op, callerIsSecondArg);
	DeltaValue func;
	if (!GetOperatorMember(caller, opStr.c_str(), &func, vm))
		return Relational_NotOverloaded;
	else {
		DELTA_ON_UNWINDING_RETURN_TEST(vm, Relational_Exception);
		PASS_OPERATOR_ARGUMENTS(Relational_ExecutionError);
		RELATIONAL_INVOKE_OPERATOR(opStr.c_str());
	}
}

/////////////////////////////////////////////////////////////////////

bool OperatorOverloading::HasGlobalRelational (DeltaRelOpType op)
	{ return Global().relational[op].Type() !=  DeltaValue_Undefined; }

OperatorOverloading::RelationalResult OperatorOverloading::HandleGlobalRelational (
		DeltaRelOpType			op,
		DeltaValue*				left,	
		DeltaValue*				right,
		DeltaVirtualMachine*	vm
	) {

	DASSERT(op >= 0 && op < TOTAL_DELTAOBJECT_RELOP_TYPES);

	DeltaValue& func = Global().relational[op];
	if (func.Type() == DeltaValue_Undefined) 
		return Relational_NotOverloaded;
	else {
		PassArguments(left, right, vm);		
		RELATIONAL_INVOKE_OPERATOR(op);
	}
}

/////////////////////////////////////////////////////////////////////

void OperatorOverloading::SetGlobalRelational (
		DeltaRelOpType		op,
		const DeltaValue*	func
	) {
	DASSERT(IsSingletonCreated());
	DASSERT(op >= 0 && op < TOTAL_DELTAOBJECT_RELOP_TYPES);
	Global().relational[op].Assign(func);
}

const DeltaValue& OperatorOverloading::GetGlobalRelational(DeltaRelOpType op) {
	DASSERT(IsSingletonCreated());
	DASSERT(op >= 0 && op < TOTAL_DELTAOBJECT_RELOP_TYPES);
	return Global().relational[(util_ui8) op];
}

/////////////////////////////////////////////////////////////////////

DeltaRelOpType OperatorOverloading::GetRelationalOp (const char* opStr) {
	DASSERT(ustrpos(relatOperators, TOTAL_VALID_RELAT_OPERATORS, "==") ==  DeltaRelOp_EQ);
	DASSERT(ustrpos(relatOperators, TOTAL_VALID_RELAT_OPERATORS, "!=") ==  DeltaRelOp_NE);
	DASSERT(ustrpos(relatOperators, TOTAL_VALID_RELAT_OPERATORS, ">") ==  DeltaRelOp_GT);
	DASSERT(ustrpos(relatOperators, TOTAL_VALID_RELAT_OPERATORS, ">=") ==  DeltaRelOp_GE);
	DASSERT(ustrpos(relatOperators, TOTAL_VALID_RELAT_OPERATORS, "<") ==  DeltaRelOp_LT);
	DASSERT(ustrpos(relatOperators, TOTAL_VALID_RELAT_OPERATORS, "<=") ==  DeltaRelOp_LE);
	util_ui16 i = ustrpos(relatOperators, TOTAL_VALID_RELAT_OPERATORS, opStr);
	return i == TOTAL_VALID_RELAT_OPERATORS ? DeltaRelOp_INVALID : (DeltaRelOpType) i;
}

//-------------------------------------------------------------------
