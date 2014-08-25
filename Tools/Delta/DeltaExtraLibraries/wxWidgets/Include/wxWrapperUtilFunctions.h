#ifndef WXWRAPPERUTILFUNCTIONS_H
#define WXWRAPPERUTILFUNCTIONS_H

#include "DeltaWxWrapper.h"
#include "DeltaLibraryFunctionContext.h"
#include "DeltaWxLibraryTemplates.h"
#include "DeltaWxDefineMap.h"
#include "DeltaWxNativeInstanceToWrapperMap.h"
#include "DeltaWxWrapperToExternIdMap.h"
#include "DeltaWxCollectableDestructors.h"
#include "uvalidatable.h"
#include "usingleton.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaStdLibTemplatesUtils.h"
#include "DeltaCollectableSimple.h"
#include "DDebug.h"
#include "DeltaExternId.h"
#include <map>
#include <list>
#include <algorithm>

///////////////////////////////////////////////////////////

using namespace wxWidgets;

///////////////////////////////////////////////////////////

#define WX_INVALID_ARGUMENT_TYPE_ERROR(errorMsg)		\
	DLIB_ERROR_CHECK(									\
		true,											\
		uconstructstr(									\
			"Invalid argument passed (%s). " errorMsg,	\
			DPTR(vm)->GetActualArg(_argNo)->TypeStr()	\
		)												\
	)

///////////////////////////////////////////////////////////

