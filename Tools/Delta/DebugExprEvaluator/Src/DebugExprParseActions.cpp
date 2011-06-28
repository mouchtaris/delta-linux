// DebugExprParseActions.cpp
// Debug expression evaluator main translation schemes.
// ScriptFighter Project.
// A. Savidis, February 2002.
// Refactored June 2008, disabling any calls to overloaded operators.
//

#include <stdarg.h>

#include "DebugExprLexAn.h"
#include "DebugExprParseActions.h"
#include "DDebug.h"
#include "ufunctors.h"
#include "Jump.h"
#include "Arithmetic.h"
#include "Table.h"
#include "DeltaDebugExtensions.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaTable.h"
#include "DeltaObject.h"
#include "DeltaTableFactory.h"
#include "ListLib.h"
#include "VectorLib.h"
#include "VMRegLib.h"
#include "DebugAttributeInformation.h"
#include "ustlhelpers.h"

#include <algorithm>

/////////////////////////////////////////////////////////////

DeltaVirtualMachine*		DebugExprParseActions::targetVM		= (DeltaVirtualMachine*) 0;
char						DebugExprParseActions::errorText[1024];
bool						DebugExprParseActions::hasError		= false;
DebugEvaluatedExpr*			DebugExprParseActions::resultExpr	= (DebugEvaluatedExpr*) 0;
std::list<DeltaValue*>*		DebugExprParseActions::tempObjects	= (std::list<DeltaValue*>*) 0;

USINGLETON_INSTANCE(usingleton<ExprListDestructorClass>)

DebugExprList* EXPRLIST_LATE_DESTROY (DebugExprList* e) {
	ExprListDestructor::GetSingleton().add(DNEWCLASS(DestroyableExprList, (e))); 
	return e;
}

/////////////////////////////////////////////////////////////

void DebugExprParseActions::SingletonCreate (void) {
	
	DASSERT(!IsSingletonCreated());
	tempObjects	 = DNEW(std::list<DeltaValue*>);	
	InstallReservedTableAttributeGetters();
	InstallUnaryLibraryFunctions();
	InitialiseMonitors();
	ExprListDestructor::SingletonCreate();
}

void DebugExprParseActions::SingletonDestroy (void) {
	
	DASSERT(IsSingletonCreated());
	
	udelete(tempObjects);
	CleanUpReservedTableAttributeGetters();
	CleanUpUnaryLibraryFunctions();
	CleanUpMonitors();
	ExprListDestructor::SingletonDestroy();
}

/////////////////////////////////////////////////////////////

void DebugExprParseActions::Initialise (DeltaVirtualMachine* vm) {

	DASSERT(IsSingletonCreated());
	DASSERT(vm);

	CleanUp ();
	hasError = false;
	targetVM = vm;
}
					
/////////////////////////////////////////////////////////////

void DebugExprParseActions::CleanUp (void) {

	DASSERT(IsSingletonCreated());

	unullify(targetVM);
	unullify(resultExpr);
	ClearTempObjects();
	ExprListDestructor::GetSingleton().commit();
}
					
/////////////////////////////////////////////////////////////
// This is the callback passed in VM evaluation functions.
//
static void ErrorCallback (const char* error) 
	{ DebugExprParseActions::Error(error); }

/////////////////////////////////////////////////////////////
// When an error is detected, the lexical analyser is forced
// to finish.
//
void DebugExprParseActions::Error (const char* format,...) {

	va_list args;
	va_start(args, format);
	vsprintf(errorText, format, args);
	va_end(args);

	DebugExprLexAn::ForceEOF();
	hasError = true;
}
					
/////////////////////////////////////////////////////////////

bool DebugExprParseActions::HasError (void) 
	{ DASSERT(IsSingletonCreated()); return hasError; }
					
/////////////////////////////////////////////////////////////

const std::string DebugExprParseActions::GetLastError (void) {
	DASSERT(IsSingletonCreated()); 
	DASSERT(strlen(errorText) < sizeof(errorText));
	return errorText;
}

/////////////////////////////////////////////////////////////
// Returns a new temporary object for the evaluation of the
// expression.
//
DeltaValue* DebugExprParseActions::NewTempValue (void) {
	
	DASSERT(IsSingletonCreated());

	DeltaValue* temp = DNEW(DeltaValue);
	DPTR(tempObjects)->push_back(temp);
	return temp;
}

