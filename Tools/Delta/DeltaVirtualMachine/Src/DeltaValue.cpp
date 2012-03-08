// DelatObject.cpp
// Implementation of the DeltaValue class.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999.
// Refactored July 2005 to consider new operand translation.
// Extended August 2005 to support VM aware functions.
// Changed July 2008, extern id are now ref-counted (not cloned).
//

#include <stdio.h>
#include <string.h>

#include "DDebug.h"
#include "uerrorclass.h"
#include "DeltaValue.h"
#include "DeltaString.h"
#include "DeltaTable.h"
#include "DeltaObject.h"
#include "DeltaVirtualMachine.h"
#include "DeltaCollectableSimple.h"
#include "DeltaCollectableContainer.h"
#include "DeltaRuntimeError.h"
#include "ustrings.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaVMRegistry.h"
#include "DeltaExternIdFactory.h"
#include "OperatorOverloading.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "DeltaTableFactory.h"
#include "DeltaValueValidator.h"
#include "DeltaClosure.h"

//-----------------------------------------------------------------

#define	ERROR_HANDLER(what, errclass)		\
	uerror::GetSingleton().post##errclass(	\
		"Variable: error %s!", what			\
	); return;

#ifndef	DDEBUG_USE
#ifdef	DEBUG_VM
#undef	DEBUG_VM
#endif
#endif

//	#define	DEBUG_VM

#ifdef	DEBUG_VM
#define	REGISTER_IN_VALIDATOR() GetDeltaValueValidator()->Register(this);
#define	CANCEL_FROM_VALIDATOR()	GetDeltaValueValidator()->Cancel(this);
#define	VALIDATE_SELF()			DASSERT(GetDeltaValueValidator()->Validate(this));
#define	VALIDATE_OBJECT(val)	DASSERT(GetDeltaValueValidator()->Validate(val));
#else
#define	REGISTER_IN_VALIDATOR() ;
#define	CANCEL_FROM_VALIDATOR()	;
#define	VALIDATE_SELF()			;
#define	VALIDATE_OBJECT(val)	;
#endif

#define	DELTA_TOSTRING_MAXLENGTH	64
util_ui32	DeltaValue::toStringMaxLen = DELTA_TOSTRING_MAXLENGTH;	

//-----------------------------------------------------------------
// CONSTRUCTORS.

DeltaValue::DeltaValue (void) { 
	type				= DeltaValue_Undefined; 
	residencyType		= Internal;
	residencyContext	= (void*) 0;
	REGISTER_IN_VALIDATOR();
}

DeltaValue::DeltaValue (_Nil) { 
	type				= DeltaValue_Nil; 
	residencyType		= Internal;
	residencyContext	= (void*) 0;
	REGISTER_IN_VALIDATOR();
}

#define	DEFAULT_CTOR() \
		new (this) DeltaValue()

//////////////////////////////////

DeltaValue::DeltaValue (const DeltaValue& val) 
	{ DEFAULT_CTOR(); Assign(val); }

DeltaValue::DeltaValue (const DeltaValue* val)
	{ new (this) DeltaValue(*val); }

//////////////////////////////////

DeltaValue::DeltaValue (const char* s)
	{ DEFAULT_CTOR(); FromString(s); }

DeltaValue::DeltaValue (const std::string& s)
	{ DEFAULT_CTOR(); FromString(s); }

DeltaValue::DeltaValue (bool b)	
	{ DEFAULT_CTOR(); FromBool(b); }

DeltaValue::DeltaValue (DeltaNumberValueType n)
	{ DEFAULT_CTOR(); FromNumber(n); }

DeltaValue::DeltaValue (		
	void*							val,
	DeltaExternIdType				type,
	void							(*toString)(DeltaString*, void*),
	const std::string&				typeStr,
	const DeltaExternIdFieldGetter*	fieldGetter
) { DEFAULT_CTOR(); FromExternId(val, type, toString, typeStr, fieldGetter); }

DeltaValue::DeltaValue (DeltaExternId* externId)
	{ DEFAULT_CTOR(); FromExternIdDirectly(externId); }

DeltaValue::DeltaValue (DeltaTable* t)
	{ DEFAULT_CTOR(); FromTable(t); }

DeltaValue::DeltaValue (DeltaLibraryFunc lf, DeltaLibraryFuncArgsBinder* binder)
	{ DEFAULT_CTOR(); FromLibraryFunc(lf, binder); }

DeltaValue::DeltaValue(DeltaCodeAddress pf, DeltaVirtualMachine* vm, DeltaClosure* closure)
	{ DEFAULT_CTOR(); FromProgramFunc(pf, vm, closure); }

DeltaValue::DeltaValue(DeltaCodeAddress mf, DeltaTable* t, DeltaVirtualMachine* vm, DeltaClosure* closure)
	{ DEFAULT_CTOR(); FromMethodFunc(mf, t, vm, closure); }

DeltaValue::DeltaValue (const DeltaStdFuncInfo* f, DeltaVirtualMachine* vm, DeltaClosure* closure)
	{ DEFAULT_CTOR(); FromProgramFunc(f, vm, closure); }

DeltaValue::DeltaValue (const DeltaStdFuncInfo* f, DeltaTable* t, DeltaVirtualMachine* vm, DeltaClosure* closure)
	{ DEFAULT_CTOR(); FromMethodFunc(f, t, vm, closure); }

//////////////////////////////////

DeltaValue::~DeltaValue() { 
	VALIDATE_SELF();
	CollectGarbage();
	CANCEL_FROM_VALIDATOR();
}

//-----------------------------------------------------------------
// Residency management for garbage collection.
//

DeltaValue::ResidencyType DeltaValue::GetResidencyType (void) const {
	VALIDATE_SELF();
	DASSERT(residencyType == Contained || residencyType == Stacked || residencyType == Internal);
	return residencyType; 
}

//**************************

void* DeltaValue::GetResidencyContext (void) const {
	VALIDATE_SELF();
	DASSERT(residencyType != Contained || residencyContext);
	DASSERT(residencyType == Contained || !residencyContext);
	return residencyContext; 
}

//**************************

void DeltaValue::SetResidencyType (ResidencyType t, void* c) {

	VALIDATE_SELF();

	DASSERT(residencyType == Internal);	// Type is not alterable more than once.
	DASSERT(t != Internal);				// This is only given automatically.

	if ((residencyType = t) == Stacked) {
		DASSERT(!c);					// Can't give context for stack objects.
		residencyContext = (void*) 0;
	}
	else {
		DASSERT(t == Contained && c);	// But should give for contained.
		residencyContext = c;
	}
}

//-----------------------------------------------------------------

DeltaValue::AssignFunc DeltaValue::assignFuncs[TOTAL_DELTAVALUE_TYPES] = {
	&DeltaValue::AssignFrom_Number,  
	&DeltaValue::AssignFrom_String,  
	&DeltaValue::AssignFrom_Table,  
	&DeltaValue::AssignFrom_ProgramFunc,
	&DeltaValue::AssignFrom_LibraryFunc,
	&DeltaValue::AssignFrom_Bool,  
	&DeltaValue::AssignFrom_ExternId,  
	&DeltaValue::AssignFrom_Nil,  
	&DeltaValue::AssignFrom_MethodFunc,
	&DeltaValue::AssignFrom_Undefined
};

