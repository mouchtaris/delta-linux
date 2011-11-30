// ListLib.cpp
// Implementation of most STL list API and iterators.
// ScriptFighter Project.
// A. Savidis, June 2001.
// Refactored November 2006, adding more facilities
// for iteration.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#include "DeltaLibraryCreators.h"
#include "MiscLib.h"
#include "ListLib.h"
#include "ListIterLib.h"
#include "ListLibP.h"
#include "DeltaStdLib.h"

#include <list>

#define	VAL_TO_LIST(val) \
			DPTR(VGET_INST(ListCollectableContainer, val, LIST_TYPE_STR))->GetNativeContainer()

inline bool VAL_IS_LIST (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == LIST_TYPE_STR; }

/////////////////////////////////////////////////////

bool DeltaStdUtilities::IsList (const DeltaValue* val) const 
	{ return VAL_IS_LIST(*val); }	

const DeltaStdUtilities::ValList& DeltaStdUtilities::GetList (const DeltaValue* val) const 
	{ return VAL_TO_LIST(const_cast<DeltaValue*>(val)->ToExternId()); }

DeltaStdUtilities::ValList& DeltaStdUtilities::GetList (DeltaValue* val) const 
	{ return VAL_TO_LIST(val->ToExternId()); }

void DeltaStdUtilities::MakeList (DeltaValue* at) const
	{ DeltaList_Make(*at); }

CollectableContainer* DeltaStdUtilities::GetListContainer (DeltaValue* val) const
	{ return VGET_INST(ListCollectableContainer, val->ToExternId(), LIST_TYPE_STR); }

const CollectableContainer* DeltaStdUtilities::GetListContainer (const DeltaValue* val) const
	{ return VGET_INST(ListCollectableContainer, const_cast<DeltaValue*>(val)->ToExternId(), LIST_TYPE_STR); }

/////////////////////////////////////////////////////

static DeltaExternIdFieldGetter* listGetter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return DPTR(listGetter)->GetAllKeys(val, at); }

//************************

static bool GetSize (void* val, DeltaValue* at) 
	{ at->FromNumber(VAL_TO_LIST(val).size()); return true; }

//************************

static bool GetFirst (void* val, DeltaValue* at) {
	std::list<DeltaValue>& l = VAL_TO_LIST(val);
	if (l.size())
		{ at->Assign(l.front()); return true; }
	else 
		{ at->Undefine(); return false; }
}

static bool GetFirstLastSizePrecond (void* val) {
	std::list<DeltaValue>& l = VAL_TO_LIST(val);
	return l.size() > 1;
}

//************************

static bool GetLast (void* val, DeltaValue* at) {
	std::list<DeltaValue>& l = VAL_TO_LIST(val);
	if (l.size())
		{ at->Assign(l.back()); return true; }
	else 
		{ at->Undefine(); return false; }
}

//************************

static DeltaExternIdFieldGetter::GetByStringFuncEntry listGetters[] = {
	{ "keys",		&GetKeys,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "size",		&GetSize,	&GetFirstLastSizePrecond		},
	{ "first",		&GetFirst,	&GetFirstLastSizePrecond		},
	{ "last",		&GetLast,	&GetFirstLastSizePrecond		}
};

//************************

static bool GetByNumber (void* val, util_ui32 index, DeltaValue* at) {
	std::list<DeltaValue>& l = VAL_TO_LIST(val);
	if (index < l.size())
		{ at->Assign(ulistgetbyindex(l, index)); return true; }
	else 
		{ at->Undefine(); return false; }
}

static util_ui32 GetByNumberSize (void* val) {
	std::list<DeltaValue>& l = VAL_TO_LIST(val);
	return (util_ui32) l.size();
}

/////////////////////////////////////////////////////

