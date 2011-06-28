// DebugReservedTableAttributes.cpp
// Support the reserved table attributes.
// ScriptFighter Project.
// A. Savidis, July 2008.
//

#include "DebugExprParseActions.h"
#include "DDebug.h"
#include "DeltaObject.h"
#include "ListLib.h"
#include "VectorLib.h"
#include "VMRegLib.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "DeltaValueToDebugWatchValueInfo.h"
#include "DebugClosureInformation.h"
#include "ustlhelpers.h"

DebugExprParseActions::ReservedAttrMap*	
							DebugExprParseActions::reservedAttrs = (ReservedAttrMap*) 0;

/////////////////////////////////////////////////////////////

void DebugExprParseActions::InstallReservedTableAttributeGetters (void) {
	DASSERT(!reservedAttrs);
	reservedAttrs	= DNEW(ReservedAttrMap);
	(*DPTR(reservedAttrs))[DEBUG_TABLE_BASES_HIDDEN_KEY]				= &GetBases;
	(*DPTR(reservedAttrs))[DEBUG_TABLE_DERIVED_HIDDEN_KEY]				= &GetDerived;
	(*DPTR(reservedAttrs))[DEBUG_TABLE_MOSTDERIVED_HIDDEN_KEY]			= &GetMostDerived;
	(*DPTR(reservedAttrs))[DEBUG_TABLE_DELEGATES_HIDDEN_KEY]			= &GetDelegates;
	(*DPTR(reservedAttrs))[DEBUG_TABLE_DELEGATORS_HIDDEN_KEY]			= &GetDelegators;
	(*DPTR(reservedAttrs))[DEBUG_TABLE_OVERLOADING_ENABLED_HIDDEN_KEY]	= &GetOverloadingEnabled;
	(*DPTR(reservedAttrs))[DEBUG_TABLE_MADEIN_HIDDEN_KEY]				= &GetMadeIn;
	(*DPTR(reservedAttrs))[DEBUG_FUNC_OR_METHOD_DEFINEDIN_HIDDEN_KEY]	= &GetDefinedIn; 
	(*DPTR(reservedAttrs))[DEBUG_FUNC_OR_METHOD_CLOSURE_HIDDEN_KEY]		= &GetClosure; 
	(*DPTR(reservedAttrs))["_length_"]									= &GetLength;
	(*DPTR(reservedAttrs))["_indices_"]									= &GetIndices;
	(*DPTR(reservedAttrs))[DEBUG_FUNC_OR_METHOD_VM_HIDDEN_KEY]			= &GetVM;
	(*DPTR(reservedAttrs))[DEBUG_LIBFUNC_BOUNDARGS_HIDDEN_KEY]			= &GetBoundArgs;
}

void DebugExprParseActions::CleanUpReservedTableAttributeGetters (void) 
	{ udelete(reservedAttrs); }