/////////////////////////////////////////////////////////////////////

void DeltaValue::AssignFrom_Number  (DeltaValue* dest, const DeltaValue* src) 
	{ dest->val.numObj = src->val.numObj; }				

void DeltaValue::AssignFrom_String  (DeltaValue* dest, const DeltaValue* src)
	{ dest->val.strObj = DPTR(src->val.strObj)->Clone(); }				

void DeltaValue::AssignFrom_Table  (DeltaValue* dest, const DeltaValue* src) {
	DASSERT(ValidatableHandler::Validate(src->val.tableObj, DPTR(src->val.tableObj)->GetSerialNo()));
	dest->val.tableObj = src->val.tableObj;
	DPTR(dest->val.tableObj)->IncRefCounter(dest);
}

void DeltaValue::AssignFrom_ProgramFunc  (DeltaValue* dest, const DeltaValue* src) {
	dest->val.programFuncObj.func		= src->val.programFuncObj.func;
	dest->val.programFuncObj.vm			= src->val.programFuncObj.vm;
	DPTR(dest->val.programFuncObj.vm)->GetExternId()->IncRefCounter(dest);
	if ((dest->val.programFuncObj.closure = src->val.programFuncObj.closure))
		DPTR(dest->val.programFuncObj.closure)->IncRefCounter(dest);
}

void DeltaValue::AssignFrom_MethodFunc  (DeltaValue* dest, const DeltaValue* src){
	dest->val.methodFuncObj.func		= src->val.methodFuncObj.func;
	dest->val.methodFuncObj.self		= src->val.methodFuncObj.self;
	dest->val.methodFuncObj.vm			= src->val.methodFuncObj.vm;
	if ((dest->val.methodFuncObj.closure = src->val.methodFuncObj.closure))
		DPTR(dest->val.methodFuncObj.closure)->IncRefCounter(dest);
	DPTR(dest->val.methodFuncObj.self)->IncRefCounter(dest);
	DPTR(dest->val.methodFuncObj.vm)->GetExternId()->IncRefCounter(dest);	// Refer to the unique externid of the vm
}

void DeltaValue::AssignFrom_LibraryFunc  (DeltaValue* dest, const DeltaValue* src) {
	dest->val.libraryFuncObj.f		= src->val.libraryFuncObj.f;
	dest->val.libraryFuncObj.binder	= src->val.libraryFuncObj.binder;
	if (dest->val.libraryFuncObj.binder)
		DPTR(dest->val.libraryFuncObj.binder)->IncRefCounter(dest);
}

void DeltaValue::AssignFrom_Bool  (DeltaValue* dest, const DeltaValue* src)
	{ dest->val.boolObj = src->val.boolObj; }	

void DeltaValue::AssignFrom_ExternId  (DeltaValue* dest, const DeltaValue* src) {
	dest->val.externIdObj = DPTR(src->val.externIdObj);  
	DPTR(dest->val.externIdObj)->IncRefCounter(dest); 
}			

void DeltaValue::AssignFrom_Nil  (DeltaValue* dest, const DeltaValue* src){}

void DeltaValue::AssignFrom_Undefined  (DeltaValue* dest, const DeltaValue* src){}

/////////////////////////////////////////////////////////////////////

static bool argumentChecker[] = {	// Nil and Undefined disable overloading to be applied.
	true,	//		Number  
	true,	//		String  
	true,	//		Table  
	true,	//		ProgramFunc
	true,	//		LibraryFunc
	true,	//		Bool  
	true,	//		ExternId  
	false,	//		<NIL>
	true,	//		MethodFunc
	false	//		<UNDEF>
};

static bool callerChecker[] = {		// Table and ExternId may overloaded assignment and type casting.
	false,	//		Number  
	false,	//		String  
	true,	//		<TABLE>
	false,	//		ProgramFunc
	false,	//		LibraryFunc
	false,	//		Bool  
	true,	//		<EXTERNID>  
	false,	//		Nil  
	false,	//		MethodFunc
	false	//		Undefined
};

#define	CALLER_CHECK(x)			(callerChecker[(x)->type])
#define	ARG_CHECK(x)			(argumentChecker[(x)->type])

#define	NOT_OVERLOADED(type)	OperatorOverloading::type##_NotOverloaded
#define	OVERLOADED_RETRY(type)	OperatorOverloading::type##_OverloadedButRequestedRetry
#define	OVERLOADED_ERROR(type)	OperatorOverloading::type##_ExecutionError
#define	RESULT_T(type)			OperatorOverloading::type##Result

#define	DEFAULT_ASSIGNMENT()										\
		if (true) {													\
		CollectGarbage();											\
		DASSERT((util_ui32) src->type < uarraysize(assignFuncs));	\
		(*assignFuncs[type = src->type])(this, src);				\
		typeTag = src->typeTag;										\
		} else

/////////////////////////////////////////////////////////////////////

bool DeltaValue::AssignOverloadable (const DeltaValue* src) {

	VALIDATE_SELF();
	VALIDATE_OBJECT(src);

	if (this != src) {

		if (CALLER_CHECK(this) && ARG_CHECK(src))
			switch (OperatorOverloading::HandleAssign(this, src)) {
				case NOT_OVERLOADED(ArithAssign)	:
				case OVERLOADED_RETRY(ArithAssign)	: break;
				case OVERLOADED_ERROR(ArithAssign)	: return false;
				default: return true;
			}

		if (CALLER_CHECK(src) && ARG_CHECK(this))
			switch (OperatorOverloading::HandleTypeCasting(this, src)) {
				case NOT_OVERLOADED(TypeCasting)	:
				case OVERLOADED_RETRY(TypeCasting)	: break;
				case OVERLOADED_ERROR(TypeCasting)	: return false;
				default: return true;
			}

		DEFAULT_ASSIGNMENT();
		return true;
	}
	else
		return true;
}

//*****************************

bool DeltaValue::Assign (const DeltaValue* src) {

	VALIDATE_SELF();
	VALIDATE_OBJECT(src);

	if (this != src) 
		{ DEFAULT_ASSIGNMENT(); }
	return true;
}

//-----------------------------------------------------------------

DeltaTable*	DeltaValue::GetTable (void) {

	VALIDATE_SELF();

	return	type == DeltaValue_Table ? val.tableObj :
			type == DeltaValue_MethodFunc ? val.methodFuncObj.self :
			(DeltaTable*) 0;
}

//******************************

CollectableContainer* DeltaValue::GetContainer (void) {

	VALIDATE_SELF();

	if (CollectableContainer* cont = GetTable())
		return cont;
	else
	if (type == DeltaValue_ExternId && DPTR(val.externIdObj)->GetType() == DeltaExternId_CollectableContainer)
		return (CollectableContainer*) DPTR(val.externIdObj)->GetValue();
	else
		return (CollectableContainer*) 0;
}

//-----------------------------------------------------------------
// Equality is checked according to the following rules.
//
// If same object, true.
// If types different, false.
// 
// Tables:	refer to the same table.
// Strings:	non null and !strcmp.
// Numbers:	equality check.
// Bools:	equality check.
// Func addresses: equality check.
// Extern funcs: equality check.
// Member funcs: same addresses and tables.
// Extern Id: equality of ids.
// Nil: true.
// Undefined: false.
//