/////////////////////////////////////////////////////////////
// Cleans-up all temporary objects created during expression
// evaluation.
//
void DebugExprParseActions::ClearTempObjects (void) {
	
	DASSERT(IsSingletonCreated());

	std::for_each(
		DPTR(tempObjects)->begin(),
		DPTR(tempObjects)->end(),
		uptrdestructorfunctor<DeltaValue*>()
	);

	DPTR(tempObjects)->clear();
}

/////////////////////////////////////////////////////////////

#define	EXECUTE_RELATIONAL(op)	\
	case DebugExprRelOp_##op :	\
	{ result = Execute_##op(arg1, arg2, targetVM, ErrorCallback); break; }

DebugEvaluatedExpr* DebugExprParseActions::Manage_RelationalExpr (
		DebugEvaluatedExpr*		e1,
		DebugEvaluatedExpr*		e2, 
		DebugExprRelationalOp	op
	) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	DASSERT(e1 && e2);

	bool result = false;
	DeltaValue* arg1 = DPTR(e1)->GetResult();
	DeltaValue* arg2 = DPTR(e2)->GetResult();

	if (arg1->Type() == DeltaValue_Table || arg2->Type() == DeltaValue_Table)
		result = Manage_RelationalExprForTables(arg1, arg2, op);
	else
		switch(op) {
			EXECUTE_RELATIONAL(EQ)
			EXECUTE_RELATIONAL(NE)
			EXECUTE_RELATIONAL(GE)
			EXECUTE_RELATIONAL(GT)
			EXECUTE_RELATIONAL(LE)
			EXECUTE_RELATIONAL(LT)
			default: DASSERT(false);
		}

	if (HasError())	// If an error has been caused from this evaluation.
		return (DebugEvaluatedExpr*) 0;
	else {
		DeltaValue* obj = NewTempValue();
		obj->FromBool(result);
		return DebugEvaluatedExpr::New(obj, false);
	}
}

/////////////////////////////////////////////////////////////

static const char* relOpStrings[] = {
	"==",
	"!=",
	">",
	">=",
	"<",
	"<="
};

bool DebugExprParseActions::Manage_RelationalExprForTables (
		DeltaValue*			arg1,
		DeltaValue*			arg2, 
		DebugExprRelationalOp	op
	) {
	if (op == DebugExprRelOp_EQ)
		return arg1->Type() == arg2->Type() && arg1->ToTable() == arg2->ToTable();
	else
	if (op == DebugExprRelOp_NE)
		return  arg1->Type() != arg2->Type() ||  arg1->ToTable() != arg2->ToTable();
	else {
		Error(
			"Can't evaluate relational %s %s %s (invalid args)!",
			arg1->TypeStr(),
			relOpStrings[op],
			arg2->TypeStr()
		);
		return false;
	}
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_ExprANDExpr (
		DebugEvaluatedExpr*	e1,
		DebugEvaluatedExpr*	e2
	) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	DASSERT(e1 && e2);

	DeltaValue* obj = NewTempValue();
	DPTR(obj)->FromBool(
		DPTR(e1)->GetResult()->TrueTest() && DPTR(e2)->GetResult()->TrueTest()
	);

	return DebugEvaluatedExpr::New(obj, false);
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_ExprORExpr (
		DebugEvaluatedExpr*	e1,
		DebugEvaluatedExpr*	e2
	) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	DASSERT(e1 && e2);

	DeltaValue* obj = NewTempValue();
	DPTR(obj)->FromBool(
		DPTR(e1)->GetResult()->TrueTest() || DPTR(e2)->GetResult()->TrueTest()
	);

	return DebugEvaluatedExpr::New(obj, false);
}

/////////////////////////////////////////////////////////////

static const char* arithOpStrings[] = {
	"+",
	"-",
	"/",
	"*",
	"%"
};

