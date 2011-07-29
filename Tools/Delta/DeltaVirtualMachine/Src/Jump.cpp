// Jump.cpp
// Implementation of the execution logic for conditional JUMP
// jumps statements.
// Delta VirtualMachine implementation.
// ScriptFighter Project.
// A. Savidis, November 1999.
// Updated July 2005, refactoring in operand translation.
// Extended July 2005 to include operator overloading.
// Extended March 2009 to support overloading for externids too.
// TODO:
// 1) in JUMP add macros for the arg1 
// 2) a profiling technique (uprofiler with array of util_ui32, indexed
// with logical macros). then an instance for the entire vm package (incocations
// in it with #ifdefs).
//

#include <string.h>
#include "DDebug.h"
#include "Jump.h"
#include "JumpP.h"
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

static bool					InvokeDefaultDispatcher (RELATIONAL_FUNC_ARGS);
static RelationalOp_Func	GetDispatcherWithoutOverloading (DeltaValue* lhs, DeltaValue* rhs);
static bool					TryOverloadingWithRhs (RELATIONAL_FUNC_ARGS);

/////////////////////////////////////////////////////////////////////
// Inline caching with quickening support.

#define	IMPLEMENT_UNCACHED_OPERATION(name, expr)											\
bool DELTAVALUE_OPERATION name (RELATIONAL_FUNC_ARGS)										\
	{ return expr; }

#ifdef	CACHE_ENABLED

#ifdef	CACHE_PROFILE
static DeltaCacheProfiler cacheProfiler("Jump");
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