bool DeltaValue::Number_Equality (DeltaValue* obj1, DeltaValue* obj2)
	{ return obj1->val.numObj == obj2->val.numObj; }

bool DeltaValue::Bool_Equality (DeltaValue* obj1, DeltaValue* obj2)
	{ return obj1->val.boolObj == obj2->val.boolObj; }

bool DeltaValue::Table_Equality (DeltaValue* obj1, DeltaValue* obj2)
	{ return obj1->val.tableObj == obj2->val.tableObj; }

bool DeltaValue::String_Equality (DeltaValue* obj1, DeltaValue* obj2) {
	return	obj1->val.strObj &&
			obj2->val.strObj &&
			DPTR(obj1->val.strObj)->Content() == DPTR(obj2->val.strObj)->Content(); 
}

bool DeltaValue::ProgramFunc_Equality (DeltaValue* obj1, DeltaValue* obj2) {
	return	obj1->val.programFuncObj.func	==	obj2->val.programFuncObj.func &&
			obj1->val.programFuncObj.vm		==	obj2->val.programFuncObj.vm;
}

bool DeltaValue::MethodFunc_Equality (DeltaValue* obj1, DeltaValue* obj2) {
	return	obj1->val.methodFuncObj.func	==	obj2->val.methodFuncObj.func	&&
			obj1->val.methodFuncObj.self	==	obj2->val.methodFuncObj.self	&&
			obj1->val.methodFuncObj.vm		==	obj2->val.methodFuncObj.vm;
}

bool DeltaValue::LibraryFunc_Equality (DeltaValue* obj1, DeltaValue* obj2) {
	return	obj1->val.libraryFuncObj.f == obj2->val.libraryFuncObj.f &&
			obj1->val.libraryFuncObj.binder == obj2->val.libraryFuncObj.binder;
}

bool DeltaValue::ExternId_Equality (DeltaValue* obj1, DeltaValue* obj2) {
	return	*obj1->val.externIdObj == *obj2->val.externIdObj;
}

bool DeltaValue::Nil_Equality (DeltaValue* obj1, DeltaValue* obj2)
	{ return true; }

bool DeltaValue::Undefined_Equality (DeltaValue* obj1, DeltaValue* obj2)
	{ return true; }


DeltaValue::EqualityCheck DeltaValue::equalityCheckFuncs[TOTAL_DELTAVALUE_TYPES] = {
	DeltaValue::Number_Equality,
	DeltaValue::String_Equality,
	DeltaValue::Table_Equality,
	DeltaValue::ProgramFunc_Equality,
	DeltaValue::LibraryFunc_Equality,
	DeltaValue::Bool_Equality,
	DeltaValue::ExternId_Equality,
	DeltaValue::Nil_Equality,
	DeltaValue::MethodFunc_Equality,
	DeltaValue::Undefined_Equality
};

//****************************

bool DeltaValue::Equal (const DeltaValue* val) const {

	VALIDATE_SELF();
	VALIDATE_OBJECT(val);

	DASSERT((util_ui32) type < uarraysize(equalityCheckFuncs));

	return	this == val			? true	:
			type != val->type	? false :
			(*equalityCheckFuncs[type])(
				const_cast<DeltaValue*>(this), 
				const_cast<DeltaValue*>(val)
			);
}

bool DeltaValue::Equal (const DeltaValue& val) const 
	{ return Equal(&val); }

//-----------------------------------------------------------------
// TRUE TEST.
// Number:		!=0
// String:		not ""
// Bool:		true
// Table:		not 0 elements
// Func addr:	true
// Member func: true
// Extern func:	true
// Extern id:	not 0
// Nil:			false
// Undefined:	false
//
bool DeltaValue::Number_TrueTest (const DeltaValue* val)
	{ return val->val.numObj != 0; }

bool DeltaValue::Bool_TrueTest (const DeltaValue* val)
	{ return val->val.boolObj == true; }

bool DeltaValue::String_TrueTest (const DeltaValue* val)
	{ return val->val.strObj->Content() != ""; }

bool DeltaValue::Table_TrueTest (const DeltaValue* val)
	{ return true; }

bool DeltaValue::ProgramFunc_TrueTest (const DeltaValue* val)
	{ return true; }

bool DeltaValue::MethodFunc_TrueTest (const DeltaValue* val)
	{ return true; }

bool DeltaValue::LibraryFunc_TrueTest (const DeltaValue* val)
	{ return true; }

bool DeltaValue::ExternId_TrueTest (const DeltaValue* val)
	{ return DPTR(val->val.externIdObj)->ToBool() == true; }

bool DeltaValue::Nil_TrueTest (const DeltaValue* val)
	{ return false; }

bool DeltaValue::Undefined_TrueTest (const DeltaValue* val)
	{ return false; }

DeltaValue::TrueTestFunc DeltaValue::trueTestFuncs[TOTAL_DELTAVALUE_TYPES] = {
	DeltaValue::Number_TrueTest,
	DeltaValue::String_TrueTest,
	DeltaValue::Table_TrueTest,
	DeltaValue::ProgramFunc_TrueTest,
	DeltaValue::LibraryFunc_TrueTest,
	DeltaValue::Bool_TrueTest,
	DeltaValue::ExternId_TrueTest,
	DeltaValue::Nil_TrueTest,
	DeltaValue::MethodFunc_TrueTest,
	DeltaValue::Undefined_TrueTest
};

bool DeltaValue::TrueTest (void) const {
	DASSERT((util_ui32) type < uarraysize(trueTestFuncs));
	return (*trueTestFuncs[type])(this); 
}

//-----------------------------------------------------------------

struct ValueConversionData {
	DeltaValue* at;
	std::string	typeStr;
	ValueConversionData  (DeltaValue* _at, const std::string& _typeStr) :
		at(_at), typeStr(_typeStr){}
};

//****************************

static bool ValueConversionPred (DeltaObject* p, void* closure) {
	if (DPTR(p)->IsUserDataOfExternId()) {
		ValueConversionData*	data		= (ValueConversionData*) closure;
		DeltaExternId*			externId	= p->GetExternIdWhenUserData();
		if (externId->GetTypeStr() == data->typeStr)
			{ data->at->FromExternIdDirectly(externId); return true; }
	}
	return false;
}

//****************************

static bool HandleValueConversion (DeltaObject* p, const std::string typeStr, DeltaValue* at) {
	DASSERT(GetDeltaObjectValidator()->Validate(p));
	ValueConversionData data(at, typeStr); 
	return DPTR(p)->Visit(&ValueConversionPred, &data);
}

//****************************

bool DeltaValue::ConvertToExternId (const std::string& typeStr, DeltaValue* at) {
	VALIDATE_SELF();
	at->Undefine();
	if (type == DeltaValue_ExternId) {
		if (GetExternIdTypeString() == typeStr)
			at->Assign(this);
		else
		if (DeltaObject* p = (DeltaObject*) GetExternIdUserData())
			return HandleValueConversion(p, typeStr, at);
	}
	else
	if (type == DeltaValue_Table && val.tableObj->IsOfClass(DELTA_OBJECT_CLASS_ID))
		return HandleValueConversion((DeltaObject*) val.tableObj, typeStr, at);
	return true;
}

