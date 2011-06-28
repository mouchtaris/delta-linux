// VectorLib.cpp
// Implementation of vector API.
// ScriptFighter Project.
// A. Savidis, June 2001.
// Refactored December 2007, encompassed in the 'std' API object. 
//

#include "DeltaLibraryCreators.h"
#include "uvector.h"
#include "VectorLibP.h"
#include "VectorLib.h"
#include "VectorIterLib.h"
#include "DeltaStdLibFuncNames.h"
#include "DeltaStdLib.h"

#define	VAL_TO_VECTOR(val) \
			VGET_INST(VectorCollectableContainer, val, VECTOR_TYPE_STR)->GetNativeContainer()

inline bool VAL_IS_VECTOR (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == VECTOR_TYPE_STR; }

/////////////////////////////////////////////////////

bool DeltaStdUtilities::IsVector (const DeltaValue* val) const 
	{ return VAL_IS_VECTOR(*val); }	

const DeltaStdUtilities::ValVector& DeltaStdUtilities::GetVector (const DeltaValue* val) const 
	{ return VAL_TO_VECTOR(const_cast<DeltaValue*>(val)->ToExternId()); }

DeltaStdUtilities::ValVector& DeltaStdUtilities::GetVector (DeltaValue* val) const 
	{ return VAL_TO_VECTOR(val->ToExternId()); }

CollectableContainer* DeltaStdUtilities::MakeVector (DeltaValue* at) const
	{ return DeltaVector_Make(*at); }

/////////////////////////////////////////////////////

static DeltaExternIdFieldGetter* vectorGetter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return DPTR(vectorGetter)->GetAllKeys(val, at); }

//************************

static bool GetSize (void* val, DeltaValue* at) 
	{ at->FromNumber(VAL_TO_VECTOR(val).size()); return true; }

//************************

static bool GetFirst (void* val, DeltaValue* at) {
	uvector<DeltaValue>& v = VAL_TO_VECTOR(val);
	if (v.size())
		{ at->Assign(v[0]); return true; }
	else 
		{ at->Undefine(); return false; }
}

static bool GetFirstLastSizePrecond (void* val) {
	uvector<DeltaValue>& v = VAL_TO_VECTOR(val);
	return v.size() > 1;
}

//************************

static bool GetLast (void* val, DeltaValue* at) {
	uvector<DeltaValue>& v = VAL_TO_VECTOR(val);
	if (v.size())
		{ at->Assign(v[v.size() - 1]); return true; }
	else 
		{ at->Undefine(); return false; }
}

//************************

static DeltaExternIdFieldGetter::GetByStringFuncEntry vectorGetters[] = {
	{ "keys",		&GetKeys,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "size",		&GetSize,	&GetFirstLastSizePrecond		},
	{ "first",		&GetFirst,	&GetFirstLastSizePrecond		},
	{ "last",		&GetLast,	&GetFirstLastSizePrecond		}
};

//************************

static bool GetByNumber (void* val, util_ui32 index, DeltaValue* at) {
	uvector<DeltaValue>& v = VAL_TO_VECTOR(val);
	if (index < v.size())
		{ at->Assign(v[index]); return true; }
	else 
		{ at->Undefine(); return false; }
}

static util_ui32 GetByNumberSize (void* val) {
	uvector<DeltaValue>& v = VAL_TO_VECTOR(val);
	return v.size();
}
/////////////////////////////////////////////////////

static void vector_new_LibFunc (DeltaVirtualMachine*);
static void vector_setval_LibFunc (DeltaVirtualMachine*);
static void vector_getval_LibFunc (DeltaVirtualMachine*);
static void vector_push_back_LibFunc (DeltaVirtualMachine*);
static void vector_push_front_LibFunc (DeltaVirtualMachine*);
static void vector_pop_back_LibFunc (DeltaVirtualMachine*);
static void vector_pop_front_LibFunc (DeltaVirtualMachine*);
static void vector_front_LibFunc (DeltaVirtualMachine*);
static void vector_back_LibFunc (DeltaVirtualMachine*);
static void vector_resize_LibFunc (DeltaVirtualMachine*);
static void vector_total_LibFunc (DeltaVirtualMachine*);
static void vector_iterator_LibFunc (DeltaVirtualMachine* vm);

