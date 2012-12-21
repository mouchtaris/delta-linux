// DeltaStdLibTemplatesUtils.h
// Some generator macros for std lib templates.
// A. Savidis, March 2009.
//

#ifndef	DELTASTDLIBTEMPLATESUTILS_H
#define	DELTASTDLIBTEMPLATESUTILS_H

#include "DeltaStdLibTemplates.h"

/////////////////////////////////////////////////////
// Extracts an external typed value that is subclass of validatable.
//
#define	DLIB_GET_VALIDATABLE_ARG(val, var, type, typestr, vget)				\
	DLIB_ARGEX(val, typestr)												\
	type* var = vget(DLIB_ARGVAL(val));										\
	DLIB_ERROR_CHECK(														\
		!var,																\
		uconstructstr(														\
			"invalid %s handle 0x%x, probably destroyed!",					\
			typestr,														\
			(util_ui32) DLIB_ARGVAL(val)									\
		)																	\
	)

#define	DLIB_GET_VALIDATABLE_BASECLASS_ARG(val, var, type, typestr, vget)	\
	DLIB_ARGEX_BASECLASS(val)												\
	type* var = vget(DLIB_ARGVAL(val));										\
	DLIB_ERROR_CHECK(														\
		!var,																\
		uconstructstr(														\
			"invalid %s handle 0x%x, probably destroyed!",					\
			typestr,														\
			(util_ui32) DLIB_ARGVAL(val)									\
		)																	\
	)