//-----------------------------------------------------------------
// After garbage collection is requested from a DeltaValue,
// the object type is DeltaValue_Undefined. Garbage collection
// concerns:
//		String
//		Table
//		Extern id
//		Tables of member functions.
//

void DeltaValue::Collect_Number  (DeltaValue* val)
	{}				

void DeltaValue::Collect_String  (DeltaValue* val)
	{ DeltaString::Delete(val->val.strObj); }

void DeltaValue::Collect_Table  (DeltaValue* val)
	{ DPTR(val->val.tableObj)->DecRefCounter(val); }

void DeltaValue::Collect_ProgramFunc  (DeltaValue* val) {
	if (val->val.programFuncObj.closure)
		DPTR(val->val.programFuncObj.closure)->DecRefCounter(val);
	DPTR(val->val.programFuncObj.vm)->GetExternId()->DecRefCounter(val);
}

void DeltaValue::Collect_MethodFunc (DeltaValue* val) {
	if (val->val.methodFuncObj.closure)
		DPTR(val->val.methodFuncObj.closure)->DecRefCounter(val);
	DPTR(val->val.methodFuncObj.self)->DecRefCounter(val);
	if (val->Type() == DeltaValue_MethodFunc)	// The previous call could also cause collection.
		DPTR(val->val.methodFuncObj.vm)->GetExternId()->DecRefCounter(val);
}

void DeltaValue::Collect_LibraryFunc  (DeltaValue* val) {
	if (val->val.libraryFuncObj.binder) 
		DPTR(val->val.libraryFuncObj.binder)->DecRefCounter(val); 
}

void DeltaValue::Collect_Bool (DeltaValue* val)
	{}

void DeltaValue::Collect_ExternId (DeltaValue* val) 
	{ DPTR(val->val.externIdObj)->DecRefCounter(val); }

void DeltaValue::Collect_Nil (DeltaValue* val)
	{}			

void DeltaValue::Collect_Undefined (DeltaValue* val)
	{}	
	
DeltaValue::CollectFunc DeltaValue::collectFuncs[TOTAL_DELTAVALUE_TYPES] = {
	&DeltaValue::Collect_Number,  
	&DeltaValue::Collect_String,  
	&DeltaValue::Collect_Table,  
	&DeltaValue::Collect_ProgramFunc,
	&DeltaValue::Collect_LibraryFunc,
	&DeltaValue::Collect_Bool,  
	&DeltaValue::Collect_ExternId,  
	&DeltaValue::Collect_Nil,  
	&DeltaValue::Collect_MethodFunc,
	&DeltaValue::Collect_Undefined
};

void DeltaValue::CollectGarbage (void) {
	DASSERT((util_ui32) type < uarraysize(collectFuncs));
	(*collectFuncs[type])(this);
	type = DeltaValue_Undefined;
	typeTag.clear();
}

//-----------------------------------------------------------------
// 'FROM' CONVERTERS.	

void DeltaValue::FromNumber (DeltaNumberValueType num) {
	VALIDATE_SELF();
	CollectGarbage();

	type = DeltaValue_Number;
	val.numObj = (num == 0.0 ? 0.0 : num);	// To avoid precision problems with 0 representation.
}

//////////////////////////////////

void DeltaValue::FromString (const std::string& s) {

	VALIDATE_SELF();
	CollectGarbage();

	type		= DeltaValue_String;
	val.strObj	= DeltaString::New(s);
}

//////////////////////////////////

void DeltaValue::FromLibraryFunc (DeltaLibraryFunc func, DeltaLibraryFuncArgsBinder* binder) {

	VALIDATE_SELF();

	// Check necessary to avoid collecting self binder when changing only the function.
	//
	if (type != DeltaValue_LibraryFunc || val.libraryFuncObj.binder != binder) 
		CollectGarbage();

	type						= DeltaValue_LibraryFunc;
	val.libraryFuncObj.f		= func;
	val.libraryFuncObj.binder	= binder;
	if (binder)
		DPTR(binder)->IncRefCounter(this);
}

//////////////////////////////////

void DeltaValue::FromBool (bool boolVal) {

	VALIDATE_SELF();
	CollectGarbage();

	type=DeltaValue_Bool;
	val.boolObj=boolVal;
}

//////////////////////////////////

// Again, when creating an object of table type,
// we should not forget to increase the reference counter
// of that table. Check initially if assigning to self, so as
// to avoid deleting the same table if it was the only reference.
//
void DeltaValue::FromTable (DeltaTable* table) {

	VALIDATE_SELF();
	if (type==DeltaValue_Table && val.tableObj==table)
		return;

	CollectGarbage();

	type = DeltaValue_Table;
	DASSERT(ValidatableHandler::Validate(table, DPTR(table)->GetSerialNo()));
	val.tableObj = table;
	DPTR(table)->IncRefCounter(this);
}

//////////////////////////////////

void DeltaValue::FromProgramFunc (
		DeltaCodeAddress		funcAddr, 
		DeltaVirtualMachine*	vm,
		DeltaClosure*			closure
	) {
	VALIDATE_SELF();
	DASSERT(ValidatableHandler::Validate(vm, DPTR(vm)->GetSerialNo()));
	FromProgramFunc(DPTR(vm)->GetFuncInfo(funcAddr), vm, closure);
}

//////////////////////////////////

void DeltaValue::FromProgramFunc (
		const DeltaStdFuncInfo*	func, 
		DeltaVirtualMachine*	vm,
		DeltaClosure*			closure
	) {

	VALIDATE_SELF();
	DASSERT(func && ValidatableHandler::Validate(vm, DPTR(vm)->GetSerialNo()));
	DASSERT(func == DPTR(vm)->GetFuncInfo(func->GetAddress()));

	if (	type == DeltaValue_ProgramFunc			&&
			val.programFuncObj.func		== func		&& 
			val.programFuncObj.vm		== vm		&&
			val.programFuncObj.closure	== closure	)

			return;

	CollectGarbage();

	type						= DeltaValue_ProgramFunc;
	val.programFuncObj.func		= func;
	val.programFuncObj.vm		= vm;
	val.programFuncObj.closure	= closure;
	if (closure)									// Refer to the closure.
		DPTR(closure)->IncRefCounter(this);
	DPTR(vm)->GetExternId()->IncRefCounter(this);	// Refer to the unique externid of the vm
}

//////////////////////////////////
// Here, we also check if we assign with the same table or closure, just
// to avoid collecting the same assigned table.
//
void DeltaValue::FromMethodFunc (
		DeltaCodeAddress		funcAddr, 
		DeltaTable*				self, 
		DeltaVirtualMachine*	vm,
		DeltaClosure*			closure
	) {

	VALIDATE_SELF();
	DASSERT(ValidatableHandler::Validate(vm, DPTR(vm)->GetSerialNo()));
	FromMethodFunc(DPTR(vm)->GetFuncInfo(funcAddr), self, vm, closure);
}

//////////////////////////////////