static void	vector_get_internal_LibFunc (DeltaVirtualMachine* vm);
static void	vector_set_internal_LibFunc (DeltaVirtualMachine* vm);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "new",		vector_new_LibFunc			},
	{ "setval",		vector_setval_LibFunc		},
	{ "getval",		vector_getval_LibFunc		},
	{ "push_front",	vector_push_front_LibFunc	},
	{ "push_back",	vector_push_back_LibFunc	},
	{ "pop_front",	vector_pop_front_LibFunc	},
	{ "pop_back",	vector_pop_back_LibFunc		},
	{ "front",		vector_front_LibFunc		},
	{ "back",		vector_back_LibFunc			},
	{ "resize",		vector_resize_LibFunc		},
	{ "total",		vector_total_LibFunc		},
	{ "iterator",	vector_iterator_LibFunc		}
};

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "setval", "iterator")

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_VECTOR(val),VECTOR_TYPE_STR)

static void Initialise (void) {
	unew(vectorGetter);
	DPTR(vectorGetter)->SetGetByString(vectorGetters, uarraysize(vectorGetters));
	DPTR(vectorGetter)->SetKeyToGetKeys("keys");
	DPTR(vectorGetter)->SetGetByNumber(&GetByNumber, &GetByNumberSize);
	DELTALIBFUNC_CREATE_METHODS_TABLE();
	DeltaLibFuncBinder::AddInternal(&vector_get_internal_LibFunc);
	DeltaLibFuncBinder::AddInternal(&vector_set_internal_LibFunc);
}

static void CleanUp (void) 
	{ udelete(vectorGetter); DELTALIBFUNC_DESTROY_METHODS_TABLE(); }

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaVector, "vector", DELTA_STDLIB_NAMESPACE_PREFIX "vector_", Initialise(), CleanUp())

/////////////////////////////////////////////////////

static bool IsVector (const DeltaValue& val)
	{ return VAL_IS_VECTOR(val); }

//***************************

static void InstallOverloadedDot (DeltaValue& val) {

	DeltaTable* userdata = DNULLCHECK(val.GetExternIdUserData());

	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&IsVector);
	binder->BindArg(val);

	DPTR(userdata)->BasicSet(
		DELTA_OPERATOR_OBJECT_GET, 
		DeltaValue(&vector_get_internal_LibFunc, binder)	
	);

	DPTR(userdata)->BasicSet(
		DELTA_OPERATOR_OBJECT_SET, 
		DeltaValue(&vector_set_internal_LibFunc, binder)	
	);
}

//***************************

DVM_FUNC CollectableContainer* DeltaVector_Make (DeltaValue& val) {

	VectorCollectableContainer* cont = DNEW(VectorCollectableContainer);
	DPTR(cont)->ToValue(&val);

	DELTALIBFUNC_DELEGATE_METHODS(val);
	InstallOverloadedDot(val);

	return cont;
}

//***************************

static VectorCollectableContainer* DeltaVector_MakeAndReturnCollectableContainer (DeltaValue& val) {
	VectorCollectableContainer* v = DNEW(VectorCollectableContainer);
	DPTR(v)->ToValue(&val);
	DELTALIBFUNC_DELEGATE_METHODS(val);
	InstallOverloadedDot(val);
	return v;
}

//***************************

DVM_FUNC uvector<DeltaValue>* DeltaVector_Get (void* val) {
	VectorCollectableContainer* v = VGET_INST(VectorCollectableContainer, val, VECTOR_TYPE_STR);
	return v ? &DPTR(v)->GetNativeContainer() : (uvector<DeltaValue>*) 0;
}

/////////////////////////////////////////////////////
// Local parameters are: vectorObj, vectorVal
// vectorAdapter, nativeVector.

#define	GET_VECTOR(retvalreset,argno)																						\
	DeltaValue* argObj;																										\
	GET_POLYMORPHIC_EXTERNID(vectorVal, VECTOR_TYPE_STR, retvalreset, argno);												\
	VectorCollectableContainer* vectorAdapter =	VGET_INST(VectorCollectableContainer, vectorVal, VECTOR_TYPE_STR);			\
	uvector<DeltaValue>& POSSIBLY_UNUSED_LOCAL(nativeVector) = DPTR(vectorAdapter)->GetNativeContainer();

/////////////////////////////////////////////////////
// Legal vector indices are filtered and redirected to 
// the vector, the rest go to userdata.

#define	FILTER_VECTOR_LEGAL_INDEX(_action)												\
	if (index->Type() == DeltaValue_Number) {											\
		DeltaNumberValueType	f = index->ToNumber();									\
		util_ui32				i = (util_ui32) f;										\
		if (f >= 0.0 && ((DeltaNumberValueType) i) == f && nativeVector.validindex(i))	\
			{ _action; return; }														\
	}

//***************************

