// DeltaStdLibErrors.h
// Formatting for the various error messages of
// Delta standard library.
// ScriptFighter Project.
// A. Savidis, June 2001.
//

#ifndef	DELTASTDLIBERRORS_H
#define	DELTASTDLIBERRORS_H

#include "DeltaByteCodeTypes.h"
#include "ustrings.h"
#include "utypes.h"
#include "uerrorclass.h"

/////////////////////////////////////////////////////////
// Error formats.
//
#define	DEFORMAT_ARGSNO	\
			"%d args expected (%d passed) to '%s'"

#define	DEFORMAT_MOREARGSNO	\
			"at least '%d' args expected (%d passed) to '%s'"

#define	DEFORMAT_UNMATCHED_OPT_ARGS	\
			"'%d' out of the '%d' args were not matched to '%s'"

#define	DEFORMAT_LESSARGSNO	\
			"at most '%d' args expected (%d passed) to '%s'"

#define	DEFORMAT_ARGTYPE \
			"arg #%d of type '%s' expected (%s passed) to '%s'"

#define	DEFORMAT_REJECTNEGATIVE \
			"arg #%d of Number type >=0  expected to '%s'"

#define	DEFORMAT_NEGATIVE_ANGLE \
			"Positive angle values in [0,360) only (%f passed to '%s')"

#define	DEFORMAT_INVALID_VMID \
			"invalid VM id '%s' passed to '%s'"

#define	DEFORMAT_VMID_OR_VM_EXPECTED \
			"arg of VM / id expected to '%s'"

#define	DEFORMAT_VM_NOT_RUN \
			"VM with id '%s' supplied to '%s' not run yet (use 'std::vmrun')"

#define	DEFORMAT_VM_ALREADY_LOADED \
			"in %s(VM id %s), a VM with this id already exists!"

#define	DEFORMAT_VM_LIB_NOT_REGISTERED \
			"in %s(lib id '%s'), no library with this id was registered!"

#define	DEFORMAT_VM_LIB_RUN_ERROR \
			"in %s(lib id '%s'), errors occured while running vm!"

#define	DEFORMAT_VM_LIB_IMPORT_ERROR \
			"in %s(lib id '%s'), failed to load library from '%s'!"


#define	DEFORMAT_VM_LIB_WAS_NOT_IMPORTED \
			"in %s(vm '%s'), this vm was not created through import!"

#define	DEFORMAT_ILLEGAL_STRING_INDEX \
			"string index '%d' out of '%d' length in '%s'"

#define	DEFORMAT_ILLEGAL_ASCII_CODE \
			"illegal (non ascii) code '%d' in '%s'"

#define	DEFORMAT_NOT_SINGLE_CHARACTER_STRING \
			"not a single character string '%s' provided in '%s'"

#define	DEFORMAT_LIBFUNC_PRECOND_FAILED \
			"precondition '%s' to call '%s' not satisfied!"

#define	DEFORMAT_EXTERNID_NOT_INSTANCE \
			"call '%s()' not supported for arg #%d externid '%s' (no userdata)!"

#define	DEFORMAT_NO_TYPE_MATCHING_FOR_EXTERNID \
			"failed to convert arg #%d to externid of type '%s' in %s!"

#define	DEFORMAT_ERROR_WHILE_TRYING_TO_MATCH_EXTERNID_TYPE \
			"while trying to convert arg #%d to externid of type '%s' in %s!"

/////////////////////////////////////////////////////////
// Reset macros, used when upon failure, the return 
// value should be reset to something harmless.
//
#define	RESET_EMPTY \
		return;

#define	RESET_ZERO_RETURNVALUE \
	{	vm->GetReturnValue().FromNumber(0); \
		return; }

#define	RESET_FALSE_RETURNVALUE \
	{	vm->GetReturnValue().FromBool(false); \
		return;	}

#define	RESET_TRUE_RETURNVALUE \
	{	vm->GetReturnValue().FromBool(true); \
		return;	}

#define	RESET_NIL_RETURNVALUE \
	{	vm->GetReturnValue().FromNil(); \
		return;	}

#define	RESET_UNDEFINED_RETURNVALUE \
	{	vm->GetReturnValue().Undefine(); \
		return;	}

/////////////////////////////////////////////////////////
// Checking of total arguments supplied.
//
#define	DeltaTotalArgsCheck(n, func, retvalreset)			\
	if (DPTR(vm)->TotalActualArgs() != (n)) {				\
		DPTR(vm)->PrimaryError(								\
			DEFORMAT_ARGSNO,								\
			(n),											\
			vm->TotalActualArgs(),							\
			func											\
		);													\
		retvalreset											\
	}

#define DeltaCheckPrecond(expr, descr, retvalreset)			\
	if (!(expr)) {											\
		DPTR(vm)->PrimaryError(								\
			DEFORMAT_LIBFUNC_PRECOND_FAILED,				\
			ucstringarg(descr),								\
			CURR_FUNC										\
		);													\
		retvalreset											\
	}