void DeltaValue::FromMethodFunc (
		const DeltaStdFuncInfo*	func, 
		DeltaTable*				self, 
		DeltaVirtualMachine*	vm,
		DeltaClosure*			closure
	) {

	VALIDATE_SELF();
	DASSERT(self && ValidatableHandler::Validate(vm, DPTR(vm)->GetSerialNo()));
	DASSERT(func && func == DPTR(vm)->GetFuncInfo(func->GetAddress()));

	if (	type == DeltaValue_MethodFunc				&&
			val.methodFuncObj.func		== func			&& 
			val.methodFuncObj.self		== self			&&
			val.methodFuncObj.vm		== vm			&&
			val.methodFuncObj.closure	== closure	)

			return;

	CollectGarbage();

	type						= DeltaValue_MethodFunc;
	val.methodFuncObj.func		= func;
	val.methodFuncObj.self		= self;
	val.methodFuncObj.vm		= vm;
	val.methodFuncObj.closure	= closure;

	DPTR(self)->IncRefCounter(this);				// Refer to self object
	if (closure)									// Refer to the closure.
		DPTR(closure)->IncRefCounter(this);
	DPTR(vm)->GetExternId()->IncRefCounter(this);	// Refer to the unique externid of the vm
}

//////////////////////////////////

DeltaTable* DeltaValue::GetMethodFuncSelf (void) const {

	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_MethodFunc);
	return val.methodFuncObj.self;
}

//////////////////////////////////

void DeltaValue::ChangeMethodFuncSelf (DeltaTable* table) {

	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_MethodFunc && table);

	if (val.methodFuncObj.self	!= table) {
		DPTR(val.methodFuncObj.self)->DecRefCounter(this); 
		val.methodFuncObj.self	= table;
		DPTR(table)->IncRefCounter(this);
	}
}

//////////////////////////////////
	
void DeltaValue::FromExternIdDirectly (DeltaExternId* externId) {

	VALIDATE_SELF();

	DASSERT(externId && DeltaExternIdFactory::GetSingleton().HasProduced(externId));
	if (type == DeltaValue_ExternId && DPTR(val.externIdObj) == DPTR(externId))
		return;

	CollectGarbage();
	type			=	DeltaValue_ExternId;
	val.externIdObj =	externId;
	DPTR(val.externIdObj)->IncRefCounter(this);
}

//////////////////////////////////

void DeltaValue::FromExternIdBySerialNo (util_ui32 serialNo) {

	VALIDATE_SELF();

	DeltaExternId* externId = DELTA_EXTERNID_VGET_INST((void*) serialNo);
	DASSERT(externId && DeltaExternIdFactory::GetSingleton().HasProduced(externId));

	if (type == DeltaValue_ExternId && DPTR(val.externIdObj) == DPTR(externId))
		return;

	CollectGarbage();
	type			=	DeltaValue_ExternId;
	val.externIdObj =	externId;
	DPTR(val.externIdObj)->IncRefCounter(this);
}

//////////////////////////////////
// If we happen to assign the same extern id (checked by uuid), we do not
// apply the assignment to avoid collecting it in case it had only this reference.
//
void DeltaValue::FromExternId (
		void*							_val,
		DeltaExternIdType				_type,
		void							(*_toString)(DeltaString*, void*),
		const std::string&				_typeStr,
		const DeltaExternIdFieldGetter*	_fieldGetter
	) {

	VALIDATE_SELF();

	if (type == DeltaValue_ExternId	&& DPTR(val.externIdObj)->Equal(_typeStr, _val)) {
		DASSERT(DPTR(val.externIdObj)->GetType() == _type);
		DASSERT(DPTR(val.externIdObj)->GetToStringFunc() == _toString);
	}
	else {
		CollectGarbage();
		type = DeltaValue_ExternId;
		if (!(val.externIdObj =	DeltaExternId::Get(_typeStr, _val)))
			val.externIdObj =	DeltaExternIdFactory::New(
									_val,
									_type,
									_toString,
									_typeStr,
									_fieldGetter
								);
		DPTR(val.externIdObj)->IncRefCounter(this);
	}
}

//////////////////////////////////

void DeltaValue::FromNil (void) {
	VALIDATE_SELF();
	CollectGarbage();
	type = DeltaValue_Nil;
}

void DeltaValue::Undefine (void) {
	VALIDATE_SELF();
	CollectGarbage();
	type = DeltaValue_Undefined;
}

//-----------------------------------------------------------------
// 'TO' CONVERTERS.

void DeltaValue::ToNumber (DeltaNumberValueType* num) {

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_Number);
	DASSERT(num);

	*num = val.numObj;
}

//********************************

DeltaNumberValueType DeltaValue::ToNumber (void) const {
	VALIDATE_SELF();
	DASSERT(type == DeltaValue_Number);
	return val.numObj;
}

//////////////////////////////////

const std::string& DeltaValue::ToString (void) const {
	VALIDATE_SELF();
	DASSERT(type == DeltaValue_String);
	return DPTR(val.strObj)->Content();
}

//////////////////////////////////

DeltaTable*	DeltaValue::ToTable (void) {
	VALIDATE_SELF();
	DASSERT(type == DeltaValue_Table);
	return val.tableObj;
}

const DeltaTable*	DeltaValue::ToTable (void) const {
	VALIDATE_SELF();
	DASSERT(type == DeltaValue_Table);
	return val.tableObj;
}

//********************************

void DeltaValue::ToTable (DeltaTable** table) {

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_Table);
	DASSERT(table);

	*table = val.tableObj;
}

//////////////////////////////////

void DeltaValue::ToLibraryFunc (DeltaLibraryFunc* func) {

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_LibraryFunc);
	DASSERT(func);

	*func = val.libraryFuncObj.f;
}

//********************************

const DeltaLibraryFuncArgsBinder* DeltaValue::GetLibraryFuncArgsBinder (void) const {
	VALIDATE_SELF();
	DASSERT(type == DeltaValue_LibraryFunc);
	return val.libraryFuncObj.binder;
}

//********************************

DeltaLibraryFuncArgsBinder* DeltaValue::GetLibraryFuncArgsBinder (void) {
	VALIDATE_SELF();
	DASSERT(type == DeltaValue_LibraryFunc);
	return val.libraryFuncObj.binder;
}

//********************************

DeltaLibraryFunc DeltaValue::ToLibraryFunc (void) const {

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_LibraryFunc);
	return val.libraryFuncObj.f;
}

//////////////////////////////////

void DeltaValue::ToBool (bool* boolVal) {

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_Bool);
	DASSERT(boolVal);

	*boolVal = val.boolObj;
}

bool DeltaValue::ToBool (void) const {
	VALIDATE_SELF();
	DASSERT(type == DeltaValue_Bool);
	return val.boolObj;
}

//////////////////////////////////

void DeltaValue::ToProgramFunc (
		DeltaCodeAddress*		funcAddr, 
		DeltaVirtualMachine**	vm, 
		DeltaClosure**			closure
	) {

	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_ProgramFunc && funcAddr && vm);
	DASSERT(val.programFuncObj.vm);

	*funcAddr	= val.programFuncObj.func->GetAddress();
	*vm			= val.programFuncObj.vm;
	if (closure)
		*closure = val.programFuncObj.closure;
}

