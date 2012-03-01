// DeltaLibraryFuncArgsBinder.cpp
// ScriptFigher Project.
// A. Savidis, September 2008.
//
#include "DeltaLibraryFuncArgsBinder.h"
#include <algorithm>
#include "uerrorclass.h"

///////////////////////////////////////////////////////////////

DeltaLibraryFuncArgsBinder::DeltaLibraryFuncArgsBinder (CallerCheckerFunc f) {

	listVal.SetResidencyType(DeltaValue::Contained, (CollectableContainer*) this);

	DeltaVirtualMachine::StdUtilities* utils = DNULLCHECK(DeltaVirtualMachine::GetStdUtilities());
	utils->MakeList(&listVal);
	DPTR(listVal.GetExternId())->GetCreationInfo().SetCreatorInformation(DELTALIBFUNCARGSBINDER_CLASSID);

	listPtr			= &utils->GetList(&listVal);
	listCont		= utils->GetListContainer(&listVal);
	callerChecker	= f;
}

//**************************

DeltaLibraryFuncArgsBinder::DeltaLibraryFuncArgsBinder (const DeltaLibraryFuncArgsBinder& binder) {
	new(this) DeltaLibraryFuncArgsBinder;
	callerChecker = binder.callerChecker;
	const std::list<DeltaValue>& l = *DNULLCHECK(binder.listPtr);
	for (std::list<DeltaValue>::const_iterator i = l.begin(); i != l.end(); ++i)
		BindArg(*i);	
}

//**************************

DeltaLibraryFuncArgsBinder*	DeltaLibraryFuncArgsBinder::New (const DeltaLibraryFuncArgsBinder* b) 
	{ return b ? DNEWCLASS(DeltaLibraryFuncArgsBinder, (*DPTR(b))) : DNEW(DeltaLibraryFuncArgsBinder); }

DeltaLibraryFuncArgsBinder*	DeltaLibraryFuncArgsBinder::New (CallerCheckerFunc checker) 
	{ DASSERT(checker); return DNEWCLASS(DeltaLibraryFuncArgsBinder, (checker)); }

///////////////////////////////////////////////////////////////

