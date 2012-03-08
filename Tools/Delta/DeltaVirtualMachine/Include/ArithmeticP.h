// ArithmeticP.h
// Private definitions for arithmetic instructions.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, November 2009 (refactoring).
//

#ifndef	ARITHMETICP_H
#define	ARITHMETICP_H

#include "DeltaStdDefs.h"
#include "DeltaValue.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "uerrorclass.h"
#include <string>

/////////////////////////////////////////////////////////////

struct BinOpStringTraits {
	static inline const std::string Str (DeltaValue* x)
		{ return x->TypeStr(); }
};

struct LeftArgBinOpStringTraits : public BinOpStringTraits {
	static inline const char* Role (void) { return "LHS"; }
	static inline const std::string LhsStr(DeltaValue* lhs, DeltaValue* rhs) 
		{ return Str(lhs); }
	static inline const std::string RhsStr(DeltaValue* lhs, DeltaValue* rhs) 
		{ return Str(rhs); }
};

struct RightArgBinOpStringTraits : public BinOpStringTraits {
	static inline const char* Role (void) { return "RHS"; }
	static inline const std::string LhsStr(DeltaValue* lhs, DeltaValue* rhs) 
		{ return Str(rhs); }
	static inline const std::string RhsStr(DeltaValue* lhs, DeltaValue* rhs) 
		{ return Str(lhs); }
};

//////////////////////////////////////////////////////////////////////
// This is a technique to have two compatible error managers
// that can be used in arithmetic expression evaluation,
// one reporting an error to a callback, and another to the
// running VM, so that the execution functions can be used
// for both debug expression evaluator and instruction execution.
//

class DeltaArithmeticErrorHandler {
	public:
	virtual void OnDivisionByZero (DeltaNumberValueType number) const = 0;
	virtual void OnModuloByZero (DeltaNumberValueType number) const = 0;
	virtual void OnInvalidOperation (const char* type1, const char* op, const char* type2) const = 0;

	DeltaArithmeticErrorHandler (void) {}
	virtual ~DeltaArithmeticErrorHandler(){}
};

//////////////////////////////////////////////////////////////////////

typedef void (*ArithmeticOp_Func) (
			DeltaValue*,					// Arg1
			DeltaValue*,					// Arg2
			DeltaValue*,					// Result
			DeltaArithOpType,				// Operator
			DeltaArithmeticErrorHandler*,	// Error handler			
			DeltaVirtualMachine*,
			void**							// Cache
		);

//////////////////////////////////////////////////////////////////////

class DebugExprArithmeticErrorHandler : public DeltaArithmeticErrorHandler {

	private:
	void (*handler)(const char*);

	public:
	virtual void OnDivisionByZero (DeltaNumberValueType number) const
		{ (*handler)(uconstructstr("Division of '%f' by zero!", number).c_str()); }

	virtual void OnModuloByZero (DeltaNumberValueType number) const
		{ (*handler)(uconstructstr("Modulo of '%f' by zero!", number).c_str()); }

	virtual void OnInvalidOperation (const char* type1, const char* op, const char* type2) const {
		(*handler)(
			uconstructstr(
				"Arithmetic %s %s %s not allowed!", 
				type1, 
				op, 
				type2
			).c_str()
		);
	}

	DebugExprArithmeticErrorHandler (void (*_handler)(const char*)) : handler(_handler){}
	virtual ~DebugExprArithmeticErrorHandler(){}
};

//////////////////////////////////////////////////////////////////////

class ArithmeticInstructionErrorHandler : public DeltaArithmeticErrorHandler {

	private:
	DeltaVirtualMachine* vm;

	public:
	virtual void OnDivisionByZero (DeltaNumberValueType number) const
		{ DASSERT(vm); DPTR(vm)->SetErrorCode(DELTA_ARITH_OPERATOR_ERROR)->PrimaryError("Division of %f with 0", number); }

	virtual void OnModuloByZero (DeltaNumberValueType number) const
		{ DASSERT(vm); DPTR(vm)->SetErrorCode(DELTA_ARITH_OPERATOR_ERROR)->PrimaryError("Modulo of %f with 0", number); }