#define	DeltaAtLeastTotalArgsCheck(n, func, retvalreset)	\
	if (DPTR(vm)->TotalActualArgs()<(n)) {					\
		DPTR(vm)->PrimaryError(								\
			DEFORMAT_MOREARGSNO,							\
			(n),											\
			vm->TotalActualArgs(),							\
			func											\
		);													\
		retvalreset											\
	}

#define	DeltaAtMostTotalArgsCheck(n, func, retvalreset)		\
	if (DPTR(vm)->TotalActualArgs()>(n)) {					\
		DPTR(vm)->PrimaryError(								\
			DEFORMAT_LESSARGSNO,							\
			(n),											\
			vm->TotalActualArgs(),							\
			func											\
		);													\
		retvalreset											\
	}

/////////////////////////////////////////////////////////
// Checking of argument type.
//
#define	DeltaGetArg(var, n, type, typestr, func, retvalreset)					\
  	var = vm->GetActualArg(n);													\
	if ((var)->Type() != type) {												\
		DPTR(vm)->PrimaryError(													\
			DEFORMAT_ARGTYPE,													\
			n,																	\
			typestr,															\
			var->TypeStr(),														\
			func																\
		);																		\
		retvalreset																\
	}

#define	DeltaGetOverloadedArg(var, n, typecheck, typestr, func, retvalreset)	\
  	var = vm->GetActualArg(n);													\
	if (!typecheck((var)->Type())) {											\
		DPTR(vm)->PrimaryError(													\
			DEFORMAT_ARGTYPE,													\
			n,																	\
			typestr,															\
			var->TypeStr(),														\
			func																\
		);																		\
		retvalreset																\
	}

/////////////////////////////////////////////////////////
// Wrong extern id type, which is likely to result in 
// a fatal run-time error.
//
#define	DeltaErrorExternId(argNo, typeFound, typeExpected, func)				\
	DPTR(vm)->PrimaryError(														\
		"at arg '%d' in '%s', extern id '%s' expected, but '%s' passed",		\
		argNo,																	\
		ucstringarg(func),														\
		ucstringarg(typeExpected),												\
		ucstringarg(typeFound)													\
	)

/////////////////////////////////////////////////////////

#define	ISSUE_FUNC(name) \
	const char* CURR_FUNC = name;

/////////////////////////////////////////////////////////

#define	GET_NUMBER(var, func, retvalreset, argno)									\
	DeltaGetArg(argObj, argno, DeltaValue_Number, "Number", func, retvalreset);		\
	DeltaNumberValueType var;														\
	argObj->ToNumber(&var);

#define	GET_UNSIGNEDINT(var, func, retvalreset, argno)								\
	DeltaGetArg(argObj, argno, DeltaValue_Number, "Number", func, retvalreset);		\
	DeltaNumberValueType num##var = argObj->ToNumber();								\
	if (num##var < 0) {																\
		DPTR(vm)->PrimaryError(														\
			"at arg '%d' in '%s' unsigned expected (negative passed)",				\
			argno,																	\
			func																	\
		);																			\
		retvalreset																	\
	}																				\
	util_ui32 var = (util_ui32) num##var;
		
#define	GET_SIGNEDINT(var, func, retvalreset, argno)								\
	DeltaGetArg(argObj, argno, DeltaValue_Number, "Number", func, retvalreset);		\
	DeltaNumberValueType var = argObj->ToNumber();									\
	if (((util_i32) var) != var) {													\
		DPTR(vm)->PrimaryError(														\
			"at arg '%d' in '%s', signed int expected, but real passed",			\
			argno,																	\
			func																	\
		);																			\
		retvalreset																	\
	}

#define	GET_POSITIVENUMBER(var, func, retvalreset, argno)							\
	DeltaGetArg(argObj, argno, DeltaValue_Number, "Number", func, retvalreset);		\
	DeltaNumberValueType var = argObj->ToNumber();									\
	if (var < 0) {																	\
		DPTR(vm)->PrimaryError(														\
			"at arg '%d' in '%s', illegal %f negative value",						\
			argno,																	\
			func,																	\
			var																		\
		);																			\
		retvalreset																	\
	}