//********************************

void DeltaValue::ToProgramFunc (
		UPTR(const DeltaStdFuncInfo)*	func, 
		DeltaVirtualMachine**			vm, 
		DeltaClosure**					closure
	) {

	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_ProgramFunc && func && vm);
	DASSERT(val.programFuncObj.vm);

	*func	= val.programFuncObj.func;
	*vm		= val.programFuncObj.vm;
	if (closure)
		*closure = val.programFuncObj.closure;
}

//********************************

DeltaVirtualMachine* DeltaValue::GetProgramFuncVM (void) const {
	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_ProgramFunc);
	return DNULLCHECK(val.programFuncObj.vm);
}

//********************************

DeltaClosure* DeltaValue::GetProgramFuncClosure (void) const {
	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_ProgramFunc);
	return val.programFuncObj.closure;
}

//********************************

const DeltaStdFuncInfo* DeltaValue::GetProgramFunc (void) const {
	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_ProgramFunc);
	return val.programFuncObj.func;
}

//////////////////////////////////

void DeltaValue::ToMethodFunc (
		DeltaCodeAddress*		funcAddr, 
		DeltaTable**			table, 
		DeltaVirtualMachine**	vm,
		DeltaClosure**			closure
	) {

	VALIDATE_SELF();

	DASSERT(Type() == DeltaValue_MethodFunc && funcAddr && table && vm);
	DASSERT(val.methodFuncObj.self && val.methodFuncObj.vm);

	*funcAddr	= val.methodFuncObj.func->GetAddress();
	*table		= val.methodFuncObj.self;
	*vm			= val.methodFuncObj.vm;
	if (closure)
		*closure = val.methodFuncObj.closure;
}

//********************************

void DeltaValue::ToMethodFunc (
		UPTR(const DeltaStdFuncInfo)*	func, 
		DeltaTable**					table, 
		DeltaVirtualMachine**			vm,
		DeltaClosure**					closure
	) {

	VALIDATE_SELF();

	DASSERT(Type() == DeltaValue_MethodFunc && func && table && vm);
	DASSERT(val.methodFuncObj.self && val.methodFuncObj.vm);

	*func		= val.methodFuncObj.func;
	*table		= val.methodFuncObj.self;
	*vm			= val.methodFuncObj.vm;
	if (closure)
		*closure = val.methodFuncObj.closure;
}

//********************************

DeltaVirtualMachine* DeltaValue::GetMethodFuncVM (void) const {
	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_MethodFunc);
	return DNULLCHECK(val.methodFuncObj.vm);
}

//********************************

DeltaClosure* DeltaValue::GetMethodFuncClosure (void) const {
	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_MethodFunc);
	return val.methodFuncObj.closure;
}

//********************************

const DeltaStdFuncInfo* DeltaValue::GetMethodFunc (void) const {
	VALIDATE_SELF();
	DASSERT(Type() == DeltaValue_MethodFunc);
	return val.methodFuncObj.func;
}

//////////////////////////////////

util_ui32 DeltaValue::ToExternIdSerialNo (void) const {
	VALIDATE_SELF();
	DASSERT(type == DeltaValue_ExternId && val.externIdObj);
	return DPTR(val.externIdObj)->GetSerialNo();
}

//********************************

void* DeltaValue::ToExternId (void) {
	VALIDATE_SELF();
	DASSERT(type == DeltaValue_ExternId && val.externIdObj);
	return DPTR(val.externIdObj)->GetValue();
}

void* DeltaValue::ToExternId (std::string& typeStr) {

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_ExternId && val.externIdObj);

	typeStr	= DPTR(val.externIdObj)->GetTypeStr();
	return DPTR(val.externIdObj)->GetValue();
}

//********************************

const DeltaExternIdFieldGetter* DeltaValue::GetExternIdFieldGetter (void) const {

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_ExternId && val.externIdObj);

	return DPTR(val.externIdObj)->GetFieldGetter();
}

//********************************

DeltaTable* DeltaValue::GetExternIdUserData (void) {

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_ExternId && val.externIdObj);

	return DPTR(val.externIdObj)->GetUserData();
}

//********************************

const std::string DeltaValue::GetExternIdTypeString (void) const{

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_ExternId && val.externIdObj);

	return DPTR(val.externIdObj)->GetTypeStr();
}

//********************************

const DeltaExternId* DeltaValue::GetExternId (void) const{

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_ExternId && val.externIdObj);

	return DPTR(val.externIdObj);
}

//********************************

DeltaExternId* DeltaValue::GetExternId (void) {

	VALIDATE_SELF();
	DASSERT(type == DeltaValue_ExternId && val.externIdObj);

	return DPTR(val.externIdObj);
}

//-----------------------------------------------------------------

void DeltaValue::ToString_FromNumber (DeltaValue* val, DeltaString* cvtStr)
	{ cvtStr->Add(unum2string(val->val.numObj)); }

void DeltaValue::ToString_FromBool (DeltaValue* val, DeltaString* cvtStr)
{ cvtStr->Add(std::string(val->val.boolObj ? "true" : "false")); }

void DeltaValue::ToString_FromString (DeltaValue* val, DeltaString* cvtStr)
	{ cvtStr->Add(val->val.strObj->Content()); }

//*****************************

void DeltaValue::ToStringHandler_FromLibraryFunc (
		DeltaValue* val, 
		DeltaString* at, 
		void (DeltaLibraryFuncArgsBinder::*f)(DeltaString* at, void*)
	) {
	at->Add(
		std::string(val->TypeStr())															+ 
		"('"																				+ 
		DeltaLibFuncBinder::GetName(DeltaLibFuncBinder::GetSerialNo(val->ToLibraryFunc()))	+
		"')"
	);
	if (val->val.libraryFuncObj.binder)
		(DPTR(val->val.libraryFuncObj.binder)->*f)(at, (void*) 0);
}

void DeltaValue::ToString_FromLibraryFunc (DeltaValue* val, DeltaString* cvtStr)
	{ return ToStringHandler_FromLibraryFunc(val, cvtStr, &DeltaLibraryFuncArgsBinder::ConvertToString); }

void DeltaValue::ToStringNeverOverloaded_FromLibraryFunc (DeltaValue* val, DeltaString* cvtStr)
	{ return ToStringHandler_FromLibraryFunc(val, cvtStr, &DeltaLibraryFuncArgsBinder::ConvertToStringBySkippingUserCode); }

//*****************************

static const std::string ClosureToString (DeltaClosure* closure) 
	{ return !closure ? "" : uconstructstr(", closure(%u vars)", DPTR(closure)->GetTotal()); }

void DeltaValue::ToString_FromProgramFunc (DeltaValue* val, DeltaString* cvtStr) {

	DeltaVirtualMachine* vm = DPTR(val->val.programFuncObj.vm);
	DPTR(vm)->Validate();
	
	cvtStr->Add(
		uconstructstr(
			"%s(name '%s', addr %u, vm 0x%x('%s'), src '%s'%s)", 
			val->TypeStr(),
			val->val.programFuncObj.func->GetName().c_str(),
			val->val.programFuncObj.func->GetAddress(),
			vm->GetSerialNo(),
			vm->Id(),
			vm->Source(),
			ClosureToString(val->val.programFuncObj.closure).c_str()
		)
	);
}