#define	IMPLEMENT_CACHED_OPERATION(name, expr, t1, t2)										\
bool DELTAVALUE_OPERATION name (RELATIONAL_FUNC_ARGS) {										\
	CACHE_SET(& name##Cached);																\
	return expr;																			\
}																							\
bool DELTAVALUE_OPERATION name##Cached (RELATIONAL_FUNC_ARGS) {								\
	if (lhs->Type() == DeltaValue_##t1 && rhs->Type() == DeltaValue_##t2)					\
		{ CACHE_HIT(); return expr;	}														\
	else {																					\
		CACHE_MISS();																		\
		return InvokeDefaultDispatcher(lhs, rhs, relOp, handler, vm CACHE_ARGUMENT);		\
	}																						\
}

#else	// !CACHE_ENABLED

#define	CACHE_ON_INSTRUCTION()	
#define	CACHE_ARGUMENT			, cache
#define	CACHE_PASS_ARGUMENT		, &(instr->executor)

#define	IMPLEMENT_CACHED_OPERATION(name, expr, t1, t2)									\
bool DELTAVALUE_OPERATION name (RELATIONAL_FUNC_ARGS) {									\
	return expr;																		\
}																						\

#endif	// CACHE_ENABLED

/////////////////////////////////////////////////////////////////////

void Execute_JUMP (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	DELTA_INSTRUCTIONS_TAKE_THE_JUMP();	
	if (instr->DELTA_JUMP_OPERAND_EXIT_BLOCKS_TOTAL.GetType() != DeltaOperand_NotUsed) {
		DASSERT(instr->DELTA_JUMP_OPERAND_EXIT_BLOCKS_TOTAL.IsInternal());
		DPTR(vm)->DoExitMultipleBlocks((util_ui16) instr->DELTA_JUMP_OPERAND_EXIT_BLOCKS_TOTAL.GetValue());
	}
}

/////////////////////////////////////////////////////////////////////

static const char* relOpStrings[] = {
	"==",
	"!=",
	">",
	">=",
	"<",
	"<="
};

/////////////////////////////////////////////////////////////////////
// Some functions that are very common in the various dispatch tables.

static bool Error (RELATIONAL_FUNC_ARGS)
	{ handler->OnInvalidOperation(lhs->TypeStr(), relOpStrings[relOp], rhs->TypeStr()); return false; }

static bool ReturnTrue (RELATIONAL_FUNC_ARGS) 
	{ return true; }

static bool ReturnFalse (RELATIONAL_FUNC_ARGS) 
	{ return false; }

//////////////////////////////////////////////////////////////////////

template <class Traits> struct RelationalOverloading {

typedef const std::string CS;

bool Error (CS& op, CS& left, CS& right, CS& overloader, DeltaVirtualMachine* vm) {
	DELTA_ON_UNWINDING_RETURN_TEST(vm, false);
	DPTR(vm)->Error(
		"in %s %s %s: overloaded %s but call failed!",
		left.c_str(),
		op.c_str(),
		right.c_str(),
		overloader.c_str()
	);
	return false;
}

//********************************

bool operator()(RELATIONAL_FUNC_ARGS, bool isSecond, RelationalOp_Func altHandler) {

	DASSERT(lhs->Type() != DeltaValue_Nil && rhs->Type() != DeltaValue_Nil);

	switch (OperatorOverloading::HandleRelational(relOpStrings[relOp], isSecond, lhs, rhs, vm)) {

		case OperatorOverloading::Relational_OverloadedButRequestedRetry	:
		case OperatorOverloading::Relational_NotOverloaded					:

			if (!isSecond)	// Can still try overloading on the second arg.
				return	(*altHandler)(
							lhs, 
							rhs, 
							relOp, 
							handler, 
							vm 
							CACHE_ARGUMENT
						);
			else	// We try the global operator, and we swap argument positions as they were reverted.
			if (!OperatorOverloading::HasGlobalRelational(relOp))
				return	(*altHandler)(
							rhs, 
							lhs, 
							relOp, 
							handler, 
							vm 
							CACHE_ARGUMENT
						);
			else
				switch (OperatorOverloading::HandleGlobalRelational(relOp, rhs, lhs, vm)) {

					case OperatorOverloading::Relational_True:	
						return true;

					case OperatorOverloading::Relational_False:	
						return false;

					case OperatorOverloading::Relational_OverloadedButRequestedRetry:
						return	(*altHandler)(
									rhs, 
									lhs, 
									relOp, 
									handler, 
									vm 
									CACHE_ARGUMENT
								);

					case OperatorOverloading::Relational_ExecutionError: 
						return	Error(
									relOpStrings[relOp], 
									rhs->TypeStr(), 
									lhs->TypeStr(), 
									"globally", 
									vm
								);
					default: 
						{ DASSERT(false); return false; }
				}

		case OperatorOverloading::Relational_ExecutionError : 
			return	Error(
						relOpStrings[relOp], 
						Traits::LhsStr(lhs, rhs), 
						Traits::RhsStr(lhs, rhs), 
						Traits::Role(), 
						vm
					);

		case OperatorOverloading::Relational_Exception : 
			return false;

		case OperatorOverloading::Relational_True:	
			return true;

		case OperatorOverloading::Relational_False:	
			return false;

		default: { DASSERT(false); return false; }
	}
}
};

//------------------------------------------------------------------
// TABLE.

static bool TableTable (RELATIONAL_FUNC_ARGS);
static bool TableBool (RELATIONAL_FUNC_ARGS);
static bool TableWithAny (RELATIONAL_FUNC_ARGS);
static bool AnyWithTable (RELATIONAL_FUNC_ARGS);

//////////////////////////////////////////////////////////////////////
// TABLE op TABLE.
//
static RelationalOp_Func relationalTableTable[]={
	DELTAVALUE_OPERATION TableEQTable,	// ==
	DELTAVALUE_OPERATION TableNETable,	// !=
	Error,								// >
	Error,								// >=
	Error,								// <
	Error								// <=
};

static bool TableTable (RELATIONAL_FUNC_ARGS)
	{ return (*relationalTableTable[relOp])(lhs, rhs, relOp, handler, vm CACHE_ARGUMENT); }

//*******************************
// Equality test is applied only by reference.

bool DELTAVALUE_OPERATION TableEQTable (RELATIONAL_FUNC_ARGS) 
	{ return lhs->val.tableObj == rhs->val.tableObj; }

bool DELTAVALUE_OPERATION TableNETable (RELATIONAL_FUNC_ARGS) 
	{ return lhs->val.tableObj != rhs->val.tableObj; }

//////////////////////////////////////////////////////////////////////
// TABLE op Bool. TRUE TEST.
//
static RelationalOp_Func relationalTableBool[]={
	DELTAVALUE_OPERATION TableEQBool,	// ==
	DELTAVALUE_OPERATION TableNEBool,	// !=
	Error,								// >
	Error,								// >=
	Error,								// <
	Error								// <=
};

static bool TableBool (RELATIONAL_FUNC_ARGS) {
	return (*relationalTableBool[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_UNCACHED_OPERATION(TableEQBool, rhs->val.boolObj)
IMPLEMENT_UNCACHED_OPERATION(TableNEBool, !rhs->val.boolObj)

//////////////////////////////////////////////////////////////////////

static bool TableWithAny (RELATIONAL_FUNC_ARGS)  {
	return	RelationalOverloading<LeftArgBinOpStringTraits>()(
				lhs, rhs, relOp, handler, vm CACHE_ARGUMENT, false, &TryOverloadingWithRhs 
			); 
} 

static bool AnyWithTable (RELATIONAL_FUNC_ARGS) { 
	return	RelationalOverloading<RightArgBinOpStringTraits>()(
				rhs, lhs, relOp, handler, vm CACHE_ARGUMENT, true, GetDispatcherWithoutOverloading(lhs, rhs)
			); 
} 

//-------------------------------------------------------------------
// NUMBER.

static bool NumberNumber (RELATIONAL_FUNC_ARGS);
static bool NumberBool (RELATIONAL_FUNC_ARGS);

//////////////////////////////////////////////////////////////////////
// NUMBER op NUMBER.
//
static RelationalOp_Func relationalNumberNumber[]={
	DELTAVALUE_OPERATION NumberEQNumber,	// ==
	DELTAVALUE_OPERATION NumberNENumber,	// !=
	DELTAVALUE_OPERATION NumberGTNumber,	// >
	DELTAVALUE_OPERATION NumberGENumber,	// >=
	DELTAVALUE_OPERATION NumberLTNumber,	// <
	DELTAVALUE_OPERATION NumberLENumber		// <=
};

static bool NumberNumber (RELATIONAL_FUNC_ARGS) {
	return (*relationalNumberNumber[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_CACHED_OPERATION(NumberEQNumber, lhs->val.numObj  == rhs->val.numObj, Number, Number)
IMPLEMENT_CACHED_OPERATION(NumberNENumber, lhs->val.numObj  != rhs->val.numObj, Number, Number)
IMPLEMENT_CACHED_OPERATION(NumberGTNumber, lhs->val.numObj  >  rhs->val.numObj, Number, Number)
IMPLEMENT_CACHED_OPERATION(NumberGENumber, lhs->val.numObj  >= rhs->val.numObj, Number, Number)
IMPLEMENT_CACHED_OPERATION(NumberLTNumber, lhs->val.numObj  <  rhs->val.numObj, Number, Number)
IMPLEMENT_CACHED_OPERATION(NumberLENumber, lhs->val.numObj  <= rhs->val.numObj, Number, Number)

//////////////////////////////////////////////////////////////////////
// NUMBER op BOOL.
//

static RelationalOp_Func relationalNumberBool[]={
	DELTAVALUE_OPERATION NumberEQBool,	// ==
	DELTAVALUE_OPERATION NumberNEBool,	// !=
	Error,								// >
	Error,								// >=
	Error,								// <
	Error								// <=
};

static bool NumberBool (RELATIONAL_FUNC_ARGS) {
	return (*relationalNumberBool[relOp])(
		lhs, 
		rhs, 
		relOp, 
		handler, 
		vm
		CACHE_ARGUMENT
	);
}

IMPLEMENT_CACHED_OPERATION(NumberEQBool, rhs->val.boolObj == (lhs->val.numObj != 0), Number, Bool)
IMPLEMENT_CACHED_OPERATION(NumberNEBool, rhs->val.boolObj == (lhs->val.numObj == 0), Number, Bool)

//-------------------------------------------------------------------
// STRING.

static bool StringString (RELATIONAL_FUNC_ARGS);
static bool StringBool (RELATIONAL_FUNC_ARGS);

//////////////////////////////////////////////////////////////////////
// STRING op STRING.
//

static RelationalOp_Func relationalStringString[]={
	DELTAVALUE_OPERATION StringEQString,	// ==
	DELTAVALUE_OPERATION StringNEString,	// !=
	DELTAVALUE_OPERATION StringGTString,	// >
	DELTAVALUE_OPERATION StringGEString,	// >=
	DELTAVALUE_OPERATION StringLTString,	// <
	DELTAVALUE_OPERATION StringLEString		// <=
};

static bool StringString (RELATIONAL_FUNC_ARGS) {
	return (*relationalStringString[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_CACHED_OPERATION(StringEQString, DPTR(lhs->val.strObj)->Content() == DPTR(rhs->val.strObj)->Content(), String, String)
IMPLEMENT_CACHED_OPERATION(StringNEString, DPTR(lhs->val.strObj)->Content() != DPTR(rhs->val.strObj)->Content(), String, String)
IMPLEMENT_CACHED_OPERATION(StringGTString, DPTR(lhs->val.strObj)->Content() > DPTR(rhs->val.strObj)->Content(), String, String)
IMPLEMENT_CACHED_OPERATION(StringGEString, DPTR(lhs->val.strObj)->Content() >= DPTR(rhs->val.strObj)->Content(), String, String)
IMPLEMENT_CACHED_OPERATION(StringLTString, DPTR(lhs->val.strObj)->Content() < DPTR(rhs->val.strObj)->Content(), String, String)
IMPLEMENT_CACHED_OPERATION(StringLEString, DPTR(lhs->val.strObj)->Content() <= DPTR(rhs->val.strObj)->Content(), String, String)

//////////////////////////////////////////////////////////////////////
// STRING op BOOL.
//

static RelationalOp_Func relationalStringBool[]={
	DELTAVALUE_OPERATION StringEQBool,	// ==
	DELTAVALUE_OPERATION StringNEBool,	// !=
	Error,								// >
	Error,								// >=
	Error,								// <
	Error								// <=
};

static bool StringBool (RELATIONAL_FUNC_ARGS) {
	return (*relationalStringBool[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_CACHED_OPERATION(
	StringEQBool, 
	rhs->val.boolObj == (DPTR(lhs->val.strObj)->Length() != 0), 
	String, 
	Bool
)
IMPLEMENT_CACHED_OPERATION(
	StringNEBool, 
	rhs->val.boolObj == !DPTR(lhs->val.strObj)->Length(), 
	String, 
	Bool
)

//-------------------------------------------------------------------
// ProgramFunc.

static bool  ProgramFuncProgramFunc (RELATIONAL_FUNC_ARGS);
static bool  ProgramFuncBool (RELATIONAL_FUNC_ARGS);

//////////////////////////////////////////////////////////////////////
// ProgramFunc op ProgramFunc.
//

static RelationalOp_Func relationalProgramFuncProgramFunc[]={
	DELTAVALUE_OPERATION ProgramFuncEQProgramFunc,	// ==
	DELTAVALUE_OPERATION ProgramFuncNEProgramFunc,	// !=
	Error,											// >
	Error,											// >=
	Error,											// <
	Error											// <=
};

static bool ProgramFuncProgramFunc (RELATIONAL_FUNC_ARGS) {
	return (*relationalProgramFuncProgramFunc[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_CACHED_OPERATION(
	ProgramFuncEQProgramFunc, 
	lhs->val.programFuncObj.func	== rhs->val.programFuncObj.func &&
	lhs->val.programFuncObj.vm		== rhs->val.programFuncObj.vm, 
	ProgramFunc, 
	ProgramFunc
)

IMPLEMENT_CACHED_OPERATION(
	ProgramFuncNEProgramFunc, 
	lhs->val.programFuncObj.func	!= rhs->val.programFuncObj.func ||
	lhs->val.programFuncObj.vm		!= rhs->val.programFuncObj.vm, 
	ProgramFunc, 
	ProgramFunc
)

//////////////////////////////////////////////////////////////////////
// ProgramFunc op Bool.
//
static RelationalOp_Func relationalProgramFuncBool[]={
	DELTAVALUE_OPERATION ProgramFuncEQBool,	// ==
	DELTAVALUE_OPERATION ProgramFuncNEBool,	// !=
	Error,									// >
	Error,									// >=
	Error,									// <
	Error									// <=
};

static bool  ProgramFuncBool (RELATIONAL_FUNC_ARGS) {
	return (*relationalProgramFuncBool[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_CACHED_OPERATION(ProgramFuncEQBool, rhs->val.boolObj, ProgramFunc, Bool)
IMPLEMENT_CACHED_OPERATION(ProgramFuncNEBool, !rhs->val.boolObj, ProgramFunc, Bool)

//-------------------------------------------------------------------
// LibFunc.

static bool LibraryFuncLibraryFunc (RELATIONAL_FUNC_ARGS);
static bool LibraryFuncBool (RELATIONAL_FUNC_ARGS);

//////////////////////////////////////////////////////////////////////
// LibFunc op LibFunc.
//
static RelationalOp_Func relationalLibraryFuncLibraryFunc[]={
	DELTAVALUE_OPERATION LibraryFuncEQLibraryFunc,	// ==
	DELTAVALUE_OPERATION LibraryFuncNELibraryFunc,	// !=
	Error,											// >
	Error,											// >=
	Error,											// <
	Error											// <=
};

static bool LibraryFuncLibraryFunc (RELATIONAL_FUNC_ARGS) {
	return (*relationalLibraryFuncLibraryFunc[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_CACHED_OPERATION(
	LibraryFuncEQLibraryFunc, 
	lhs->val.libraryFuncObj.f == rhs->val.libraryFuncObj.f, 
	LibraryFunc, 
	LibraryFunc
)
IMPLEMENT_CACHED_OPERATION(
	LibraryFuncNELibraryFunc, 
	lhs->val.libraryFuncObj.f != rhs->val.libraryFuncObj.f, 
	LibraryFunc, 
	LibraryFunc
)

//////////////////////////////////////////////////////////////////////
// LibFunc op Bool.
//
static RelationalOp_Func relationalLibraryFuncBool[]={
	DELTAVALUE_OPERATION LibraryFuncEQBool,	// ==
	DELTAVALUE_OPERATION LibraryFuncNEBool,	// !=
	Error,									// >
	Error,									// >=
	Error,									// <
	Error									// <=
};

static bool LibraryFuncBool (RELATIONAL_FUNC_ARGS) {
	return (*relationalLibraryFuncBool[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_CACHED_OPERATION(LibraryFuncEQBool, rhs->val.boolObj, LibraryFunc, Bool)
IMPLEMENT_CACHED_OPERATION(LibraryFuncNEBool, !rhs->val.boolObj, LibraryFunc, Bool)

//-------------------------------------------------------------------
// Bool.

IMPLEMENT_CACHED_OPERATION(BoolEQNumber, lhs->val.boolObj == (rhs->val.numObj != 0), Bool, Number)
IMPLEMENT_CACHED_OPERATION(BoolNENumber, lhs->val.boolObj == (rhs->val.numObj == 0), Bool, Number)
IMPLEMENT_CACHED_OPERATION(BoolEQString, lhs->val.boolObj == (DPTR(rhs->val.strObj)->Length() != 0), Bool, String)
IMPLEMENT_CACHED_OPERATION(BoolNEString, lhs->val.boolObj == !DPTR(rhs->val.strObj)->Length(), Bool, String)
IMPLEMENT_UNCACHED_OPERATION(BoolEQTable, lhs->val.boolObj)
IMPLEMENT_UNCACHED_OPERATION(BoolNETable, !lhs->val.boolObj)
IMPLEMENT_CACHED_OPERATION(BoolEQProgramFunc, lhs->val.boolObj, Bool, ProgramFunc)
IMPLEMENT_CACHED_OPERATION(BoolNEProgramFunc, !lhs->val.boolObj, Bool, ProgramFunc)
IMPLEMENT_CACHED_OPERATION(BoolEQLibraryFunc, lhs->val.boolObj, Bool, LibraryFunc)
IMPLEMENT_CACHED_OPERATION(BoolNELibraryFunc, !lhs->val.boolObj, Bool, LibraryFunc)
IMPLEMENT_CACHED_OPERATION(BoolEQBool, lhs->val.boolObj == rhs->val.boolObj, Bool, Bool)
IMPLEMENT_CACHED_OPERATION(BoolNEBool, lhs->val.boolObj != rhs->val.boolObj, Bool, Bool)
IMPLEMENT_UNCACHED_OPERATION(BoolEQExternId, lhs->val.boolObj == rhs->val.externIdObj->ToBool())
IMPLEMENT_UNCACHED_OPERATION(BoolNEExternId, lhs->val.boolObj != rhs->val.externIdObj->ToBool())
IMPLEMENT_CACHED_OPERATION(BoolEQNil, false, Bool, Nil)
IMPLEMENT_CACHED_OPERATION(BoolNENil, true, Bool, Nil)
IMPLEMENT_CACHED_OPERATION(NilEQBool, false, Nil, Bool)
IMPLEMENT_CACHED_OPERATION(NilNEBool, true, Nil, Bool)
IMPLEMENT_CACHED_OPERATION(BoolEQMethodFunc, lhs->val.boolObj, Bool, MethodFunc)
IMPLEMENT_CACHED_OPERATION(BoolNEMethodFunc, !lhs->val.boolObj, Bool, MethodFunc)
IMPLEMENT_CACHED_OPERATION(BoolEQUndef, !lhs->val.boolObj, Bool, Undefined)
IMPLEMENT_CACHED_OPERATION(BoolNEUndef, lhs->val.boolObj, Bool, Undefined)
IMPLEMENT_CACHED_OPERATION(UndefEQBool, !rhs->val.boolObj, Undefined, Bool)
IMPLEMENT_CACHED_OPERATION(UndefNEBool, rhs->val.boolObj, Undefined, Bool)

#define	ERR					Error,
#define	OP					DELTAVALUE_OPERATION 
#define	END_OF_DISPATCHER	(RelationalOp_Func) 0

static RelationalOp_Func relationalBoolAny[] = {
//					==						!=						>		<		>=		<=
/*	Number	*/		OP BoolEQNumber,		OP BoolNENumber,		ERR		ERR		ERR		ERR	
/*	String	*/		OP BoolEQString,		OP BoolNEString,		ERR		ERR		ERR		ERR	
/*	Table	*/		OP BoolEQTable,			OP BoolNETable,			ERR		ERR		ERR		ERR	
/*	Func	*/		OP BoolEQProgramFunc,	OP BoolNEProgramFunc,	ERR		ERR		ERR		ERR	
/*	Lib		*/		OP BoolEQLibraryFunc,	OP BoolNELibraryFunc,	ERR		ERR		ERR		ERR	
/*	Bool	*/		OP BoolEQBool,			OP BoolNEBool,			ERR		ERR		ERR		ERR	
/*	ExtId	*/		OP BoolEQExternId,		OP BoolNEExternId,		ERR		ERR		ERR		ERR	
/*	Nil		*/		OP BoolEQNil,			OP BoolNENil,			ERR		ERR		ERR		ERR	
/*	Method	*/		OP BoolEQMethodFunc,	OP BoolNEMethodFunc,	ERR		ERR		ERR		ERR	
/*	Undef	*/		OP BoolEQUndef,			OP BoolNEUndef,			ERR		ERR		ERR		ERR 
					END_OF_DISPATCHER
};

static bool BoolAny (RELATIONAL_FUNC_ARGS) {
	return (*relationalBoolAny[rhs->Type() * TOTAL_DELTAOBJECT_RELOP_TYPES + relOp])(
				lhs,
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

/////////////////////////////////////////////////////////////////////

static RelationalOp_Func relationalAnyBool[] = {
//					==						!=						>		<		>=		<=
/*	Number	*/		OP NumberEQBool,		OP NumberNEBool,		ERR		ERR		ERR		ERR	
/*	String	*/		OP StringEQBool,		OP StringNEBool,		ERR		ERR		ERR		ERR	
/*	Table	*/		OP TableEQBool,			OP TableNEBool,			ERR		ERR		ERR		ERR	
/*	Func	*/		OP ProgramFuncEQBool,	OP ProgramFuncNEBool,	ERR		ERR		ERR		ERR	
/*	Lib		*/		OP LibraryFuncEQBool,	OP LibraryFuncNEBool,	ERR		ERR		ERR		ERR	
/*	Bool	*/		OP BoolEQBool,			OP BoolNEBool,			ERR		ERR		ERR		ERR	
/*	ExtId	*/		OP ExternIdEQBool,		OP ExternIdNEBool,		ERR		ERR		ERR		ERR	
/*	Nil		*/		OP NilEQBool,			OP NilNEBool,			ERR		ERR		ERR		ERR	
/*	Method	*/		OP MethodFuncEQBool,	OP MethodFuncNEBool,	ERR		ERR		ERR		ERR	
/*	Undef	*/		OP UndefEQBool,			OP UndefNEBool,			ERR		ERR		ERR		ERR 
					END_OF_DISPATCHER
};

static bool  AnyBool (RELATIONAL_FUNC_ARGS) {
	return (*relationalAnyBool[lhs->Type()* TOTAL_DELTAOBJECT_RELOP_TYPES + relOp])(
				lhs,
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

//-------------------------------------------------------------------
// ExternId.

static bool ExternIdExternId (RELATIONAL_FUNC_ARGS);
static bool ExternIdBool (RELATIONAL_FUNC_ARGS);
static bool ExternIdWithAny (RELATIONAL_FUNC_ARGS);
static bool AnyWithExternId (RELATIONAL_FUNC_ARGS);

//////////////////////////////////////////////////////////////////////
// ExternId op ExternId.
//
static RelationalOp_Func relationalExternIdExternId[]={
	DELTAVALUE_OPERATION ExternIdEQExternId,	// ==
	DELTAVALUE_OPERATION ExternIdNEExternId,	// !=
	Error,										// >
	Error,										// >=
	Error,										// <
	Error										// <=
};

static bool ExternIdExternId (RELATIONAL_FUNC_ARGS) {
	return (*relationalExternIdExternId[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_CACHED_OPERATION(ExternIdEQExternId, lhs->Equal(rhs), ExternId, ExternId)
IMPLEMENT_CACHED_OPERATION(ExternIdNEExternId, !lhs->Equal(rhs), ExternId, ExternId)

//////////////////////////////////////////////////////////////////////
// ExternId op Bool.
//
static RelationalOp_Func relationalExternIdBool[]={
	DELTAVALUE_OPERATION ExternIdEQBool,	// ==
	DELTAVALUE_OPERATION ExternIdNEBool,	// !=
	Error,									// >
	Error,									// >=
	Error,									// <
	Error									// <=
};

static bool ExternIdBool (RELATIONAL_FUNC_ARGS) {
	return (*relationalExternIdBool[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_UNCACHED_OPERATION(ExternIdEQBool, rhs->val.boolObj == lhs->val.externIdObj->ToBool())
IMPLEMENT_UNCACHED_OPERATION(ExternIdNEBool, rhs->val.boolObj != lhs->val.externIdObj->ToBool()) 

//////////////////////////////////////////////////////////////////////
// Overloaded versions.

static bool ExternIdWithAny (RELATIONAL_FUNC_ARGS){
	return	RelationalOverloading<LeftArgBinOpStringTraits>()(
				lhs, rhs, relOp, handler, vm CACHE_ARGUMENT, false, &TryOverloadingWithRhs
			); 
} 

static bool AnyWithExternId (RELATIONAL_FUNC_ARGS){
	return	RelationalOverloading<RightArgBinOpStringTraits>()(
				rhs, lhs, relOp,  handler, vm CACHE_ARGUMENT, true, GetDispatcherWithoutOverloading(lhs, rhs)
			); 
} 

//-------------------------------------------------------------------
// NIL.

static bool NilNil (RELATIONAL_FUNC_ARGS);
static bool NilBool (RELATIONAL_FUNC_ARGS);

IMPLEMENT_CACHED_OPERATION(NilEQNil, true, Nil, Nil)
IMPLEMENT_CACHED_OPERATION(NilNENil, false, Nil, Nil)

static RelationalOp_Func relationalNilNil[]={
	DELTAVALUE_OPERATION NilEQNil,		// ==
	DELTAVALUE_OPERATION NilNENil,		// !=
	Error,								// >
	Error,								// >=
	Error,								// <
	Error								// <=
};

static  bool NilNil (RELATIONAL_FUNC_ARGS)
	{ return (*relationalNilNil[relOp])(lhs, rhs, relOp, handler, vm CACHE_ARGUMENT);  }

static RelationalOp_Func relationalNilBool[]={
	DELTAVALUE_OPERATION NilEQBool,		// ==
	DELTAVALUE_OPERATION NilNEBool,		// !=
	Error,								// >
	Error,								// >=
	Error,								// <
	Error								// <=
};

static bool NilBool (RELATIONAL_FUNC_ARGS) { 
	return	(*relationalNilBool[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm 
				CACHE_ARGUMENT
			);  
}

//-------------------------------------------------------------------
// MethodFunc.

static bool MethodFuncMethodFunc (RELATIONAL_FUNC_ARGS);
static bool MethodFuncBool (RELATIONAL_FUNC_ARGS);

//////////////////////////////////////////////////////////////////////
// MethodFunc op MethodFunc.
//
static RelationalOp_Func relationalMethodFuncMethodFunc[]={
	DELTAVALUE_OPERATION MethodFuncEQMethodFunc,	// ==
	DELTAVALUE_OPERATION MethodFuncNEMethodFunc,	// !=
	Error,											// >
	Error,											// >=
	Error,											// <
	Error											// <=
};

static bool MethodFuncMethodFunc (RELATIONAL_FUNC_ARGS) {
	return (*relationalMethodFuncMethodFunc[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

IMPLEMENT_CACHED_OPERATION(
	MethodFuncEQMethodFunc, 
	lhs->val.methodFuncObj.func	== rhs->val.methodFuncObj.func		&&
	lhs->val.methodFuncObj.self	== rhs->val.methodFuncObj.self		&&
	lhs->val.methodFuncObj.vm		== rhs->val.methodFuncObj.vm,
	MethodFunc, 
	MethodFunc
)

IMPLEMENT_CACHED_OPERATION(
	MethodFuncNEMethodFunc, 
	lhs->val.methodFuncObj.func	!= rhs->val.methodFuncObj.func		||
	lhs->val.methodFuncObj.self	!= rhs->val.methodFuncObj.self		||
	lhs->val.methodFuncObj.vm		!= rhs->val.methodFuncObj.vm, 
	MethodFunc, 
	MethodFunc
)

//////////////////////////////////////////////////////////////////////
// MethodFunc op Bool.
//
static RelationalOp_Func relationalMethodFuncBool[]={
	DELTAVALUE_OPERATION MethodFuncEQBool,		// ==
	DELTAVALUE_OPERATION MethodFuncNEBool,		// !=
	Error,										// >
	Error,										// >=
	Error,										// <
	Error										// <=
};

IMPLEMENT_CACHED_OPERATION(MethodFuncEQBool, rhs->val.boolObj, MethodFunc, Bool)
IMPLEMENT_CACHED_OPERATION(MethodFuncNEBool, !rhs->val.boolObj, MethodFunc, Bool)

static bool MethodFuncBool (RELATIONAL_FUNC_ARGS) {
	return (*relationalMethodFuncBool[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm
				CACHE_ARGUMENT
			);
}

//-------------------------------------------------------------------
// BUILTIN (NON-OVERLOADED CALLS) DISPATCHER.
//
static RelationalOp_Func relationalDifferentTypes[]={
	ReturnFalse,	// ==
	ReturnTrue,		// !=
	Error,			// >
	Error,			// >=
	Error,			// <
	Error			// <=
};

// No chaching in case of different types where only == and != apply
// since we do not care about the type.

static bool DifferentTypes (RELATIONAL_FUNC_ARGS) {
	return (*relationalDifferentTypes[relOp])(
				lhs, 
				rhs, 
				relOp, 
				handler, 
				vm 
				CACHE_ARGUMENT
			);
}

////////////////////////////////////////////////////////////////////

#define	BA					BoolAny,
#define	NN					NumberNumber,
#define	NB					NumberBool,
#define	SS					StringString,
#define	SB					StringBool,
#define	TT					TableTable,
#define	TB					TableBool,
#define	FF					ProgramFuncProgramFunc,
#define	FB					ProgramFuncBool,
#define	RR					LibraryFuncLibraryFunc,
#define	RB					LibraryFuncBool,
#define	MM					MethodFuncMethodFunc,
#define	MB					MethodFuncBool,
#define	XX					ExternIdExternId,
#define	XB					ExternIdBool,
#define	LB					NilBool,
#define	UB					NilBool,
#define	UNDEFUNDEF			NilNil,
#define	NILNIL				NilNil,
#define	_					DifferentTypes,

static RelationalOp_Func dispatcherWithoutOverloading[] = {

/*					Number	String	Table	Func	Lib		Bool	ExtId	Nil		Method	Undef	*/
/*	Number	*/		NN		_		_		_		_		NB		_		_		_		_
/*	String	*/		_		SS		_		_		_		SB		_		_		_		_
/*	Table	*/		_		_		TT		_		_		TB		_		_		_		_
/*	Func	*/		_		_		_		FF		_		FB		_		_		_		_
/*	Lib		*/		_		_		_		_		RR		RB		_		_		_		_
/*	Bool	*/		BA		BA		BA		BA		BA		BA		BA		BA		BA		BA
/*	ExtId	*/		_		_		_		_		_		XB		XX		_		_		_
/*	Nil		*/		_		_		_		_		_		LB		_		NILNIL	_		_
/*	Method	*/		_		_		_		_		_		MB		_		_		MM		_
/*	Undef	*/		_		_		_		_		_		UB		_		_		_		UNDEFUNDEF
					END_OF_DISPATCHER
};

////////////////////////////////////////////////////////////////////

static RelationalOp_Func GetDispatcherWithoutOverloading (DeltaValue* lhs, DeltaValue* rhs)
	{ return DISPATCHER_WITHOUT_OVERLOADING(lhs, rhs); }

//////////////////////////////////////////////////////////////////////
// Functions needed for the expression evaluator (debugger).
// Those functions use directly objects, hence, they need not be
// part of an instruction set. However, the VM is still used.
//

#define	UNUSED_CACHE						\
		, &unused
#define EXECUTE_BUILTIN_FOR_DEBUGGER(op)	\
		EXECUTE_BUILTIN_FOR_DEBUGGER_EX(op, UEMPTY, UEMPTY, Rel, Relational, bool, void* unused, UNUSED_CACHE)

EXECUTE_BUILTIN_FOR_DEBUGGER(EQ)
EXECUTE_BUILTIN_FOR_DEBUGGER(NE)
EXECUTE_BUILTIN_FOR_DEBUGGER(GE)
EXECUTE_BUILTIN_FOR_DEBUGGER(GT)
EXECUTE_BUILTIN_FOR_DEBUGGER(LE)
EXECUTE_BUILTIN_FOR_DEBUGGER(LT)

/////////////////////////////////////////////////////////////////////
// Functions needed for the built-in relational operator lib functions.
//

#define BUILTIN_RELATIONAL(op)				\
		BUILTIN_OPERATOR_EX(op, UEMPTY, UEMPTY, Rel, Relational, bool, void* unused, UNUSED_CACHE)

BUILTIN_RELATIONAL(EQ)
BUILTIN_RELATIONAL(NE)
BUILTIN_RELATIONAL(GE)
BUILTIN_RELATIONAL(GT)
BUILTIN_RELATIONAL(LE)
BUILTIN_RELATIONAL(LT)

//-------------------------------------------------------------------
// NORMAL (OVERLOADED CALLS) DISPATCHER.
//
#define	AB		AnyBool,
#define	TA		TableWithAny,
#define	AT		AnyWithTable,
#define	XA		ExternIdWithAny,
#define	AX		AnyWithExternId,
#define	NEVER	(RelationalOp_Func) 0,

////////////////////////////////////////////////////////////////////
// If we come here, lhs was either Object or ExternId.
// Thus, if rhs is not also Object or ExternId we may use the
// default function '_' (DifferentTypes),
// which applies only to == and != being cummulative.
//
static RelationalOp_Func overloadableRetryWithRhs[] = {

/*	Number	*/		_
/*	String	*/		_
/*	Table	*/		AT
/*	Func	*/		_
/*	Lib		*/		_
/*	Bool	*/		AB
/*	ExtId	*/		AX
/*	Nil		*/		NEVER
/*	Method	*/		_
/*	Undef	*/		_
					END_OF_DISPATCHER
};

static bool TryOverloadingWithRhs (RELATIONAL_FUNC_ARGS) {
	return	(*DNULLCHECK(overloadableRetryWithRhs[rhs->Type()]))(
				lhs, 
				rhs, 
				relOp, 
				handler,
				vm
				CACHE_ARGUMENT
			);
}

////////////////////////////////////////////////////////////////////

static RelationalOp_Func dispatcherWithOverloading[] = {

/*					Number	String	Table	Func	Lib		Bool	ExtId	Nil		Method	Undef	*/
/*	Number	*/		NN		_		AT		_		_		NB		AX		_		_		_
/*	String	*/		_		SS		AT		_		_		SB		AX		_		_		_
/*	Table	*/		TA		TA		TA		TA		TA		TA		TA		_		TA		TA
/*	Func	*/		_		_		AT		FF		_		FB		AX		_		_		_
/*	Lib		*/		_		_		AT		_		RR		RB		AX		_		_		_
/*	Bool	*/		BA		BA		AT		BA		BA		BA		AX		BA		BA		BA
/*	ExtId	*/		XA		XA		XA		XA		XA		XA		XA		_		XA		XA
/*	Nil		*/		_		_		_		_		_		LB		_		NILNIL	_		_
/*	Method	*/		_		_		AT		_		_		MB		AX		_		MM		_
/*	Undef	*/		_		_		AT		_		_		UB		AX		_		_		UNDEFUNDEF
					END_OF_DISPATCHER
};

/////////////////////////////////////////////////////////////////////

#define EXECUTE_FOR_VM(op)														\
void Execute_J##op (DeltaInstruction* instr, DeltaVirtualMachine* vm) {			\
	DELTA_TRANSLATE_OPERAND(DELTA_BINARY_RELAT_OPERAND_LEFT, lhs, temp1);		\
	DASSERT(lhs != &DPTR(vm)->GetReturnValue());								\
	DELTA_TRANSLATE_OPERAND(DELTA_BINARY_RELAT_OPERAND_RIGHT, rhs, temp2);		\
	DASSERT(rhs != &DPTR(vm)->GetReturnValue());								\
	static RelationalInstructionErrorHandler handler;							\
	handler.Set(vm);															\
	CACHE_ON_INSTRUCTION();														\
	RelationalOp_Func f = 0x00;													\
	UVOIDIN2FUNC(instr->GetExecutor(), RelationalOp_Func, f);					\
	if (!f)																		\
		f = DISPATCHER_WITH_OVERLOADING(lhs, rhs);								\
	if ((*f)(lhs, rhs, DeltaRelOp_##op, &handler, vm CACHE_PASS_ARGUMENT))		\
		{ DELTA_INSTRUCTIONS_TAKE_THE_JUMP(); }									\
}

static bool InvokeDefaultDispatcher (RELATIONAL_FUNC_ARGS)
	{ return (*DISPATCHER_WITH_OVERLOADING(lhs, rhs))(lhs, rhs, relOp, handler, vm CACHE_ARGUMENT); }

////////////////////////////////////////////////////////////////////

EXECUTE_FOR_VM(EQ)
EXECUTE_FOR_VM(NE)
EXECUTE_FOR_VM(GE)
EXECUTE_FOR_VM(GT)
EXECUTE_FOR_VM(LE)
EXECUTE_FOR_VM(LT)

void Execute_JTRUETEST (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	DELTA_TRANSLATE_OPERAND(DELTA_BINARY_RELAT_OPERAND_LEFT, arg, temp);
	if (arg->TrueTest())
		{ DELTA_INSTRUCTIONS_TAKE_THE_JUMP(); }
}

void Execute_JFALSETEST (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	DELTA_TRANSLATE_OPERAND(DELTA_BINARY_RELAT_OPERAND_LEFT, arg, temp);
	if (!arg->TrueTest())
		{ DELTA_INSTRUCTIONS_TAKE_THE_JUMP(); }
}

//-------------------------------------------------------------------
