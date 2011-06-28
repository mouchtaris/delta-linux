// Arithmetic.cpp
// Execution functions for arithmetic instructions.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Revised July 2005, due to operand translation refactoring.
// Revised May 2009, changed the dispatch table implementation.
//

#include "DDebug.h"
#include "Arithmetic.h"
#include "ArithmeticP.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "DeltaString.h"
#include "DeltaTable.h"
#include "DeltaRuntimeError.h"
#include "OperatorOverloading.h"
#include "DeltaExceptionHandling.h"
#include "Execute.h"

#define	CACHE_ENABLED
//	#define	CACHE_PROFILE

static void					InvokeDefaultDispatcher (ARITHMETIC_FUNC_ARGS);
// TODO: really unused? (nm, 2011/02/03)
//static ArithmeticOp_Func	GetDispatcherWithoutOverloading (DeltaValue* lhs, DeltaValue* rhs);
static void					TryOverloadingWithRhs (ARITHMETIC_FUNC_ARGS);

/////////////////////////////////////////////////////////////////////
// Inline caching with quickening support.

#define	IMPLEMENT_UNCACHED_OPERATION(name, expr)			\
bool DELTAVALUE_OPERATION name (ARITHMETIC_FUNC_ARGS)		\
	{ result->FromNumber( expr); }

#ifdef	CACHE_ENABLED

#ifdef	CACHE_PROFILE
static DeltaCacheProfiler cacheProfiler("Arithmetic");
#define	PROFILE_INSTRUCTION()	cacheProfiler.CacheInstruction()
#define	PROFILE_HIT()			cacheProfiler.CacheHit()
#define	PROFILE_MISS()			cacheProfiler.CacheMiss()
#else
#define	PROFILE_INSTRUCTION()	
#define	PROFILE_HIT()			
#define	PROFILE_MISS()			
#endif

#define	CACHE_ON_INSTRUCTION()	PROFILE_INSTRUCTION()
#define	CACHE_ARGUMENT			, cache
#define	CACHE_SET(f)			UFUNCIN2VOID(f, *cache)
#define	CACHE_MISS()			if (true) { unullify(*cache); PROFILE_MISS(); } else
#define	CACHE_HIT()				PROFILE_HIT()
#define	CACHE_PASS_ARGUMENT		, &(instr->executor)

//********************************

