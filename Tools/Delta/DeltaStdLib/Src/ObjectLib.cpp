// ObjectLib.cpp
// Dynamic OOP library functions for the DeltaVirtualMachine.
// ScriptFighter Project.
// A. Savidis, December 2005.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#include "DeltaLibraryCreators.h"
#include "ObjectLib.h"

//------------------------------------------------------------------

static void inherit_LibFunc (DeltaVirtualMachine*);
static void inheritredirect_LibFunc (DeltaVirtualMachine*);
static void uninherit_LibFunc (DeltaVirtualMachine*);
static void isderived_LibFunc (DeltaVirtualMachine*);
static void getbases_LibFunc (DeltaVirtualMachine*);
static void getbase_LibFunc (DeltaVirtualMachine*);
static void getderived_LibFunc (DeltaVirtualMachine*);
static void getmostderived_LibFunc (DeltaVirtualMachine*);

static void	delegate_LibFunc (DeltaVirtualMachine*);
static void	undelegate_LibFunc (DeltaVirtualMachine*);
static void	isdelegate_LibFunc (DeltaVirtualMachine*);
static void	isdirectdelegate_LibFunc (DeltaVirtualMachine*);
static void	isdelegator_LibFunc (DeltaVirtualMachine*);
static void	getdelegates_LibFunc (DeltaVirtualMachine*);
static void	getdelegators_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "inherit",			inherit_LibFunc },
	{ "inheritredirect",	inheritredirect_LibFunc },
	{ "uninherit",			uninherit_LibFunc },
	{ "isderived",			isderived_LibFunc },
	{ "getbases",			getbases_LibFunc },
	{ "getbase",			getbase_LibFunc },
	{ "getderived",			getderived_LibFunc },
	{ "getmostderived",		getmostderived_LibFunc },

	{ "delegate",			delegate_LibFunc },
	{ "undelegate",			undelegate_LibFunc },
	{ "isdelegate",			isdelegate_LibFunc },
	{ "isdirectdelegate",	isdirectdelegate_LibFunc },
	{ "isdelegator",		isdelegator_LibFunc },
	{ "getdelegates",		getdelegates_LibFunc },
	{ "getdelegators",		getdelegators_LibFunc }
};

DELTA_LIBRARY_SUBAPI_INSTALLER(DeltaObject, "object", DELTA_STDLIB_NAMESPACE_PREFIX)

//------------------------------------------------------------------

#define	ObjectOrExternId(type) \
	((type) == DeltaValue_Table || (type) == DeltaValue_ExternId)

#define	GET_OBJECT_OR_USERDATA_TEMPLATE(var, func, retvalreset, argno, type)						\
	DeltaGetOverloadedArg(argObj, argno, ObjectOrExternId, "Object / ExternId", func, retvalreset)	\
	type* var;																						\
	if (argObj->Type() == DeltaValue_Table)															\
		ucastassign(var, argObj->ToTable());														\
	else {																							\
		ucastassign(var, argObj->GetExternIdUserData());											\
		if (!var) {																					\
			DPTR(vm)->PrimaryError(																	\
				DEFORMAT_EXTERNID_NOT_INSTANCE,														\
				CURR_FUNC,																			\
				argno,																				\
				argObj->GetExternIdTypeString().c_str()												\
			);																						\
			retvalreset																				\
		}																							\
	}

#define	GET_OBJECT_OR_USERDATA(var, func, retvalreset, argno)										\
	GET_OBJECT_OR_USERDATA_TEMPLATE(var, func, retvalreset, argno, DeltaObject)

#define	GET_TABLE_OR_USERDATA(var, func, retvalreset, argno)										\
	GET_OBJECT_OR_USERDATA_TEMPLATE(var, func, retvalreset, argno, DeltaTable)

//------------------------------------------------------------------

#define	CHECK_OBJECT(_proto, _ret, _role)						\
	if (!(_proto)->IsOfClass(DELTA_OBJECT_CLASS_ID)) {			\
		DPTR(vm)->PrimaryError(									\
			"in '%s', %s 0x%x is simple Table, not an Object!", \
			CURR_FUNC,											\
			(util_ui32) _proto,									\
			_role												\
		);														\
		_ret													\
		return;													\
	}