#define	EXECUTE_ARITHMETIC(op)	\
	case DebugExpArithOp_##op : \
{ Execute_##op(arg1, arg2, result, targetVM, ErrorCallback); break; }

DebugEvaluatedExpr* DebugExprParseActions::Manage_ArithmeticExpr (
		DebugEvaluatedExpr*		e1,
		DebugEvaluatedExpr*		e2,
		DebugExprArithmeticOp	op
	) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	DASSERT(e1 && e2);

	DeltaValue* arg1 = DPTR(e1)->GetResult();
	DeltaValue* arg2 = DPTR(e2)->GetResult();	

	if (arg1->Type() == DeltaValue_Table || arg2->Type() == DeltaValue_Table) {
		Error(
			"Can't evaluate arithmetic %s %s %s (invalid args)!",
			arg1->TypeStr(),
			arithOpStrings[op],
			arg2->TypeStr()
		);
		return (DebugEvaluatedExpr*) 0;
	}

	DeltaValue* result = NewTempValue();

	switch (op) {
		EXECUTE_ARITHMETIC(ADD)
		EXECUTE_ARITHMETIC(SUB)
		EXECUTE_ARITHMETIC(DIV)
		EXECUTE_ARITHMETIC(MUL)
		EXECUTE_ARITHMETIC(MOD)
		default: DASSERT(false);
	}

	return	HasError() ?	
				(DebugEvaluatedExpr*) 0 :
				DebugEvaluatedExpr::New(result, false);	
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_UMINUSExpression (DebugEvaluatedExpr* e) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	DASSERT(e);

	DeltaValue* arg = DPTR(e)->GetResult();
	if (arg->Type() != DeltaValue_Number) {
		Error("Can't use unary '-' on %s (not a number)!", arg->TypeStr());
		return (DebugEvaluatedExpr*) 0;
	}
	else {
		DeltaValue* result = NewTempValue();
		result->FromNumber(-arg->ToNumber());
		return DebugEvaluatedExpr::New(result, false);
	}
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_NOTExpression (DebugEvaluatedExpr* e) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	DASSERT(e);

	DeltaValue* result = NewTempValue();
	result->FromBool(
		!DPTR(e)->GetResult()->TrueTest()
	);

	return DebugEvaluatedExpr::New(result, false);
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_ConstValue (DeltaNumberValueType val) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();

	DeltaValue* result = NewTempValue();
	result->FromNumber(val);
	
	return DebugEvaluatedExpr::New(result, false);
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_ConstValue (void) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();

	DeltaValue* result = NewTempValue();
	result->FromNil();
	
	return DebugEvaluatedExpr::New(result, false);
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_ConstValue (bool val) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();

	DeltaValue* result = NewTempValue();
	result->FromBool(val);
	
	return DebugEvaluatedExpr::New(result, false);
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_ConstValue (char* val) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();

	DeltaValue* result = NewTempValue();
	result->FromString(val);
	return DebugEvaluatedExpr::New(result, false);
}

/////////////////////////////////////////////////////////////

char* DebugExprParseActions::Manage_Name (char* s1, char* s2) 
	{ return DebugEvaluatedExpr::StringLateDestroy(ucopystr(uconstructstr("%s::%s", s1, s2))); }