bool DeltaLibraryFuncArgsBinder::IsLibraryMethodMatchingCaller (
		const DeltaValue&		caller,
		const DeltaValue&		method,
		DeltaValue*				actualCaller,
		bool					bounded
	) {
	if (method.Type() != DeltaValue_LibraryFunc)
		return false;

	if (const DeltaLibraryFuncArgsBinder* b = method.GetLibraryFuncArgsBinder()) {
		if (bounded) {
			if (DPTR(b)->MatchesCallerDirectly(caller))
				{ actualCaller->Assign(caller); return true; }
		}
		else {
			CallerCheckerHandler checker(b->GetCallerChecker());
			if (DPTR(b)->MatchesCaller(caller, &checker))
				{ actualCaller->Assign(checker.GetCaller()); return true; }
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////

bool DeltaLibraryFuncArgsBinder::PushCallerIfLibraryMethod (
		const DeltaValue&		caller,
		const DeltaValue&		method, 
		DeltaVirtualMachine*	vm,
		bool					bounded
	) {
	DASSERT(method.Type() == DeltaValue_LibraryFunc);

	DeltaValue actualCaller;
	if (IsLibraryMethodMatchingCaller(caller, method, &actualCaller, bounded))
		DPTR(vm)->PushActualArg(actualCaller);
	else
	if (UERROR_ISRAISED()) {
		DPTR(vm)->PossiblePrimaryError(
			"while trying to match caller for library method '%s'",
			method.ConvertToStringBySkippingUserCode().c_str()
		); 
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////

bool DeltaLibraryFuncArgsBinder::CallerCheckerPred (DeltaObject* obj, void* closure) {
	CallerCheckerHandler* handler = (CallerCheckerHandler*) closure;
	if (handler->Check(DeltaValue(obj)))
		return true;	// Proto matches as it is.
	else
	if (DPTR(obj)->IsUserDataOfExternId() && handler->Check(DeltaValue(DPTR(obj)->GetExternIdWhenUserData())))
		return true;	// Externid of object matches.
	else
		return false;
}

///////////////////////////////////////////////////////////////

bool DeltaLibraryFuncArgsBinder::MatchesCallerDirectly (const DeltaValue& val) const
	{ return callerChecker ? (*callerChecker)(val) : false; }

///////////////////////////////////////////////////////////////

bool DeltaLibraryFuncArgsBinder::MatchesCaller (const DeltaValue& val, CallerCheckerHandler* handler) const {
	if (!handler->HasCallerChecker())			// Most quick case to check, no caller matches.
		return false;
	else {
		DeltaObject* p = (DeltaObject*) 0;
		if (val.Type() == DeltaValue_ExternId)	// Most common expected case.
			if (handler->Check(val))			// Matched directly the externid.
				return true;
			else
			if (!(p = (DeltaObject*) const_cast<DeltaValue&>(val).GetExternIdUserData()))
				return false;					// No matching of externid which has no user data.
			else
				DASSERT(GetDeltaObjectValidator()->Validate(p) && DPTR(p)->IsOfClass(DELTA_OBJECT_CLASS_ID));
		else									// If no externid or not an object then should match the original caller.
		if (val.Type() == DeltaValue_Table) {
			DeltaTable* t =  const_cast<DeltaValue&>(val).ToTable();
			if (t->IsOfClass(DELTA_TABLE_CLASS_ID))
				return handler->Check(val);
			else {
				p = (DeltaObject*) t;
				DASSERT(GetDeltaObjectValidator()->Validate(p) && p->IsOfClass(DELTA_OBJECT_CLASS_ID));
			}
		}
		else
			return handler->Check(val);

		DASSERT(p);
		if (!DPTR(p)->Visit(&CallerCheckerPred, handler))
			UDOMINOERROR("In library method: error while trying to match a caller!");
		else
			return handler->Matched();
		return false;	// Fallback case.
	}
}

///////////////////////////////////////////////////////////////

void DeltaLibraryFuncArgsBinder::PushArgs (DeltaVirtualMachine* vm) const {
	for (std::list<DeltaValue>::const_reverse_iterator i = listPtr->rbegin(); i != listPtr->rend(); ++i)
		DPTR(vm)->PushActualArg(*i);
}

///////////////////////////////////////////////////////////////

void DeltaLibraryFuncArgsBinder::ConvertToStringHandler (
		DeltaString* at,
		void (DeltaValue::*tostring) (DeltaString* at) const
	) {
	if (GetTotalArgs()) {
		if (GetPrintState().IsInProgress())
			DPTR(at)->Add("self_binder");
		else {
			DELTA_CONTAINER_TOSTRING_PROCESSING();
			at->Add("(bound:");
			(listVal.*tostring)(at);
			at->Add(")");
		}
	}
	if (callerChecker)
		at->Add("[callable as method]");
}

///////////////////////////////////////////////////////////////

void DeltaLibraryFuncArgsBinder::ConvertToString (DeltaString* at, void* closure)
	{ ConvertToStringHandler(at, &DeltaValue::ConvertToString); }

void DeltaLibraryFuncArgsBinder::ConvertToStringBySkippingUserCode (DeltaString* at, void* closure)
	{ ConvertToStringHandler(at, &DeltaValue::ConvertToStringBySkippingUserCode); }

///////////////////////////////////////////////////////////////

void DeltaLibraryFuncArgsBinder::ExplicitDestructor (void) 
	{ unullify(listCont); listVal.Undefine(); CollectableContainer::ExplicitDestructor(); }

DeltaLibraryFuncArgsBinder::~DeltaLibraryFuncArgsBinder()
	{ ExplicitDestructor(); }

///////////////////////////////////////////////////////////////

void DeltaLibraryFuncArgsBinder::BindAll (
		 DeltaLibraryObjectCreator::FuncEntry*	funcs,
		 util_ui16								from,
		 util_ui16								to,			// Non inclusive
		 DeltaExternId*							externId
	) {

	DASSERT(from <= to);
	DeltaTable*	userData = externId->GetUserData();
	DASSERT(userData);

	DeltaLibraryFuncArgsBinder* binder = DNEW(DeltaLibraryFuncArgsBinder);
	DeltaValue arg;
	arg.FromExternIdBySerialNo(DPTR(externId)->GetSerialNo());
	binder->BindArg(arg);
	
	for (funcs += from; from != to; ++from, ++funcs) {
		DeltaValue index(funcs->name), content(funcs->func, binder);
		bool result = DPTR(userData)->Set(index, content);
		DASSERT(result); // We expect no failure at this point.
	}
}

void DeltaLibraryFuncArgsBinder::BindAll (
		 DeltaLibraryObjectCreator::FuncEntry*	funcs,
		 util_ui16								from,
		 util_ui16								to, // Non inclusive
		 DeltaValue&							externId
	) { BindAll(funcs, from, to, externId.GetExternId()); }

///////////////////////////////////////////////////////////////

void DeltaLibraryFuncArgsBinder::DelegateLibraryMethods (
		DeltaValue&								externId,
		DeltaTable*								owner
	) { DPTR(externId.GetExternIdUserData())->DelegateInternal(owner); }

///////////////////////////////////////////////////////////////

void DeltaLibraryFuncArgsBinder::DelegateLibraryMethods (
		DeltaExternId*							externId,
		DeltaTable*								owner
	) { DPTR(DPTR(externId)->GetUserData())->DelegateInternal(owner); }

///////////////////////////////////////////////////////////////

void DeltaLibraryFuncArgsBinder::InstallLibraryMethods (
		 DeltaLibraryObjectCreator::FuncEntry*	funcs,
		 util_ui16								from,
		 util_ui16								to,
		 CallerCheckerFunc						checker,
		 DeltaTable*							owner
	) {

	DASSERT(checker && owner && from <= to);
	DeltaLibraryFuncArgsBinder* binder = DNEWCLASS(DeltaLibraryFuncArgsBinder, (checker));
	
	for (funcs += from; from != to; ++from, ++funcs) {
		DeltaValue index(funcs->name), content(funcs->func, binder);
		bool result = DPTR(owner)->Set(index, content);
		DASSERT(result); // We expect no failure at this point.
	}
}

///////////////////////////////////////////////////////////////

void DeltaLibraryFuncArgsBinder::BindArg (const DeltaValue& arg) {
	listPtr->push_back(arg); 
	listPtr->back().SetResidencyType(DeltaValue::Contained, listCont);
}

///////////////////////////////////////////////////////////////