#define	CHECK_OBJECTS(_ret)										\
	CHECK_OBJECT(base, _ret, "BASE")							\
	CHECK_OBJECT(derived, _ret, "DERIVED")

//------------------------------------------------------------------
// INHERITANCE

static void inherit_impl(
		const char*				func,
		DeltaVirtualMachine*	vm, 
		bool (DeltaObject::*	f)(DeltaObject*)
	) {

	ISSUE_FUNC(func);
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

	DeltaValue* argObj;
	GET_OBJECT_OR_USERDATA(derived, CURR_FUNC, RESET_EMPTY, 0);
	GET_OBJECT_OR_USERDATA(base, CURR_FUNC, RESET_EMPTY, 1);

	CHECK_OBJECTS(RESET_EMPTY);

	CHECK_CPPFUNC_RESULT((DPTR(derived)->*f)(base));
}

//------------------------------------------------------------------
// inherit(derived, base).
// inheritredirect(derived, base).
//
static void inherit_LibFunc (DeltaVirtualMachine* vm) 
	{ inherit_impl("inherit", vm, &DeltaObject::Inherit); }

static void inheritredirect_LibFunc (DeltaVirtualMachine* vm) 
	{ inherit_impl("inheritredirect", vm, &DeltaObject::InheritRedirect); }

////////////////////////////////////////////////////////////////////
// uninherit(derived, base).
//
static void uninherit_LibFunc (DeltaVirtualMachine* vm) {
	ISSUE_FUNC("uninherit");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

	DeltaValue* argObj;
	GET_OBJECT_OR_USERDATA(derived, CURR_FUNC, RESET_EMPTY, 0);
	GET_OBJECT_OR_USERDATA(base, CURR_FUNC, RESET_EMPTY, 1);

	CHECK_OBJECTS(RESET_EMPTY);

	bool result = DPTR(derived)->UnInherit(base);
	CHECK_CPPFUNC_RESULT(result);
}

////////////////////////////////////////////////////////////////////
// bool isderived(derived, base).
//
static void isderived_LibFunc (DeltaVirtualMachine* vm) {
	ISSUE_FUNC("isderived");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue* argObj;
	GET_OBJECT_OR_USERDATA(derived, CURR_FUNC, RESET_FALSE_RETURNVALUE, 0);
	GET_OBJECT_OR_USERDATA(base, CURR_FUNC, RESET_FALSE_RETURNVALUE, 1);

	CHECK_OBJECTS(RESET_FALSE_RETURNVALUE);

	DPTR(vm)->GetReturnValue().FromBool(
		DPTR(derived)->IsDerived(base)
	);
}

////////////////////////////////////////////////////////////////////
// Table getbases(proto).
//
static void getbases_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("getbases");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_OBJECT_OR_USERDATA(proto, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);
	CHECK_OBJECT(proto, RESET_NIL_RETURNVALUE, "DERIVED");

	DPTR(vm)->GetReturnValue().FromTable(	
		DPTR(proto)->GetBasesAsTable()
	);
}

////////////////////////////////////////////////////////////////////
// Table getbase(proto, Number(order)).
//
static void getbase_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("getbase");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_OBJECT_OR_USERDATA(proto, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);
	CHECK_OBJECT(proto, RESET_NIL_RETURNVALUE, "DERIVED");
	GET_UNSIGNEDINT(i, CURR_FUNC, RESET_NIL_RETURNVALUE, 1);

	DeltaObject* base = DPTR(proto)->GetBase(i);
	if (proto)
		DPTR(vm)->GetReturnValue().FromTable(base);
	else
		DPTR(vm)->GetReturnValue().FromNil();
}

////////////////////////////////////////////////////////////////////
// Proto getderived(proto).
//
static void getderived_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("getderived");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue* argObj;
	GET_OBJECT_OR_USERDATA(proto, CURR_FUNC, RESET_FALSE_RETURNVALUE, 0);
	
	if (DeltaObject* d = DPTR(proto)->GetDerived())
		DPTR(vm)->GetReturnValue().FromTable(DPTR(d));
	else
		DPTR(vm)->GetReturnValue().FromNil();
}

