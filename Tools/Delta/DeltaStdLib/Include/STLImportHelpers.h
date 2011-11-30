// STLImportHelpers.h
// Template library to make the Delta importing of STL
// as lib functions far easier and generic.
// ScriptFighter Project.
// A. Savidis, November 2006.
//

#ifndef	STLIMPORTHELPERS_H
#define	STLIMPORTHELPERS_H

#include "DeltaCollectableContainer.h"
#include "DeltaValue.h"
#include "DeltaExternId.h"
#include "MiscLib.h"
#include "DeltaExternIdFactory.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "DeltaStdLibTemplatesUtils.h"
#include "DeltaString.h"
#include "ustrings.h"

/////////////////////////////////////////////////////

inline void FUN_RESET_EMPTY (DeltaVirtualMachine* vm) {}

inline void FUN_RESET_ZERO_RETURNVALUE (DeltaVirtualMachine* vm) 
	{ vm->GetReturnValue().FromNumber(0); }

inline void FUN_RESET_FALSE_RETURNVALUE (DeltaVirtualMachine* vm) 
	{ vm->GetReturnValue().FromBool(false); }

inline void FUN_RESET_TRUE_RETURNVALUE(DeltaVirtualMachine* vm) 
	{ vm->GetReturnValue().FromBool(false); }

inline void FUN_RESET_NIL_RETURNVALUE (DeltaVirtualMachine* vm) 
	{ vm->GetReturnValue().FromNil(); }

#define	RESET(call) call; return (Tadapted*) 0;

/////////////////////////////////////////////////////
// Extracting a generic iterator argument (functionality), 
// this time as a template instead of a macro.
//
template <class	Tadapted> struct GET_ITERATOR_TEMPLATE {
	
	DeltaValue iter;

	bool IsObjectOrExternId (DeltaValueType type)
		{ return type == DeltaValue_ExternId || type == DeltaValue_Table; }

	Tadapted* operator()(
			DeltaVirtualMachine*	vm, 
			const char*				func,
			void					(*resetFun)(DeltaVirtualMachine* vm),
			util_ui32				argNo,
			const char*				iterTypeStr
		) {
		DeltaValue* obj;
		DeltaGetOverloadedArg(obj, argNo, IsObjectOrExternId, "ExternId / Object", func, RESET(resetFun(vm)));
		
		if (!obj->ConvertToExternId(iterTypeStr, &iter)) {
			DPTR(vm)->Error("while trying to convert '%s' to '%s'!", obj->TypeStr(), iterTypeStr);
			RESET(resetFun(vm));
		}
		
		if (iter.Type() == DeltaValue_Undefined) {
			DPTR(vm)->PrimaryError("no conversion of arg #%d to '%s' possible in '%s'!", argNo, iterTypeStr, func);
			RESET(resetFun(vm));
		} 

		void* val = iter.ToExternId();
		CollectableSimple* collectableIter = VGET_INST(CollectableSimple, val, iterTypeStr);
		DASSERT(collectableIter);
		return (Tadapted*) collectableIter->GetValue();
	}
};

//*********************

#define	GET_ITERATOR(argNo, reset)										\
	TIadapted* iterAdapter;												\
	GET_ITERATOR_TEMPLATE<TIadapted> iteratorGetter;					\
	iterAdapter = iteratorGetter(										\
		vm, CURR_FUNC, reset, argNo, NameTraits::itertypestring			\
	);																	\
	if (!iterAdapter) {													\
		DPTR(vm)->Error(												\
			"in %s() argument %d not of '%s' type!",					\
				CURR_FUNC, argNo,										\
				NameTraits::itertypestring								\
		);																\
		reset(vm); return;												\
	}																	\
	OriginalIterator& POSSIBLY_UNUSED_LOCAL(nativeIter) = DPTR(iterAdapter)->GetIterator();

#define	GET_ITERATOR_VALUE()	iteratorGetter.iter

/////////////////////////////////////////////////////
// Extracting a generic container argument (functionality),
// this time as a template instead of a macro.
//