static void vector_get_internal_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("vector internal []");
	DeltaTotalArgsCheck(3, CURR_FUNC, RESET_NIL_RETURNVALUE);

	GET_VECTOR(RESET_NIL_RETURNVALUE, 0);
	GET_TABLE(userdata, CURR_FUNC, RESET_NIL_RETURNVALUE, 1); 
	DeltaValue* index = DPTR(vm)->GetActualArg(2);

	FILTER_VECTOR_LEGAL_INDEX(DLIB_RETVAL_REF.Assign(nativeVector[i]));
	DPTR(userdata)->BasicGet(*index, &DLIB_RETVAL_REF);
}

//***************************

static void vector_set_internal_LibFunc (DeltaVirtualMachine* vm) {
	
	ISSUE_FUNC("vector internal []=");
	DeltaTotalArgsCheck(4, CURR_FUNC, RESET_EMPTY);

	GET_VECTOR(RESET_EMPTY, 0);
	GET_TABLE(userdata, CURR_FUNC, RESET_EMPTY, 1); 
	DeltaValue* index	= DPTR(vm)->GetActualArg(2);
	DeltaValue* content = DPTR(vm)->GetActualArg(3);

	FILTER_VECTOR_LEGAL_INDEX(nativeVector[i].Assign(content));
	DPTR(userdata)->BasicSet(*index, *content);
}

/////////////////////////////////////////////////////

#define	GET_INDEX(retvalreset,argno)														\
	DeltaValue* indexObj;																	\
	DeltaGetArg(indexObj, argno, DeltaValue_Number, "Number", CURR_FUNC, retvalreset);		\
	DeltaNumberValueType indexVal;															\
	DASSERT(indexObj);																		\
	indexObj->ToNumber(&indexVal);															\
	if (indexVal < 0) {																		\
		DPTR(vm)->PrimaryError(																\
			DEFORMAT_REJECTNEGATIVE,														\
			argno,																			\
			CURR_FUNC																		\
			);																				\
			retvalreset;																	\
	}

#define	CHECK_VECTORBOUNDS(retvalreset)														\
	if (nativeVector.size() <= (util_ui32) indexVal) {										\
		DPTR(vm)->PrimaryError(																\
			DEFORMAT_VECTORBOUNDS,															\
			(util_ui32) indexVal,															\
			CURR_FUNC																		\
		);																					\
		retvalreset;																		\
	}

#define	CHECK_VECTORNOTEMPTY(retvalreset)													\
	if (!nativeVector.size()) {																\
		DPTR(vm)->PrimaryError(																\
			DEFORMAT_VECTOREMPTY,															\
			CURR_FUNC																		\
		);																					\
		retvalreset;																		\
	}

void VectorCollectableContainer::ToValue (DeltaValue* at) {
	at->FromExternId(
		(void*) GetSerialNo(), 
		DeltaExternId_CollectableContainer,
		DELTA_EXTERNID_TOSTRINGNULL,
		VECTOR_TYPE_STR,
		vectorGetter
	);
	DPTR(at->GetExternIdUserData())->BasicSet(DELTA_STD_ALGORITHMS_SUPERCLASS, DELTA_STD_ALGORITHMS_CONTAINER);
}

/////////////////////////////////////////////////////
// ExternId vector_new(n).
// Return the newly created dynamic garbage collectable vector.
//
static void vector_new_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("vector_new");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
	GET_INDEX(RESET_NIL_RETURNVALUE, 0);

	VectorCollectableContainer*	vectorAdapter = DeltaVector_MakeAndReturnCollectableContainer(DPTR(vm)->GetReturnValue());
	uvector<DeltaValue>&		nativeVector = DPTR(vectorAdapter)->GetNativeContainer();
	nativeVector.resize((util_ui32) indexVal);
	
	// Set the residency type of all vector objects.
	for (util_ui32 i = 0; i < nativeVector.size(); ++i)
		nativeVector[i].SetResidencyType(DeltaValue::Contained, vectorAdapter);
}

/////////////////////////////////////////////////////
// vector_setval(v,i,x).
// Set the value of element 'i' in 'v' with value 'x'.
//
static void vector_setval_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("vector_setval");
	DeltaTotalArgsCheck(3, CURR_FUNC, RESET_EMPTY);

	GET_VECTOR(RESET_EMPTY, 0);
	GET_INDEX(RESET_EMPTY, 1);
	CHECK_VECTORBOUNDS(RESET_EMPTY);

	nativeVector[(util_ui32) indexVal].Assign(DPTR(vm)->GetActualArg(2));
}

