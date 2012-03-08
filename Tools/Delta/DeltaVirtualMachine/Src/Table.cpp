// Table.cpp
// Execution functions for managing tables.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Changed August 2007, error propagation from inside the tables / protos
// and final VM error outside. Also, false value returned only on error,
// while content set to nil automatically after get when not inside.
// Extended October 2010 to support inline caching.
//
#include "DDebug.h"
#include "DeltaTable.h"
#include "Table.h"
#include "DeltaByteCodeTypes.h"
#include "DeltaVirtualMachine.h"
#include "DeltaValue.h"
#include "DeltaString.h"
#include "DeltaRuntimeError.h"
#include "OperatorOverloading.h"
#include "Execute.h"
#include "DeltaObject.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "uerrorclass.h"
#include "ArithmeticP.h"

#define	CACHE_ENABLED
//	#define	CACHE_PROFILE

#ifdef	CACHE_PROFILE
static DeltaCacheProfiler cacheProfiler("Table");
#define	PROFILE_INSTRUCTION()	cacheProfiler.CacheInstruction()
#define	PROFILE_HIT()			cacheProfiler.CacheHit()
#define	PROFILE_MISS()			cacheProfiler.CacheMiss()
#else
#define	PROFILE_INSTRUCTION()	
#define	PROFILE_HIT()			
#define	PROFILE_MISS()			
#endif

#define	CACHE_ON_INSTRUCTION()	PROFILE_INSTRUCTION()
#define	CACHE_MISS()			if (true) { instr->ResetExecutor(); PROFILE_MISS(); } else
#define	CACHE_HIT()				PROFILE_HIT()
#define	CACHE_SET(f)			instr->SetExecutor(UFUNC2VOID(f))

typedef void (*CachedFunc)(DeltaInstruction*, DeltaVirtualMachine*);

//------------------------------------------------------------------
// Due to a GCC bug, functions which are declared through friend-declarations
// of a class, are not looked-up unless they are invoked with an argument
// whose type is related to the class whose friend the function is.
// Therefore, taking the function's pointer (which does not involve invocation
// and -thus- proper look-up) causes an error for an "undeclared identifier".
//
// Work-around: explicitly declare the functions outside the friend-class:
extern void TableCallGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void TableCallBoundedGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void ExternIdCallGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm);
extern void ExternIdCallBoundedGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm);

//------------------------------------------------------------------
// OBJNEW / OBJDONE

inline const std::string GetTableConstructionContext (DeltaVirtualMachine* vm) {
	if (const DeltaStdFuncInfo* info = DPTR(vm)->GetCurrFuncInfo())
		return	uconstructstr(
					"%s '%s'", 
					DeltaValue::GetTypeStr(info->IsMethod() ? DeltaValue_MethodFunc : DeltaValue_ProgramFunc), 
					info->GetName().c_str()
				);
	else
		return DBG_GLOBAL_CODE_DESCRIPTION;
}

//********************************

#define	DELTA_OBJECT_CREATION(_operand, _after_creation)						\
	if (true) {																	\
	DASSERT(instr->_operand.IsStorageType());									\
	if (DeltaValue*	val	= DPTR(vm)->GetStorageObject(instr->_operand)) {		\
		DeltaObject* newObject = DeltaObject::NewObject();						\
		DPTR(newObject)->GetCreationInfo().SetCreatorInformation(				\
			vm->Source(),														\
			vm->Line(),															\
			GetTableConstructionContext(vm)										\
		);																		\
		if (instr->_operand.IsTempVar())										\
			val->FromTable(newObject);											\
		else																	\
			val->AssignOverloadable(newObject);									\
		_after_creation;														\
	}																			\
	else																		\
		DASSERT(vm->HasProducedError());										\
	} else

#define	DELTA_PUSH_CONSTRUCTED_OBJECT											\
	if (true) {																	\
		DPTR(vm)->PushConstructedObject(newObject);								\
		DASSERT(DPTR(vm)->GetCurrConstructedObject() == newObject);				\
	} else

////////////////////////////////////////////////////////////////////

void Execute_OBJNEW (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ DELTA_OBJECT_CREATION(DELTA_OBJNEW_OPERAND_RESULT, DELTA_PUSH_CONSTRUCTED_OBJECT); }

//*******************************

void Execute_OBJNEWSET (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DELTA_TRANSLATE_OPERAND(DELTA_SET_OPERAND_CONTENT,	content, temp1);
	DELTA_TRANSLATE_OPERAND(DELTA_SET_OPERAND_INDEX,	index,   temp2);

	DeltaTable* t = DNULLCHECK(DPTR(vm)->GetCurrConstructedObject());
	if (!DPTR(t)->BoundedSet(index, content))
		DPTR(vm)->PossiblePrimaryError(
			"failure in set (upon construction) over '%s' " DELTA_OBJECT_SN_FORMAT " with index '%s' and content '%s' !",
			DPTR(t)->GetExtClassString(),
			DPTR(t)->GetSerialNo(),
			index->ConvertToStringBySkippingUserCode().c_str(),	
			content->ConvertToStringBySkippingUserCode().c_str()
		);
}

//*******************************

void Execute_OBJNEWEMPTY (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ DELTA_OBJECT_CREATION(DELTA_OBJNEWEMPTY_OPERAND_RESULT, UEMPTY); }

void Execute_OBJDONE (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	DASSERT(DPTR(vm)->GetCurrConstructedObject());
	DPTR(vm)->PopConstructedObject();	
}

//------------------------------------------------------------------

inline bool IsSelfIndex (DeltaValue& index) 
	{ return index.Type() == DeltaValue_String && index.ToString() == DELTA_SELF_POINTER_ID; }

////////////////////////////////////////////////////////////////////
// We always use a temp var to store a table container value since due to
// overloading, while calling get / set, a user table may be collected!
// The latter does not affect setting table elements as table have reference
// semantics. However, if it is a method, we need use the method *directly* 
// to alter method self, as methods have value-copy semantics. 

#define	DELTA_GET_CONTAINER_OPERAND(arg, var)							\
	DeltaValue temp0; DeltaValue* var;									\
	if (instr->arg.IsStorageType())										\
		if ((var = DPTR(vm)->GetStorageObject(instr->arg)) != 0) {		\
			DASSERT(DPTR(vm)->ValidateStackValuePtr(var));				\
			DASSERT(var != &DPTR(vm)->GetReturnValue());				\
			if (var->Type() == DeltaValue_Table)						\
				var = new (&temp0) DeltaValue(var->ToTable());			\
		}																\
		else															\
			{ DASSERT(vm->HasProducedError()); return; }				\
	else {																\
		DASSERT(instr->arg.GetType() == DeltaOperand_NewSelf);			\
		var = vm->FromOperandToNewSelf(instr->arg, &temp0);				\
	}

#define	DELTA_GET_CONTAINER_OPERAND_FAST(arg, var)						\
	DeltaValue temp0; DeltaValue* var;									\
	if (instr->arg.IsStorageType())	{									\
		if (!(var = DPTR(vm)->GetStorageObject(instr->arg))) 			\
			{ DASSERT(vm->HasProducedError()); return; }				\
	}																	\
	else {																\
		DASSERT(instr->arg.GetType() == DeltaOperand_NewSelf);			\
		var = vm->FromOperandToNewSelf(instr->arg, &temp0);				\
	}

//------------------------------------------------------------------
// SET

#define	IMPLEMENT_CACHED_SET(																		\
		_default_exec,																				\
		_container_type,																			\
		_extract_table_from_container,																\
		_table_expr,																				\
		_operand_assertions,																		\
		_get_content_operand,																		\
		_set_func,																					\
		_set_func_name,																				\
		_set_index_var,																				\
		_use_index,																					\
		_index_to_string																			\
	)																								\
	_operand_assertions;																			\
	DELTA_GET_CONTAINER_OPERAND(DELTA_SET_OPERAND_CONTAINER, container);							\
	_get_content_operand;																			\
	if (container->Type() != _container_type) {														\
		CACHE_MISS();																				\
		Execute_##_default_exec(instr, vm);															\
	}																								\
	else {																							\
		CACHE_HIT();																				\
		_extract_table_from_container(_set_func_name, _index_to_string);							\
		if (!DPTR(_table_expr)->_set_func(_use_index, *content))									\
			DPTR(vm)->PossiblePrimaryError(															\
				"failure in %s over '%s' " DELTA_OBJECT_SN_FORMAT " with index '%s' and content '%s' !",	\
				_set_func_name,																		\
				container->TypeStr(),																\
				DPTR(_table_expr)->GetSerialNo(),													\
				(_index_to_string),																	\
				content->ConvertToStringBySkippingUserCode().c_str()								\
			);																						\
	}