/////////////////////////////////////////////////////
// Extracts an external typed value that is wrapped as a validatable value.
// Although no requirements for the external type are set, it's malpractice
// to have it as validatable since then the extra wrapping as a validatabe 
// value is redundant.
//
#define	DLIB_GET_VALIDATABLEVAL_ARG(var, type, typestr, cid)						\
	type* var;																		\
	DLIB_ARGEX(var##_val, typestr);													\
	typedef ValidatableValue<type, cid> T;											\
	T* var##_wrap = VGET_VALUE(type, cid, DLIB_ARGVAL(var##_val));					\
	DLIB_ERROR_CHECK(																\
		!var##_wrap,																\
		uconstructstr(																\
			"invalid '%s' handle 0x%x, probably closed!",							\
			typestr, (util_ui32) DLIB_ARGVAL(var##_val)								\
		)																			\
	)																				\
	var = var##_wrap->GetValue();													\

/////////////////////////////////////////////////////
// Extracts an external typed value that is wrapped as a collectable simple.
// No particular requirements for the external type are set (e.g. may be or not
// a validatable).
//
template <typename T> T* DLIB_EXTRACT_COLLECTABLESIMPLE_ARG_VALUE (const DeltaArg<void*>& var, const char* typeStr) {
	CollectableSimple* col = VGET_INST(CollectableSimple, DLIB_ARGVAL(var), typeStr);
	DASSERT(col);
	return (T*) DPTR(col)->GetValue();
}

#define	DLIB_GET_COLLECTABLESIMPLE_ARG(var, type, typestr)							\
	type* var;																		\
	unullify(var);																	\
	DLIB_ARGEX(var##_val, typestr);													\
	var = DLIB_EXTRACT_COLLECTABLESIMPLE_ARG_VALUE<type>(var##_val, typestr);

/////////////////////////////////////////////////////
// Produces a to string converter for external typed values that
// are also subclasses of validatable.
//
#define	DLIB_TOSTRING_FUNC(type, typestr, vget)											\
static void ToString (DeltaString* at, void* val) {										\
	type* b = vget(val);																\
	if (!b)																				\
		DPTR(at)->Add(																	\
			uconstructstr("%s(handle 0x%x, destroyed!)", typestr, (util_ui32) val)		\
		);																				\
	else {																				\
		DASSERT((util_ui32) val == DPTR(b)->GetSerialNo());								\
		DPTR(at)->Add(																	\
			uconstructstr(																\
				"%s(0x%x)",																\
				DPTR(b)->GetExtClassString(),											\
				DPTR(b)->GetSerialNo()													\
			)																			\
		);																				\
	}																					\
}

/////////////////////////////////////////////////////
// Requires 'getter' field getter and 'getters' array to be 
// syntactically visible.
//
#define	DLIB_INITIALIZE_CLEANUP_FUNCS_EX(type, apiname, prefix, init, clean)	\
	static void Initialise (void) {												\
		unew(getter);															\
		DPTR(getter)->SetGetByString(getters, uarraysize(getters));				\
		DPTR(getter)->SetKeyToGetKeys("keys");									\
		init																	\
	}																			\
	static void CleanUp (void)													\
		{ udelete(getter); clean }												\
	DELTA_LIBRARY_SUBAPI_INSTALLER_EX(											\
		Delta##type,															\
		apiname,																\
		prefix,																	\
		Initialise(), CleanUp()													\
	)

#define	DLIB_INITIALIZE_CLEANUP_FUNCS(type, apiname, prefix)			\
	DLIB_INITIALIZE_CLEANUP_FUNCS_EX(type, apiname, prefix, _DLIB_EMPTY_, _DLIB_EMPTY_)

/////////////////////////////////////////////////////
// Regains the original external typed-value stored within a
// validatable value wrapper.
//
#define	DLIB_REGAIN_VALIDATABLEVAL_FUNC(type, cid, name)			\
static type* name (void* val) {										\
	ValidatableValue<type, cid>* v = VGET_VALUE(type, cid, val);	\
	DASSERT(v);														\
	return DPTR(v)->GetValue();										\
}

/////////////////////////////////////////////////////
// Regains the original external typed-value stored within a
// collectable simple (validatable also) wrapper.
//
#define	DLIB_REGAIN_COLLECTABLESIMPLE_FUNC(type, typestr, name)			\
static type* name (void* val) {											\
	CollectableSimple* v = VGET_INST(CollectableSimple, val, typestr);	\
	DASSERT(v);															\
	return (type*) DPTR(v)->GetValue();									\
}

/////////////////////////////////////////////////////

#define	DLIB_VALIDATABLEVAL_TOSTRING_FUNC(type, cid)									\
static void ToString (DeltaString* at, void* val) {										\
	ValidatableValue<type, cid>* v = VGET_VALUE(type, cid, val);						\
	if (!v)																				\
		DPTR(at)->Add(																	\
		uconstructstr("%s(handle 0x%x, destroyed!)", cid::ID, (util_ui32) val)			\
		);																				\
	else																				\
		DPTR(at)->Add(																	\
			uconstructstr("%s(0x%x)", cid::ID, DPTR(v)->GetSerialNo())					\
		);																				\
}

/////////////////////////////////////////////////////
// Basic to string converter with type string and serial number only.
//
#define	DLIB_COLLECTABLESIMPLE_TOSTRING_FUNC(typestr)									\
static void ToString (DeltaString* at, void* val) {										\
	CollectableSimple* v = VGET_INST(CollectableSimple, val, typestr);					\
	DASSERT(v);																			\
	DPTR(at)->Add(uconstructstr("%s(0x%x)", typestr, DPTR(v)->GetSerialNo()));			\
}

#define	DLIB_TOEXTERNID_VALUE(gctype, typestr)											\
	at->FromExternId(																	\
		(void*) v->GetSerialNo(),														\
		DeltaExternId_##gctype,															\
		ToString,																		\
		typestr,																		\
		getter																			\
	);																					\

/////////////////////////////////////////////////////
// The group below requires 'funcs' lib func array, 'getter' field getter 
// and 'ToString' converter function to be syntactically visible.
//
#define	DLIB_TOEXTERNID_FUNC(type, typestr, name)								\
static void name (DeltaValue* at, type* v) {									\
	DLIB_TOEXTERNID_VALUE(NonCollectable, typestr)								\
	DELTALIBFUNC_BIND_ALL(funcs, *at);											\
}

#define	DLIB_TOEXTERNID_FUNC_WITH_LIBRARY_METHODS(type, typestr, name)			\
static void name (DeltaValue* at, type* v) {									\
	DLIB_TOEXTERNID_VALUE(NonCollectable, typestr)								\
	DELTALIBFUNC_DELEGATE_METHODS(*at);											\
}

//****************************

#define	DLIB_DELTACONVERTIBLE_TOEXTERNID_FUNC(name)										\
static void name (DeltaValue* at, const std::string& classId, util_ui32 serialNo) {		\
	at->FromExternId(																	\
		(void*) serialNo,																\
		DeltaExternId_NonCollectable,													\
		ToString,																		\
		classId,																		\
		getter																			\
	);																					\
	DELTALIBFUNC_BIND_ALL(funcs, *at);													\
}

//****************************

#define	DLIB_VALIDATABLEVAL_TOEXTERNID_FUNC(type, cid, name)					\
static void name (DeltaValue* at, type* val) {									\
	typedef ValidatableValue<type, cid> T;										\
	T* v = DNEWCLASS(T, (val));													\
	at->FromExternId(															\
		(void*) v->GetSerialNo(),												\
		DeltaExternId_NonCollectable,											\
		ToString,																\
		cid::ID																	\
	);																			\
	DELTALIBFUNC_BIND_ALL(funcs, *at);											\
}

//****************************

#define	DLIB_COLLECTABLESIMPLE_TOEXTERNID_FUNC(type, typestr, name, destructor)	\
static void name (DeltaValue* at, type* val) {									\
	CollectableSimple* v = CollectableSimple::New(typestr, val, destructor);	\
	DASSERT(v);																	\
	DLIB_TOEXTERNID_VALUE(CollectableSimple, typestr)							\
	DELTALIBFUNC_BIND_ALL(funcs, *at);											\
}

#define	DLIB_COLLECTABLESIMPLE_TOEXTERNID_FUNC_WITH_LIBRARY_METHODS(type, typestr, name, destructor)	\
static void name (DeltaValue* at, type* val) {															\
	CollectableSimple* v = CollectableSimple::New(typestr, val, destructor);							\
	DASSERT(v);																							\
	DLIB_TOEXTERNID_VALUE(CollectableSimple, typestr)													\
	DELTALIBFUNC_DELEGATE_METHODS(*at);																	\
}

#define	DLIB_SET_CREATOR_INFORMATION()												\
	if (true)																		\
		DLIB_RETVAL_REF.GetExternId()->GetCreationInfo().SetCreatorInformation(		\
			vm->Source(),															\
			vm->Line(),																\
			CURR_FUNC																\
		); else

/////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.