#define	IMPLEMENT_CACHED_OPERATION(name, expr, t1, t2,_precheck)							\
void DELTAVALUE_OPERATION name (ARITHMETIC_FUNC_ARGS) {										\
	_precheck;																				\
	CACHE_SET(name##Cached);																\
	result->FromNumber(expr);																\
}																							\
void DELTAVALUE_OPERATION name##Cached (ARITHMETIC_FUNC_ARGS) {								\
	if (lhs->Type() == DeltaValue_##t1 && rhs->Type() == DeltaValue_##t2)					\
		{ _precheck; CACHE_HIT(); result->FromNumber(expr); }								\
	else {																					\
		CACHE_MISS();																		\
		InvokeDefaultDispatcher(lhs, rhs, result, arithOp, handler, vm CACHE_ARGUMENT);		\
	}																						\
}

#else	// !CACHE_ENABLED

#define	CACHE_ON_INSTRUCTION()	
#define	CACHE_ARGUMENT			, cache
#define	CACHE_PASS_ARGUMENT		, &(instr->executor)

#define	IMPLEMENT_CACHED_OPERATION(name, expr, t1, t2, _precheck)						\
void DELTAVALUE_OPERATION name (ARITHMETIC_FUNC_ARGS) {									\
	_precheck;																			\
	result->FromNumber(expr);															\
}																						\

#endif	// CACHE_ENABLED

//////////////////////////////////////////////////////////////////////

static const char* arithOpStrings[]= {
	"+",
	"-",
	"*",
	"/",
	"%"
};

//////////////////////////////////////////////////////////////////////

static void Error (ARITHMETIC_FUNC_ARGS) 
	{ ARITHMETIC_ERROR(lhs, rhs, arithOp, handler); }

// TODO: really unused? (nm, 2011/02/03)
//static void SwapPositionsError (ARITHMETIC_FUNC_ARGS)
//	{ ARITHMETIC_ERROR(rhs, lhs, arithOp, handler); }

static void TryOverloadingWithRhs (ARITHMETIC_FUNC_ARGS);

//------------------------------------------------------------------

template <class Traits> struct ArithOverloading {

typedef const std::string CS;

void Error (CS& op, CS& left, CS& right, CS& overloader, DeltaVirtualMachine* vm) {

	DELTA_ON_UNWINDING_RETURN_TEST(vm, UNWIND_EMPTY);
	DPTR(vm)->Error(
		"in %s %s %s: overloaded %s but call failed!",
		left.c_str(),
		op.c_str(),
		right.c_str(),
		overloader.c_str()
	);
}

void operator()(
		DeltaValue*						lhs,	
		DeltaValue*						rhs,
		DeltaValue*						result,
		DeltaArithOpType				arithOp,
		bool							isSecond,
		DeltaArithmeticErrorHandler*	handler,
		DeltaVirtualMachine*			vm,
		void**							cache
	) {
	
	switch (OperatorOverloading::HandleArithmetic(arithOpStrings[arithOp], isSecond, lhs, rhs, result, vm)) {

		case OperatorOverloading::ArithAssign_OverloadedAndSucceeded: 
			return;

		case OperatorOverloading::ArithAssign_OverloadedButRequestedRetry	:
		case OperatorOverloading::ArithAssign_NotOverloaded					:

			if (!isSecond) {
				TryOverloadingWithRhs(
					lhs, 
					rhs, 
					result, 
					arithOp, 
					handler, 
					vm
					CACHE_ARGUMENT
				); 
				return; 
			}
			else	// Already tried with the rightmost operand. Swap arguments to original positions.
			if (!OperatorOverloading::HasGlobalArithmetic(arithOp))
					{ ::Error(rhs, lhs, result, arithOp, handler, vm CACHE_ARGUMENT); return; }
			else
				switch (OperatorOverloading::HandleGlobalArithmetic(arithOp, rhs, lhs, result, vm)) {

					case OperatorOverloading::ArithAssign_OverloadedAndSucceeded: 
						return;

					case OperatorOverloading::ArithAssign_OverloadedButRequestedRetry:
						{ ::Error(rhs, lhs, result, arithOp, handler, vm CACHE_ARGUMENT); return; }

					case OperatorOverloading::ArithAssign_ExecutionError:
						{ Error(arithOpStrings[arithOp], rhs->TypeStr(), lhs->TypeStr(), "globally", vm); return; }

					default: 
						{ DASSERT(false); return; }
				}

		case OperatorOverloading::ArithAssign_ExecutionError: {
				Error(
					arithOpStrings[arithOp], 
					Traits::LhsStr(lhs, rhs), 
					Traits::RhsStr(lhs, rhs), 
					Traits::Role(), 
					vm
				); 
				return; 
		}

		case OperatorOverloading::ArithAssign_Exception:
			return;

		default: { DASSERT(false); return; }
	}
}
};

/////////////////////////////////////////////////////////////////////

static void TableWithAny (ARITHMETIC_FUNC_ARGS)  
	{ ArithOverloading<LeftArgBinOpStringTraits>()(lhs, rhs, result, arithOp, false, handler, vm CACHE_ARGUMENT); } 

static void AnyWithTable (ARITHMETIC_FUNC_ARGS)
	{ ArithOverloading<RightArgBinOpStringTraits>()(rhs, lhs, result, arithOp, true, handler, vm CACHE_ARGUMENT); } 

static void ExternIdWithAny (ARITHMETIC_FUNC_ARGS)
	{ ArithOverloading<LeftArgBinOpStringTraits>()(lhs, rhs, result, arithOp, false, handler, vm CACHE_ARGUMENT); } 

static void AnyWithExternId (ARITHMETIC_FUNC_ARGS)
	{ ArithOverloading<RightArgBinOpStringTraits>()(rhs, lhs, result, arithOp, true, handler, vm CACHE_ARGUMENT); } 

//------------------------------------------------------------------

static void NumberNumber (ARITHMETIC_FUNC_ARGS);
static void NumberBool (ARITHMETIC_FUNC_ARGS);

///////////////////////////////////////////////////////////////////
// NUMBER op NUMBER.
//
static ArithmeticOp_Func arithmeticNumberNumber[]={
	DELTAVALUE_OPERATION NumberADDNumber,
	DELTAVALUE_OPERATION NumberSUBNumber,
	DELTAVALUE_OPERATION NumberMULNumber,
	DELTAVALUE_OPERATION NumberDIVNumber,
	DELTAVALUE_OPERATION NumberMODNumber
};

static void NumberNumber (ARITHMETIC_FUNC_ARGS) 
	{ (*arithmeticNumberNumber[arithOp])(lhs, rhs, result, arithOp, handler, vm CACHE_ARGUMENT); }

IMPLEMENT_CACHED_OPERATION(NumberADDNumber, lhs->val.numObj + rhs->val.numObj, Number, Number, UEMPTY)
IMPLEMENT_CACHED_OPERATION(NumberSUBNumber, lhs->val.numObj - rhs->val.numObj, Number, Number, UEMPTY)
IMPLEMENT_CACHED_OPERATION(NumberMULNumber, lhs->val.numObj * rhs->val.numObj, Number, Number, UEMPTY)

#define	DIV_ZERO_CHECK(op)										\
	if (rhs->val.numObj == 0)									\
		{ handler->On##op##ByZero(lhs->val.numObj); return; }

IMPLEMENT_CACHED_OPERATION(
	NumberDIVNumber, 
	lhs->val.numObj / rhs->val.numObj, 
	Number, 
	Number, 
	DIV_ZERO_CHECK(Division)
)
IMPLEMENT_CACHED_OPERATION(
	NumberMODNumber, 
	((util_ui32) lhs->val.numObj) % ((util_ui32) rhs->val.numObj), 
	Number, 
	Number, 
	DIV_ZERO_CHECK(Modulo)
)

///////////////////////////////////////////////////////////////////
// NUMBER op BOOL.
//
static ArithmeticOp_Func arithmeticNumberBool[]={
	DELTAVALUE_OPERATION NumberADDBool,
	DELTAVALUE_OPERATION NumberSUBBool,
	DELTAVALUE_OPERATION NumberMULBool,
	Error,	// Division
	Error	// Modulo
};

static void NumberBool (ARITHMETIC_FUNC_ARGS) {
	(*arithmeticNumberBool[arithOp])(
		lhs, 
		rhs, 
		result, 
		arithOp, 
		handler, 
		vm 
		CACHE_ARGUMENT
	); 
}

IMPLEMENT_CACHED_OPERATION(NumberADDBool, lhs->val.numObj + BOOL2NUM(rhs->val.boolObj), Number, Bool, UEMPTY)
IMPLEMENT_CACHED_OPERATION(NumberSUBBool, lhs->val.numObj - BOOL2NUM(rhs->val.boolObj), Number, Bool, UEMPTY)
IMPLEMENT_CACHED_OPERATION(NumberMULBool, lhs->val.numObj * BOOL2NUM(rhs->val.boolObj), Number, Bool, UEMPTY)

//------------------------------------------------------------------
// STRING + ANY, ANY + STRING (concatenation).
//

#define	STRING_ADD_ANY_ERROR_CHECK()	\
	if (UERROR_ISRAISED())				\
		DPTR(vm)->PossiblePrimaryError("in %s+%s operation!", lhs->TypeStr(), rhs->TypeStr())

#ifdef	CACHE_ENABLED
static void StringAdditionCached (ARITHMETIC_FUNC_ARGS) {
	if (lhs->Type() == DeltaValue_String || rhs->Type() == DeltaValue_String ) {
		CACHE_HIT();
		DASSERT(!UERROR_ISRAISED());
		result->FromString(lhs->ConvertToString() + rhs->ConvertToString());
		STRING_ADD_ANY_ERROR_CHECK();
	}
	else {
		CACHE_MISS();
		InvokeDefaultDispatcher(lhs, rhs, result, arithOp, handler, vm CACHE_ARGUMENT);	
	}
}

static void StringAddition (ARITHMETIC_FUNC_ARGS) {
	CACHE_SET(&StringAdditionCached);
	DASSERT(!UERROR_ISRAISED());
	result->FromString(lhs->ConvertToString() + rhs->ConvertToString());
	STRING_ADD_ANY_ERROR_CHECK();
}
#else
static void StringAddition (ARITHMETIC_FUNC_ARGS) 
	{ result->FromString(lhs->ConvertToString() + rhs->ConvertToString()); STRING_ADD_ANY_ERROR_CHECK(); }
#endif

//////////////////////////////////////////////////////////////////

static ArithmeticOp_Func arithmeticStringAny[] = {
	StringAddition,
	Error,
	Error,
	Error,	// Division
	Error	// Modulo
};

static void StringConcat (ARITHMETIC_FUNC_ARGS) {
	(*arithmeticStringAny[arithOp])(
		lhs, 
		rhs, 
		result, 
		arithOp, 
		handler, 
		vm 
		CACHE_ARGUMENT
	);
}

static void StringConcatBySkippingUserCode (ARITHMETIC_FUNC_ARGS) {
	if (arithOp == DeltaArithOp_ADD)
		result->FromString(lhs->ConvertToStringBySkippingUserCode() + rhs->ConvertToStringBySkippingUserCode());
	else
		ARITHMETIC_ERROR(lhs, rhs, arithOp, handler);
}

//------------------------------------------------------------------
// BOOL

static void BoolBool (ARITHMETIC_FUNC_ARGS);
static void BoolNumber (ARITHMETIC_FUNC_ARGS);

///////////////////////////////////////////////////////////////////
// BOOL op BOOL.
//
static ArithmeticOp_Func arithmeticBoolBool[]={
	DELTAVALUE_OPERATION BoolADDBool,
	DELTAVALUE_OPERATION BoolSUBBool,
	DELTAVALUE_OPERATION BoolMULBool,
	Error,	// Division
	Error	// Modulo
};

static void BoolBool (ARITHMETIC_FUNC_ARGS) {
	(*arithmeticBoolBool[arithOp])(
		lhs, 
		rhs, 
		result, 
		arithOp, 
		handler, 
		vm 
		CACHE_ARGUMENT
	);
}

IMPLEMENT_CACHED_OPERATION(BoolADDBool, BOOL2NUM(lhs->val.boolObj) + BOOL2NUM(rhs->val.boolObj), Bool, Bool, UEMPTY)
IMPLEMENT_CACHED_OPERATION(BoolSUBBool, BOOL2NUM(lhs->val.boolObj) - BOOL2NUM(rhs->val.boolObj), Bool, Bool, UEMPTY)
IMPLEMENT_CACHED_OPERATION(BoolMULBool, BOOL2NUM(lhs->val.boolObj) * BOOL2NUM(rhs->val.boolObj), Bool, Bool, UEMPTY)

///////////////////////////////////////////////////////////////////
// BOOL op NUMBER.

static ArithmeticOp_Func arithmeticBoolNumber[]={
	DELTAVALUE_OPERATION BoolADDNumber,
	DELTAVALUE_OPERATION BoolSUBNumber,
	DELTAVALUE_OPERATION BoolMULNumber,
	Error,	// Division
	Error	// Modulo
};

IMPLEMENT_CACHED_OPERATION(BoolADDNumber, BOOL2NUM(rhs->val.boolObj) + lhs->val.numObj, Bool, Number, UEMPTY)
IMPLEMENT_CACHED_OPERATION(BoolSUBNumber, BOOL2NUM(rhs->val.boolObj) - lhs->val.numObj, Bool, Number, UEMPTY)
IMPLEMENT_CACHED_OPERATION(BoolMULNumber, BOOL2NUM(rhs->val.boolObj) * lhs->val.numObj, Bool, Number, UEMPTY)

static void BoolNumber (ARITHMETIC_FUNC_ARGS) {
	(*arithmeticBoolNumber[arithOp])(
		lhs, 
		rhs, 
		result, 
		arithOp, 
		handler, 
		vm 
		CACHE_ARGUMENT
	);
}

//-----------------------------------------------------------------
// BUILTIN (NON-OVERLOADED CALLS) DISPATCHER.
//
#define	NN						NumberNumber,
#define	NB						NumberBool,
#define	BN						BoolNumber,
#define	BB						BoolBool,
#define	SS						StringConcatBySkippingUserCode,
#define	_						Error,
#define	END_OF_DISPATCHER		(ArithmeticOp_Func) 0

static ArithmeticOp_Func dispatcherWithoutOverloading[] = {

/*					Number	String	Table	Func	Lib		Bool	ExtId	Nil		Method	Undef	*/
/*	Number	*/		NN		SS		_		_		_		NB		_		_		_		_
/*	String	*/		SS		SS		SS		SS		SS		SS		SS		SS		SS		SS
/*	Table	*/		_		SS		_		_		_		_		_		_		_		_
/*	Func	*/		_		SS		_		_		_		_		_		_		_		_
/*	Lib		*/		_		SS		_		_		_		_		_		_		_		_
/*	Bool	*/		BN		SS		_		_		_		BB		_		_		_		_
/*	ExtId	*/		_		SS		_		_		_		_		_		_		_		_
/*	Nil		*/		_		SS		_		_		_		_		_		_		_		_
/*	Method	*/		_		SS		_		_		_		_		_		_		_		_
/*	Undef	*/		_		SS		_		_		_		_		_		_		_		_
					END_OF_DISPATCHER
};

//////////////////////////////////////////////////////////////////////
// Functions needed for the expression evaluator (debugger).
// Those functions use directly objects, hence, they need not be
// part of an instruction set. However, the VM is still used.
//
#define	UNUSED_CACHE						\
		, &unused
#define EXECUTE_BUILTIN_FOR_DEBUGGER(op) \
		EXECUTE_BUILTIN_FOR_DEBUGGER_EX(op, RESULT_DEF, RESULT_PARAM, Arith, Arithmetic, void, void* unused, UNUSED_CACHE)

EXECUTE_BUILTIN_FOR_DEBUGGER(ADD)
EXECUTE_BUILTIN_FOR_DEBUGGER(SUB)
EXECUTE_BUILTIN_FOR_DEBUGGER(MUL)
EXECUTE_BUILTIN_FOR_DEBUGGER(DIV)
EXECUTE_BUILTIN_FOR_DEBUGGER(MOD)

//////////////////////////////////////////////////////////////////////
// Functions needed for the built-in arithmetic operator lib functions.
//
#define BUILTIN_ARITHMETIC(op) \
		BUILTIN_OPERATOR_EX(op, RESULT_DEF, RESULT_PARAM, Arith, Arithmetic, void, void* unused, UNUSED_CACHE)

BUILTIN_ARITHMETIC(ADD)
BUILTIN_ARITHMETIC(SUB)
BUILTIN_ARITHMETIC(MUL)
BUILTIN_ARITHMETIC(DIV)
BUILTIN_ARITHMETIC(MOD)

//-----------------------------------------------------------------
// NORMAL (OVERLOADED CALLS) DISPATCHER.
//

#undef	SS
#define	SS		StringConcat,
#define	TA		TableWithAny,
#define	AT		AnyWithTable,
#define	XA		ExternIdWithAny,
#define	AX		AnyWithExternId,
#define	NEVER	(ArithmeticOp_Func) 0,

//////////////////////////////////////////////////////////////////////
// When we try overloaded operators for objects or extern ids when
// first arg they may fail. So we try if the second arg can do the job.
//
static ArithmeticOp_Func overloadableRetryWithRhs[] = {

/*	Number	*/		_
/*	String	*/		SS
/*	Table	*/		AT
/*	Func	*/		_
/*	Lib		*/		_
/*	Bool	*/		_
/*	ExtId	*/		AX
/*	Nil		*/		NEVER
/*	Method	*/		_
/*	Undef	*/		_
					END_OF_DISPATCHER
};

static void TryOverloadingWithRhs (ARITHMETIC_FUNC_ARGS) {
	(*DNULLCHECK(overloadableRetryWithRhs[rhs->Type()]))(
		lhs, 
		rhs, 
		result, 
		arithOp, 
		handler,
		vm
		CACHE_ARGUMENT
	);
}

////////////////////////////////////////////////////////////////////

static ArithmeticOp_Func dispatcherWithOverloading[] = {

/*					Number	String	Table	Func	Lib		Bool	ExtId	Nil		Method	Undef	*/
/*	Number	*/		NN		SS		AT		_		_		NB		AX		_		_		_
/*	String	*/		SS		SS		SS		SS		SS		SS		SS		SS		SS		SS
/*	Table	*/		TA		SS		TA		TA		TA		TA		TA		_		TA		TA
/*	Func	*/		_		SS		AT		_		_		_		AX		_		_		_
/*	Lib		*/		_		SS		AT		_		_		_		AX		_		_		_
/*	Bool	*/		BN		SS		AT		_		_		BB		AX		_		_		_
/*	ExtId	*/		XA		SS		XA		XA		XA		XA		XA		_		XA		XA
/*	Nil		*/		_		SS		_		_		_		_		_		_		_		_
/*	Method	*/		_		SS		AT		_		_		_		AX		_		_		_
/*	Undef	*/		_		SS		AT		_		_		_		AX		_		_		_
					END_OF_DISPATCHER
};

////////////////////////////////////////////////////////////////////

static void	InvokeDefaultDispatcher (ARITHMETIC_FUNC_ARGS) {
	(*DISPATCHER_WITH_OVERLOADING(lhs, rhs))(
		lhs, 
		rhs, 
		result, 
		arithOp, 
		handler,
		vm
		CACHE_ARGUMENT
	);
}


////////////////////////////////////////////////////////////////////

#define EXECUTE_FOR_VM(op)																	\
void Execute_##op (DeltaInstruction* instr, DeltaVirtualMachine* vm) {						\
	DELTA_TRANSLATE_OPERAND(DELTA_BINARY_ARITH_OPERAND_LEFT, lhs, temp1);					\
	DASSERT(lhs != &DPTR(vm)->GetReturnValue());											\
	DELTA_TRANSLATE_OPERAND(DELTA_BINARY_ARITH_OPERAND_RIGHT, rhs, temp2);					\
	DASSERT(rhs != &DPTR(vm)->GetReturnValue());											\
	static ArithmeticInstructionErrorHandler handler(vm);									\
	handler.Set(vm);																		\
	DeltaValue* lv = DPTR(vm)->GetStorageObject(instr->DELTA_BINARY_ARITH_OPERAND_RESULT);	\
	CACHE_ON_INSTRUCTION();																	\
	ArithmeticOp_Func f	= 0x00;																\
	if (UVOIDIN2FUNC(instr->GetExecutor(), ArithmeticOp_Func, f))							\
		(*f)(																				\
			lhs,																			\
			rhs,																			\
			lv,																				\
			DeltaArithOp_##op,																\
			&handler,																		\
			vm																				\
			CACHE_PASS_ARGUMENT																\
		);																					\
	else {																					\
		DeltaValue result;																	\
		DASSERT(lv && DPTR(vm)->ValidateStackValuePtr(lv));									\
		DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_RETURN_VOID(								\
			(*DISPATCHER_WITH_OVERLOADING(lhs, rhs))(										\
				lhs,																		\
				rhs,																		\
				&result,																	\
				DeltaArithOp_##op,															\
				&handler,																	\
				vm																			\
				CACHE_PASS_ARGUMENT															\
			),																				\
			vm																				\
		);																					\
		if (!DPTR(vm)->HasProducedError() && !EXCEPTION_HANDLERS->IsUnwinding()) {			\
			if (instr->DELTA_BINARY_ARITH_OPERAND_RESULT.IsTempVar())						\
				lv->Assign(result); 														\
			else																			\
				lv->AssignOverloadable(result);												\
		}																					\
	}																						\
}

////////////////////////////////////////////////////////////////////

EXECUTE_FOR_VM(ADD)
EXECUTE_FOR_VM(SUB)
EXECUTE_FOR_VM(MUL)
EXECUTE_FOR_VM(DIV)
EXECUTE_FOR_VM(MOD)

//------------------------------------------------------------------
