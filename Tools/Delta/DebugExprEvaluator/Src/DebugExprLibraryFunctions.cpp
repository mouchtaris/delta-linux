// DebugExprLibraryFunctions.cpp
// Support the invocation of specific unary lib funcitons.
// ScriptFighter Project.
// A. Savidis, July 2008.
//

#include "DebugExprParseActions.h"
#include "DeltaDebugCommands.h"
#include "DDebug.h"
#include "DeltaVMRegistry.h"
#include "VMRegLib.h"
#include "DeltaObjectValidator.h"
#include "DeltaTableFactory.h"
#include "DeltaExternIdFactory.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "DeltaClosure.h"
#include "DeltaVMRegistry.h"
#include "VMRegLib.h"

DebugExprParseActions::LibraryFuncMap* DebugExprParseActions::libFuncs = (LibraryFuncMap*) 0;

#define	FUNCTION_TOTAL_ARGS_WITHOUT_CLOSURE	2
#define	FUNCTION_TOTAL_ARGS_WITH_CLOSURE	3
#define	METHOD_TOTAL_ARGS_WITHOUT_CLOSURE	3
#define	METHOD_TOTAL_ARGS_WITH_CLOSURE		4

#define	CHECK_TOTAL_ARGS_EITHER(n1,n2)										\
	if (DPTR(args)->size() != n1 && DPTR(args)->size() != n2) {				\
		Error(																\
			"%s needs either %u or % uargs (%u supplied)!",					\
			_func, n1, n2, args->size()										\
		);																	\
		return (DeltaValue*) 0;												\
	}

#define	CHECK_TOTAL_ARGS(n)													\
	if (DPTR(args)->size() != n) {											\
		Error("%s needs %u args (%u supplied)!", _func, n, args->size());	\
		return (DeltaValue*) 0;												\
	}

#define	CHECK_ARG_TYPE(obj, i, type)										\
	if (DPTR(obj)->Type() != type) {										\
		Error("in %s, arg %u of type %s expected!",							\
			_func, i, DeltaValue::GetTypeStr(type)							\
		);																	\
		return (DeltaValue*) 0;												\
	}

#define	ERROR_ARG_NEITHER_TYPE(i, t1, t2)									\
	if (true) {																\
		Error("in %s, arg %u of type %s or %s expected!",					\
			_func, i,														\
			DeltaValue::GetTypeStr(t1),										\
			DeltaValue::GetTypeStr(t2)										\
		);																	\
		return (DeltaValue*) 0;												\
	} else return (DeltaValue*) 0

#define	ENTERING(func) const char* _func = func;

/////////////////////////////////////////////////////////////

void DebugExprParseActions::InstallUnaryLibraryFunctions (void) {

	DASSERT(!libFuncs);
	libFuncs = DNEW(LibraryFuncMap);

	(*DPTR(libFuncs))[DEBUG_LIBRARYFUNC_TYPEOF]			= &CallTypeOf;
	(*DPTR(libFuncs))[DEBUG_LIBRARYFUNC_ISUNDEFINED]	= &CallIsUndefined;
	(*DPTR(libFuncs))[DEBUG_LIBRARYFUNC_EXTERNIDTYPE]	= &CallExternIdType;
	(*DPTR(libFuncs))[DEBUG_LIBRARYFUNC_GETVALUETAG]	= &CallGetValueTag;

	(*DPTR(libFuncs))[DEBUG_OBJECT_ABSREF_PREFIX]		= &CallToObject;
	(*DPTR(libFuncs))[DEBUG_LIBRARYFUNC_2VM]			= &CallToVM;
	(*DPTR(libFuncs))[DEBUG_METHODFUNC_ABSREF_PREFIX]	= &CallToMethod;
	(*DPTR(libFuncs))[DEBUG_PROGRAMFUNC_ABSREF_PREFIX]	= &CallToFunction;
	(*DPTR(libFuncs))[DEBUG_EXTERNID_ABSREF_PREFIX]		= &CallToExternId;
	(*DPTR(libFuncs))[DEBUG_LIBRARYFUNC_ABSREF_PREFIX]	= &CallToLibraryFunc;
}