template <class	Tadapted> struct GET_CONTAINER_TEMPLATE {

	bool IsObjectOrExternId (DeltaValueType type)
		{ return type == DeltaValue_ExternId || type == DeltaValue_Table; }

	Tadapted* operator()(
			DeltaVirtualMachine*	vm, 
			const char*				func,
			void					(*resetFun)(DeltaVirtualMachine* vm),
			unsigned				argNo,
			const char*				contTypeStr
		) {

		DeltaValue* obj;
		DeltaGetOverloadedArg(obj, argNo, IsObjectOrExternId, "ExternId / Object", func, RESET(resetFun(vm)));
		
		DeltaValue cont;
		if (!obj->ConvertToExternId(contTypeStr, &cont)) {
			DPTR(vm)->Error("while trying to convert '%s' to '%s'!", obj->TypeStr(), contTypeStr);
			RESET(resetFun(vm));
		}
		
		if (cont.Type() == DeltaValue_Undefined) {
			DPTR(vm)->PrimaryError("no conversion of arg #%d to '%s' possible in '%s'!", argNo, contTypeStr, func);
			RESET(resetFun(vm));
		} 

		void* val = cont.ToExternId();

		Tadapted* contValue = VGET_INST(Tadapted, val, contTypeStr);
		DASSERT(contValue);
		return contValue;
	}
};

//*********************

#define	GET_CONTAINER(argNo, reset)														\
	TCadapted* contAdapter;																\
	contAdapter = GET_CONTAINER_TEMPLATE<TCadapted>()(									\
		vm, CURR_FUNC, reset, argNo, NameTraits::conttypestring							\
	);																					\
	if (!contAdapter) {																	\
		DPTR(vm)->Error(																\
			"in %s() argument %d not of '%s' type!",									\
			CURR_FUNC, argNo, NameTraits::conttypestring								\
		);																				\
		reset(vm); return;																\
	}																					\
	OriginalContainer& nativeCont = DPTR(contAdapter)->GetNativeContainer();

//*********************

#define	DEFORMAT_CONTAINERANDITERATORMISMATCH					\
			"iterator and container do not match in '%s'!"

#define	DEFORMAT_ITERATOR_RVALUE_INVALID						\
			"'%s' iterator (rvalue) is invalid in '%s'!"

#define	DEFORMAT_INVALIDITERATOR								\
			"'%s' iterator is invalid in '%s'!"

#define	DEFORMAT_ENDITERATOR									\
			"'%s' iterator reached 'end' in '%s'!"

#define	DEFORMAT_BEGINITERATOR									\
			"'%s' iterator reached 'begin' in '%s'!"

#define	CHECK_ITERATOR_CONTAINER_MISMATCH(ret)					\
	if (!iterAdapter->MatchesContainer(contAdapter)) {			\
		DPTR(vm)->PrimaryError(									\
			DEFORMAT_CONTAINERANDITERATORMISMATCH,				\
			CURR_FUNC											\
		);														\
		ret;													\
	}

#define	CHECK_INVALID_ITERATOR(ret)			\
	if (!iterAdapter->IsValid()) {			\
		DPTR(vm)->PrimaryError(				\
			DEFORMAT_INVALIDITERATOR,		\
			NameTraits::iter2string,		\
			CURR_FUNC						\
		);									\
		ret;								\
	}

#define	ITERATOR_BEGIN(cont)	\
		ITERATOR_ACCESS_TRAITS<OriginalIterator>::Begin(cont)

#define	ITERATOR_END(cont)		\
		ITERATOR_ACCESS_TRAITS<OriginalIterator>::End(cont)

#define	ITERATOR_GET(iter)		\
		ITERATOR_ACCESS_TRAITS<OriginalIterator>::Get(iter)

#define	ITERATOR_GETBYSKIPPINGUSERCODE(iter)		\
		ITERATOR_ACCESS_TRAITS<OriginalIterator>::GetBySkippingUserCode(iter)

#define	ITERATOR_SET(iter, val)	\
		ITERATOR_ACCESS_TRAITS<OriginalIterator>::Set(iter, val)