static void list_new_LibFunc (DeltaVirtualMachine*);
static void list_iterator_LibFunc (DeltaVirtualMachine*);
static void list_push_back_LibFunc (DeltaVirtualMachine*);
static void list_push_front_LibFunc (DeltaVirtualMachine*);
static void list_pop_back_LibFunc (DeltaVirtualMachine*);
static void list_back_LibFunc (DeltaVirtualMachine*);
static void list_front_LibFunc (DeltaVirtualMachine*);
static void list_pop_front_LibFunc (DeltaVirtualMachine*);
static void list_insert_LibFunc (DeltaVirtualMachine*);
static void list_remove_LibFunc (DeltaVirtualMachine*);
static void list_erase_LibFunc (DeltaVirtualMachine*);
static void list_clear_LibFunc (DeltaVirtualMachine*);
static void list_total_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "new",		list_new_LibFunc		},
	{ "iterator",	list_iterator_LibFunc	},
	{ "push_back",	list_push_back_LibFunc	},
	{ "push_front", list_push_front_LibFunc	},
	{ "pop_back",	list_pop_back_LibFunc	},
	{ "back",		list_back_LibFunc		},
	{ "pop_front",	list_pop_front_LibFunc	},
	{ "front",		list_front_LibFunc		},
	{ "insert",		list_insert_LibFunc 	},
	{ "remove",		list_remove_LibFunc 	},
	{ "erase",		list_erase_LibFunc 		},
	{ "clear",		list_clear_LibFunc 		},
	{ "total",		list_total_LibFunc 		}
};

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "iterator", "total")

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_LIST(val), LIST_TYPE_STR)

static void Initialise (void) {
	unew(listGetter);
	DPTR(listGetter)->SetGetByString(listGetters, uarraysize(listGetters));
	DPTR(listGetter)->SetKeyToGetKeys("keys");
	DPTR(listGetter)->SetGetByNumber(&GetByNumber, &GetByNumberSize);
	DELTALIBFUNC_CREATE_METHODS_TABLE();
}

static void CleanUp(void) 
	{ udelete(listGetter); DELTALIBFUNC_DESTROY_METHODS_TABLE(); }

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaList, "list", DELTA_STDLIB_NAMESPACE_PREFIX "list_", Initialise(), CleanUp())

/////////////////////////////////////////////////////

DVM_FUNC std::list<DeltaValue>* DeltaList_Get (DeltaValue& obj) {
	if (obj.Type() != DeltaValue_ExternId)
		return (std::list<DeltaValue>*) 0;
	else
		return DeltaList_Get(DNULLCHECK(obj.ToExternId()), obj.GetExternIdTypeString());
}

DVM_FUNC void* DeltaList_GetCollectableContainer (DeltaValue& obj) {
	if (obj.Type() != DeltaValue_ExternId)
		return (void*) 0;
	else {
		DASSERT(obj.GetExternIdTypeString() == LIST_TYPE_STR);
		return DNULLCHECK(VGET_INST(CollectableContainer, DNULLCHECK(obj.ToExternId()), LIST_TYPE_STR));
	}
}

DVM_FUNC std::list<DeltaValue>* DeltaList_Get (void* val, const std::string& typeStr) 
	{ return LIST_TYPE_STR != typeStr ? (std::list<DeltaValue>*) 0 : &VAL_TO_LIST(val); }

DVM_FUNC void DeltaList_Make (DeltaValue& val) {
	DNEW(ListCollectableContainer)->ToValue(&val);
	DELTALIBFUNC_DELEGATE_METHODS(val);
}

DVM_FUNC void DeltaList_MakeForDebuggerOnly (DeltaValue& val) {
	DNEW(ListCollectableContainer)->ToValue(&val);
	val.SetTypeTag("this list was created by the debugger");
}

/////////////////////////////////////////////////////
// Local parameters are: listObj, listVal, listTypeStr, listAdapter,
// nativeList.
//