////////////////////////////////////////////////////////////////////
// Proto getmostderived(proto).
//
static void getmostderived_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("getmostderived");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue* argObj;
	GET_OBJECT_OR_USERDATA(proto, CURR_FUNC, RESET_FALSE_RETURNVALUE, 0);
	
	DPTR(vm)->GetReturnValue().FromTable(DPTR(proto)->GetMostDerived());
}

//------------------------------------------------------------------
// DELEGATION

static void delegate_impl(
		const char*				func,
		DeltaVirtualMachine*	vm, 
		bool (DeltaTable::*	f)(DeltaTable*)
	) {

	ISSUE_FUNC(func);
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

	DeltaValue* argObj;
	GET_TABLE_OR_USERDATA(from, CURR_FUNC, RESET_EMPTY, 0);
	GET_TABLE_OR_USERDATA(to, CURR_FUNC, RESET_EMPTY, 1);
	
	CHECK_CPPFUNC_RESULT((DPTR(from)->*f)(to));
}

////////////////////////////////////////////////////////////////////
// delegate(object(from), object(to))
// undelegate(object(from), object(to))
//
static void	delegate_LibFunc (DeltaVirtualMachine* vm) 
	{ delegate_impl("delegate", vm, &DeltaTable::Delegate); }

static void	undelegate_LibFunc (DeltaVirtualMachine* vm)
	{ delegate_impl("undelegate", vm, &DeltaTable::Undelegate); }

////////////////////////////////////////////////////////////////////

static void isdelegate_impl(
		const char*				func,
		DeltaVirtualMachine*	vm, 
		bool (DeltaTable::*f)(DeltaTable*) const
	) {
	ISSUE_FUNC(func);
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_FALSE_RETURNVALUE);

	DeltaValue* argObj;
	GET_TABLE_OR_USERDATA(a, CURR_FUNC, RESET_EMPTY, 0);
	GET_TABLE_OR_USERDATA(b, CURR_FUNC, RESET_EMPTY, 1);

	DPTR(vm)->GetReturnValue().FromBool((DPTR(a)->*f)(b));
}

////////////////////////////////////////////////////////////////////
// bool isdelegate(object(from), object(to))
// bool isdirectdelegate(object(from), object(to))
// bool isdelegator(object(to), object(from))
//

static void	isdelegate_LibFunc (DeltaVirtualMachine* vm) 
	{ isdelegate_impl("isdelegate", vm, &DeltaTable::IsDelegate); }

static void	isdirectdelegate_LibFunc (DeltaVirtualMachine* vm)
	{ isdelegate_impl("isdirectdelegate", vm, &DeltaTable::IsDirectDelegate); }

static void	isdelegator_LibFunc (DeltaVirtualMachine* vm)
	{ isdelegate_impl("isdelegator", vm, &DeltaTable::IsDelegator); }

////////////////////////////////////////////////////////////////////

static void gettable_impl(
		const char*				func,
		DeltaVirtualMachine*	vm, 
		DeltaTable*	 (DeltaTable::*f)(void)
	) {
	ISSUE_FUNC("getbases");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

	DeltaValue* argObj;
	GET_TABLE_OR_USERDATA(obj, CURR_FUNC, RESET_NIL_RETURNVALUE, 0);

	DPTR(vm)->GetReturnValue().FromTable((DPTR(obj)->*f)());
}

// Table getdelegates(object(a))
// Table getdelegators(object(a))

static void	getdelegates_LibFunc (DeltaVirtualMachine* vm) 
	{ gettable_impl("getdelegates", vm, &DeltaTable::GetDelegatesAsTable); }

static void	getdelegators_LibFunc (DeltaVirtualMachine* vm)
	{ gettable_impl("getdelegators", vm, &DeltaTable::GetDelegatorsAsTable); }

//------------------------------------------------------------------