// Local variables required are: argObj.
//
#define	GET_VM(var, retvalreset, argno)												\
	GET_POLYMORPHIC_EXTERNID(var##Val, DELTA_VM_TYPE_STR, retvalreset, argno);		\
	DeltaVirtualMachine* var = DELTAVM_VGET_INST(var##Val);							\
	VALIDATE_EXTERNID(var, var##Val, retvalreset, var##ValTypeStr)

/////////////////////////////////////////////////////////

#define	GET_STRING(var, func, retvalreset, argno)									\
	DeltaGetArg(argObj, argno, DeltaValue_String, "String", func, retvalreset);		\
	std::string var = argObj->ToString();

#define	GET_EXTERNID(var, func, retvalreset, argno)									\
	DeltaGetArg(argObj, argno, DeltaValue_ExternId, "ExternId", func, retvalreset);	\
	std::string var##TypeStr;														\
	void* POSSIBLY_UNUSED_LOCAL(var) = argObj->ToExternId(var##TypeStr);

#define	IS_OBJECT_OR_EXTERNID(t)													\
	((t) == DeltaValue_Table || (t) == DeltaValue_ExternId)

#define	GET_POLYMORPHIC_EXTERNID(var, typestr, retvalreset, argno)																\
	DeltaGetOverloadedArg(argObj, argno, IS_OBJECT_OR_EXTERNID, "Object / ExternId", CURR_FUNC, retvalreset);					\
	void* var = (void*) 0;																										\
	std::string var##TypeStr;																									\
	{	DeltaValue actualCaller;																								\
		if (!argObj->ConvertToExternId(typestr, &actualCaller))	{																\
			DPTR(vm)->PrimaryError(DEFORMAT_ERROR_WHILE_TRYING_TO_MATCH_EXTERNID_TYPE, argno, typestr, CURR_FUNC);				\
			retvalreset																											\
		}																														\
		if (actualCaller.IsUndefined())																							\
			{ DPTR(vm)->PrimaryError(DEFORMAT_NO_TYPE_MATCHING_FOR_EXTERNID, argno, typestr, CURR_FUNC); retvalreset }			\
		DASSERT(actualCaller.Type() == DeltaValue_ExternId && actualCaller.GetExternIdTypeString() == typestr);					\
		var##TypeStr = typestr;																									\
		var = actualCaller.ToExternId(var##TypeStr);																			\
	}																															\
	DASSERT(var)

#define	GET_EXTERNIDEX(func, retvalreset, argno)									\
	DeltaGetArg(argObj, argno, DeltaValue_ExternId, "ExternId", func, retvalreset);	\
	val = argObj->ToExternId(typeStr);

#define	GET_USERDATA(var) \
	DeltaTable* var = argObj->GetExternIdUserData()

/////////////////////////////////////////////////////////

#define	GET_BOOL(var, func, retvalreset, argno)												\
	DeltaGetArg(argObj, argno, DeltaValue_Bool, "Bool", func, retvalreset);					\
	bool var;																				\
	argObj->ToBool(&var);

#define	GET_TABLE(var, func, retvalreset, argno)											\
	DeltaGetArg(argObj, argno, DeltaValue_Table, "Table", func, retvalreset);				\
	DeltaTable* POSSIBLY_UNUSED_LOCAL(var) = argObj->ToTable();

#define	GET_LIBFUNC(var, func, retvalreset, argno)											\
	DeltaGetArg(argObj, argno, DeltaValue_LibraryFunc, "LibraryFunc", func, retvalreset);	\
	DeltaLibraryFunc POSSIBLY_UNUSED_LOCAL(var) = argObj->ToLibraryFunc();					\
	const DeltaLibraryFuncArgsBinder* POSSIBLY_UNUSED_LOCAL(var##Binder) = argObj->GetLibraryFuncArgsBinder();

#define	GET_INSTANCE(var, func, retvalreset, argno)											\
	DeltaGetArg(argObj, argno, DeltaValue_Table, "Object", func, retvalreset);				\
	DeltaObject* var = (DeltaObject*) argObj->ToTable();

#define	CHECK_CALLABLE(t)						\
	(	(t) == DeltaValue_ProgramFunc	||		\
		(t) == DeltaValue_MethodFunc	||		\
		(t) == DeltaValue_Table	)

#define	GET_CALLABLE(func, retvalreset, argno) \
	DeltaGetOverloadedArg(argObj, argno, CHECK_CALLABLE, "Normal / member function or table", func, retvalreset);

///////////////////////////////////////////////////////

#define	UERROR_HANDLER_DELTALIB(what, errclass)	\
	DPTR(vm)->PrimaryError(						\
		"in '%s', %s!",							\
		CURR_FUNC,								\
		what									\
	); goto FAIL;

///////////////////////////////////////////////////////

#define	VALIDATE_EXTERNID(call, val, act, typeStr)								\
	if (!(call)) {																\
		DPTR(vm)->PrimaryError(													\
			"in '%s', extern id of type '%s' and value 0x%x was destroyed!",	\
			CURR_FUNC,															\
			typeStr.c_str(),													\
			(util_ui32) val														\
		);																		\
		act;																	\
	}

#define	STD_LIBFUNC_LOCALS	\
	DeltaValue*	argObj; \
	void*			val;	\
	std::string		typeStr

#define	CHECK_CPPFUNC_RESULT(r)										\
	if (!r)															\
		DPTR(vm)->PrimaryError("Call to '%s' failed!", CURR_FUNC);

///////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.