void DebugExprParseActions::CleanUpUnaryLibraryFunctions (void) 
	{ udelete(libFuncs); }

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::CallTypeOf (DebugExprList* args) {
	ENTERING(DEBUG_LIBRARYFUNC_TYPEOF);
	CHECK_TOTAL_ARGS(1);
	DeltaValue* arg	= DPTR(DPTR(args)->front())->GetResult();
	DeltaValue* result	= NewTempValue();
	DPTR(result)->FromString(arg->TypeStr());
	return result;
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::CallIsUndefined(DebugExprList* args) {
	ENTERING(DEBUG_LIBRARYFUNC_ISUNDEFINED);
	CHECK_TOTAL_ARGS(1);
	DeltaValue* arg	= DPTR(DPTR(args)->front())->GetResult();
	DeltaValue* result	= NewTempValue();
	DPTR(result)->FromBool(arg->Type() == DeltaValue_Undefined);
	return result;
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::CallExternIdType (DebugExprList* args) {

	ENTERING(DEBUG_LIBRARYFUNC_EXTERNIDTYPE);
	CHECK_TOTAL_ARGS(1);
	DeltaValue* arg	= DPTR(DPTR(args)->front())->GetResult();

	if (arg->Type() == DeltaValue_ExternId) {

		DeltaValue* result	= NewTempValue();

		std::string typeStr;
		arg->ToExternId(typeStr);

		DPTR(result)->FromString(typeStr);
		return result;
	}
	else
		return (DeltaValue*) 0;
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::CallGetValueTag (DebugExprList* args) {

	ENTERING(DEBUG_LIBRARYFUNC_GETVALUETAG);
	CHECK_TOTAL_ARGS(1);
	DeltaValue* arg	= DPTR(DPTR(args)->front())->GetResult();
	DeltaValue* result	= NewTempValue();
	DPTR(result)->FromString(arg->GetTypeTag());
	return result;
}

/////////////////////////////////////////////////////////////

static DeltaTable* ToObject (util_ui32 serial) {
	DeltaTable* t = DELTATABLE_VGET_INST((void*) serial);
	if (!t)
		return (DeltaTable*) 0;
	if (!DeltaTableFactory::GetSingleton().HasProduced(t))
		if (!GetDeltaObjectValidator()->Validate((DeltaObject*) t))
			return (DeltaTable*) 0;
	return t;
}

//***************************
// (num)

DeltaValue* DebugExprParseActions::CallToObject (DebugExprList* args) {

	ENTERING(DEBUG_OBJECT_ABSREF_PREFIX);
	CHECK_TOTAL_ARGS(1);

	DeltaValue* arg = DPTR(DPTR(args)->front())->GetResult();
	CHECK_ARG_TYPE(arg, 1, DeltaValue_Number);

	util_ui32 sn = (util_ui32) DPTR(arg)->ToNumber();
	DeltaTable* t = ToObject(sn);
	if (!t) {
		Error("in %s, invalid object reference %d!", _func, sn);
		return (DeltaValue*) 0;
	}
	else {
		DeltaValue* result	= NewTempValue();
		result->FromTable(t);
		return result;
	}
}

/////////////////////////////////////////////////////////////

static DeltaVirtualMachine* ToVM (util_ui32 serial) {
	DeltaVirtualMachine* vm = DELTAVM_VGET_INST((void*) serial);
	if (!vm || !VMRegistry().Exists(vm))
		return (DeltaVirtualMachine*) 0;
	else
		return vm;
}

//***************************
// (num)

DeltaValue* DebugExprParseActions::CallToVM (DebugExprList* args) {

	ENTERING("_2vm_");
	CHECK_TOTAL_ARGS(1);

	DeltaVirtualMachine*	vm	= (DeltaVirtualMachine*) 0;
	DeltaValue*			arg = DPTR(DPTR(args)->front())->GetResult();

	std::string ref;
	if (arg->Type() == DeltaValue_Number) {
		util_ui32 sn = (util_ui32) DPTR(arg)->ToNumber();
		ref = uconstructstr("reference %u", sn);
		vm = ToVM(sn);
	}
	else
	if (arg->Type() == DeltaValue_String) {
		std::string id(arg->ToString());
		ref = uconstructstr("id '%s'", id.c_str());
		vm = VMRegistry().Get(id.c_str());
	}
	else
		ERROR_ARG_NEITHER_TYPE(0, DeltaValue_Number, DeltaValue_String);

	if (!vm) {
		Error("in %s, invalid vm %s!", _func, ref.c_str());
		return (DeltaValue*) 0;
	}
	else {
		DeltaValue* result	= NewTempValue();
		DeltaVM_Make(result, vm);
		return result;
	}
}

/////////////////////////////////////////////////////////////
// (addr, table, vm, [closure])

#define	GET_OPTIONAL_CLOSURE_ARGUMENT(_total)								\
	DeltaClosure* closure = DELTA_NIL_CLOSURE;								\
	if (DPTR(args)->size() == _total) {										\
		++i, arg = DPTR(*i)->GetResult();									\
		sn = (util_ui32) DPTR(arg)->ToNumber();								\
		if (sn != VALIDATABLE_CANTHAVE_SERIALNO) {							\
			if (!(closure = CLOSURE_GET_INST(sn))) {						\
				Error("in %s, invalid closure reference %u!", _func, sn);	\
				return (DeltaValue*) 0;										\
			}																\
		}																	\
	} else

#define	CHECK_FUNCTION_OR_METHOD_ADDRESS(_typestr, _is_method)				\
	if (!DPTR(vm)->ValidateFuncAddress(addr, _is_method)) {					\
		Error("in %s, invalid %s address %d!", _func, _typestr, addr);		\
		return (DeltaValue*) 0;												\
	} else

#define	GET_VM_ARGUMENT()													\
	sn = (util_ui32) DPTR(arg)->ToNumber();									\
	DeltaVirtualMachine* vm = ToVM(sn);										\
	if (!vm) {																\
		Error("in %s, invalid vm reference %u!", _func, sn);				\
		return (DeltaValue*) 0;												\
	} else

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::CallToMethod (DebugExprList* args) {

	ENTERING(DEBUG_METHODFUNC_ABSREF_PREFIX);
	CHECK_TOTAL_ARGS_EITHER(METHOD_TOTAL_ARGS_WITHOUT_CLOSURE, METHOD_TOTAL_ARGS_WITH_CLOSURE);
	DebugExprList::iterator i = DPTR(args)->begin();

	DeltaValue* arg = DPTR(*i)->GetResult();
	CHECK_ARG_TYPE(arg, 1, DeltaValue_Number);
	DeltaCodeAddress addr = (DeltaCodeAddress) DPTR(arg)->ToNumber(); 

	++i;
	arg = DPTR(*i)->GetResult();
	CHECK_ARG_TYPE(arg, 2, DeltaValue_Number);
	util_ui32 sn = (util_ui32) DPTR(arg)->ToNumber();
	DeltaTable* t = ToObject(sn);
	if (!t) {
		Error("in %s, invalid object reference %d!", _func, sn);
		return (DeltaValue*) 0;
	}

	++i;
	arg = DPTR(*i)->GetResult();
	CHECK_ARG_TYPE(arg, 3, DeltaValue_Number);

	GET_VM_ARGUMENT();
	CHECK_FUNCTION_OR_METHOD_ADDRESS("method", true);

	GET_OPTIONAL_CLOSURE_ARGUMENT(METHOD_TOTAL_ARGS_WITH_CLOSURE);

	DeltaValue* result	= NewTempValue();
	DPTR(result)->FromMethodFunc(addr, t, vm);
	return result;
}

/////////////////////////////////////////////////////////////
// (addr, vm, [closure])
	
DeltaValue* DebugExprParseActions::CallToFunction (DebugExprList* args) {
	
	ENTERING(DEBUG_PROGRAMFUNC_ABSREF_PREFIX);
	CHECK_TOTAL_ARGS_EITHER(FUNCTION_TOTAL_ARGS_WITHOUT_CLOSURE, FUNCTION_TOTAL_ARGS_WITH_CLOSURE);
	DebugExprList::iterator i = DPTR(args)->begin();

	DeltaValue* arg = DPTR(*i)->GetResult();
	CHECK_ARG_TYPE(arg, 1, DeltaValue_Number);
	DeltaCodeAddress addr = (DeltaCodeAddress) DPTR(arg)->ToNumber(); 

	++i;
	arg = DPTR(*i)->GetResult();
	CHECK_ARG_TYPE(arg, 2, DeltaValue_Number);

	util_ui32 sn;
	GET_VM_ARGUMENT();
	CHECK_FUNCTION_OR_METHOD_ADDRESS("function", false);

	GET_OPTIONAL_CLOSURE_ARGUMENT(FUNCTION_TOTAL_ARGS_WITH_CLOSURE);

	DeltaValue* result	= NewTempValue();
	DPTR(result)->FromProgramFunc(addr, vm, closure);
	return result;
}

/////////////////////////////////////////////////////////////

static DeltaExternId* ToExternId (util_ui32 serial) {
	DeltaExternId* val = DELTA_EXTERNID_VGET_INST(serial);
	if (!val || !DeltaExternIdFactory::GetSingleton().HasProduced(val))
		return (DeltaExternId*) 0;
	else
		return val;
}

//***************************
// (num)

DeltaValue* DebugExprParseActions::CallToExternId (DebugExprList* args) {

	ENTERING(DEBUG_EXTERNID_ABSREF_PREFIX);
	CHECK_TOTAL_ARGS(1);

	DeltaValue* arg = DPTR(DPTR(args)->front())->GetResult();
	CHECK_ARG_TYPE(arg, 1, DeltaValue_Number);

	util_ui32 sn = (util_ui32) DPTR(arg)->ToNumber();
	DeltaExternId* val = ToExternId(sn);
	if (!val) {
		Error("in %s, invalid externid reference %d!", _func, sn);
		return (DeltaValue*) 0;
	}
	else {
		DeltaValue* result	= NewTempValue();
		DPTR(result)->FromExternIdBySerialNo(sn);
		return result;
	}
}

//***************************
// (libfunc, num)

DeltaValue* DebugExprParseActions::CallToLibraryFunc (DebugExprList* args) {

	ENTERING(DEBUG_LIBRARYFUNC_ABSREF_PREFIX);
	CHECK_TOTAL_ARGS(2);

	DebugExprList::iterator i = DPTR(args)->begin();
	DeltaValue* arg = DPTR(*i)->GetResult();

	CHECK_ARG_TYPE(arg, 1, DeltaValue_LibraryFunc);
	DeltaLibraryFunc f = arg->ToLibraryFunc();

	++i;
	arg = DPTR(*i)->GetResult();
	CHECK_ARG_TYPE(arg, 2, DeltaValue_Number);
	util_ui32 sn = (util_ui32) DPTR(arg)->ToNumber();
	
	DeltaLibraryFuncArgsBinder* binder = DELTALIBFUNCARGSBINDER_VGET_INST(sn);
	if (!binder) {
		Error("in %s, invalid libfunc args binder reference %d!", _func, sn);
		return (DeltaValue*) 0;
	}
	else {
		DeltaValue* result	= NewTempValue();
		DPTR(result)->FromLibraryFunc(f, binder);
		return result;
	}
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr*	DebugExprParseActions::Manage_UnaryLibraryFuncCall (const char* func, DebugExprList* args) {
	
	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();

	DeltaValue* result = (DeltaValue*) 0;
	LibraryFuncMap::iterator i = DPTR(libFuncs)->find(func);

	if (i == DPTR(libFuncs)->end())
		Error("Unsupported %s()!", func);
	else 
	if (!(result = (*i->second)(args)))
		if (!HasError())
			Error("Call %s() failed!", func);

	return result ? DebugEvaluatedExpr::New(result, false) : (DebugEvaluatedExpr*) 0;
}

/////////////////////////////////////////////////////////////

DebugExprList* DebugExprParseActions::Manage_ExpressionList (DebugEvaluatedExpr* expr) {
	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT_RET((DebugExprList*) 0);

	DebugExprList* el = EXPRLIST_LATE_DESTROY(DNEW(DebugExprList));
	DPTR(el)->push_back(expr);
	return el;
}

DebugExprList* DebugExprParseActions::Manage_ExpressionList (DebugExprList* elist, DebugEvaluatedExpr* expr) {
	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT_RET((DebugExprList*) 0);

	DPTR(elist)->push_back(DPTR(expr));
	return elist;
}

/////////////////////////////////////////////////////////////