//////////////////////////////////////////////////////////////////////

#define	SET_CONTENT_STORAGE_OPERAND		\
			DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_SET_OPERAND_CONTENT, content)

#define	SET_CONTENT_NOT_STORAGE_OPERAND	\
			DELTA_TRANSLATE_NON_STORAGE_OPERAND(DELTA_SET_OPERAND_CONTENT, content, temp)

#define	SET_INDEX_CONST_STRING_ASSERTION			\
			DASSERT(instr->DELTA_SET_OPERAND_INDEX.GetType() == DeltaOperand_ConstString);

#define	SET_INDEX_CONST_NUMBER_ASSERTION			\
			DASSERT(instr->DELTA_SET_OPERAND_INDEX.GetType() == DeltaOperand_ConstNumber);

#define	CONTENT_IS_STORAGE_ASSERTION			\
			DASSERT(instr->DELTA_SET_OPERAND_CONTENT.IsStorageType());

#define	CONTENT_NOT_STORAGE_ASSERTION			\
			DASSERT(!instr->DELTA_SET_OPERAND_CONTENT.IsStorageType());

#define	SET_INDEX								\
			instr->DELTA_SET_OPERAND_INDEX

#define	GET_VALUE_TABLE(_func_name, _index_to_string)

#define	GET_EXTERNID_USERDATA(_func_name, _index_to_string)								\
		DeltaTable* userData = container->GetExternIdUserData();						\
		if (!userData) {																\
			DPTR(vm)->SetErrorCode(DELTA_EXTERNID_MISSING_USERDATA_ERROR)->				\
			PrimaryError(																\
				"failure in %s over externid '%s' with index '%s' (no userdata)!",		\
				_func_name,																\
				container->GetExternIdTypeString().c_str(),								\
				(_index_to_string)														\
			);																			\
			return;																		\
		} else

//////////////////////////////////////////////////////////////////////

#define	TABLE_SET_CONSTSTRING_INDEX(_default_exec, _func, _func_name, _content_assert, _content_operand)	\
	IMPLEMENT_CACHED_SET(																					\
		_default_exec,																						\
		DeltaValue_Table,																					\
		GET_VALUE_TABLE,																					\
		container->ToTable(),																				\
		_content_assert SET_INDEX_CONST_STRING_ASSERTION,													\
		_content_operand,																					\
		_func,																								\
		_func_name,																							\
		UEMPTY,																								\
		(const char*) SET_INDEX.GetValue(),																	\
		(const char*) SET_INDEX.GetValue()																	\
	)

#define	TABLE_SET_CONSTNUMBER_INDEX(_default_exec, _func, _func_name, _content_assert, _content_operand)	\
	IMPLEMENT_CACHED_SET(																					\
		_default_exec,																						\
		DeltaValue_Table,																					\
		GET_VALUE_TABLE,																					\
		container->ToTable(),																				\
		_content_assert SET_INDEX_CONST_NUMBER_ASSERTION,													\
		_content_operand,																					\
		_func,																								\
		_func_name,																							\
		UEMPTY,																								\
		*((DeltaNumberValueType*) SET_INDEX.GetValue()),													\
		unum2string(*((DeltaNumberValueType*) SET_INDEX.GetValue())).c_str()								\
	)

void TableSetConstStringContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_SET_CONSTSTRING_INDEX(OBJSET, Set, "set", CONTENT_IS_STORAGE_ASSERTION, SET_CONTENT_STORAGE_OPERAND); }

void TableBoundedSetConstStringContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_SET_CONSTSTRING_INDEX(BOUNDEDOBJSET, BoundedSet, "bounded set", CONTENT_IS_STORAGE_ASSERTION, SET_CONTENT_STORAGE_OPERAND); }

void TableSetConstStringContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_SET_CONSTSTRING_INDEX(OBJSET, Set, "set", CONTENT_NOT_STORAGE_ASSERTION, SET_CONTENT_NOT_STORAGE_OPERAND); }

void TableBoundedSetConstStringContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_SET_CONSTSTRING_INDEX(BOUNDEDOBJSET, BoundedSet, "boundedset", CONTENT_NOT_STORAGE_ASSERTION, SET_CONTENT_NOT_STORAGE_OPERAND); }

void TableSetConstNumberContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_SET_CONSTNUMBER_INDEX(OBJSET, Set, "set", CONTENT_IS_STORAGE_ASSERTION, SET_CONTENT_STORAGE_OPERAND); }

void TableBoundedSetConstNumberContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_SET_CONSTNUMBER_INDEX(BOUNDEDOBJSET, BoundedSet, "bounded set", CONTENT_IS_STORAGE_ASSERTION, SET_CONTENT_STORAGE_OPERAND); }

void TableSetConstNumberContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_SET_CONSTNUMBER_INDEX(OBJSET, Set, "set", CONTENT_NOT_STORAGE_ASSERTION, SET_CONTENT_NOT_STORAGE_OPERAND); }

void TableBoundedSetConstNumberContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_SET_CONSTNUMBER_INDEX(BOUNDEDOBJSET, BoundedSet, "bounded set", CONTENT_NOT_STORAGE_ASSERTION, SET_CONTENT_NOT_STORAGE_OPERAND); }

//////////////////////////////////////////////////////////////////////

#define	EXTERNID_SET_CONSTSTRING_INDEX(_default_exec, _func, _func_name, _content_assert, _content_operand)	\
	IMPLEMENT_CACHED_SET(																					\
		_default_exec,																						\
		DeltaValue_ExternId,																				\
		GET_EXTERNID_USERDATA,																				\
		userData,																							\
		_content_assert SET_INDEX_CONST_STRING_ASSERTION,													\
		_content_operand,																					\
		_func,																								\
		_func_name,																							\
		UEMPTY,																								\
		(const char*) SET_INDEX.GetValue(),																	\
		(const char*) SET_INDEX.GetValue()																	\
	)

#define	EXTERNID_SET_CONSTNUMBER_INDEX(_default_exec, _func, _func_name, _content_assert, _content_operand)	\
	IMPLEMENT_CACHED_SET(																					\
		_default_exec,																						\
		DeltaValue_ExternId,																				\
		GET_EXTERNID_USERDATA,																				\
		userData,																							\
		_content_assert SET_INDEX_CONST_NUMBER_ASSERTION,													\
		_content_operand,																					\
		_func,																								\
		_func_name,																							\
		UEMPTY,																								\
		*((DeltaNumberValueType*) SET_INDEX.GetValue()),													\
		unum2string(*((DeltaNumberValueType*) SET_INDEX.GetValue())).c_str()								\
	)

void ExternIdSetConstStringContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_SET_CONSTSTRING_INDEX(OBJSET, Set, "set", CONTENT_IS_STORAGE_ASSERTION, SET_CONTENT_STORAGE_OPERAND); }

void ExternIdBoundedSetConstStringContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_SET_CONSTSTRING_INDEX(BOUNDEDOBJSET, BoundedSet, "bounded set", CONTENT_IS_STORAGE_ASSERTION, SET_CONTENT_STORAGE_OPERAND); }

void ExternIdSetConstStringContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_SET_CONSTSTRING_INDEX(OBJSET, Set, "set", CONTENT_NOT_STORAGE_ASSERTION, SET_CONTENT_NOT_STORAGE_OPERAND); }

void ExternIdBoundedSetConstStringContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_SET_CONSTSTRING_INDEX(BOUNDEDOBJSET, BoundedSet, "boundedset", CONTENT_NOT_STORAGE_ASSERTION, SET_CONTENT_NOT_STORAGE_OPERAND); }

void ExternIdSetConstNumberContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_SET_CONSTNUMBER_INDEX(OBJSET, Set, "set", CONTENT_IS_STORAGE_ASSERTION, SET_CONTENT_STORAGE_OPERAND); }

void ExternIdBoundedSetConstNumberContentVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_SET_CONSTNUMBER_INDEX(BOUNDEDOBJSET, BoundedSet, "bounded set", CONTENT_IS_STORAGE_ASSERTION, SET_CONTENT_STORAGE_OPERAND); }

void ExternIdSetConstNumberContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_SET_CONSTNUMBER_INDEX(OBJSET, Set, "set", CONTENT_NOT_STORAGE_ASSERTION, SET_CONTENT_NOT_STORAGE_OPERAND); }

void ExternIdBoundedSetConstNumberContentNotVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_SET_CONSTNUMBER_INDEX(BOUNDEDOBJSET, BoundedSet, "bounded set", CONTENT_NOT_STORAGE_ASSERTION, SET_CONTENT_NOT_STORAGE_OPERAND); }

//////////////////////////////////////////////////////////////////////
// SET METHOD SELF.

// Fallback only in case of success.

#define	METHOD_SET_SELF_IMPL()																\
	if (true)																				\
	switch (content->Type()) {																\
		case DeltaValue_Table:																\
			container->ChangeMethodFuncSelf(content->ToTable());							\
			break;																			\
		case DeltaValue_ExternId:															\
			if (DeltaTable* userData = content->GetExternIdUserData())						\
				container->ChangeMethodFuncSelf(userData);									\
			else {																			\
				DPTR(vm)->SetErrorCode(DELTA_EXTERNID_MISSING_USERDATA_ERROR)->				\
				PrimaryError(																\
					"setting new 'self' on method with externid '%s' (has no userdata)!",	\
					container->GetExternIdTypeString().c_str()								\
				);																			\
				return;																		\
			}																				\
			break;																			\
		default: {																			\
			DPTR(vm)->SetErrorCode(DELTA_SET_METHOD_SELF_INVALID_VALUE_ERROR)->				\
			PrimaryError(																	\
				"setting new 'self' on method with '%s' instead of object!",				\
				content->TypeStr()															\
			);																				\
			return;																			\
		}																					\
	} else

#define	METHOD_CHECK_SELF_INDEX(_index, _opid)												\
		if (!IsSelfIndex(*_index)) {														\
			DPTR(vm)->SetErrorCode(DELTA_INVALID_METHOD_VALUE_INDEXING)->					\
			PrimaryError(																	\
				"failure in %s over '%s' with index '%s' (can index only 'self')!",			\
				_opid,																		\
				container->TypeStr(),														\
				(_index)->ConvertToStringBySkippingUserCode().c_str()						\
			);																				\
			return;																			\
		} else

//**********************************

#define	IMPLEMENT_CACHED_METHOD_SET_SELF(													\
		_default_exec,																		\
		_opid,																				\
		_operand_assertions,																\
		_extract_and_check_index															\
	)																						\
	_operand_assertions;																	\
	DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_SET_OPERAND_CONTAINER, container);				\
	if (container->Type() != DeltaValue_MethodFunc) {										\
		CACHE_MISS();																		\
		Execute_##_default_exec(instr, vm);													\
	}																						\
	else {																					\
		CACHE_HIT();																		\
		DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_SET_OPERAND_CONTAINER, content);				\
		_extract_and_check_index(_opid);													\
		METHOD_SET_SELF_IMPL();																\
	}
		
#define	METHOD_SET_SELF_INDEX_STORAGE_ASSERTION												\
		DASSERT(SET_INDEX.IsStorageType());

#define	METHOD_SET_SELF_INDEX_CONST_STRING_ASSERTION										\
		DASSERT(SET_INDEX.GetType() == DeltaOperand_ConstString);							\
		DASSERT(!strcmp((const char*) SET_INDEX.GetValue(), DELTA_SELF_POINTER_ID));

#define	METHOD_SET_SELF_INDEX_CONST_STRING_EXTRACT_AND_CHECK(_opdid)

#define	METHOD_SET_SELF_INDEX_USER_VAR_EXTRACT_AND_CHECK(_opid)								\
		DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_SET_OPERAND_INDEX, index);					\
		METHOD_CHECK_SELF_INDEX(index, _opid);
		
//**********************************

void MethodSetSelfConstStringIndexCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_METHOD_SET_SELF(
		OBJSET,
		"set",
		METHOD_SET_SELF_INDEX_CONST_STRING_ASSERTION,
		METHOD_SET_SELF_INDEX_CONST_STRING_EXTRACT_AND_CHECK
	);
}

void MethodSetSelfUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_METHOD_SET_SELF(
		OBJSET,
		"set",
		METHOD_SET_SELF_INDEX_STORAGE_ASSERTION,
		METHOD_SET_SELF_INDEX_USER_VAR_EXTRACT_AND_CHECK
	);
}

void MethodBoundedSetSelfConstStringIndexCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_METHOD_SET_SELF(
		BOUNDEDOBJSET,
		"bounded set",
		METHOD_SET_SELF_INDEX_CONST_STRING_ASSERTION,
		METHOD_SET_SELF_INDEX_CONST_STRING_EXTRACT_AND_CHECK
	);
}

void MethodBoundedSetSelfUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_METHOD_SET_SELF(
		BOUNDEDOBJSET,
		"bounded set",
		METHOD_SET_SELF_INDEX_STORAGE_ASSERTION,
		METHOD_SET_SELF_INDEX_USER_VAR_EXTRACT_AND_CHECK
	);
}

//////////////////////////////////////////////////////////////////////

template <class OpTraits> struct SetElemTemplate {

	static bool IsGetOperation (const DeltaInstruction& instr)
		{ return instr.GetOpCode() == DeltaVM_OBJGET || instr.GetOpCode() == DeltaVM_BOUNDEDOBJGET; }

	static void Do (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

		DELTA_GET_CONTAINER_OPERAND(DELTA_SET_OPERAND_CONTAINER, container);
		DELTA_TRANSLATE_OPERAND(DELTA_SET_OPERAND_CONTENT,	content, temp1);
		DELTA_TRANSLATE_OPERAND(DELTA_SET_OPERAND_INDEX,	index,   temp2);

		switch (container->Type()) {

			case DeltaValue_Table : {

				DELTA_RECORD_VM_SERIALNO_EX(vm);
				bool result = OpTraits::Set(DPTR(container->ToTable()), *index, *content);
				DELTA_VALIDATE_VM_SERIALNO_EX(vm, UEMPTY);

				if (!result) {
					DPTR(vm)->PossiblePrimaryError(
						"failure in %s over '%s' with index '%s' and content '%s'!",
						OpTraits::OpId(),
						container->TypeStr(),
						index->ConvertToStringBySkippingUserCode().c_str(),
						content->ConvertToStringBySkippingUserCode().c_str()
					);
					return;
				}

#ifdef	CACHE_ENABLED
				if (instr->DELTA_SET_OPERAND_INDEX.IsConstStringOrNumber())
					CACHE_SET(
						OpTraits::GetTableCache(
							instr->DELTA_SET_OPERAND_INDEX.GetType() == DeltaOperand_ConstNumber, 
							instr->DELTA_SET_OPERAND_CONTENT.IsStorageType()
						)
					);
#endif
				break;
			}

			case DeltaValue_MethodFunc :			// Methods functions have a single index.

				METHOD_CHECK_SELF_INDEX(index, OpTraits::OpStr());

				if (instr->result.IsTempVar() && instr->DELTA_SET_OPERAND_INDEX.GetType() != DeltaOperand_ConstString)	
					DPTR(vm)->SetErrorCode(DELTA_SET_METHOD_SELF_NEEDS_CONST_INDEX_ERROR)->PrimaryError( // Not using a compile-time string const?
						"setting 'self' on '%s' via non-const string index possible only using vars!",
						container->TypeStr()
					);
				else {
					METHOD_SET_SELF_IMPL();
					DASSERT(SET_INDEX.GetType() == DeltaOperand_ConstString || SET_INDEX.IsStorageType());
#ifdef	CACHE_ENABLED
					CACHE_SET(
						OpTraits::GetMethodSelfCache(
							SET_INDEX.GetType() == DeltaOperand_ConstString
						)
					);
#endif
				}
				break;

			case DeltaValue_ExternId:  { // Indexing an extern id's user data actually.

				GET_EXTERNID_USERDATA(OpTraits::OpId(), index->ConvertToStringBySkippingUserCode().c_str());

				DELTA_RECORD_VM_SERIALNO_EX(vm);
				bool result = OpTraits::Set(DPTR(userData), *index, *content);
				DELTA_VALIDATE_VM_SERIALNO_EX(vm, UEMPTY);

				if (!result) {
					DPTR(vm)->PossiblePrimaryError(
						"failure in %s over externid '%s' with index '%s'!", 
						OpTraits::OpId(),
						container->GetExternIdTypeString().c_str(),
						index->ConvertToStringBySkippingUserCode().c_str()
					);
					return;
				}

#ifdef	CACHE_ENABLED
				if (instr->DELTA_SET_OPERAND_INDEX.IsConstStringOrNumber())
					CACHE_SET(
						OpTraits::GetExternIdCache(
							instr->DELTA_SET_OPERAND_INDEX.GetType() == DeltaOperand_ConstNumber, 
							instr->DELTA_SET_OPERAND_CONTENT.IsStorageType()
						)
					);
#endif

				break;
			}

			default:
				DELTA_ON_UNWINDING_RETURN_TEST(vm, UEMPTY);
				DPTR(vm)->SetErrorCode(DELTA_INDEXING_INVALID_VALUE_ERROR)->PrimaryError(
					"failure in %s over '%s'; not allowed for this type!",
					OpTraits::OpStr(),
					container->TypeStr()
				);
		}
	}
};

