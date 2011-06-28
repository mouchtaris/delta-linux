#ifndef WXWRAPPERUTILFUNCTIONS_H
#define WXWRAPPERUTILFUNCTIONS_H

#include "uvalidatable.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaStdLibTemplatesUtils.h"
#include "DDebug.h"
#include "DeltaExternId.h"
#include <map>

// _wxclassid is the string const you choose to identify wx classes.
// _wxclass is the native wx class.

// Returns an ExternId. _wxvar is _wxclass* (native wx object).
// Must create wx2String(DeltaString* at, void* val).

#define	DLIB_WXMAKE_EX(arg, _wxvar, _classid, _tostring)								\
	arg.FromExternId(																	\
		(void*) DNEWCLASS(ValidatableUntypedValue, (_wxvar, _classid))->GetSerialNo(),	\
		DeltaExternId_NonCollectable,													\
		_tostring,																		\
		_classid																		\
	);

#define	DLIB_WXMAKE(_wxvar, _classid, _tostring)	DLIB_WXMAKE_EX(DLIB_RETVAL_REF, _wxvar, _classid, _tostring)

#define	WXNEWCLASS_DEFINED(T,val,Tbase,Arg)		\
	Tbase *_##val = (Arg);						\
	if (_##val == (Tbase*)0) {					\
		val = (T*)0;							\
	} else {									\
		val = DNEWCLASS(T, (_##val));			\
	}

#define WXNEWCLASS(T,val,Tbase,Arg)		\
	T* val;								\
	WXNEWCLASS_DEFINED(T,val,Tbase,Arg)

// wxVal is the var defined via DLIB_ARGEX_BASECLASS(wxVal).

#define	DLIB_WXGETCLASSID(wxVal)	\
			VALIDATABLE_VGET_INST(DLIB_ARGVAL(wxVal))->GetExtClassString()

// wxVar's class is classIdFrom and must become classIdTo.
// classIdFrom* tmp = (classIdFrom*) wxVar; is safe.

#define	DLIB_WXGET_WRAPPER(sn) VGET_INST(wxWidgets::Wrapper, sn, "wxInstance")

#define DLIB_WXTYPECAST(wxclass, var, wxclassid)			\
	VGET_INST(DeltaWx##wxclass, var, "wx::"#wxclassid)

#define DLIB_WXTYPECAST_BASE(wxclass, var, wxclassid)							\
	(wx##wxclass*)DLIB_WXTYPECAST(wxclass, var, wxclassid)->GetNativeInstance()

#define DLIB_WXISBASE(class, var, classid, obj)								\
	DeltaWx##class *obj = VGET_INST(DeltaWx##class, var, "wx::"#classid)

#define	DLIB_WXGET_DEFINED(_wxclassid, _wxclass, _var)					\
	DLIB_ARGEX_BASECLASS(_##_var);										\
	_var = VGET_INST(_wxclass, DLIB_ARGVAL(_##_var), _wxclassid);		\
	DLIB_ERROR_CHECK(													\
		!_var,															\
		uconstructstr(													\
			"invalid %s handle 0x%x, probably destroyed",				\
			_wxclassid,													\
			(util_ui32) DLIB_ARGVAL(_##_var)							\
		)																\
	)

#define	DLIB_WXGET(_wxclassid, _wxclass, _var)				\
	_wxclass* _var;											\
	DLIB_WXGET_DEFINED("wx::"#_wxclassid, _wxclass, _var)	\

#define	DLIB_WXGET_BASE(_classid, _class, _var)									\
	wx##_class* _var;															\
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Nil) {				\
		_var = (wx##_class*) 0;													\
		++_argNo;																\
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {	\
		DLIB_WXGET(_classid, DeltaWx##_class, _##_var)							\
		_var = (wx##_class*) DPTR(_##_var)->GetNativeInstance();				\
	} else {																	\
		DLIB_ERROR_CHECK(														\
			true,																\
			uconstructstr(														\
				"invalid argument passed (%s). Expected externid or nil",		\
				DPTR(vm)->GetActualArg(_argNo)->TypeStr()						\
			)																	\
		)																		\
	}

#define DLIB_WXDELETE(_wxclassid, _wxclass, _var)							\
	DLIB_WXGET(_wxclassid, DeltaWx##_wxclass, _var)							\
	wx##_wxclass* nativeInst = (wx##_wxclass*)_var->GetNativeInstance();	\
	if (nativeInst)															\
		delete nativeInst;													\
	DDELETE(_var);

#define WX_GETDEFINE(_var)		\
	int _var = 0;				\
	WX_GETDEFINE_DEFINED(_var)

#define WX_GETDEFINE_DEFINED(_var)													\
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number)				\
		_var = (int) DPTR(vm)->GetActualArg(_argNo)->ToNumber();					\
	else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {			\
		_var = wxWidgets::DefineSearch(DPTR(vm)->GetActualArg(_argNo)->ToString());	\
	} else {																		\
		DLIB_ERROR_CHECK(															\
			true,																	\
			uconstructstr(															\
				"invalid argument passed (%s). Expected string or number",			\
				DPTR(vm)->GetActualArg(_argNo)->TypeStr()							\
			)																		\
		)																			\
	}																				\
	++_argNo;

#define WX_GETTABLE(_var)			\
	DeltaTable *_var;				\
	WX_GETTABLE_DEFINED(_var)

#define WX_GETTABLE_DEFINED(_var)										\
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Table)		\
		_var = DPTR(vm)->GetActualArg(_argNo)->ToTable();				\
	else																\
		DLIB_ERROR_CHECK(												\
			true,														\
			uconstructstr(												\
				"invalid argument passed (%s). Expected table",			\
				DPTR(vm)->GetActualArg(_argNo)->TypeStr()				\
			)															\
		)																\
	++_argNo;

#define WX_GETSTRING_DEFINED(_var)												\
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {			\
		_var = wxString(														\
			DPTR(vm)->GetActualArg(_argNo)->ToString().c_str(), wxConvUTF8);	\
	} else																		\
		DLIB_ERROR_CHECK(														\
			true,																\
			uconstructstr(														\
				"invalid argument passed (%s). Expected string",				\
				DPTR(vm)->GetActualArg(_argNo)->TypeStr()						\
			)																	\
		)																		\
	++_argNo;

#define WX_GETSTRING(_var)		\
	wxString _var;				\
	WX_GETSTRING_DEFINED(_var)

#define WX_SETSTRING_EX(at, _var)							\
	(at).FromString(std::string(_var.mb_str(wxConvUTF8)));

#define WX_SETSTRING(_var)		WX_SETSTRING_EX(DLIB_RETVAL_REF, _var)

#define WX_GETNUMBER_DEFINED(_var)												\
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {			\
		_var = DPTR(vm)->GetActualArg(_argNo)->ToNumber();						\
	} else																		\
		DLIB_ERROR_CHECK(														\
			true,																\
			uconstructstr(														\
				"invalid argument passed (%s). Expected number",				\
				DPTR(vm)->GetActualArg(_argNo)->TypeStr()						\
			)																	\
		)																		\
	++_argNo;

#define WX_GETNUMBER(_var)								\
	double _var;										\
	WX_GETNUMBER_DEFINED(_var)

#define WX_SETNUMBER_EX(at, _var)						\
	(at).FromNumber(_var);

#define WX_SETNUMBER(_var)		WX_SETNUMBER_EX(DLIB_RETVAL_REF, _var)

#define WX_GETBOOL_DEFINED(_var)											\
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Bool) {		\
		_var = DPTR(vm)->GetActualArg(_argNo)->ToBool();					\
	} else																	\
		DLIB_ERROR_CHECK(													\
			true,															\
			uconstructstr(													\
				"invalid argument passed (%s). Expected boolean",			\
				DPTR(vm)->GetActualArg(_argNo)->TypeStr()					\
			)																\
		)																	\
	++_argNo;

#define WX_GETBOOL(_var)		\
	bool _var;					\
	WX_GETBOOL_DEFINED(_var)

#define WX_SETBOOL_EX(at, _var)		\
	(at).FromBool(_var);

#define WX_SETBOOL(_var)		WX_SETBOOL_EX(DLIB_RETVAL_REF, _var)

#define WX_SETOBJECT_EX(_arg, _wxclass, _var)									\
	if (_var && _var->GetNativeInstance()) {									\
	_wxclass##Utils::To##_wxclass(&(_arg), _var);								\
	} else {																	\
		if (_var) {																\
			DDELETE(_var);														\
		}																		\
		(_arg).FromNil();														\
	}

#define WX_SETOBJECT(_wxclass, _var)	WX_SETOBJECT_EX(DLIB_RETVAL_REF, _wxclass, _var)

#define WX_SETTABLE_RETVAL(table, value)								\
	DPTR(table)->Set(DeltaValue((DeltaNumberValueType)0), value);

#define WX_FUNC_ARGRANGE_START(name, atleast, atmost, reset)		\
	static void name##_LibFunc (DeltaVirtualMachine* vm) {			\
		ISSUE_FUNC(#name);											\
		util_ui32 _argNo = 0;										\
		Delta##reset##Reset reseter(vm);							\
		DeltaAtLeastTotalArgsCheck(atleast, CURR_FUNC, RESET_EMPTY)	\
		DeltaAtMostTotalArgsCheck(atmost, CURR_FUNC, RESET_EMPTY)	\
		int n = DPTR(vm)->TotalActualArgs();						\
		std::string _sig1, _sig2;

#define WX_WRAPPER_UTIL_FUNCS_DEFINITION(T)							\
	namespace T##Utils{												\
		void ToString (DeltaString* at, void* val);					\
		void InstallAll(DeltaTable *methods);						\
		void To##T (DeltaValue* at, DeltaWx##T* b);					\
		DeltaObject* GetMethods();									\
	}																\
	extern void Install_DeltaWx##T##_Lib (DeltaObject* mainAPI);	\
	extern void CleanUp_DeltaWx##T##_Lib (void);

#define WX_WRAPPER_DEFINITION(T, classid, TBase)			\
	VCLASSID_HEADER(DeltaWx##T##ClassId)					\
	typedef DeltaNativeClassSingleBaseWrapper<				\
					DeltaWx##T##ClassId,					\
					DeltaWx##TBase,							\
					wx##T									\
			> DeltaWx##T;									\
	WX_WRAPPER_UTIL_FUNCS_DEFINITION(T)

#define WX_WRAPPER_DEFINITION_BASE(T, classid)				\
	VCLASSID_HEADER(DeltaWx##T##ClassId)					\
	typedef DeltaNativeClassNoBaseWrapper<					\
					DeltaWx##T##ClassId,					\
					wxWidgets::Wrapper,						\
					wx##T									\
			> DeltaWx##T;									\
	WX_WRAPPER_UTIL_FUNCS_DEFINITION(T)

#define WX_LIBRARY_FUNCS_DEFINITION(wxclass)								\
	extern void Install_DeltaWx##wxclass##_Lib (DeltaObject* mainAPI);		\
	extern void CleanUp_DeltaWx##wxclass##_Lib (void);

#define WX_INSTALL_LIBRARY_IMPLEMENTATION(wxclass)				\
	void wx##wxclass##InstallLibraryMethods()					\
	{															\
		wxclass##Utils::InstallAll(methods);					\
	}

#define WX_LIBRARY_FUNCS_IMPLEMENTATION(wxclass, classid)					\
	WX_INSTALL_LIBRARY_IMPLEMENTATION(wxclass)								\
	DLIB_INITIALIZE_CLEANUP_FUNCS_EX(										\
		Wx##wxclass, #classid,												\
		"wx::"#classid"_",													\
		methods = DeltaObject::NativeCodeHelpers::NewObject();				\
		DELTALIBFUNC_SET_METHODS_TABLE_CREATION_INFO(methods, apiClassName);\
		wx##wxclass##InstallLibraryMethods();,								\
		DELTALIBFUNC_DESTROY_METHODS_TABLE();								\
	)

#define WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(wxclass, classid, init, cleanup)	\
	WX_INSTALL_LIBRARY_IMPLEMENTATION(wxclass)								\
	DLIB_INITIALIZE_CLEANUP_FUNCS_EX(										\
		Wx##wxclass, #classid,												\
		"wx::"#classid"_",													\
		methods = DeltaObject::NativeCodeHelpers::NewObject();				\
		DELTALIBFUNC_SET_METHODS_TABLE_CREATION_INFO(methods, apiClassName);\
		init,																\
		DELTALIBFUNC_DESTROY_METHODS_TABLE();								\
		cleanup																\
	)

#define DLIB_WX_TOSTRING(T, wxclassid)								\
	void T##Utils::ToString (DeltaString* at, void* val) {			\
	DeltaWx##T* b = VGET_INST(DeltaWx##T, val, "wx::"wxclassid);	\
		if (!b)														\
			DPTR(at)->Add(											\
				uconstructstr("%s(handle 0x%x, destroyed!)",		\
					DeltaWx##T##ClassId::ID, (util_ui32) val)		\
			);														\
		else {														\
			DASSERT((util_ui32) val == DPTR(b)->GetSerialNo());		\
			DPTR(at)->Add(											\
				uconstructstr(										\
					"%s(0x%x)",										\
					DPTR(b)->GetExtClassString(),					\
					DPTR(b)->GetSerialNo()							\
				)													\
			);														\
		}															\
	}

#define DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(wxclass, wxclassid)							\
	static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;					\
	inline bool VAL_IS_##wxclass (DeltaValue val)												\
	{																							\
		return (val.Type() == DeltaValue_ExternId &&											\
				VGET_INST(DeltaWx##wxclass,														\
						  val.ToExternId(),														\
						  "wx::"wxclassid));													\
	}																							\
	DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_##wxclass(val), "wx::"wxclassid)\
	DeltaObject* wxclass##Utils::GetMethods() { return methods; }

#define DLIB_WX_TOEXTERNID(wxclass)												\
	void wxclass##Utils::To##wxclass (DeltaValue* at, DeltaWx##wxclass* b)		\
	{																			\
		at->FromExternId(														\
			(void*) b->GetSerialNo(),											\
			DeltaExternId_NonCollectable,										\
			wxclass##Utils::ToString,											\
			"wxInstance",														\
			getter																\
		);																		\
		DELTALIBFUNC_DELEGATE_METHODS(*at);										\
	}

#define DLIB_WX_INSTALLALL_FUNC(wxclassid)										\
	util_ui16 from = METHODS_START_INDEX;										\
	util_ui16 to = METHODS_START_INDEX + METHODS_TOTAL;							\
	DASSERT(!strcmp(funcs[from].name, METHODS_FIRST));							\
	DASSERT(!strcmp(funcs[to-1].name, METHODS_LAST));							\
	DeltaLibraryObjectCreator::FuncEntry *functions = funcs;					\
	DeltaTable* table = DNEW(DELTA_OBJECT);										\
	for (functions += from; from != to; ++from, ++functions) {					\
		DeltaValue index(functions->name), content(functions->func, binder);	\
		bool result = DPTR(methods)->Set(index, content);						\
		DASSERT(result);														\
		result = DPTR(table)->Set(index, content);								\
		DASSERT(result);														\
	}																			\
	DPTR(methods)->Set(DeltaValue(wxclassid), DeltaValue(table));

#define DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(wxclass, wxclassid, baseclass)	\
	VCLASSID_IMPL(DeltaWx##wxclass##ClassId, "wx::"##wxclassid)					\
	DLIB_WX_TOSTRING(wxclass, wxclassid)										\
	DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(wxclass, wxclassid)				\
	DLIB_WX_TOEXTERNID(wxclass)													\
	void wxclass##Utils::InstallAll(DeltaTable *methods)						\
	{																			\
		DELTALIBFUNC_INSTALL_METHODS(funcs);									\
		DPTR(methods)->DelegateInternal(baseclass##Utils::GetMethods());		\
	}

#define DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(wxclass, wxclassid)		\
	VCLASSID_IMPL(DeltaWx##wxclass##ClassId, "wx::"##wxclassid)					\
	DLIB_WX_TOSTRING(wxclass, wxclassid)										\
	DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(wxclass, wxclassid)				\
	DLIB_WX_TOEXTERNID(wxclass)													\
	void wxclass##Utils::InstallAll(DeltaTable *methods)						\
	{																			\
		DELTALIBFUNC_INSTALL_METHODS(funcs);									\
	}

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF1(T,name) static void T##_##name##_LibFunc (DeltaVirtualMachine* vm);
#define WX_FUNC1(T, name) { #name, T##_##name##_LibFunc }
#define WX_FUNCS_START static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
#define WX_FUNCS_END };

////////////////////////////////////////////////////////////////

namespace wxWidgets {
	void DefineInit(void);
	void DefineExit(void);
	int DefineSearch(std::string str, bool *found = NULL);

	VCLASSID_HEADER(WrapperClassId)
	typedef DeltaNativeClassWrapperSuperclass<wxWidgets::WrapperClassId> Wrapper;
}

#endif