#define	GET_LIST(retvalreset,argno) \
	DeltaValue* listObj; \
	DeltaGetArg(listObj, argno, DeltaValue_ExternId, "ExternId", CURR_FUNC, retvalreset); \
	std::string listTypeStr; \
	void* listVal  = listObj->ToExternId(listTypeStr); \
	if (LIST_TYPE_STR != listTypeStr) { \
		DeltaErrorExternId(argno, LIST_TYPE_STR, listTypeStr.c_str(), CURR_FUNC); \
		retvalreset \
	} \
	DASSERT(listVal); \
	ListCollectableContainer* listAdapter = VGET_INST(ListCollectableContainer, listVal, listTypeStr); \
	DASSERT(listAdapter); \
	std::list<DeltaValue>& POSSIBLY_UNUSED_LOCAL(nativeList) = DPTR(listAdapter)->GetNativeContainer();

/////////////////////////////////////////////////////
// Local parameters are: iterObj, iterVal, iterTypeStr, iterAdapter,
// nativeIter.
//
#define	GET_LIST_ITERATOR(retvalreset,argno) \
	DeltaValue* iterObj; \
	DeltaGetArg(iterObj, argno, DeltaValue_ExternId, "ExternId", CURR_FUNC, retvalreset); \
	std::string iterTypeStr; \
	void* iterVal = iterObj->ToExternId(iterTypeStr); \
	if (LISTITER_TYPE_STR != iterTypeStr) { \
		DeltaErrorExternId(argno, LISTITER_TYPE_STR, iterTypeStr.c_str(), CURR_FUNC); \
		retvalreset; \
	} \
	DASSERT(iterVal); \
	CollectableSimple* collectableIter = VGET_INST(CollectableSimple, iterVal, iterTypeStr); \
	DASSERT(collectableIter); \
	ListIteratorAdapter* iterAdapter = (ListIteratorAdapter*) collectableIter->GetValue(); \
	std::list<DeltaValue>::iterator& nativeIter = DPTR(iterAdapter)->GetIterator();

/////////////////////////////////////////////////////

#define	GET_ITERATOR_AND_LIST(retvalreset) \
	GET_LIST_ITERATOR(retvalreset, 0); \
	GET_LIST(retvalreset, 1);

#define	GET_LIST_AND_ITERATOR(retvalreset) \
	GET_LIST(retvalreset, 0); \
	GET_LIST_ITERATOR(retvalreset, 1);

#define	CHECK_ITERATOR_LIST_MISMATCH(ret) \
	if (iterAdapter->GetNativeContainer() != &listAdapter->GetNativeContainer()) { \
		DPTR(vm)->PrimaryError( \
			DEFORMAT_LISTANDITERATORMISMATCH, \
			CURR_FUNC \
		); \
		ret; \
	}

#define	DEFORMAT_INVALIDLISTITERATOR \
			"List iterator is invalid in '%s'"

#define	CHECK_INVALID_LIST_ITERATOR(ret) \
	if (!iterAdapter->IsValid()) { \
		DPTR(vm)->PrimaryError( \
			DEFORMAT_INVALIDLISTITERATOR, \
			CURR_FUNC \
		); \
		ret; \
	}

#define	CHECK_LISTEND_ITERATOR(ret) \
	if (nativeIter == iterAdapter->GetNativeContainer()->end()) { \
		DPTR(vm)->PrimaryError( \
			DEFORMAT_LISTENDITERATOR, \
			CURR_FUNC \
		); \
		ret; \
	}

/////////////////////////////////////////////////////

void ListCollectableContainer::ToValue (DeltaValue* at) {
	at->FromExternId(
		(void*) GetSerialNo(),
		DeltaExternId_CollectableContainer,
		DELTA_EXTERNID_TOSTRINGNULL,
		LIST_TYPE_STR,
		listGetter
	);
	DPTR(at->GetExternIdUserData())->BasicSet(DELTA_STD_ALGORITHMS_SUPERCLASS, DELTA_STD_ALGORITHMS_CONTAINER);
}