////////////////////////////////////////////////////////////////////

#ifdef	CACHE_ENABLED
#define	EXECUTE_SETGET_CACHED(setget_default)					\
	CACHE_ON_INSTRUCTION();										\
	CachedFunc f = 0x00;										\
	if (uvoid2func(instr->GetExecutor(), f))					\
		(*f)(instr, vm);										\
	else														\
		setget_default(instr, vm);
#else
#define	EXECUTE_SETGET_CACHED(setget_default)					\
		setget_default(instr, vm);
#endif

#define	CHACHE_SET_TOTAL_FUNCS					4
#define	CHACHE_METHOD_SET_SELF_TOTAL_FUNCS		2
#define	CACHE_SET_INDEX(is_num , is_storage)	((is_num << 1) + (is_storage))
#define	B2N(b)									(b ? 1 : 0)

#define	SET_TRAITS_IMPL(_class, _func, _opstr, _opid)															\
struct _class {																									\
	static CachedFunc	cachedTableSetFuncs[CHACHE_SET_TOTAL_FUNCS];											\
	static CachedFunc	cachedExternIdSetFuncs[CHACHE_SET_TOTAL_FUNCS];											\
	static CachedFunc	cachedMethodSelfFuncs[CHACHE_METHOD_SET_SELF_TOTAL_FUNCS];								\
	static CachedFunc	GetTableCache (bool is_num, bool is_storage)											\
							{ return cachedTableSetFuncs[CACHE_SET_INDEX(B2N(is_num), B2N(is_storage))]; }		\
	static CachedFunc	GetExternIdCache (bool is_num, bool is_storage)											\
							{ return cachedExternIdSetFuncs[CACHE_SET_INDEX(B2N(is_num), B2N(is_storage))]; }	\
	static CachedFunc	GetMethodSelfCache (bool is_const_string)												\
							{ return cachedMethodSelfFuncs[B2N(is_const_string)]; }								\
	static bool			Set (DeltaTable* t, DeltaValue& i, DeltaValue& c)										\
							{ return DPTR(t)->_func(i, c); }													\
	static const char*	OpStr (void)																			\
							{ return _opstr; }																	\
	static const char*	OpId (void)																				\
							{ return _opid; }																	\
};																												\
CachedFunc _class::cachedTableSetFuncs[CHACHE_SET_TOTAL_FUNCS] = {												\
	Table##_func##ConstStringContentNotVarCached,																\
	Table##_func##ConstStringContentVarCached,																	\
	Table##_func##ConstNumberContentNotVarCached,																\
	Table##_func##ConstNumberContentVarCached																	\
};																												\
CachedFunc _class::cachedExternIdSetFuncs[CHACHE_SET_TOTAL_FUNCS] = {											\
	ExternId##_func##ConstStringContentNotVarCached,															\
	ExternId##_func##ConstStringContentVarCached,																\
	ExternId##_func##ConstNumberContentNotVarCached,															\
	ExternId##_func##ConstNumberContentVarCached																\
};																												\
CachedFunc _class::cachedMethodSelfFuncs[CHACHE_METHOD_SET_SELF_TOTAL_FUNCS] = {								\
	Method##_func##SelfUserVarCached,																			\
	Method##_func##SelfConstStringIndexCached																	\
};

SET_TRAITS_IMPL(SetTraits, Set, "'[]=' or '.='", "set")
SET_TRAITS_IMPL(BoundedSetTraits, BoundedSet, "'[[]]=' or '..='", "bounded set")

void Execute_OBJSET (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXECUTE_SETGET_CACHED(SetElemTemplate<SetTraits>::Do); }

void Execute_BOUNDEDOBJSET (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXECUTE_SETGET_CACHED(SetElemTemplate<BoundedSetTraits>::Do); }

//------------------------------------------------------------------
// The following is for use by the source-level debugger.
// No overloading is applied, neither inheritance lookup - only
// bounded (local) lookup.
//
void Execute_OBJGET (
		DeltaValue*	table,
		DeltaValue*	index,
		DeltaValue*	result,
		void		(*callback)(const char*)
	) {
	if (table->Type() != DeltaValue_Table)
		(*callback)(
			uconstructstr(
				"Type '%s' used with index '%s', 'Object' expected!", 
				table->TypeStr(),
				index->ConvertToString().c_str()
			).c_str()
		);
	else
	if (!DPTR(table->ToTable())->DeltaTable::BasicGet(*index, result))
		(*callback)(uerror::GetSingleton().getreport().c_str());
}

//////////////////////////////////////////////////////////////////////
// GET
#define	IMPLEMENT_CACHED_GET(																\
		_default_exec,																		\
		_container_type,																	\
		_extract_table_from_container,														\
		_table_expr,																		\
		_operand_assertions,																\
		_get_func,																			\
		_get_func_name,																		\
		_container_equal_content_handler,													\
		_get_index_var,																		\
		_use_index,																			\
		_index_to_string																	\
	)																						\
	_operand_assertions;																	\
	DELTA_GET_CONTAINER_OPERAND_FAST(DELTA_GET_OPERAND_CONTAINER, container);				\
	DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_GET_OPERAND_CONTENT, content);					\
	_container_equal_content_handler;														\
	_get_index_var;																			\
	if (container->Type() != _container_type) {												\
		CACHE_MISS();																		\
		Execute_##_default_exec(instr, vm);													\
	}																						\
	else {																					\
		CACHE_HIT();																		\
		_extract_table_from_container(_get_func_name, _index_to_string);					\
		if (!DPTR(_table_expr)->_get_func(_use_index, content))								\
			DPTR(vm)->PossiblePrimaryError(													\
				"failure in %s over '%s' " DELTA_OBJECT_SN_FORMAT " with index '%s'!",			\
				_get_func_name,																\
				container->TypeStr(),														\
				DPTR(_table_expr)->GetSerialNo(),											\
				(_index_to_string)															\
			);																				\
	}

//////////////////////////////////////////////////////////////////////

#define	CONTENT_IS_TEMPVAR_ASSERTION			\
			DASSERT(instr->DELTA_GET_OPERAND_CONTENT.IsTempVar());