//*****************************

void DeltaValue::ToString_FromMethodFunc (DeltaValue* val, DeltaString* cvtStr) {
		
	DeltaVirtualMachine* vm = DPTR(val->val.methodFuncObj.vm);
	DPTR(vm)->Validate();

	cvtStr->Add(
		uconstructstr(
			"%s(addr %u, self 0x%x, vm 0x%x('%s'), src '%s'%s)",
			val->TypeStr(),
			val->val.methodFuncObj.func->GetAddress(),
			DPTR(val->val.methodFuncObj.self)->GetSerialNo(),
			vm->GetSerialNo(),
			vm->Id(),
			vm->Source(),
			ClosureToString(val->val.methodFuncObj.closure).c_str()
		)
	);
}

//*****************************

void DeltaValue::ToStringNeverOverloaded_FromTable (DeltaValue* val, DeltaString* at)
	{ DPTR(val->ToTable())->ConvertToStringBySkippingUserCode(at); }

void DeltaValue::ToString_FromTable (DeltaValue* val, DeltaString* at) {

	DeltaTable* t = val->val.tableObj;
	DASSERT(t);
	
	// Do we have overloaded string conversion?

	DeltaValue f;
	if (OperatorOverloading::GetOperatorMember(val, DELTA_OPERATOR_OBJECT_TOSTRING, &f)) {

		UCHECK_PRIMARY_ERROR(
			f.IsCallable(),
			uconstructstr(
				"slot '%s' for %s 0x%x not a function / functor but '%s'",
				DELTA_OPERATOR_OBJECT_TOSTRING,
				DPTR(t)->GetExtClassString(),
				DPTR(t)->GetSerialNo(),
				f.TypeStr()
			)
		);

		DeltaValue s;
		const DeltaValue* args[] = { val, (DeltaValue*) 0 };
		
		UCHECK_DOMINO_ERROR(
			f(args, &s),
			uconstructstr(
				"calling '%s' of type '%s' for %s 0x%x failed",
				DELTA_OPERATOR_OBJECT_TOSTRING,
				f.TypeStr(),
				DPTR(t)->GetExtClassString(),
				DPTR(t)->GetSerialNo()
			)
		);

		UCHECK_PRIMARY_ERROR(
			s.Type() == DeltaValue_String,
			uconstructstr(
				"result of calling member '%s' for %s 0x%x is of type '%s'",
				DELTA_OPERATOR_OBJECT_TOSTRING,
				DPTR(t)->GetExtClassString(),
				DPTR(t)->GetSerialNo(),
				s.TypeStr()
			)
		);

		DPTR(at)->Add(s.ToString());
	}
	else
		DPTR(t)->ConvertToString(at);
}

//*****************************

void DeltaValue::ToString_FromExternId (DeltaValue* val, DeltaString* cvtStr) {
	DASSERT(val->val.externIdObj);
	DPTR(val->val.externIdObj)->ConvertToString(cvtStr);
}

void DeltaValue::ToStringNeverOverloaded_FromExternId (DeltaValue* val, DeltaString* cvtStr) {
	DASSERT(val->val.externIdObj);
	DPTR(val->val.externIdObj)->ConvertToStringBySkippingUserCode(cvtStr);
}

void DeltaValue::ToString_FromNil (DeltaValue* val, DeltaString* cvtStr)
	{ DPTR(cvtStr)->Add(val->TypeStr()); }

void DeltaValue::ToString_FromUndefined (DeltaValue* val, DeltaString* cvtStr)
	{ DPTR(cvtStr)->Add(val->TypeStr()); }

//*****************************

DeltaValue::ConvertToString_Func DeltaValue::convertToStringFuncs[TOTAL_DELTAVALUE_TYPES] = {
	DeltaValue::ToString_FromNumber,
	DeltaValue::ToString_FromString,
	DeltaValue::ToString_FromTable,
	DeltaValue::ToString_FromProgramFunc,
	DeltaValue::ToString_FromLibraryFunc,
	DeltaValue::ToString_FromBool,
	DeltaValue::ToString_FromExternId,
	DeltaValue::ToString_FromNil,
	DeltaValue::ToString_FromMethodFunc,
	DeltaValue::ToString_FromUndefined,
};

DeltaValue::ConvertToString_Func DeltaValue::convertNeverOverloadedToStringFuncs[TOTAL_DELTAVALUE_TYPES] = {
	DeltaValue::ToString_FromNumber,
	DeltaValue::ToString_FromString,
	DeltaValue::ToStringNeverOverloaded_FromTable,
	DeltaValue::ToString_FromProgramFunc,
	DeltaValue::ToStringNeverOverloaded_FromLibraryFunc,
	DeltaValue::ToString_FromBool,
	DeltaValue::ToStringNeverOverloaded_FromExternId,
	DeltaValue::ToString_FromNil,
	DeltaValue::ToString_FromMethodFunc,
	DeltaValue::ToString_FromUndefined,
};

void DeltaValue::ConvertToString (DeltaString* cvtStr) const {
	DASSERT((util_ui32) type < uarraysize(convertToStringFuncs));
	(*convertToStringFuncs[type])(const_cast<DeltaValue*>(this), cvtStr);
}

void DeltaValue::ConvertToStringBySkippingUserCode (DeltaString* at) const {
	DASSERT((util_ui32) type < uarraysize(convertToStringFuncs));
	(*convertNeverOverloadedToStringFuncs[type])(const_cast<DeltaValue*>(this), at);
}

//*****************************

const std::string DeltaValue::ConvertToStringHandler (
		void (DeltaValue::*tostring) (DeltaString* at) const,
		util_ui32 maxLen
	) const {

	VALIDATE_SELF();
	DeltaString* at = DeltaString::New();
	at->SetMaxLength(maxLen);

	try { (this->*tostring)(at); }
	catch (DeltaString::MaxLengthExceededException) 
		{ DPTR(at)->SetMaxLength(0); DPTR(at)->Add("..."); }

	std::string result(DPTR(at)->Content());
	DeltaString::Delete(at);

	return result;
}

const std::string DeltaValue::ConvertToString (void) const
	{ return ConvertToStringHandler(&DeltaValue::ConvertToString, 0); }

const std::string DeltaValue::ConvertToStringBySkippingUserCode (void) const
	{ return ConvertToStringHandler(&DeltaValue::ConvertToStringBySkippingUserCode, toStringMaxLen); }

void DeltaValue::SetMaxLengthForConvertToStringBySkippingUserCode (util_ui32 maxLen)
	{ toStringMaxLen = maxLen; }

util_ui32 DeltaValue::GetMaxLengthForConvertToStringBySkippingUserCode (void)
	{ return toStringMaxLen; }

//-----------------------------------------------------------------

bool DeltaValue::IsCallable (void) const {
	return	type == DeltaValue_ProgramFunc	||
			type == DeltaValue_MethodFunc	||
			type == DeltaValue_LibraryFunc	||
			(type == DeltaValue_Table && DPTR(val.tableObj)->IsCallable());
}