/////////////////////////////////////////////////////
// ExternId list(x1,...,xn).
// Return the newlly created dynamic garbage collectable list.
// When each element is added in the list, we should add a reference
// to it.
//
static void list_new_LibFunc (DeltaVirtualMachine* vm) {
	
	ISSUE_FUNC("list_new");
	DeltaList_Make(DPTR(vm)->GetReturnValue());
	DLIB_SET_CREATOR_INFORMATION();
	
	std::list<DeltaValue>*		nativeList	= DeltaList_Get(DPTR(vm)->GetReturnValue());
	void*						listCont	= DeltaList_GetCollectableContainer(DPTR(vm)->GetReturnValue());

	for (util_ui32 i = 0; i < DPTR(vm)->TotalActualArgs(); ++i) {
		nativeList->push_back(*DPTR(vm)->GetActualArg(i));		
		nativeList->back().SetResidencyType(
			DeltaValue::Contained,
			listCont
		);
	}
}

/////////////////////////////////////////////////////
// iterator list_iterator(l). Iterator is reset (not to begin or end).
//
static void list_iterator_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_iterator");
	DeltaAtLeastTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
	GET_LIST(RESET_NIL_RETURNVALUE, 0);

	DeltaListIter_Make(&DPTR(vm)->GetReturnValue());

	void* val = DPTR(vm)->GetReturnValue().ToExternId();
	CollectableSimple* collectableIter = DNULLCHECK(VGET_INST(CollectableSimple, val, LISTITER_TYPE_STR));

	ListIteratorAdapter* iterAdapter = DNULLCHECK((ListIteratorAdapter*) collectableIter->GetValue());
	DPTR(iterAdapter)->SetContainer(listAdapter);	
}

/////////////////////////////////////////////////////
// void list_push_back(l,x1, x2,...,xn).
// Appends an item to a list.
//
static void list_push_back_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_push_back");
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);
	GET_LIST(RESET_EMPTY, 0);

	for (util_ui32 argNo = 1; argNo < DPTR(vm)->TotalActualArgs(); ++argNo) {

		DeltaValue& added = *DPTR(vm)->GetActualArg(argNo);
		nativeList.push_back(added);
		
		nativeList.back().SetResidencyType(
			DeltaValue::Contained,
			(CollectableContainer*) listAdapter
		);
	}
}

/////////////////////////////////////////////////////
// void list_push_front(l,x1,x2,..,xn).
// Inserts an item int front of a list.
//
static void list_push_front_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_push_front");
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);
	GET_LIST(RESET_EMPTY, 0);

	for (util_ui32 argNo = 1; argNo < DPTR(vm)->TotalActualArgs(); ++argNo) {

		DeltaValue& added = *DPTR(vm)->GetActualArg(argNo);
		nativeList.push_front(added);
		
		nativeList.front().SetResidencyType(
			DeltaValue::Contained,
			(CollectableContainer*) listAdapter
		);
	}
}

/////////////////////////////////////////////////////
// Value list_pop_back(l).
// Removes last element of a list. 
//
static void list_pop_back_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_pop_back");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);
	GET_LIST(RESET_EMPTY, 0);

	if (nativeList.empty())
		DPTR(vm)->PrimaryError("in %s(): list is empty!", CURR_FUNC);
	else {
		DPTR(vm)->GetReturnValue().Assign(nativeList.back());
		DPTR(listAdapter)->notify_removed(&nativeList.back());
		nativeList.pop_back();
	}
}

/////////////////////////////////////////////////////
// Value list_back(l).
// Returns the last element of a list. 
//
static void list_back_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_back");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);
	GET_LIST(RESET_EMPTY, 0);

	if (nativeList.empty())
		DPTR(vm)->PrimaryError("in %s(): list is empty!", CURR_FUNC);
	else
		vm->GetReturnValue().Assign(nativeList.back());
}