char* DebugExprParseActions::Manage_AttributeIndex (char* s) 
	{ return DebugEvaluatedExpr::StringLateDestroy(ucopystr(uconstructstr("@%s", s))); }

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr*	DebugExprParseActions::Manage_LAMBDA (void) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	CHECK_DEBUGINFO_INCLUDED();

	if (!GetDebugger(targetVM).IsTracingAtProgramFunction()) {
		Error("Can't use lambda (allowed only in functions and methods)!");
		return (DebugEvaluatedExpr*) 0;
	}
	else {
		const DeltaDebugFuncInfo*	func		= GetDebugger(targetVM).GetTracedProgramFunction();
		const DeltaClosure*			closure		= GetDebugger(targetVM).GetTracedProgramFunctionClosure();
		DASSERT(func);
		DeltaValue* result = NewTempValue(); 
		DeltaValue* self	= GetDebugger(targetVM).GetActualArg(DELTA_SELF_POINTER_ID);
		if (self)
			result->FromMethodFunc(func->GetAddress(), self->ToTable(), targetVM, const_cast<DeltaClosure*>(closure));
		else
			result->FromProgramFunc(func->GetAddress(), targetVM, const_cast<DeltaClosure*>(closure));
		return DebugEvaluatedExpr::New(result, false);
	}
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::Manage_GlobalFunction (DeltaVirtualMachine* vm, const std::string& id) {
	DeltaCodeAddress addr = vm->GlobalFuncAddress(id.c_str());
	return addr != DELTA_CANTBE_A_FUNCTION_ADDRESS ? new (NewTempValue()) DeltaValue(addr, vm) : (DeltaValue*) 0;
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_AnyLvalue (const std::string& id) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	CHECK_DEBUGINFO_INCLUDED();

	DeltaValue* obj = (DeltaValue*) 0;

	// TRY LOCAL VAR, LOCAL CONST, FORMAL ARG OR CLOSURE VAR.
	if (GetDebugger(targetVM).IsTracingAtProgramFunction())
		if (!(obj = GetDebugger(targetVM).GetLocalVar(id)) &&
			!(obj = GetDebugger(targetVM).GetLocalVar((std::string(DELTA_HIDDENCONSTVAR_NAME_PREFIX) + id))) &&
			!(obj = GetDebugger(targetVM).GetActualArg(id)))
			obj = GetDebugger(targetVM).GetClosureVar(id);
	
	// TRY OUTER FUNCTION
	if (!obj) {
		DeltaCodeAddress addr, line, scope;
		if (GetDebugger(targetVM).GetScopeClosestFunction(id, &addr, &line, &scope)) {
			if (GetDebugger(targetVM).IsTracingAtProgramFunction() &&
				GetDebugger(targetVM).GetTracedProgramFunction()->GetAddress() == addr)	// Is it the current function actually?
				return Manage_LAMBDA();
			else {
				obj = NewTempValue();
				obj->FromProgramFunc(addr, targetVM);
			}
		}
	}

	// TRY VISIBLE STATIC VAR
	if (!obj) {											

		std::string prefix(DELTA_STATICVAR_NAME_PREFIX);

		if (!(obj = GetDebugger(targetVM).GetGlobalVar(prefix + id, true)) && 
			GetDebugger(targetVM).IsTracingAtProgramFunction()) {

			std::list<std::string> funcs;
			GetDebugger(targetVM).GetAllEnclosingFunctions(&funcs);

			for (std::list<std::string>::iterator i = funcs.begin(); i != funcs.end(); ++i)
				if ((obj = GetDebugger(targetVM).GetGlobalVar(prefix + *i + id, true)) != NULL)
					break;
		}
	}

	// TRY GLOBAL VAR / CONST
	if (!obj)
		if (!(obj = GetDebugger(targetVM).GetGlobalVar(id, false)))
			obj = GetDebugger(targetVM).GetGlobalVar(std::string(DELTA_HIDDENCONSTVAR_NAME_PREFIX) + id, false);
	
	// TRY LIBFUNC
	if (!obj) {
		DeltaLibraryFunc f = DeltaLibFuncBinder::Get(id.c_str());
		if (!f) {	// Try prefixing the name with every opened namespace.
			typedef std::list<std::string> NameList;
			const NameList& l = targetVM->GetDebugInfo().GetOpenedNamespaces();
			for (NameList::const_iterator i = l.begin(); i != l.end(); ++i)
				if ((f = DeltaLibFuncBinder::Get(*i + DELTA_LIBRARYNAMESPACE_SEPARATOR + id)) != NULL)
					break;
		}
		if (f)
			{ obj = NewTempValue(); DPTR(obj)->FromLibraryFunc(f); }
	}
	// TRY A USED LIBRARY CONST
	if (!obj) {

		DeltaNumberValueType	num;
		std::string				str;

		if (targetVM->GetDebugInfo().GetUsedLibraryConst(id, &num))
			{ obj = NewTempValue(); DPTR(obj)->FromNumber(num); }
		else
		if (targetVM->GetDebugInfo().GetUsedLibraryConst(id, &str))
			{ obj = NewTempValue(); DPTR(obj)->FromString(str); }
		else {
			
			typedef std::list<std::string> NameList;
			const NameList& l = targetVM->GetDebugInfo().GetOpenedNamespaces();

			for (NameList::const_iterator i = l.begin(); i != l.end(); ++i) {
				std::string fullName = *i + DELTA_LIBRARYNAMESPACE_SEPARATOR + id;
				if (targetVM->GetDebugInfo().GetUsedLibraryConst(fullName, &num))
					{ obj = NewTempValue(); DPTR(obj)->FromNumber(num); break; }
				else
				if (targetVM->GetDebugInfo().GetUsedLibraryConst(fullName, &str))
					{ obj = NewTempValue(); DPTR(obj)->FromString(str); break; }
			}
		}
	}

	// TRY A BYTECODE LIBRARY FUNC
	if (!obj) {

		std::list<std::string> l = utokenizestr(id, ":");
		if (l.size() == 2) {		// <lib>::<func>

			std::string libName = l.front(), funcName = l.back();

			typedef std::list<std::string> NameList;
			const NameList& l = targetVM->GetDebugInfo().GetUsedByteCodeLibraries();

			if (std::find(l.begin(), l.end(), libName) != l.end())
				if (DeltaVirtualMachine* vm = DeltaVM_Get(DNULLCHECK(GetDebugger(targetVM).GetGlobalVar(libName, false))))
					obj = Manage_GlobalFunction(vm, funcName);
		}
	}

	if (!obj) {											// Could not find.
		Error("'%s': unknown variable!", id.c_str());
		return (DebugEvaluatedExpr*) 0;
	}
	else 
		return DebugEvaluatedExpr::New(obj, true);
	
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr*	DebugExprParseActions::Manage_FormalLvalue (const std::string& id) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	CHECK_DEBUGINFO_INCLUDED();

	DeltaValue* obj = GetDebugger(targetVM).GetActualArg(id.c_str());
	if (!obj) {
		Error("'%s': unknown formal arg!", id.c_str());
		return (DebugEvaluatedExpr*) 0;
	}
	else 
		return DebugEvaluatedExpr::New(obj, true);
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_LocalLvalue (const std::string& id) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	CHECK_DEBUGINFO_INCLUDED();

	DeltaValue* obj = GetDebugger(targetVM).GetLocalVar(id.c_str());
	if (!obj) {
		Error("'%s': unknown local var!", id.c_str());
		return (DebugEvaluatedExpr*) 0;
	}
	else
		return DebugEvaluatedExpr::New(obj, true);
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_GlobalLvalue (const std::string& id, bool isStatic) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();
	CHECK_DEBUGINFO_INCLUDED();

	std::string name(isStatic ? DELTA_STATICVAR_NAME_PREFIX : "");
	name.append(id);

	if (DeltaValue* obj = GetDebugger(targetVM).GetGlobalVar(name.c_str(), isStatic))
		return DebugEvaluatedExpr::New(obj, true);
	else
	if (!isStatic && (obj = Manage_GlobalFunction(targetVM, id)))
		return DebugEvaluatedExpr::New(obj, true);
	else {
		Error("'%s': unknown global var!", name.c_str());
		return (DebugEvaluatedExpr*) 0;
	}
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr*	DebugExprParseActions::Manage_AssignLvalue (DebugEvaluatedExpr* lvalue, DebugEvaluatedExpr* rvalue) {
	if (!lvalue || !rvalue)
		return (DebugEvaluatedExpr*) 0;
	else {
		lvalue->GetResult()->Assign(rvalue->GetResult());
		return lvalue;		
	}
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_TableContent (DebugEvaluatedExpr* table, const char* index, bool isLocal) {
	
	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();

	if (*index == '@' && DPTR(table)->GetResult()->Type() == DeltaValue_Table) { // An attribute is requested.
		
		DeltaTable* owner = DPTR(table)->GetResult()->ToTable();

		if (const DeltaTable::Attribute* attr = DPTR(owner)->GetAttribute(index + 1)) {
			DeltaValue* result = NewTempValue();
			DebugAttributeInformation::ConvertToDelta(attr, result);
			return DebugEvaluatedExpr::New(result, false);	// Attributes immutable in expr evaluation.
		}
		else {
			Error("'%s': unknown attribute in '%s'!", index, DPTR(owner)->GetExtClassString()); 
			return (DebugEvaluatedExpr*) 0;
		}
	}
	else {
		DeltaValue* indexObj = NewTempValue();
		DPTR(indexObj)->FromString(index);

		return Manage_TableContent(table, DebugEvaluatedExpr::New(indexObj, false), isLocal);
	}
}

/////////////////////////////////////////////////////////////

bool DebugExprParseActions::Manage_ExternIdContent (DeltaValue* val, const DeltaValue& index, DeltaValue* result) {

	DASSERT(val->Type()== DeltaValue_ExternId);

	if (index.Type() == DeltaValue_Number) {
		if (const DeltaExternIdFieldGetter* fieldGetter	= val->GetExternIdFieldGetter())
			return	DPTR(fieldGetter)->Get(
						val->ToExternId(utempobj(std::string())),
						(util_ui32) index.ToNumber(), 
						result
					);
	}
	else
	if (index.Type() == DeltaValue_String) {
		std::string fieldName = index.ToString();
		if (DeltaExternId::IsReservedFieldKey(fieldName)) 
			{ DeltaExternId::GetReservedField(fieldName, *val, result); return true; }
		else
		if (const DeltaExternIdFieldGetter* fieldGetter	= val->GetExternIdFieldGetter())
			return DPTR(fieldGetter)->Get(val->ToExternId(utempobj(std::string())), fieldName, result);
	}

	return false;	// Fallback case.
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_TableContent (DebugEvaluatedExpr* e1, DebugEvaluatedExpr* e2, bool isLocal) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();

	DeltaValue* result = NewTempValue();
	bool found = false;

	DeltaValue* table = DPTR(e1)->GetResult();
	DeltaValue* index = DPTR(e2)->GetResult();

	if (table->Type() == DeltaValue_Table) {
		
		if (isLocal)
			DPTR(table->ToTable())->BasicGetBySkippingUserCode(*index, result);
		else
			DPTR(table->ToTable())->GetBySkippingUserCode(*index, result);
		found = true;
	}
	else
	if (table->Type() == DeltaValue_MethodFunc) {

		if (index->Type() == DeltaValue_String && index->ToString() == DELTA_SELF_POINTER_ID) {
			DeltaCodeAddress		addr;
			DeltaTable*				owner;
			DeltaVirtualMachine*	vm;
			table->ToMethodFunc(&addr, &owner, &vm);
			result->FromTable(owner);
			found = true;
		}
	}
	else 
	if (table->Type() == DeltaValue_ExternId)
		found = Manage_ExternIdContent(table, *index, result);

	if (!found) {	
		Error("Can't index %s[%s]!", table->TypeStr(), index->TypeStr()); 
		return (DebugEvaluatedExpr*) 0;
	}
	else
	if (table->Type() == DeltaValue_MethodFunc || table->Type() == DeltaValue_ExternId || table->Type() == DeltaValue_LibraryFunc)
		return DebugEvaluatedExpr::New(result, false); // Self immutable in debug expr eval
	else
		return DebugEvaluatedExpr::New(result, table, index);
}

/////////////////////////////////////////////////////////////
// @x equiv self..x
//
DebugEvaluatedExpr*	DebugExprParseActions::Manage_ATTRIBUTE (const std::string& id) {

	CHECK_ERROR_EXIT();
	CHECK_DEBUGINFO_INCLUDED();

	DeltaValue* obj = (DeltaValue*) 0;
	if (id == DELTA_SELF_POINTER_ID) { // The preceeding @ has already been removed.
		DeltaObject* self = DPTR(targetVM)->GetCurrConstructedObject();
		if (!self) {
			Error("Can't use %s (not in object constructor)!", DELTA_NEWSELF_POINTER_ID); 
			return (DebugEvaluatedExpr*) 0;
		}
		else {
			DeltaValue* result = NewTempValue();
			result->FromTable(self);
			return DebugEvaluatedExpr::New(result, false);
		}
	}
	else {
		if (GetDebugger(targetVM).IsTracingAtProgramFunction())
			obj = GetDebugger(targetVM).GetActualArg(DELTA_SELF_POINTER_ID);

		if (!obj) {
			Error("Can't use @%s (attrs allowed only in methods)!", id.c_str());
			return (DebugEvaluatedExpr*) 0;
		}

		return	Manage_TableContent(
					DebugEvaluatedExpr::New(obj, true), 
					id.c_str(), 
					true
				);
	}
}

/////////////////////////////////////////////////////////////


void DebugExprParseActions::SetResultExpression (DebugEvaluatedExpr* e) {
	DASSERT(IsSingletonCreated());
	resultExpr = e;
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::GetResultExpression (void) {
	DASSERT(IsSingletonCreated());
	return resultExpr;
}

/////////////////////////////////////////////////////////////