// Call as a function. Applies only to function objects.
// Accepts a generic argument pusher, result parameter and returns true if no
// error occured during evaluation.
//
bool DeltaValue::operator()(PushArguments& argsPusher, DeltaValue* result) {

	VALIDATE_SELF();

	DASSERT(!EXCEPTION_HANDLERS->IsUnwinding());	// Cannot ignore execption handling in library functions!

	if (type == DeltaValue_Table) {

		DeltaTable* table = val.tableObj;
		DeltaValue functor;

		if (!DPTR(table)->IsOverloadingEnabled()) {
			DPTR(DeltaVirtualMachine::GetCallingVM())->SetErrorCode(DELTA_FUNCTOR_OVERLOADING_DISABLED_ERROR)->PrimaryError(
				"in calling %s as function overloading is disabled!", 
				DPTR(table)->GetExtClassString()
			);
			return false;
		} 
		else {
			bool succeeded = DPTR(table)->GetFunctor(&functor);
			if (EXCEPTION_HANDLERS->IsUnwinding())
				return true;

			if (!succeeded || functor.Type() == DeltaValue_Nil) {
				DPTR(DeltaVirtualMachine::GetCallingVM())->SetErrorCode(DELTA_FUNCTOR_SLOT_MISSING_ERROR)->PrimaryError(
					"calling %s as a function while missing '()' functor member!",
					DPTR(table)->GetExtClassString()
				);
				return false;
			} 
			else
				return functor(argsPusher, result);		// Recursive call to bind as function.
		}
	}
	else {
		DeltaVirtualMachine* vm;
		if (type == DeltaValue_ProgramFunc)
			vm = val.programFuncObj.vm;
		else
		if (type == DeltaValue_MethodFunc)
			vm = val.methodFuncObj.vm;
		else
		if (type == DeltaValue_LibraryFunc)
			vm = DeltaVirtualMachine::GetCallingVM();
		else {
			DPTR(DeltaVirtualMachine::GetCallingVM())->SetErrorCode(DELTA_NOT_CALLABLE_ERROR)->PrimaryError(
				"value of type '%s' called as a function!",
				TypeStr()
			);
			return false;
		}

		DELTA_RETURN_IF_PRODUCEDERROR(vm, false);
		DELTA_ON_UNWINDING_RETURN_TEST(vm, true);

		if (!argsPusher(DPTR(vm))) {
			DPTR(vm)->Error(
				"in calling %s an error occured in argument passing!",
				TypeStr()
			);
			return false;
		}

		DELTA_RECORD_VM_SERIALNO(vm);

		DPTR(vm)->ExtCallFunction(this);

		DELTA_VALIDATE_VM_SERIALNO(vm, false);
		DELTA_RETURN_IF_PRODUCEDERROR(vm, false);
		DELTA_ON_UNWINDING_RETURN_TEST(vm, true);

		if (result)
			result->AssignOverloadable(DPTR(vm)->GetReturnValue());

		return true;
	}
}

//********************************
// All implemented through the basic functor caller function, using
// the parameterization capability over agrument storage policy.

bool DeltaValue::PushArrayArguments::operator()(DeltaVirtualMachine* vm) {
	while (n--)
		DPTR(vm)->PushActualArg(*args++);
	return true;
}

bool DeltaValue::PushArrayListArguments::operator()(DeltaVirtualMachine* vm) {
	while (*args)
		DPTR(vm)->PushActualArg(*args++);
	return true;
}

bool DeltaValue::PushListArguments::operator()(DeltaVirtualMachine* vm) {
	for (	std::list<DeltaValue>::const_iterator i = args.begin();
			i != args.end();
			++i	)
			DPTR(vm)->PushActualArg(*i);
	return true;
}

bool DeltaValue::PushListPointerArguments::operator()(DeltaVirtualMachine* vm) {
	for (	std::list<DeltaValue*>::const_iterator i = args.begin();
			i != args.end();
			++i	)
			DPTR(vm)->PushActualArg(*i);
	return true;
}

bool DeltaValue::PushSingleArgument::operator()(DeltaVirtualMachine* vm)
	{ DPTR(vm)->PushActualArg(arg); return true; }

//********************************

bool DeltaValue::operator()(UPTR(const DeltaValue)* args, util_ui16 n, DeltaValue* result) 
	{ return (*this)(utempobj(PushArrayArguments(args, n)), result); }

bool DeltaValue::operator()(UPTR(const DeltaValue)* args, DeltaValue* result) 
	{ return (*this)(utempobj(PushArrayListArguments(args)), result); }

bool DeltaValue::operator()(DeltaValue* result) 
	{ return (*this)(utempobj(PushArrayArguments((UPTR(const DeltaValue)*) 0, 0)), result); }

bool DeltaValue::operator()(const std::list<DeltaValue>& args, DeltaValue* result) 
	{ return (*this)(utempobj(PushListArguments(args)), result); }

bool DeltaValue::operator()(const std::list<DeltaValue*>& args, DeltaValue* result) 
	{ return (*this)(utempobj(PushListPointerArguments(args)), result); }

//********************************

#define	DARG const DeltaValue&

bool DeltaValue::operator()(DARG arg, DeltaValue* result) 
	{ return (*this)(utempobj(PushSingleArgument(arg)), result); }

bool DeltaValue::operator()(DARG arg1, DARG arg2, DeltaValue* result) {
	const DeltaValue* args[] = { &arg2, &arg1, (DeltaValue*) 0 };
	return (*this)(utempobj(PushArrayListArguments(args)), result); 
}

bool DeltaValue::operator()(DARG arg1, DARG arg2, DARG arg3, DeltaValue* result) {
	const DeltaValue* args[] = { &arg3, &arg2, &arg1, (DeltaValue*) 0 };
	return (*this)(utempobj(PushArrayListArguments(args)), result); 
}

bool DeltaValue::operator()(DARG arg1, DARG arg2, DARG arg3, DARG arg4,	DeltaValue* result) {
	const DeltaValue* args[] = { &arg4, &arg3, &arg2, &arg1, (DeltaValue*) 0};
	return (*this)(utempobj(PushArrayListArguments(args)), result); 
}

#undef	DARG

//-----------------------------------------------------------------

static const char* typeStrings[] = {
	DELTA_NUMBER_TYPESTRING,		
	DELTA_STRING_TYPESTRING,		
	DELTA_TABLE_TYPESTRING	" / " DELTA_OBJECT_TYPESTRING,
	DELTA_PROGRAMFUNC_TYPESTRING,
	DELTA_LIBRARYFUNC_TYPESTRING,
	DELTA_BOOL_TYPESTRING,		
	DELTA_EXTERNID_TYPESTRING,	
	DELTA_NIL_TYPESTRING,		
	DELTA_METHODFUNC_TYPESTRING,	
	DELTA_UNDEFINED_TYPESTRING
};

const char* DeltaValue::GetTypeStr (DeltaValueType type) {
	DASSERT(
		type >= 0 && type <= DeltaValue_Undefined &&
		(util_ui32) type < uarraysize(typeStrings)
	);
	return typeStrings[type];
}

const char* DeltaValue::TypeStr (void) const {
	return	type == DeltaValue_Table ?
				DPTR(val.tableObj)->GetExtClassString() : 
				GetTypeStr(type);
}

//-----------------------------------------------------------------