/////////////////////////////////////////////////////
// Value list_pop_front(l).
// Removes first element of a list. 
//
static void list_pop_front_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_pop_front");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);
	GET_LIST(RESET_EMPTY, 0);

	if (nativeList.empty())
		DPTR(vm)->PrimaryError("in %s(): list is empty!", CURR_FUNC);
	else {
		DPTR(vm)->GetReturnValue().Assign(nativeList.front());
		DPTR(listAdapter)->notify_removed(&nativeList.front());
		nativeList.pop_front();
	}
}

/////////////////////////////////////////////////////
// Value list_front(l).
// Returns first element of a list. 
//
static void list_front_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_front");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);
	GET_LIST(RESET_EMPTY, 0);

	if (nativeList.empty())
		DPTR(vm)->PrimaryError("in %s(): list is empty!", CURR_FUNC);
	else
		vm->GetReturnValue().Assign(nativeList.front());
}

/////////////////////////////////////////////////////
// void list_insert(l, i, x).
// Inserts an element 'x' in list 'l', before iterator 'i'.
//
static void list_insert_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_insert");
	DeltaTotalArgsCheck(3, CURR_FUNC, RESET_EMPTY);
	GET_LIST_AND_ITERATOR(RESET_EMPTY);
	CHECK_ITERATOR_LIST_MISMATCH(RESET_EMPTY);

	DeltaValue& addedItem = *vm->GetActualArg(2);

	// Now insert item in list.
	// We need to get the position of the inserted item
	// so that we can set its residency.
	//
	std::list<DeltaValue>::iterator pos;

	if (nativeIter == nativeList.begin()) {
		nativeList.push_front(addedItem);
		pos = nativeList.begin();
	}
	else {
		pos = nativeIter;
		if (--pos == nativeList.begin()) {
			nativeList.insert(nativeIter, addedItem);
			pos = nativeList.begin();
		}
		else {
			--pos;
			nativeList.insert(nativeIter, addedItem);
			++pos;
		}
		++pos;
	}
	
	pos->SetResidencyType(
		DeltaValue::Contained,
		(CollectableContainer*) listAdapter
	);
}

/////////////////////////////////////////////////////
// void list_erase(l,i).
// Removes an element at position 'i' from list 'l'.
// The iterator gets invalidated automatically.
//
static void list_erase_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_erase");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

	GET_LIST_AND_ITERATOR(RESET_EMPTY);
	CHECK_ITERATOR_LIST_MISMATCH(RESET_EMPTY);
	CHECK_LISTEND_ITERATOR(RESET_EMPTY);

	// Now remove item from list, and invalidate the
	// iterator.
	//
	DeltaValue& removed = *nativeIter;
	std::list<DeltaValue>::iterator iterCopy = nativeIter;
	iterAdapter->Invalidate();
	DPTR(listAdapter)->notify_removed(&removed);
	nativeList.erase(iterCopy);
}

/////////////////////////////////////////////////////
// Bool list_remove(l,x).
// Removes an element at with value 'x' from list 'l'.
//
static void list_remove_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_remove");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

	GET_LIST(RESET_EMPTY, 0);

	DeltaValue& x = *DPTR(vm)->GetActualArg(1);
	std::list<DeltaValue>::iterator i = std::find(nativeList.begin(), nativeList.end(), x);

	if (i != nativeList.end()) {
		DPTR(listAdapter)->notify_removed(&*i);
		nativeList.erase(i);
		DPTR(vm)->GetReturnValue().FromBool(true);
	}
	else	
		DPTR(vm)->GetReturnValue().FromBool(false);
}

/////////////////////////////////////////////////////
// Number list_total(l).
// Returns total items in a list.
//
static void list_total_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_total");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);

	GET_LIST(RESET_ZERO_RETURNVALUE, 0);

	DPTR(vm)->GetReturnValue().FromNumber(nativeList.size());
}

/////////////////////////////////////////////////////
// list_clear(l).
// Clears a list.
//
static void list_clear_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("list_clear");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);

	GET_LIST(RESET_EMPTY, 0);

	DPTR(listAdapter)->notify_cleared();
	nativeList.clear();
}
			
//////////////////////////////////////////////////////