/////////////////////////////////////////////////////////////
// Returns the base prototypes (left 2 right) in a successively 
// indexed table.
//
DeltaValue* DebugExprParseActions::GetBases (DeltaValue* table) {

	DASSERT(IsSingletonCreated());

	if (table->Type() == DeltaValue_Table && 
		GetDeltaObjectValidator()->Validate((DeltaObject*) table->ToTable())	) {

		DeltaValue*	result	= NewTempValue();
		DeltaObject* proto	= (DeltaObject*) table->ToTable();
		DPTR(result)->FromTable(DPTR(proto)->GetBasesAsTable());
		return result;
	}
	else  
		return (DeltaValue*) 0;
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::GetDerived (DeltaValue* table) 
	{ return GetDerivedHandler(table, &DeltaObject::GetDerived); }

DeltaValue* DebugExprParseActions::GetMostDerived (DeltaValue* table)
	{ return GetDerivedHandler(table, &DeltaObject::GetMostDerived); }

DeltaValue* DebugExprParseActions::GetDerivedHandler (DeltaValue* table, DeltaObject* (DeltaObject::*f)(void)) {

	DASSERT(IsSingletonCreated());

	if (table->Type() == DeltaValue_Table && GetDeltaObjectValidator()->Validate((DeltaObject*) table->ToTable()))  {
		DeltaValue*	result	= NewTempValue();
		DeltaObject* proto	= (DeltaObject*) table->ToTable();
		DeltaTable*		derived = (DPTR(proto)->*f)();
		if (derived)
			DPTR(result)->FromTable(derived);
		else
			DPTR(result)->FromNil();
		return result;
	}
	else
		return (DeltaValue*) 0;
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::GetDelegationHandler (DeltaValue* table, DeltaTable* (DeltaTable::*f)(void)) {
	DASSERT(IsSingletonCreated());
	if (table->Type() == DeltaValue_Table) {
		DeltaValue* result	= NewTempValue();
		DPTR(result)->FromTable(
			(DPTR(table->ToTable())->*f)()
		);
		return result;
	}
	else  
		return (DeltaValue*) 0;
}

DeltaValue* DebugExprParseActions::GetDelegates (DeltaValue* table) 
	{ return GetDelegationHandler(table, &DeltaTable::GetDelegatesAsTable); }

DeltaValue* DebugExprParseActions::GetDelegators (DeltaValue* table)
	{ return GetDelegationHandler(table, &DeltaTable::GetDelegatorsAsTable); }

DeltaValue* DebugExprParseActions::GetOverloadingEnabled (DeltaValue* table) {
	DASSERT(IsSingletonCreated());
	if (table->Type() == DeltaValue_Table) {
		DeltaValue* result	= NewTempValue();
		DPTR(result)->FromBool(DPTR(table->ToTable())->IsOverloadingEnabled());
		return result;
	}
	else  
		return (DeltaValue*) 0;
}

DeltaValue*	DebugExprParseActions::GetMadeIn (DeltaValue* table) {
	DASSERT(IsSingletonCreated());
	if (table->Type() == DeltaValue_Table) {
		DeltaValue* result	= NewTempValue();
		DPTR(result)->FromString(table->ToTable()->GetCreationInfo().GetDesc());
		return result;
	}
	else  
		return (DeltaValue*) 0;
}

/////////////////////////////////////////////////////////////	

DeltaValue*	DebugExprParseActions::GetDefinedIn (DeltaValue* func) {

	if (func->Type() != DeltaValue_ProgramFunc && func->Type() != DeltaValue_MethodFunc)
		return (DeltaValue*) 0;

	DeltaCodeAddress		addr;
	DeltaVirtualMachine*	vm;
	DeltaValue*				result	= NewTempValue();
	
	if (func->Type() == DeltaValue_ProgramFunc)
		func->ToProgramFunc(&addr, &vm);
	else {
		DeltaTable* unused;
		func->ToMethodFunc(&addr, &unused, &vm);
	}
	DASSERT(vm);
	
	const DeltaDebugProgramInfo&	dbgInfo	 = vm->GetDebugInfo();
	const DeltaDebugFuncInfo*		funcInfo = dbgInfo.GetFuncByAddr(addr);
	DASSERT(funcInfo);
	result->FromString(uconstructstr("file '%s', line %u", vm->Source(), funcInfo->GetLine()));
	
	return result;
}

/////////////////////////////////////////////////////////////	

DeltaValue*	DebugExprParseActions::GetClosure (DeltaValue* func) {

	if (func->Type() != DeltaValue_ProgramFunc && func->Type() != DeltaValue_MethodFunc)
		return (DeltaValue*) 0;

	const DeltaStdFuncInfo*	funcInfo;
	DeltaVirtualMachine*	vm;
	DeltaClosure*			closure;
	DeltaValue*				result	= NewTempValue();
	
	if (func->Type() == DeltaValue_ProgramFunc)
		func->ToProgramFunc(&funcInfo, &vm, &closure);
	else {
		DeltaTable* unused;
		func->ToMethodFunc(&funcInfo, &unused, &vm, &closure);
	}
	
	DASSERT(vm && funcInfo);
	if (closure)
		DebugClosureInformation::ConvertToDelta(funcInfo, closure, result);
	else
		result->FromNil();
	return result;
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::GetLength (DeltaValue* table) {

	DASSERT(IsSingletonCreated());
	util_ui32 length = 0;
	bool found = false;

	switch (table->Type()) {

		case DeltaValue_Table : {
			length = DPTR(table->ToTable())->Total();
			found = true;
			break;
		}

		case DeltaValue_ExternId : {

			std::string typeStr;
			void* val = table->ToExternId(typeStr);

			if (typeStr == LIST_TYPE_STR) {
				std::list<DeltaValue>* l = DeltaList_Get(val, typeStr);
				DASSERT(l);
				length = (util_ui32) l->size();
				found = true;
			}
			else
			if (typeStr == VECTOR_TYPE_STR) {
				uvector<DeltaValue>* v = DeltaVector_Get(val);
				DASSERT(v);
				length = v->size();
				found = true;
			}

			break;
		}

		// for warning avoiding reasons
		default:;
	}

	if (found) {
		DeltaValue* result	= NewTempValue();
		result->FromNumber(length);
		return result;
	}
	else
		return (DeltaValue*) 0;
	
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::GetIndices (DeltaValue* table) {

	DASSERT(IsSingletonCreated());

	if (table->Type() == DeltaValue_Table) {
		DeltaValue* result	= NewTempValue();
		result->FromTable(table->ToTable()->GetIndices());
		return result;
	}
	else
		return (DeltaValue*) 0;
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::GetVM (DeltaValue* func) {

	if (func->Type() != DeltaValue_ProgramFunc && func->Type() != DeltaValue_MethodFunc)
		return (DeltaValue*) 0;

	DeltaCodeAddress		addr;
	DeltaVirtualMachine*	vm;
	DeltaValue*				result	= NewTempValue();
	
	if (func->Type() == DeltaValue_ProgramFunc)
		func->ToProgramFunc(&addr, &vm);
	else {
		DeltaTable* t;
		func->ToMethodFunc(&addr, &t, &vm);
	}

	if (vm)
		DeltaVM_Make(result, vm);
	else
		DPTR(result)->FromNil();
	return result;
}

/////////////////////////////////////////////////////////////

DeltaValue* DebugExprParseActions::GetBoundArgs (DeltaValue* func) {

	DeltaValue* result = NewTempValue();
	if (func->Type() == DeltaValue_LibraryFunc) {
		if (const DeltaLibraryFuncArgsBinder* binder = func->GetLibraryFuncArgsBinder())
			result->Assign(DPTR(binder)->GetListValue());
		else 
			result->FromNil();
	}
	return result;
}

/////////////////////////////////////////////////////////////

DebugEvaluatedExpr* DebugExprParseActions::Manage_TableContent_ReservedAttr (DebugEvaluatedExpr* e, const char* attr) {

	DASSERT(IsSingletonCreated());
	CHECK_ERROR_EXIT();

	DeltaValue* result	= (DeltaValue*) 0;
	DeltaValue* obj		= DNULLCHECK(DPTR(e)->GetResult());

	if (obj->Type() == DeltaValue_ExternId) {	// Try reserved externid attributes.
		if (!Manage_ExternIdContent(obj, DeltaValue(attr), result = NewTempValue()))
			unullify(result);
	}
	else {										// Try reserved table attributes.
		ReservedAttrMap::iterator i = DPTR(reservedAttrs)->find(attr);
		if (i != DPTR(reservedAttrs)->end())
			result = (*i->second)(obj);
	}

	if (!result) {
		Error("Can't index %s.%s!", obj->TypeStr(), attr);
		return (DebugEvaluatedExpr*) 0;
	}
	else
		return DebugEvaluatedExpr::New(result, false);
}

/////////////////////////////////////////////////////////////