#define	CONTENT_NOT_TEMPVAR_ASSERTION			\
			DASSERT(!instr->DELTA_GET_OPERAND_CONTENT.IsTempVar());

#define	GET_INDEX_CONST_STRING_ASSERTION		\
			DASSERT(instr->DELTA_GET_OPERAND_INDEX.GetType() == DeltaOperand_ConstString);

#define	GET_INDEX_CONST_NUMBER_ASSERTION		\
			DASSERT(instr->DELTA_GET_OPERAND_INDEX.GetType() == DeltaOperand_ConstNumber);

#define	CONTAINER_CONTENT_EQUAL_HANDLER			\
			DASSERT(container == content); DeltaValue temp2(container); container = &temp2;

#define	CONTAINER_CONTENT_NOT_EQUAL_HANDLER		\
			DASSERT(container != content);

#define	GET_INDEX								\
			instr->DELTA_GET_OPERAND_INDEX

//////////////////////////////////////////////////////////////////////

#define	TABLE_GET_CONSTSTRING_INDEX(_default_exec, _func, _func_name, _temp_assert, _content_container_eq)	\
	IMPLEMENT_CACHED_GET(																					\
		_default_exec,																						\
		DeltaValue_Table,																					\
		GET_VALUE_TABLE,																					\
		container->ToTable(),																				\
		_temp_assert GET_INDEX_CONST_STRING_ASSERTION,														\
		_func,																								\
		_func_name,																							\
		_content_container_eq,																				\
		UEMPTY,																								\
		(const char*) GET_INDEX.GetValue(),																	\
		(const char*) GET_INDEX.GetValue()																	\
	)

void TableGetConstStringUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTSTRING_INDEX(OBJGET, Get, "get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }
	
void TableGetConstStringUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTSTRING_INDEX(OBJGET, Get, "get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void TableBoundedGetConstStringUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTSTRING_INDEX(BOUNDEDOBJGET, BoundedGet, "bounded get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }

void TableBoundedGetConstStringUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTSTRING_INDEX(BOUNDEDOBJGET, BoundedGet, "bounded get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void TableGetConstStringTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTSTRING_INDEX(OBJGET, GetByUnoverloadedAssign, "get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }

void TableGetConstStringTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTSTRING_INDEX(OBJGET, GetByUnoverloadedAssign, "get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void TableBoundedGetConstStringTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTSTRING_INDEX(BOUNDEDOBJGET, BoundedGetByUnoverloadedAssign, "bounded get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }
	
void TableBoundedGetConstStringTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTSTRING_INDEX(BOUNDEDOBJGET, BoundedGetByUnoverloadedAssign, "bounded get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

//////////////////////////////////////////////////////////////////////

#define	EXTERNID_GET_CONSTSTRING_INDEX(_default_exec, _func, _func_name, _temp_assert, _content_container_eq)	\
	IMPLEMENT_CACHED_GET(																						\
		_default_exec,																							\
		DeltaValue_ExternId,																					\
		GET_EXTERNID_USERDATA,																					\
		userData,																								\
		_temp_assert GET_INDEX_CONST_STRING_ASSERTION,															\
		_func,																									\
		_func_name,																								\
		_content_container_eq,																					\
		UEMPTY,																									\
		(const char*) GET_INDEX.GetValue(),																		\
		(const char*) GET_INDEX.GetValue()																		\
	)

void ExternIdGetConstStringUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTSTRING_INDEX(OBJGET, Get, "get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }
	
void ExternIdGetConstStringUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTSTRING_INDEX(OBJGET, Get, "get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void ExternIdBoundedGetConstStringUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTSTRING_INDEX(BOUNDEDOBJGET, BoundedGet, "bounded get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }

void ExternIdBoundedGetConstStringUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTSTRING_INDEX(BOUNDEDOBJGET, BoundedGet, "bounded get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void ExternIdGetConstStringTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTSTRING_INDEX(OBJGET, GetByUnoverloadedAssign, "get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }

void ExternIdGetConstStringTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTSTRING_INDEX(OBJGET, GetByUnoverloadedAssign, "get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void ExternIdBoundedGetConstStringTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTSTRING_INDEX(BOUNDEDOBJGET, BoundedGetByUnoverloadedAssign, "bounded get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }
	
void ExternIdBoundedGetConstStringTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTSTRING_INDEX(BOUNDEDOBJGET, BoundedGetByUnoverloadedAssign, "bounded get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

//////////////////////////////////////////////////////////////////////

#define	TABLE_GET_CONSTNUMBER_INDEX(_default_exec, _func, _func_name, _temp_assert, _content_container_eq)	\
	IMPLEMENT_CACHED_GET(																					\
		_default_exec,																						\
		DeltaValue_Table,																					\
		GET_VALUE_TABLE,																					\
		container->ToTable(),																				\
		_temp_assert GET_INDEX_CONST_NUMBER_ASSERTION,														\
		Get,																								\
		"get",																								\
		_content_container_eq,																				\
		UEMPTY,																								\
		*((DeltaNumberValueType*) GET_INDEX.GetValue()),													\
		unum2string(*((DeltaNumberValueType*) GET_INDEX.GetValue())).c_str()								\
	)

void TableGetConstNumberUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTNUMBER_INDEX(OBJGET, Get, "get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }
	
void TableGetConstNumberUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTNUMBER_INDEX(OBJGET, Get, "get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void TableBoundedGetConstNumberUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTNUMBER_INDEX(BOUNDEDOBJGET, BoundedGet, "bounded get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }

void TableBoundedGetConstNumberUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ TABLE_GET_CONSTNUMBER_INDEX(BOUNDEDOBJGET, BoundedGet, "bounded get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void TableGetConstNumberTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ TABLE_GET_CONSTNUMBER_INDEX(OBJGET, GetByUnoverloadedAssign, "get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }
	
void TableGetConstNumberTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ TABLE_GET_CONSTNUMBER_INDEX(OBJGET, GetByUnoverloadedAssign, "get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void TableBoundedGetConstNumberTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ TABLE_GET_CONSTNUMBER_INDEX(BOUNDEDOBJGET, BoundedGetByUnoverloadedAssign, "bounded get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }

void TableBoundedGetConstNumberTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ TABLE_GET_CONSTNUMBER_INDEX(BOUNDEDOBJGET, BoundedGetByUnoverloadedAssign, "bounded get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

//////////////////////////////////////////////////////////////////////

#define	EXTERNID_GET_CONSTNUMBER_INDEX(_default_exec, _func, _func_name, _temp_assert, _content_container_eq)	\
	IMPLEMENT_CACHED_GET(																						\
		_default_exec,																							\
		DeltaValue_ExternId,																					\
		GET_EXTERNID_USERDATA,																					\
		userData,																								\
		_temp_assert GET_INDEX_CONST_NUMBER_ASSERTION,															\
		Get,																									\
		"get",																									\
		_content_container_eq,																					\
		UEMPTY,																									\
		*((DeltaNumberValueType*) GET_INDEX.GetValue()),														\
		unum2string(*((DeltaNumberValueType*) GET_INDEX.GetValue())).c_str()									\
	)

void ExternIdGetConstNumberUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTNUMBER_INDEX(OBJGET, Get, "get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }
	
void ExternIdGetConstNumberUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTNUMBER_INDEX(OBJGET, Get, "get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void ExternIdBoundedGetConstNumberUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTNUMBER_INDEX(BOUNDEDOBJGET, BoundedGet, "bounded get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }

void ExternIdBoundedGetConstNumberUserVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXTERNID_GET_CONSTNUMBER_INDEX(BOUNDEDOBJGET, BoundedGet, "bounded get", CONTENT_NOT_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void ExternIdGetConstNumberTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ EXTERNID_GET_CONSTNUMBER_INDEX(OBJGET, GetByUnoverloadedAssign, "get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }
	
void ExternIdGetConstNumberTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ EXTERNID_GET_CONSTNUMBER_INDEX(OBJGET, GetByUnoverloadedAssign, "get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

void ExternIdBoundedGetConstNumberTempVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ EXTERNID_GET_CONSTNUMBER_INDEX(BOUNDEDOBJGET, BoundedGetByUnoverloadedAssign, "bounded get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_NOT_EQUAL_HANDLER); }

void ExternIdBoundedGetConstNumberTempVarContentEqualContainerCached (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ EXTERNID_GET_CONSTNUMBER_INDEX(BOUNDEDOBJGET, BoundedGetByUnoverloadedAssign, "bounded get", CONTENT_IS_TEMPVAR_ASSERTION, CONTAINER_CONTENT_EQUAL_HANDLER); }

//////////////////////////////////////////////////////////////////////

#define	METHOD_GET_SELF_IMPL()																\
	if (instr->DELTA_GET_OPERAND_CONTENT.IsTempVar())										\
		content->FromTable(container->GetMethodFuncSelf());									\
	else																					\
		content->AssignOverloadable(container->GetMethodFuncSelf())

//**********************************

#define	IMPLEMENT_CACHED_METHOD_GET_SELF(													\
		_default_exec,																		\
		_opid,																				\
		_operand_assertions,																\
		_extract_and_check_index															\
	)																						\
	_operand_assertions;																	\
	DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_GET_OPERAND_CONTAINER, container);				\
	if (container->Type() != DeltaValue_MethodFunc) {										\
		CACHE_MISS();																		\
		Execute_##_default_exec(instr, vm);													\
	}																						\
	else {																					\
		CACHE_HIT();																		\
		DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_GET_OPERAND_CONTENT, content);				\
		_extract_and_check_index(_opid);													\
		METHOD_GET_SELF_IMPL();																\
	}
		
#define	METHOD_GET_SELF_INDEX_STORAGE_ASSERTION												\
		DASSERT(GET_INDEX.IsStorageType());

#define	METHOD_GET_SELF_INDEX_CONST_STRING_ASSERTION										\
		DASSERT(GET_INDEX.GetType() == DeltaOperand_ConstString);							\
		DASSERT(!strcmp((const char*) GET_INDEX.GetValue(), DELTA_SELF_POINTER_ID));

#define	METHOD_GET_SELF_INDEX_CONST_STRING_EXTRACT_AND_CHECK(_opdid)

#define	METHOD_GET_SELF_INDEX_USER_VAR_EXTRACT_AND_CHECK(_opid)								\
		DELTA_TRANSLATE_STORAGE_OPERAND(DELTA_GET_OPERAND_INDEX, index);					\
		METHOD_CHECK_SELF_INDEX(index, _opid);
		
//**********************************

void MethodGetSelfConstStringIndexCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_METHOD_GET_SELF(
		OBJGET,
		"get",
		METHOD_GET_SELF_INDEX_CONST_STRING_ASSERTION,
		METHOD_GET_SELF_INDEX_CONST_STRING_EXTRACT_AND_CHECK
	);
}

void MethodGetSelfUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_METHOD_GET_SELF(
		OBJGET,
		"get",
		METHOD_GET_SELF_INDEX_STORAGE_ASSERTION,
		METHOD_GET_SELF_INDEX_USER_VAR_EXTRACT_AND_CHECK
	);
}

void MethodBoundedGetSelfConstStringIndexCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_METHOD_GET_SELF(
		BOUNDEDOBJGET,
		"bounded get",
		METHOD_GET_SELF_INDEX_CONST_STRING_ASSERTION,
		METHOD_GET_SELF_INDEX_CONST_STRING_EXTRACT_AND_CHECK
	);
}

void MethodBoundedGetSelfUserVarCached (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_METHOD_GET_SELF(
		BOUNDEDOBJGET,
		"bounded get",
		METHOD_GET_SELF_INDEX_STORAGE_ASSERTION,
		METHOD_GET_SELF_INDEX_USER_VAR_EXTRACT_AND_CHECK
	);
}
//////////////////////////////////////////////////////////////////////

template <class OpTraits> struct GetElemTemplate {
	
	static void Do (
			DeltaInstruction*		instr, 
			DeltaVirtualMachine*	vm, 
			// Those two args below are used only when we have a method invocation.
			DeltaValue*				contentHere		= (DeltaValue*) 0,
			DeltaValue*				containerHere	= (DeltaValue*) 0
		) {

		DASSERT(!contentHere || instr->DELTA_GET_OPERAND_CONTENT.GetType() == DeltaOperand_NotUsed);
		
		DELTA_GET_CONTAINER_OPERAND(DELTA_GET_OPERAND_CONTAINER, container);
		if (containerHere)
			containerHere->Assign(container);
		DeltaValue* content = contentHere? contentHere : DPTR(vm)->GetStorageObject(instr->DELTA_GET_OPERAND_CONTENT);
		if (!content)
			{ DASSERT(DPTR(vm)->HasProducedError()); return; }

		DELTA_TRANSLATE_OPERAND(DELTA_GET_OPERAND_INDEX, index, temp1);
		
		// In t = t.x, i.e. t is content, t may be collected after get, while still having
		// container methods on the call stack, causing a crash. So we use a temp var. We also 
		// choose to copy the container var to avoid copying back to the content var.
		//
		DeltaValue temp2;
		if (container == content)
			{ temp2.Assign(container); container = &temp2; DASSERT(!temp2.IsUndefined()); }
		
		// Either method invocation or assignment to a temp var.
		bool useGetByUnoverloadedAssign = contentHere || instr->DELTA_GET_OPERAND_CONTENT.IsTempVar();	

		switch (container->Type()) {

			case DeltaValue_Table: {

				DELTA_RECORD_VM_SERIALNO_EX(vm);
				bool result = OpTraits::Get(DPTR(container->ToTable()), *index, content, useGetByUnoverloadedAssign);
				DELTA_VALIDATE_VM_SERIALNO_EX(vm, UEMPTY);

				if (!result) {
					DPTR(vm)->PossiblePrimaryError(
						"failure in %s over '%s' " DELTA_OBJECT_SN_FORMAT " with index '%s'!", 
						OpTraits::OpId(), 
						container->TypeStr(),
						container->ToTable()->GetSerialNo(),
						index->ConvertToStringBySkippingUserCode().c_str()
					);
					return;
				}

#ifdef	CACHE_ENABLED
				if (!contentHere) {		// GET / BOUNDED GET
					DASSERT(!containerHere);
					if (instr->DELTA_GET_OPERAND_INDEX.IsConstStringOrNumber())	
						CACHE_SET(
							OpTraits::GetTableCache(
								instr->DELTA_GET_OPERAND_INDEX.GetType() == DeltaOperand_ConstNumber, 
								instr->DELTA_GET_OPERAND_CONTENT.IsTempVar(), 
								instr->DELTA_GET_OPERAND_CONTENT == instr->DELTA_GET_OPERAND_CONTAINER
							)
						);
				}
				else {					// CALL METHOD
					DASSERT(containerHere);
					if (instr->DELTA_GET_OPERAND_INDEX.GetType() == DeltaOperand_ConstString)
						CACHE_SET(OpTraits::GetCallMethodCache(false));
				}
#endif
				break;
			}

			case DeltaValue_MethodFunc:		// Indexing a method.
				METHOD_CHECK_SELF_INDEX(index, OpTraits::OpStr());
				METHOD_GET_SELF_IMPL();		// Fallback from here means operation succeeded.
				DASSERT(!DPTR(vm)->HasProducedError());

#ifdef	CACHE_ENABLED
				CACHE_SET(
					OpTraits::GetMethodSelfCache(
						GET_INDEX.GetType() == DeltaOperand_ConstString
					)
				);
#endif
				break;

			case DeltaValue_ExternId: { // Indexing an extern id's user data actually.

				GET_EXTERNID_USERDATA(OpTraits::OpId(), index->ConvertToStringBySkippingUserCode().c_str());				
				DELTA_RECORD_VM_SERIALNO_EX(vm);
				bool result = OpTraits::Get(DPTR(userData), *index, content, useGetByUnoverloadedAssign);
				DELTA_VALIDATE_VM_SERIALNO_EX(vm, UEMPTY);

				if (!result) {
					DPTR(vm)->PossiblePrimaryError(
						"failure in %s over externid '%s' (userdata " DELTA_OBJECT_SN_FORMAT ") with index '%s'!", 
						OpTraits::OpId(),
						container->GetExternIdTypeString().c_str(),
						userData->GetSerialNo(),
						index->ConvertToStringBySkippingUserCode().c_str()
					);
					return;
				}

#ifdef	CACHE_ENABLED
				if (!contentHere) {		// GET / BOUNDEDGET
					DASSERT(!containerHere);
					if (instr->DELTA_GET_OPERAND_INDEX.IsConstStringOrNumber())	
						CACHE_SET(
							OpTraits::GetExternIdCache(
								instr->DELTA_GET_OPERAND_INDEX.GetType() == DeltaOperand_ConstNumber, 
								instr->DELTA_GET_OPERAND_CONTENT.IsTempVar(), 
								instr->DELTA_GET_OPERAND_CONTENT == instr->DELTA_GET_OPERAND_CONTAINER
							)
						);
				}
				else {
					DASSERT(containerHere);	// GETMETHOD / BOUNDEDGETMETHOD
					if (instr->DELTA_GET_OPERAND_INDEX.GetType() == DeltaOperand_ConstString)
						CACHE_SET(OpTraits::GetCallMethodCache(true));
				}
#endif

				break;
			}
			default : {	// In this case it is an illegal type used as a container.
				DELTA_ON_UNWINDING_RETURN_TEST(vm, UEMPTY);
				content->FromNil();
				DPTR(vm)->PrimaryError(
					"failure in %s over '%s'; not allowed for this type!",
					OpTraits::OpStr(),
					container->TypeStr()
				);
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////

#define	CACHE_GET_INDEX(is_num , is_temp, is_equal)	((is_num << 2) + (is_temp << 1) + is_equal)
#define	CHACHE_GET_TOTAL_FUNCS					8
#define	CHACHE_METHOD_GET_SELF_TOTAL_FUNCS		2

#define	GET_TRAITS_IMPL(_class, _func, _opstr, _opid)																		\
struct _class {																												\
	static CachedFunc	cachedTableGetFuncs[CHACHE_GET_TOTAL_FUNCS];														\
	static CachedFunc	cachedExternIdGetFuncs[CHACHE_GET_TOTAL_FUNCS];														\
	static CachedFunc	cachedMethodSelfFuncs[CHACHE_METHOD_GET_SELF_TOTAL_FUNCS];											\
	static CachedFunc	GetTableCache (bool is_num, bool is_temp, bool is_equal)											\
							{ return cachedTableGetFuncs[CACHE_GET_INDEX(B2N(is_num), B2N(is_temp), B2N(is_equal))]; }		\
	static CachedFunc	GetExternIdCache (bool is_num, bool is_temp, bool is_equal)											\
							{ return cachedExternIdGetFuncs[CACHE_GET_INDEX(B2N(is_num), B2N(is_temp), B2N(is_equal))]; }	\
	static CachedFunc	GetCallMethodCache (bool is_extern_id)																\
							{ return is_extern_id ? ExternIdCall##_func##Method : TableCall##_func##Method; }				\
	static CachedFunc	GetMethodSelfCache (bool is_const_string)															\
							{ return cachedMethodSelfFuncs[B2N(is_const_string)]; }											\
	static bool			Get (DeltaTable* t, DeltaValue& i, DeltaValue* c, bool unoverloaded)								\
							{ return unoverloaded ? DPTR(t)->_func##ByUnoverloadedAssign(i, c) : DPTR(t)->_func(i, c); }	\
	static const char*	OpStr (void)																						\
							{ return _opstr; }																				\
	static const char*	OpId (void)																							\
							{ return _opid ; }																				\
};																															\
CachedFunc _class::cachedTableGetFuncs[CHACHE_GET_TOTAL_FUNCS] = {															\
	Table##_func##ConstStringUserVarCached,																					\
	Table##_func##ConstStringUserVarContentEqualContainerCached,															\
	Table##_func##ConstStringTempVarCached,																					\
	Table##_func##ConstStringTempVarContentEqualContainerCached,															\
	Table##_func##ConstNumberUserVarCached,																					\
	Table##_func##ConstNumberUserVarContentEqualContainerCached,															\
	Table##_func##ConstNumberTempVarCached,																					\
	Table##_func##ConstNumberTempVarContentEqualContainerCached																\
};																															\
CachedFunc _class::cachedExternIdGetFuncs[CHACHE_GET_TOTAL_FUNCS] = {														\
	ExternId##_func##ConstStringUserVarCached,																				\
	ExternId##_func##ConstStringUserVarContentEqualContainerCached,															\
	ExternId##_func##ConstStringTempVarCached,																				\
	ExternId##_func##ConstStringTempVarContentEqualContainerCached,															\
	ExternId##_func##ConstNumberUserVarCached,																				\
	ExternId##_func##ConstNumberUserVarContentEqualContainerCached,															\
	ExternId##_func##ConstNumberTempVarCached,																				\
	ExternId##_func##ConstNumberTempVarContentEqualContainerCached															\
};																															\
CachedFunc _class::cachedMethodSelfFuncs[CHACHE_METHOD_GET_SELF_TOTAL_FUNCS] = {											\
	Method##_func##SelfUserVarCached,																						\
	Method##_func##SelfConstStringIndexCached																				\
};

GET_TRAITS_IMPL(GetTraits, Get, "'[]' or '.'", "get")
GET_TRAITS_IMPL(BoundedGetTraits, BoundedGet, "'[[]]' or '..'", "bounded get")

void Execute_OBJGET (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXECUTE_SETGET_CACHED(GetElemTemplate<GetTraits>::Do); }

void Execute_BOUNDEDOBJGET (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ EXECUTE_SETGET_CACHED(GetElemTemplate<BoundedGetTraits>::Do); }

//------------------------------------------------------------------

void Execute_OBJNEWATTR (DeltaInstruction* instr, DeltaVirtualMachine* vm) {

	DASSERT(instr->DELTA_OBJNEWATTR_OPERAND_NAME.GetType() == DeltaOperand_ConstString); // Carries the attribute id
	DeltaTable* obj = DNULLCHECK(DPTR(vm)->GetCurrConstructedObject());	// Added in the current table always.

	DELTA_TRANSLATE_OPERAND(DELTA_OBJNEWATTR_OPERAND_SETTER, setter, temp1);
	DELTA_TRANSLATE_OPERAND(DELTA_OBJNEWATTR_OPERAND_GETTER, getter, temp2);
	
	if (!DPTR(obj)->NewAttribute((char*) instr->DELTA_OBJNEWATTR_OPERAND_NAME.GetValue(), *setter, *getter)) {
		DPTR(vm)->SetErrorCode(DELTA_OBJECT_ATTRIBUTE_OPERATION_ERROR)->PrimaryError(
			"in %s new attribute '%s' failed!", 
			obj->GetExtClassString(),
			(const char*) instr->DELTA_OBJNEWATTR_OPERAND_NAME.GetValue()
		);
	}
}

//////////////////////////////////////////////////////////////////////

#define	ATTR_EXTRACT_TABLE(arg)																	\
	DASSERT(instr->DELTA_OBJSETGETATTR_OPERAND_NAME.GetType() == DeltaOperand_ConstString);		\
	DeltaValue*	t		= DPTR(vm)->GetStorageObject(instr->arg);								\
	DeltaTable*	self	= t->ToTable();															\
	DASSERT(self == DPTR(vm)->GetSelfForCurrMethod())											\

#define	ATTRSET_EXTRACT_VALUE																	\
	DELTA_TRANSLATE_OPERAND(DELTA_OBJSETATTR_OPERAND_CONTENT, value, temp)

#define ATTRGET_EXTRACT_CONTENT																	\
	DeltaValue* content;																		\
	if (!(content = DPTR(vm)->GetStorageObject(instr->DELTA_OBJGETATTR_OPERAND_CONTENT)))		\
		{ DASSERT(vm->HasProducedError()); return; }											\
	else;
		

#define	ATTR_NAME()																				\
	((const char*) instr->DELTA_OBJSETGETATTR_OPERAND_NAME.GetValue())

#define	ATTRSET_APPLY()																			\
	(DPTR(self)->SetAttribute(ATTR_NAME(), value))

#define	ATTRGET_APPLY()																			\
	(	instr->DELTA_OBJGETATTR_OPERAND_CONTENT.IsTempVar()					?					\
		DPTR(self)->GetAttributeByUnoverloadedAssign(ATTR_NAME(), content)	:					\
		DPTR(self)->GetAttribute(ATTR_NAME(), content)	)

#define	ATTR_SET_GET_IMPL(_table_arg, _extract_op, _apply_op, _op_name)							\
	ATTR_EXTRACT_TABLE(_table_arg);																\
	_extract_op;																				\
	if (!_apply_op)																				\
		DPTR(vm)->PossiblePrimaryError("in '%s' failed to %s attribute '%s'!",					\
			self->GetExtClassString(),															\
			_op_name,																			\
			ATTR_NAME()																			\
		);

//////////////////////////////////////////////////////////////////////

void Execute_OBJSETATTR (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ ATTR_SET_GET_IMPL(DELTA_OBJSETATTR_OPERAND_OBJECT, ATTRSET_EXTRACT_VALUE, ATTRSET_APPLY(), "set"); }

void Execute_OBJGETATTR (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ ATTR_SET_GET_IMPL(DELTA_OBJGETATTR_OPERAND_OBJECT, ATTRGET_EXTRACT_CONTENT, ATTRGET_APPLY(), "get"); }

//------------------------------------------------------------------
// CALL METHOD

#define	CALL_METHOD_FINALLY_INVOKE(_caller, _method, _is_bounded)									\
	DASSERT(!DPTR(vm)->HasProducedError()); 														\
	if (method.Type() != DeltaValue_LibraryFunc ||													\
		DeltaLibraryFuncArgsBinder::PushCallerIfLibraryMethod(_caller, _method, vm, _is_bounded))	\
			DPTR(vm)->CallFunction(&method); else				

#ifdef	CACHE_ENABLED
#define	EXECUTE_CALLMETHOD_CACHED_CHECK()						\
	CACHE_ON_INSTRUCTION();										\
	CachedFunc f = 0x00;										\
	if (uvoid2func(instr->GetExecutor(), f))					\
		{ (*f)(instr, vm); return; }							\
	else
#else
#define	EXECUTE_CALLMETHOD_CACHED_CHECK()
#endif

#define	IMPLEMENT_CACHED_CALL_METHOD(																	\
		_default_exec,																					\
		_container_type,																				\
		_container_type_str,																			\
		_extract_table_from_container,																	\
		_table_expr,																					\
		_get_func,																						\
		_get_func_name,																					\
		_is_bounded																						\
	)																									\
	GET_INDEX_CONST_STRING_ASSERTION;																	\
	DELTA_GET_CONTAINER_OPERAND_FAST(DELTA_GET_OPERAND_CONTAINER, container);							\
	if (container->Type() == _container_type) {															\
		CACHE_HIT();																					\
		_extract_table_from_container(_get_func_name, (const char*) GET_INDEX.GetValue());				\
		DeltaValue method;																				\
		{	/* Necessary block - do not remove! */														\
			DeltaVirtualMachine::ArgumentsPreserver preserver(vm);										\
			DELTA_RECORD_VM_SERIALNO_EX(vm);															\
			DELTA_RECORD_EXECUTION_LOOP_PASSPORT(vm);													\
			if (!DPTR(_table_expr)->_get_func((const char*) GET_INDEX.GetValue(), &method))	{			\
				DPTR(vm)->PossiblePrimaryError(															\
					"failure in %s over '%s' " DELTA_OBJECT_SN_FORMAT " with method name '%s'!",				\
					_get_func_name,																		\
					_container_type_str,																\
					DPTR(_table_expr)->GetSerialNo(),													\
					(const char*) GET_INDEX.GetValue()													\
				);																						\
				return;																					\
			}																							\
			DELTA_VALIDATE_VM_SERIALNO_EX_RETURN_VOID(vm);												\
			DELTA_VALIDATE_EXECUTION_LOOP_PASSPORT_RETURN_VOID(vm);										\
			preserver.Restore();																		\
		}																								\
		CALL_METHOD_FINALLY_INVOKE(container, method, _is_bounded);										\
	} else {																							\
		CACHE_MISS();																					\
		Execute_##_default_exec(instr, vm);																\
	}


//////////////////////////////////////////////////////////////////////

void TableCallGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_CALL_METHOD(
		CALLOBJGETMETHOD,
		DeltaValue_Table,
		container->TypeStr(),
		UNOTHING2,
		container->ToTable(),
		GetByUnoverloadedAssign,
		"call method",
		false
	);
}

void TableCallBoundedGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_CALL_METHOD(
		CALLOBJBOUNDEDGETMETHOD,
		DeltaValue_Table,
		container->TypeStr(),
		UNOTHING2,
		container->ToTable(),
		BoundedGetByUnoverloadedAssign,
		"call bounded method",
		true
	);
}

void ExternIdCallGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_CALL_METHOD(
		CALLOBJGETMETHOD,
		DeltaValue_ExternId,
		(container->GetExternIdTypeString() + "(userdata)").c_str(),
		GET_EXTERNID_USERDATA,
		userData,
		GetByUnoverloadedAssign,
		"call method",
		false
	);
}

void ExternIdCallBoundedGetMethod (DeltaInstruction* instr, DeltaVirtualMachine* vm) {
	IMPLEMENT_CACHED_CALL_METHOD(
		CALLOBJBOUNDEDGETMETHOD,
		DeltaValue_ExternId,
		(container->GetExternIdTypeString() + "(userdata)").c_str(),
		GET_EXTERNID_USERDATA,
		userData,
		BoundedGetByUnoverloadedAssign,
		"call bounded method",
		true
	);
}

//////////////////////////////////////////////////////////////////////
// The arguments preserver must be always in a separate block to ensure it is
// destroyed before we artifically push the caller.

#define	CALL_METHOD_FINALLY_NORMAL(traits)															\
	DELTA_INVOCATION_LEADING_TO_EXECUTION_LOOP_RETURN_VOID(											\
		GetElemTemplate<traits>::Do(instr, vm, &method, &caller),									\
		vm																							\
	);																								\
	if (EXCEPTION_HANDLERS->IsUnwinding()) return; else

#define	CALL_METHOD_FINALLY_FAST(traits)															\
	DELTA_CORE_EXECUTION_LOOP(																		\
		sn,																							\
		passport,																					\
		GetElemTemplate<traits>::Do(instr, vm, &method, &caller),									\
		vm,																							\
		return																						\
	);																								\
	if (EXCEPTION_HANDLERS->IsUnwinding()) return; else

#define	CALL_METHOD(traits, is_bounded, call)														\
	EXECUTE_CALLMETHOD_CACHED_CHECK(); 																\
	DeltaValue caller, method;																		\
	{ /* Necessary block - do not remove! */														\
		DeltaVirtualMachine::ArgumentsPreserver preserver(vm);										\
		{ call(traits);	}																			\
		preserver.Restore();																		\
	}																								\
	CALL_METHOD_FINALLY_INVOKE(caller, method, is_bounded);

void Execute_CALLOBJGETMETHOD (DeltaInstruction* instr, DeltaVirtualMachine* vm) 
	{ CALL_METHOD(GetTraits, false, CALL_METHOD_FINALLY_NORMAL); }

void Execute_CALLOBJBOUNDEDGETMETHOD (DeltaInstruction* instr, DeltaVirtualMachine* vm)
	{ CALL_METHOD(BoundedGetTraits, true, CALL_METHOD_FINALLY_NORMAL); }
	
void Execute_CALLOBJGETMETHOD (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport) 
	{ CALL_METHOD(GetTraits, false, CALL_METHOD_FINALLY_FAST); }

void Execute_CALLOBJBOUNDEDGETMETHOD (DeltaInstruction* instr, DeltaVirtualMachine* vm, util_ui32 sn, const util_ui32* passport)
	{ CALL_METHOD(BoundedGetTraits, true, CALL_METHOD_FINALLY_FAST); }

//------------------------------------------------------------------