/////////////////////////////////////////////////////
// DeltaValue vector_getval(v,i,x).
// Return the value of element 'i' in 'v'.
//
static void vector_getval_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("vector_getval");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_NIL_RETURNVALUE);

	GET_VECTOR(RESET_NIL_RETURNVALUE, 0);
	GET_INDEX(RESET_NIL_RETURNVALUE, 1);
	CHECK_VECTORBOUNDS(RESET_NIL_RETURNVALUE);

	DPTR(vm)->GetReturnValue().Assign(nativeVector[(util_ui32) indexVal]);
}

/////////////////////////////////////////////////////

#define	VECTOR_PUSH_FRONT_BACK_IMPL(_name)										\
	static void vector_push_##_name##_LibFunc (DeltaVirtualMachine* vm) {		\
	ISSUE_FUNC("vector_push_" #_name);											\
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);						\
	GET_VECTOR(RESET_EMPTY, 0);													\
	for (util_ui32 argNo = 1; argNo < DPTR(vm)->TotalActualArgs(); ++argNo) {	\
		DeltaValue& added = *DPTR(vm)->GetActualArg(argNo);						\
		nativeVector.push_##_name(added);										\
		nativeVector._name().SetResidencyType(									\
			DeltaValue::Contained,												\
			(CollectableContainer*) vectorAdapter								\
		);																		\
	}																			\
}

// DeltaValue vector_push_front(v, x). Pushes x at front.
VECTOR_PUSH_FRONT_BACK_IMPL(front)

// DeltaValue vector_push_back(v, x). Pushes x at back.
VECTOR_PUSH_FRONT_BACK_IMPL(back)

/////////////////////////////////////////////////////

#define	VECTOR_POP_FRONT_BACK_IMPL(_name)									\
	static void vector_pop_##_name##_LibFunc (DeltaVirtualMachine* vm) {	\
	ISSUE_FUNC("vector_pop_" #_name);										\
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);							\
	GET_VECTOR(RESET_EMPTY, 0);												\
	CHECK_VECTORNOTEMPTY(RESET_EMPTY);										\
	nativeVector.pop_##_name();												\
}

// DeltaValue vector_pop_front(v). Pops first element.
VECTOR_POP_FRONT_BACK_IMPL(front)

// DeltaValue vector_pop_back(v). Pops last element.
VECTOR_POP_FRONT_BACK_IMPL(back)

/////////////////////////////////////////////////////

#define	VECTOR_FRONT_BACK_IMPL(_name)								\
	static void vector_##_name##_LibFunc (DeltaVirtualMachine* vm) {	\
	ISSUE_FUNC("vector_" #_name);										\
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);			\
	GET_VECTOR(RESET_NIL_RETURNVALUE, 0);								\
	CHECK_VECTORNOTEMPTY(RESET_NIL_RETURNVALUE);						\
	DPTR(vm)->GetReturnValue().Assign(nativeVector._name());			\
}

// DeltaValue vector_front(v). Returns the value of first element.
VECTOR_FRONT_BACK_IMPL(front)

// DeltaValue vector_back(v). Returns the value of last element.
VECTOR_FRONT_BACK_IMPL(back)

/////////////////////////////////////////////////////
// vector_resize(v,n).
// Resizes a vector.
//
static void vector_resize_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("vector_resize");
	DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

	GET_VECTOR(RESET_EMPTY, 0);
	GET_UNSIGNEDINT(n, CURR_FUNC, RESET_EMPTY, 1);

	if (n > nativeVector.size()) {
		nativeVector.resize(n);
		for (util_ui32 i = 0; i < n; ++i) {
			DeltaValue& val = nativeVector[i];
			if (val.GetResidencyType() == DeltaValue::Internal)
				val.SetResidencyType(DeltaValue::Contained, vectorAdapter);
			else
				DASSERT(
					val.GetResidencyType() == DeltaValue::Contained &&
					val.GetResidencyContext() == vectorAdapter
				);
		}
	}
}

/////////////////////////////////////////////////////
// vector_total(v).
// Returns total number of elements. Some elements may be
// unitialised (i.e. type undef).
//
static void vector_total_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("vector_total");
	DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);

	GET_VECTOR(RESET_ZERO_RETURNVALUE, 0);

	vm->GetReturnValue().FromNumber(nativeVector.size());
}

/////////////////////////////////////////////////////
// iterator vector_iterator(v). Iterator is reset (not to begin or end).
//
static void vector_iterator_LibFunc (DeltaVirtualMachine* vm) {

	ISSUE_FUNC("vector_iterator");
	DeltaAtLeastTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
	GET_VECTOR(RESET_NIL_RETURNVALUE, 0);

	DeltaVectorIter_Make(&DPTR(vm)->GetReturnValue());
}

/////////////////////////////////////////////////////