#define	CHECK_END_ITERATOR(ret)													\
	if (nativeIter == ITERATOR_END(iterAdapter->GetNativeContainer())) {		\
		DPTR(vm)->PrimaryError(													\
			DEFORMAT_ENDITERATOR,												\
			NameTraits::iter2string,											\
			CURR_FUNC															\
		);																		\
		ret;																	\
	}

#define	CHECK_BEGIN_ITERATOR(ret)												\
	if (nativeIter == ITERATOR_BEGIN(iterAdapter->GetNativeContainer())) {		\
		DPTR(vm)->PrimaryError(													\
			DEFORMAT_BEGINITERATOR,												\
			NameTraits::iter2string,											\
			CURR_FUNC															\
		);																		\
		ret;																	\
	}

/////////////////////////////////////////////////////

template <
	class	TIadapted,		// Adapted iterator type.
	class	TCadapted,		// Adapted container type.
	class	NameTraits		// Name strings as a traits class.
> 
class ITERATOR_LIBRARY {

	typedef typename TIadapted::Original OriginalIterator;
	typedef typename TCadapted::Original OriginalContainer;

	static void DestroyIterator (void* val) {
		TIadapted* iterAdapter = (TIadapted*) val;
		iterAdapter->SetContainer((TCadapted*) 0);
		DDELETE(iterAdapter);
	}

	// 'closure' is the iterator's collectable simple serial no.

	static void Iterator2String (DeltaString* at, void* closure) { 
		
		void*					val			= DELTA_EGC_SIMPLE_GETVALUE(closure);
		TIadapted*				iterAdapter	= DPTR((TIadapted*) val);
		TCadapted*				contAdapter	= DPTR(iterAdapter)->GetContainerAdapter();
		
		if (contAdapter) {

			DPTR(at)->Add(
				uconstructstr("%s 0x%x [", 
					NameTraits::iter2string,
					(util_ui32) closure				
				)
			);				
			
			DPTR(iterAdapter)->ConvertValueToString(at);

			DPTR(at)->Add(
				uconstructstr(
					",%s(0x%x, size %d)]", 
					NameTraits::cont2string,
					DPTR(contAdapter)->GetSerialNo(),
					contAdapter->Size()
				)
			);
		}
		else
			DPTR(at)->Add(
				uconstructstr(
					"%s 0x%x [value(none),%s(none)]",
					NameTraits::iter2string,
					(util_ui32) closure,
					NameTraits::cont2string
				)
			);	
	}

	/////////////////////////////////////////////////////
	
	public:

	/////////////////////////////////////////////////////
	// ExternId iter(void).
	// This is the constructor function of a dynamic iterator 
	// garbage collectable type.
	//
	static void iter (
			DeltaVirtualMachine*		vm, 
			DeltaExternIdFieldGetter*	getter
		) {

		ISSUE_FUNC(NameTraits::iter);
		DeltaTotalArgsCheck(0, CURR_FUNC, RESET_NIL_RETURNVALUE);
		Make(&DPTR(vm)->GetReturnValue(), getter);
		DLIB_SET_CREATOR_INFORMATION();
	}

	static TIadapted* Make (DeltaValue* at, const DeltaExternIdFieldGetter* getter) {

		TIadapted*			iterAdapter		=	DNEW(TIadapted);
		CollectableSimple*	collectableIter =	CollectableSimple::New(
													NameTraits::iter2string, 
													iterAdapter, 
													DestroyIterator
												);

		at->FromExternId(
			(void*) collectableIter->GetSerialNo(), 
			DeltaExternId_CollectableSimple,
			Iterator2String,
			NameTraits::itertypestring,
			DPTR(getter)
		);
		DPTR(at->GetExternIdUserData())->BasicSet(DELTA_STD_ALGORITHMS_SUPERCLASS, DELTA_STD_ALGORITHMS_ITERATOR);
		NameTraits::add_methods(at);

		return iterAdapter;
	}