#define WX_CONSTRUCT_WINDOW_OBJECT(_class, _var, _wxvar)	\
	if (_wxvar) {											\
		var		= DNEWCLASS(DeltaWx##_class, (_wxvar));		\
		_wxvar->SetClientData(_var);						\
	}														\
	WX_SETOBJECT(_class, _var)

///////////////////////////////////////////////////////////
// wxclassid arg is not used
#define DLIB_WXTYPECAST(wxclass, var, wxclassid)								\
	GetWrapperFromExternIdValue<DeltaWx##wxclass##ClassId, DeltaWx##wxclass>((void*)var)

#define DLIB_WXTYPECAST_BASE(wxclass, var, wxclassid)							\
	GetNativeInstanceFromExternIdValue											\
		<DeltaWx##wxclass##ClassId, DeltaWx##wxclass, wx##wxclass>((void*)var)

///////////////////////////////////////////////////////////

#define DLIB_WXISBASE(class, var, classid, obj)								\
	wx##class *obj = DLIB_WXTYPECAST_BASE(class, var, classid)

///////////////////////////////////////////////////////////

#define	DLIB_WXGET(_wxclassid, _wxclass, _var)									\
	DeltaWx##_wxclass* _var = (DeltaWx##_wxclass*)0;							\
	context.UpdateContext(_argNo, _sig1, _sig2);								\
	if (GetWrapperFromArgument<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass>	\
		(#_var, &_var, &context))												\
		context.UpdateLocals(NULL, NULL, &_argNo, &_sig1, &_sig2);				\
	else																		\
		return;

#define	DLIB_WXGET_BASE(_classid, _class, _var)									\
	wx##_class* _var = (wx##_class*)0;											\
	context.UpdateContext(_argNo, _sig1, _sig2);								\
	if (GetNativeInstanceFromArgument											\
		<DeltaWx##_class##ClassId, DeltaWx##_class, wx##_class>					\
		(#_var, &_var, &context))												\
		context.UpdateLocals(NULL, NULL, &_argNo, &_sig1, &_sig2);				\
	else																		\
		return;

#define DLIB_WXGETPOINT_INVALID_ARGUMENT_MESSAGE									\
	"Expected externid of type 'point' or String with value \"[wx]DefaultPosition\""

#define DLIB_WXGETPOINT_BASE(_var)													\
	wxPoint *_var;																	\
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {				\
		const std::string _var##_ = DPTR(vm)->GetActualArg(_argNo++)->ToString();	\
		if (_var##_ == "DefaultPosition" || _var##_ == "wxDefaultPosition")			\
			_var = new wxPoint(wxDefaultPosition);									\
		else																		\
			WX_INVALID_ARGUMENT_TYPE_ERROR(DLIB_WXGETPOINT_INVALID_ARGUMENT_MESSAGE)\
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {		\
		DLIB_WXGET_BASE(point, Point, _var##_)										\
		_var = _var##_;																\
	} else {																		\
		WX_INVALID_ARGUMENT_TYPE_ERROR(DLIB_WXGETPOINT_INVALID_ARGUMENT_MESSAGE)	\
	}

#define DLIB_WXGETSIZE_INVALID_ARGUMENT_MESSAGE										\
	"Expected externid of type 'size' or String with value \"[wx]DefaultSize\""

#define DLIB_WXGETSIZE_BASE(_var)													\
	wxSize *_var;																	\
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {				\
		const std::string _var##_ = DPTR(vm)->GetActualArg(_argNo++)->ToString();	\
		if (_var##_ == "DefaultSize" || _var##_ == "wxDefaultSize")					\
			_var = new wxSize(wxDefaultSize);										\
		else																		\
			WX_INVALID_ARGUMENT_TYPE_ERROR(DLIB_WXGETSIZE_INVALID_ARGUMENT_MESSAGE)	\
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {		\
		DLIB_WXGET_BASE(size, Size, _var##_)										\
		_var = _var##_;																\
	} else {																		\
		WX_INVALID_ARGUMENT_TYPE_ERROR(DLIB_WXGETSIZE_INVALID_ARGUMENT_MESSAGE)		\
	}

///////////////////////////////////////////////////////////

#define DLIB_WXDELETE(_wxclassid, _wxclass, _var)							\
	DLIB_WXGET(_wxclassid, _wxclass, _var)									\
	DestroyWrapperAndNativeInstance											\
		<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass, wx##_wxclass>		\
		((void*)_var->GetSerialNo());

///////////////////////////////////////////////////////////

#define WX_GETDEFINE(_var)		\
	int _var = 0;				\
	WX_GETDEFINE_DEFINED(_var)

#define WX_GETDEFINE_DEFINED(_var)													\
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number)				\
		_var = (int) DPTR(vm)->GetActualArg(_argNo)->ToNumber();					\
	else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {			\
		_var = wxWidgets::DefineMapGet().Search(DPTR(vm)->GetActualArg(_argNo)->ToString());	\
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

///////////////////////////////////////////////////////////

#define WX_SETTABLE_RETVAL(table, value)							\
	DPTR(table)->Set(DeltaValue((DeltaNumberValueType)0), value);

///////////////////////////////////////////////////////////

#define WX_SETOBJECT_NO_CONTEXT_EX2(_arg, _wxclass, _var, _destructor)	\
	SetValueFromNativeInstance											\
		<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass, wx##_wxclass>	\
		(_var, &(_arg), &_destructor,									\
		&_wxclass##Utils::GetGetter, &_wxclass##Utils::GetMethods, NULL);

#define WX_SETOBJECT_NO_CONTEXT_EX(_arg, _wxclass, _var)				\
	WX_SETOBJECT_NO_CONTEXT_EX2(_arg, _wxclass, _var, LetWrapperLive)

#define WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(_arg, _wxclass, _var)	\
	SetValueFromNativeInstance															\
		<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass, wx##_wxclass>					\
		(_var, &(_arg), &DestroyWrapperAndNativeInstance								\
		<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass, wx##_wxclass>,					\
		&_wxclass##Utils::GetGetter, &_wxclass##Utils::GetMethods, NULL);

///////////////////////////////////////////////////////////

#define WX_SETOBJECT_EX2(_arg, _wxclass, _var, _destructor)				\
	context.UpdateContext(_argNo, _sig1, _sig2);						\
	if (SetValueFromNativeInstance										\
		<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass, wx##_wxclass>	\
		(_var, &(_arg), &_destructor,									\
		&_wxclass##Utils::GetGetter, &_wxclass##Utils::GetMethods,		\
		&context)) {													\
		context.UpdateLocals(NULL, NULL, &_argNo, &_sig1, &_sig2);		\
	} else																\
		return;

#define WX_SETOBJECT_EX(_arg, _wxclass, _var)							\
	WX_SETOBJECT_EX2(_arg, _wxclass, _var, LetWrapperLive)

#define WX_SETOBJECT(_wxclass, _var)									\
	WX_SETOBJECT_EX(DLIB_RETVAL_REF, _wxclass, _var)

#define WX_SET_WINDOW_OBJECT(_wxclass, _var)								\
	WX_SETOBJECT(_wxclass, _var)											\
	if (_var) {																\
		SetWrapperChild														\
		<																	\
			DeltaWx##_wxclass##ClassId,										\
			DeltaWx##_wxclass,												\
			wx##_wxclass													\
		>(_var);															\
		_var->Connect(														\
			wxEVT_DESTROY,													\
			(wxObjectEventFunction)(wxEventFunction)static_cast				\
			<wxWindowDestroyEventFunction>									\
			(&TopLevelWrapperDestroyEventHandler::OnDestroy					\
			<DeltaWx##_wxclass##ClassId,DeltaWx##_wxclass,wx##_wxclass>),	\
			NULL,															\
			TopLevelWrapperDestroyEventHandler::Instance()					\
		);																	\
	}

#define WX_SET_TOPLEVELWINDOW_OBJECT(_wxclass, _var)						\
	WX_SET_WINDOW_OBJECT(_wxclass, _var)

///////////////////////////////////////////////////////////

#define WX_SETOBJECT_COLLECTABLE_WRAPPER_EX(_arg, _wxclass, _var)		\
	SetValueFromWrapper<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass>	\
	(																	\
		_var, _arg,														\
		&DestroyWrapper<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass>,	\
		&_wxclass##Utils::GetGetter, &_wxclass##Utils::GetMethods		\
	);

#define WX_SET_BASECLASS_GETTER(_arg, _wxclass, _val)							\
	wx##_wxclass* parent	= DLIB_WXTYPECAST_BASE(_wxclass, _val, "unused");	\
	DeltaWx##_wxclass* wrapper	= DNEWCLASS(DeltaWx##_wxclass, (parent));		\
	WX_SETOBJECT_COLLECTABLE_WRAPPER_EX(_arg, _wxclass, wrapper)

///////////////////////////////////////////////////////////

#define WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE_EX(_arg, _wxclass, _var)	\
	context.UpdateContext(_argNo, _sig1, _sig2);							\
	if (SetValueFromNativeInstance											\
		<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass, wx##_wxclass>		\
		(_var, &(_arg), &DestroyWrapperAndNativeInstance					\
			<DeltaWx##_wxclass##ClassId, DeltaWx##_wxclass, wx##_wxclass>,	\
		&_wxclass##Utils::GetGetter, &_wxclass##Utils::GetMethods,			\
		&context)) {														\
		context.UpdateLocals(NULL, NULL, &_argNo, &_sig1, &_sig2);			\
	} else																	\
		return;

#define WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(_wxclass, _var)			\
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE_EX(DLIB_RETVAL_REF, _wxclass, _var)

///////////////////////////////////////////////////////////

#define WX_FUNC_ARGRANGE_START(name, atleast, atmost, reset)		\
	static void name##_LibFunc (DeltaVirtualMachine* vm) {			\
		ISSUE_FUNC(#name);											\
		util_ui32 _argNo = 0;										\
		Delta##reset##Reset reseter(vm);							\
		DeltaAtLeastTotalArgsCheck(atleast, CURR_FUNC, RESET_EMPTY)	\
		DeltaAtMostTotalArgsCheck(atmost, CURR_FUNC, RESET_EMPTY)	\
		int n = DPTR(vm)->TotalActualArgs();						\
		std::string _sig1, _sig2;									\
		WX_CREATE_CONTEXT(context);

#define WX_FUNC_START(name, n, reset)								\
	DLIB_FUNC_START(name, n, reset)									\
	WX_CREATE_CONTEXT(context);

#define WX_WRAPPER_UTIL_FUNCS_DEFINITION(T)							\
	namespace T##Utils{												\
		void InstallAll (DeltaTable *methods);						\
		bool IsValidArg (DeltaValue arg);							\
		DeltaObject* GetMethods();									\
		DeltaExternIdFieldGetter* GetGetter();						\
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

#define DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(wxclass, wxclassid)			\
	static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;	\
	bool wxclass##Utils::IsValidArg (DeltaValue val) {							\
		return	val.Type() == DeltaValue_ExternId &&							\
				VGET_INST(														\
					DeltaWx##wxclass,											\
					DELTA_EGC_SIMPLE_GETVALUE(val.ToExternId()),				\
					DeltaWx##wxclass##ClassId::ID								\
				);																\
	}																			\
	DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(						\
		wxclass##Utils::IsValidArg(val),										\
		DeltaWx##wxclass##ClassId::ID)											\
	DeltaObject* wxclass##Utils::GetMethods() { return methods; }				\
	DeltaExternIdFieldGetter* wxclass##Utils::GetGetter() { return getter; }

#define DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(wxclass, wxclassid, baseclass)	\
	VCLASSID_IMPL(DeltaWx##wxclass##ClassId, "wx::" wxclassid)					\
	DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(wxclass, wxclassid)				\
	void wxclass##Utils::InstallAll(DeltaTable *methods)						\
	{																			\
		DELTALIBFUNC_INSTALL_METHODS(funcs);									\
		DPTR(methods)->DelegateInternal(baseclass##Utils::GetMethods());		\
	}

#define DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(wxclass, wxclassid)		\
	VCLASSID_IMPL(DeltaWx##wxclass##ClassId, "wx::" wxclassid)					\
	DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(wxclass, wxclassid)				\
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

#endif