	virtual void OnInvalidOperation (const char* type1, const char* op, const char* type2) const {
		DASSERT(vm); 
		DPTR(vm)->SetErrorCode(DELTA_ARITH_OPERATOR_ERROR)->PrimaryError(
			"Arithmetic %s %s %s instruction not allowed",
			type1,
			op,
			type2
		);
	}
	void Set (DeltaVirtualMachine* _vm) { vm = DNULLCHECK(_vm); }
	ArithmeticInstructionErrorHandler (DeltaVirtualMachine* _vm = (DeltaVirtualMachine*) 0) : vm(_vm){}
	virtual ~ArithmeticInstructionErrorHandler(){}
};

//////////////////////////////////////////////////////////////////////

#define	ARITHMETIC_ERROR(lhs, rhs, arithOp, handler)		\
	handler->OnInvalidOperation(							\
		lhs->TypeStr(),										\
		arithOpStrings[arithOp],							\
		rhs->TypeStr()										\
	)

#define	BOOL2NUM(b)						\
	((DeltaNumberValueType) ((b)==true ? 1 : 0))

/////////////////////////////////////////////////////////////

#define	DISPATCHER_WITHOUT_OVERLOADING(x,y)			\
		dispatcherWithoutOverloading[(x)->Type() * TOTAL_DELTAVALUE_TYPES + (y)->Type()]

#define	DISPATCHER_WITH_OVERLOADING(x,y)				\
		dispatcherWithOverloading[(x)->Type() * TOTAL_DELTAVALUE_TYPES + (y)->Type()]

#define	RESULT_DEF						\
		DeltaValue* result,

#define	RESULT_PARAM					\
		result,

//*****************************

#define EXECUTE_BUILTIN_FOR_DEBUGGER_EX(op, res_def, res_parm, op_type, type, ret, cache_decl, cache_use)	\
ret DVM_FUNC Execute_##op (																	\
		DeltaValue*				arg1,														\
		DeltaValue*				arg2,														\
		res_def																				\
		DeltaVirtualMachine*	vm,															\
		void					(*callback)(const char*)									\
	) {																						\
	DebugExpr##type##ErrorHandler handler(callback);										\
	cache_decl;																				\
	return (*DISPATCHER_WITHOUT_OVERLOADING(arg1, arg2))(									\
			arg1,																			\
			arg2,																			\
			res_parm																		\
			Delta##op_type##Op_##op,														\
			&handler,																		\
			vm																				\
			cache_use																		\
		);																					\
}

//*****************************

#define BUILTIN_OPERATOR_EX(op, res_def, res_parm, op_type, type, ret, cache_decl, cache_use)	\
ret DVM_FUNC ExecuteBuiltIn_##op (																\
		DeltaValue*				arg1,															\
		DeltaValue*				arg2,															\
		res_def																					\
		DeltaVirtualMachine*	vm																\
	) {																							\
	type##InstructionErrorHandler handler(vm);													\
	cache_decl;																					\
	return (*DISPATCHER_WITHOUT_OVERLOADING(arg1, arg2))(										\
		arg1,																					\
		arg2,																					\
		res_parm																				\
		Delta##op_type##Op_##op,																\
		&handler,																				\
		vm cache_use																			\
	);																							\
}

/////////////////////////////////////////////////////////////

struct DeltaCacheProfiler {

	util_ui32	total;
	util_ui32	hits;
	util_ui32	misses;
	std::string	unit;

	void	CacheInstruction (void) { ++total; }
	void	CacheHit (void)			{ ++hits; }
	void	CacheMiss (void)		{ ++misses; }

	DeltaCacheProfiler (const std::string& _unit) :
		total	(0),
		hits	(0),
		misses	(0),
		unit	(_unit){}

	~DeltaCacheProfiler() {
		printf(
			"Cache report for '%s': total %u, hits %u, misses %u.\n", 
			unit.c_str(), 
			total, 
			hits, 
			misses
		);
	}
};

/////////////////////////////////////////////////////////////

#endif	// Do not ad stuff beyond this point.