	/////////////////////////////////////////////////////
	// i setbegin(i, c).
	// Sets an iterator to the beginning of a container and returns it.
	//
	static void setbegin (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::setbegin);
		DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);
	
		GET_ITERATOR(0, FUN_RESET_EMPTY);
		GET_CONTAINER(1, FUN_RESET_EMPTY);

		DPTR(iterAdapter)->SetContainer(contAdapter);	
		DPTR(iterAdapter)->ResetIterationCounter();
		DPTR(iterAdapter)->AssumeForwarded();

		nativeIter = ITERATOR_BEGIN(nativeCont);

		DPTR(vm)->GetReturnValue().Assign(GET_ITERATOR_VALUE());
	}

	/////////////////////////////////////////////////////
	// i setend(i,c).
	// Sets an iterator to the end of its container and returns it.
	//
	static void setend (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::setend);
		DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

		GET_ITERATOR(0, FUN_RESET_EMPTY);
		GET_CONTAINER(1, FUN_RESET_EMPTY);

		DPTR(iterAdapter)->SetContainer(contAdapter);
		DPTR(iterAdapter)->SetIterationCounter(contAdapter->Size());
		DPTR(iterAdapter)->AssumeForwarded();

		nativeIter = ITERATOR_END(nativeCont);

		DPTR(vm)->GetReturnValue().Assign(GET_ITERATOR_VALUE());
	}

	/////////////////////////////////////////////////////
	// Bool checkend(i, c).
	// Returns if iterator 'i' is at the end of container 'c'.
	//
	static void checkend (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::checkend);
		DeltaTotalArgsCheck(2, CURR_FUNC, RESET_TRUE_RETURNVALUE);

		GET_ITERATOR(0, FUN_RESET_TRUE_RETURNVALUE);
		GET_CONTAINER(1, FUN_RESET_TRUE_RETURNVALUE);
		CHECK_ITERATOR_CONTAINER_MISMATCH(RESET_TRUE_RETURNVALUE);

		DPTR(iterAdapter)->AssumeForwarded();

		DPTR(vm)->GetReturnValue().FromBool(
			nativeIter == ITERATOR_END(nativeCont)
		);
	}

	/////////////////////////////////////////////////////
	// Bool checkbegin(i, c).
	// Returns if iterator 'i' is at the begin of container 'c'.
	//
	static void checkbegin (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::checkbegin);
		DeltaTotalArgsCheck(2, CURR_FUNC, RESET_FALSE_RETURNVALUE);

		GET_ITERATOR(0, FUN_RESET_FALSE_RETURNVALUE);
		GET_CONTAINER(1, FUN_RESET_FALSE_RETURNVALUE);
		CHECK_ITERATOR_CONTAINER_MISMATCH(RESET_FALSE_RETURNVALUE);

		DPTR(iterAdapter)->AssumeForwarded();

		DPTR(vm)->GetReturnValue().FromBool(
			nativeIter == ITERATOR_BEGIN(nativeCont)
		);
	}

	/////////////////////////////////////////////////////
	// Bool equal(i1, i2).
	// Returns true if two iterators are equal.
	//
	static void equal (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::equal);
		DeltaTotalArgsCheck(2, CURR_FUNC, RESET_FALSE_RETURNVALUE);

		TIadapted* i1;
		{ GET_ITERATOR(0, FUN_RESET_FALSE_RETURNVALUE); i1 = iterAdapter; }

		TIadapted* i2;
		{ GET_ITERATOR(1, FUN_RESET_FALSE_RETURNVALUE); i2 = iterAdapter; }

		DPTR(vm)->GetReturnValue().FromBool(
			(DPTR(i1)->GetNativeContainer() && DPTR(i2)->GetNativeContainer())  &&
			(DPTR(i1)->GetNativeContainer() == DPTR(i2)->GetNativeContainer()) &&
			(DPTR(i1)->GetIterator() == DPTR(i2)->GetIterator())
		);
	}

	/////////////////////////////////////////////////////
	// assign(i1, i2).
	// Naturally assigns i2 to i1.
	//
	static void assign (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::assign);
		DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

		TIadapted* i1;
		{ GET_ITERATOR(0, FUN_RESET_EMPTY); i1 = iterAdapter; }

		TIadapted* i2;
		{ GET_ITERATOR(1, FUN_RESET_EMPTY); i2 = iterAdapter; }

		if (!DPTR(i2)->GetContainerAdapter()) {
			DPTR(vm)->PrimaryError(
				DEFORMAT_ITERATOR_RVALUE_INVALID,
				NameTraits::iter,
				CURR_FUNC
			);
			RESET_EMPTY;
		}
		else  // Assign containers and iterators too.
			DPTR(i1)->Assign(i2);
	}

	/////////////////////////////////////////////////////
	// i copy(i).
	// Copy construction of an iterator.
	//
	static void copy (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::copy);
		DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);
		GET_ITERATOR(0, FUN_RESET_NIL_RETURNVALUE);
		
		TIadapted* copyAdapter =	Make(
										&DPTR(vm)->GetReturnValue(), 
										GET_ITERATOR_VALUE().GetExternIdFieldGetter()
									);
		DPTR(copyAdapter)->Assign(iterAdapter);
	}

	/////////////////////////////////////////////////////
	// i fwd(i).
	// Moves an iterator forward and returns it.
	//
	static void fwd (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::fwd);
		DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);

		GET_ITERATOR(0, FUN_RESET_EMPTY);
		CHECK_INVALID_ITERATOR(RESET_EMPTY);
		
		if (DPTR(iterAdapter)->ShouldIgnoreForward())
			DPTR(iterAdapter)->AssumeForwarded();
		else {
			CHECK_END_ITERATOR(RESET_EMPTY);
			++nativeIter; 
			DPTR(iterAdapter)->IncIterationCounter();
		}

		DPTR(vm)->GetReturnValue().Assign(GET_ITERATOR_VALUE());
	}

	/////////////////////////////////////////////////////
	// i bwd(i).
	// Moves an iterator backward and returns it.
	//
	static void bwd (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::bwd);
		DeltaTotalArgsCheck(1, CURR_FUNC, RESET_EMPTY);

		GET_ITERATOR(0, FUN_RESET_EMPTY);
		CHECK_INVALID_ITERATOR(RESET_EMPTY);
		CHECK_BEGIN_ITERATOR(RESET_EMPTY);

		DPTR(iterAdapter)->AssumeForwarded();
		--nativeIter;
		DPTR(iterAdapter)->DecIterationCounter();

		DPTR(vm)->GetReturnValue().Assign(GET_ITERATOR_VALUE());
	}

	/////////////////////////////////////////////////////
	// Value getcontainer(i).
	// Returns the iterator container (if any).
	//
	static void getcontainer (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::getcontainer);
		DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

		GET_ITERATOR(0, FUN_RESET_NIL_RETURNVALUE);

		if (TCadapted* cont = DPTR(iterAdapter)->GetContainerAdapter())
			DPTR(cont)->ToValue(&DPTR(vm)->GetReturnValue());
		else
			DPTR(vm)->GetReturnValue().FromNil();

	}

	/////////////////////////////////////////////////////
	// Number getcounter(i).
	// Returns the iteration counter.
	//
	static void getcounter (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::getcounter);
		DeltaTotalArgsCheck(1, CURR_FUNC, RESET_ZERO_RETURNVALUE);

		GET_ITERATOR(0, FUN_RESET_ZERO_RETURNVALUE);
		CHECK_INVALID_ITERATOR(RESET_ZERO_RETURNVALUE);

		DPTR(vm)->GetReturnValue().Assign(
			(DeltaNumberValueType) DPTR(iterAdapter)->GetIterationCounter()
		);
	}

	/////////////////////////////////////////////////////
	// Value getval(i).
	// Returns the content of an iterator.
	//
	static void getval (DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::getval);
		DeltaTotalArgsCheck(1, CURR_FUNC, RESET_NIL_RETURNVALUE);

		GET_ITERATOR(0, FUN_RESET_NIL_RETURNVALUE);
		CHECK_INVALID_ITERATOR(RESET_NIL_RETURNVALUE);
		CHECK_END_ITERATOR(RESET_NIL_RETURNVALUE);

		DPTR(vm)->GetReturnValue().Assign(ITERATOR_GET(nativeIter));
		if (UERROR_ISRAISED())
			DPTR(vm)->PrimaryError(
				"in '%s()' while getting '%s' content", 
				CURR_FUNC, 
				NameTraits::itertypestring
			);
	}

	/////////////////////////////////////////////////////
	// void setval(i,x).
	// Sets the content of a container element whose position
	// is defined by the iterator.
	//
	static void setval(DeltaVirtualMachine* vm) {

		ISSUE_FUNC(NameTraits::setval);
		DeltaTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY);

		GET_ITERATOR(0, FUN_RESET_EMPTY);
		CHECK_INVALID_ITERATOR(RESET_EMPTY);
		CHECK_END_ITERATOR(RESET_EMPTY);

		if (!ITERATOR_SET(nativeIter, DPTR(vm)->GetActualArg(1)))
			DPTR(vm)->PrimaryError("in '%s()' failed to set value", CURR_FUNC);
	}

	/////////////////////////////////////////////////////

	static bool GetKeys (void* val, DeltaValue* at, DeltaExternIdFieldGetter* getter) 
		{ return DPTR(getter)->GetAllKeys(val, at); }

	static bool GetValue (void* val, DeltaValue* at) {
		CollectableSimple*				collectableIter = VGET_INST(CollectableSimple, val, NameTraits::iter2string);
		DASSERT(collectableIter);
		TIadapted*						iterAdapter		= (TIadapted*) DPTR(collectableIter)->GetValue();
		typename TIadapted::Original&	nativeIter		= DPTR(iterAdapter)->GetIterator();
		
		if (!iterAdapter->GetNativeContainer() || nativeIter == ITERATOR_END(iterAdapter->GetNativeContainer()))
			at->Undefine();
		else
			at->Assign(ITERATOR_GETBYSKIPPINGUSERCODE(nativeIter));
		return true;
	}

	static bool GetContainer (void* val, DeltaValue* at) {
		CollectableSimple*		collectableIter = VGET_INST(CollectableSimple, val, NameTraits::iter2string);
		DASSERT(collectableIter);
		TIadapted*				iterAdapter	 = (TIadapted*) DPTR(collectableIter)->GetValue();
		TCadapted*				cont = DPTR(iterAdapter)->GetContainerAdapter();
		if (cont)
			cont->ToValue(at);
		else
			at->Undefine();
		return true;
	}

	static bool GetCounter (void* val, DeltaValue* at) {
		CollectableSimple*				collectableIter = VGET_INST(CollectableSimple, val, NameTraits::iter2string);
		DASSERT(collectableIter);
		TIadapted*						iterAdapter		= (TIadapted*) DPTR(collectableIter)->GetValue();

		at->FromNumber(DPTR(iterAdapter)->GetIterationCounter());
		return true;
	}

	//************************

#define	ITERATOR_CONSTRUCTOR_DEF(func, type)											\
	static void func (DeltaVirtualMachine* vm)											\
		{ type::iter(vm,  type##Getter); }
		
#define	ITERATOR_GETTERS_DEF(type)														\
	static DeltaExternIdFieldGetter* type##Getter = (DeltaExternIdFieldGetter*) 0;		\
	static bool GetKeys (void* val, DeltaValue* at)										\
		{ return type::GetKeys(val, at, type##Getter); }								\
	static DeltaExternIdFieldGetter::GetByStringFuncEntry type##getters[] = {			\
		{ "keys",		GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},		\
		{ "value",		&type::GetValue,		DELTA_GETBYSTRING_NO_PRECOND	},		\
		{ "container",	&type::GetContainer,	DELTA_GETBYSTRING_NO_PRECOND	},		\
		{ "counter",	&type::GetCounter,		DELTA_GETBYSTRING_NO_PRECOND	}		\
	};

#define	ITERATOR_GETTERS_INIT(type)														\
	unew(type##Getter);																	\
	DPTR(type##Getter)->SetGetByString(type##getters, uarraysize(type##getters));		\
	DPTR(type##Getter)->SetKeyToGetKeys("keys");

#define	ITERATOR_GETTERS_CLEANUP(type)													\
	udelete(type##Getter);
};

/////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
