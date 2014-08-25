// DeltaWxLibraryTemplates.h
// Helper templates to get / set a wx native object
// from / to a delta wx extern id.
// wxWidgets DELTA wrapper.
// A. Maragudakis, May 2013.
//

#ifndef DELTA_WX_LIBRARY_TEMPLATES
#define DELTA_WX_LIBRARY_TEMPLATES

#include "DeltaLibraryFunctionContext.h"
#include "DeltaWxNativeInstanceToWrapperMap.h"
#include "DeltaWxWrapperToExternIdMap.h"
#include "DeltaWxWrapper.h"
#include "DeltaLibraryFuncArgsBinder.h"
#include "uvalidatable.h"
#include "wx/event.h"
#include <string>

////////////////////////////////////////////////////////////////

template<class ClassId, class WrapperT>
WrapperT*		GetWrapperFromExternIdValue (void* val)
{
	util_ui32 wrapperSerialNo	= (util_ui32)DELTA_EGC_SIMPLE_GETVALUE(val);
	WrapperT* wrapper			= VGET_INST(WrapperT, wrapperSerialNo, ClassId::ID);
	return wrapper;
}

template<class ClassId, class WrapperT, class Native>
WrapperT*		GetWrapperFromNativeInstance (Native* nativeInstance)
{
	DASSERT(wxWidgets::NativeInstanceToWrapperMapGet().HasKey(nativeInstance));
	wxWidgets::WrapperInfo info	= wxWidgets::NativeInstanceToWrapperMapGet().GetValue(nativeInstance);
	WrapperT* wrapper	= VGET_INST(WrapperT, info.serialNo, ClassId::ID);
	DASSERT(wrapper);
	return wrapper;
}

template<class ClassId, class WrapperT, class Native>
Native*			GetNativeInstanceFromExternIdValue (void* val)
{
	Native* nativeInstance		= (Native*)0;
	WrapperT* wrapper			= GetWrapperFromExternIdValue<ClassId, WrapperT>(val);
	if (wrapper) {
		util_ui32 wrapperSerialNo	= DPTR(wrapper)->GetSerialNo();
		nativeInstance				= (Native*)DPTR(wrapper)->GetNativeInstance();
	}
	return nativeInstance;
}

template <class ClassId, class WrapperT>
bool			GetWrapperFromArgument
	(
		const std::string& name, WrapperT** wrapper,
		DeltaLibraryFunctionContext* context
	)
{
	WX_CREATE_LIBFUNC_LOCALS
	if (context)
		context->UpdateLocals(&vm, &CURR_FUNC, &_argNo, &_sig1, &_sig2);

	bool hasError				= false;

	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Nil) {
		*wrapper				= (WrapperT*) 0;
		++_argNo;
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DeltaArg<void*> deltaArg(name);
		if (!deltaArg.Get(vm, CURR_FUNC, _argNo)) {
			DPTR(vm)->PrimaryError(deltaArg.GetError().c_str());
			hasError			= true;
		}
		if (_argNo) {_sig1 += ","; _sig2 += ","; }
		_sig1 += deltaArg.GetName();
		_sig2 += deltaArg.ToString();
		++_argNo;

		*wrapper				=	GetWrapperFromExternIdValue
										<ClassId, WrapperT>
										(deltaArg.GetValue());
		if (!*wrapper) {
			DPTR(vm)->PrimaryError("%s(%s): %s!",
				CURR_FUNC, _sig1.c_str(),
				ucstringarg(
					uconstructstr(
						"invalid %s handle 0x%x, probably destroyed",
						ClassId::ID,
						(util_ui32) deltaArg.GetValue()
					)
				)
			);
			hasError			= true;
		}
	} else {
		DPTR(vm)->PrimaryError("%s(%s): %s!",
			CURR_FUNC, _sig1.c_str(),
			ucstringarg(
				uconstructstr(
					"Invalid argument passed (%s). Expected externid or nil.",
					DPTR(vm)->GetActualArg(_argNo)->TypeStr()
				)
			)
		);
		hasError			= true;
	}

	if (hasError) {
		*wrapper			= NULL;
		DLIB_RETVAL_REF.FromNil();
	}
	if (context)
		context->UpdateContext(_argNo, _sig1, _sig2);
	return !hasError;
}

template <class ClassId, class WrapperT, class Native>
bool			GetNativeInstanceFromArgument
	(
		const std::string& name, Native** nativeInstance,
		DeltaLibraryFunctionContext* context
	)
{
	WX_CREATE_LIBFUNC_LOCALS
	if (context)
		context->UpdateLocals(&vm, &CURR_FUNC, &_argNo, &_sig1, &_sig2);

	bool hasError				= false;

	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Nil) {
		*nativeInstance			= (Native*) 0;
		++_argNo;
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DeltaArg<void*> deltaArg(name);
		if (!deltaArg.Get(vm, CURR_FUNC, _argNo)) {
			DPTR(vm)->PrimaryError(deltaArg.GetError().c_str());
			hasError			= true;
		}
		if (_argNo) {_sig1 += ","; _sig2 += ","; }
		_sig1 += deltaArg.GetName();
		_sig2 += deltaArg.ToString();
		++_argNo;

		WrapperT* wrapper		=	GetWrapperFromExternIdValue
										<ClassId, WrapperT>
										(deltaArg.GetValue());
		if (!wrapper) {
			DPTR(vm)->PrimaryError("%s(%s): %s!",
				CURR_FUNC, _sig1.c_str(),
				ucstringarg(
					uconstructstr(
						"invalid %s handle 0x%x, probably destroyed",
						ClassId::ID,
						(util_ui32) deltaArg.GetValue()
					)
				)
			);
			hasError			= true;
		}

		*nativeInstance			= (Native*) DPTR(wrapper)->GetNativeInstance();
	} else {
		DPTR(vm)->PrimaryError("%s(%s): %s!",
			CURR_FUNC, _sig1.c_str(),
			ucstringarg(
				uconstructstr(
					"Invalid argument passed (%s). Expected externid or nil.",
					DPTR(vm)->GetActualArg(_argNo)->TypeStr()
				)
			)
		);
		hasError			= true;
	}

	if (hasError) {
		*nativeInstance		= NULL;
		DLIB_RETVAL_REF.FromNil();
	}
	if (context)
		context->UpdateContext(_argNo, _sig1, _sig2);
	return !hasError;
}

template<class ClassId, class WrapperT>
void			ToString (DeltaString* at, void* val) {
	util_ui32 serialNo	= (util_ui32)DELTA_EGC_SIMPLE_GETVALUE(val);
	WrapperT* wrapper	= VGET_INST(WrapperT, serialNo, ClassId::ID);
	if (!wrapper)
		DPTR(at)->Add(
			uconstructstr("%s(handle 0x%x, destroyed!)",
				ClassId::ID, (util_ui32) val)
		);
	else {
		DASSERT(serialNo == DPTR(wrapper)->GetSerialNo());
		DPTR(at)->Add(
			uconstructstr(
				"%s(0x%x)",
				DPTR(wrapper)->GetExtClassString(),
				DPTR(wrapper)->GetSerialNo()
			)
		);
	}
}

template<class ClassId, class WrapperT>
bool			SetValueFromWrapper
	(
		WrapperT* wrapper, DeltaValue* at,
		void (*destructor)(void*),
		DeltaExternIdFieldGetter* (*getGetter)(void),
		DeltaObject* (*getMethods)(void)
	)
{
	if (wrapper) {
		util_ui32 wrapperSerialNo		=	DPTR(wrapper)->GetSerialNo();
		bool hasEntry					=	wxWidgets::WrapperToExternIdMapGet().HasKey
												(wrapperSerialNo);
		if (hasEntry) {
			util_ui32 externIdSerialNo	=	wxWidgets::WrapperToExternIdMapGet().GetValue
												(wrapperSerialNo);
			at->FromExternIdBySerialNo(externIdSerialNo);
		} else {
			CollectableSimple* collectable	=	CollectableSimple::New
												(
													"wxInstance",
													(void*)wrapperSerialNo,
													destructor
												);
			at->FromExternId(
				(void*) collectable->GetSerialNo(),
				DeltaExternId_CollectableSimple,
				ToString<ClassId, WrapperT>,
				"wxInstance",
				(*getGetter)()
			);
			DELTALIBFUNC_DELEGATE_METHODS_EX(*at, (*getMethods)());
			wxWidgets::WrapperToExternIdMapGet().AddEntry
				(wrapperSerialNo, at->ToExternIdSerialNo());
		}
	} else { at->FromNil(); return false; }
	return true;
}

template<class ClassId, class WrapperT, class Native>
bool			SetValueFromNativeInstance
	(
		Native* nativeInstance, DeltaValue* at,
		void (*destructor)(void*),
		DeltaExternIdFieldGetter* (*getGetter)(void),
		DeltaObject* (*getMethods)(void),
		DeltaLibraryFunctionContext* context
	)
{
	WX_CREATE_LIBFUNC_LOCALS
	if (context)
		context->UpdateLocals(&vm, &CURR_FUNC, &_argNo, &_sig1, &_sig2);

	if (nativeInstance) {
		bool hasEntry			= wxWidgets::NativeInstanceToWrapperMapGet().HasKey(nativeInstance);
		WrapperT* wrapper;
		if (hasEntry) {
			wxWidgets::WrapperInfo info	= wxWidgets::NativeInstanceToWrapperMapGet().GetValue(nativeInstance);
			wrapper				= VGET_INST(WrapperT, info.serialNo, ClassId::ID);
			wxWidgets::Wrapper* oldWrapper = VGET_INST(wxWidgets::Wrapper, info.serialNo, info.classId);
			if (!wrapper && oldWrapper)
			{
				DASSERT(oldWrapper->GetCastToNativeInstance() == nativeInstance);
				// nativeInstance destroyed without destroying the appropriate wrapper
				// since the wrapper is valid, but cannot be downcasted to the correct class
				DDELETE(oldWrapper);
				wrapper			= DNEWCLASS(WrapperT, (nativeInstance));
				DASSERT(!wxWidgets::NativeInstanceToWrapperMapGet().HasKey(nativeInstance));
				wxWidgets::NativeInstanceToWrapperMapGet().AddEntry
					(nativeInstance, DPTR(wrapper)->GetSerialNo(), ClassId::ID);
			}
			if (!wrapper ||
				!DPTR(wrapper)->IsValid() ||
				DPTR(wrapper)->GetNativeInstance() != nativeInstance)
			{
				wxWidgets::NativeInstanceToWrapperMapGet().RemoveEntry(nativeInstance);
				DASSERT(!VGET_INST(wxWidgets::Wrapper, info.serialNo, info.classId));

				DPTR(vm)->PrimaryError("%s(%s): %s!",
					CURR_FUNC, _sig1.c_str(),
					ucstringarg(
						uconstructstr(
							"invalid %s handle 0x%x, probably destroyed",
							info.classId.c_str(), info.serialNo
						)
					)
				);
				at->FromNil();
				return false;
			}
		} else {
			wrapper				= DNEWCLASS(WrapperT, (nativeInstance));
			wxWidgets::NativeInstanceToWrapperMapGet().AddEntry
				(nativeInstance, DPTR(wrapper)->GetSerialNo(), ClassId::ID);
		}

		SetValueFromWrapper<ClassId, WrapperT>(wrapper, at, destructor, getGetter, getMethods);
	} else { at->FromNil(); return false; }
	return true;
}

template<class ParentClassId, class ParentWrapper, class ParentNative,
		class ChildClassId, class ChildWrapper, class ChildNative>
void			SetDependencyBetweenNativeInstances
(
	ParentNative* parent, ChildNative* child
)
{
	ParentWrapper*	parentWrapper	= NULL;
	ChildWrapper*	childWrapper	= NULL;
	if (parent) {
		parentWrapper	=
			GetWrapperFromNativeInstance
				<ParentClassId, ParentWrapper, ParentNative>(parent);
	}
	if (child) {
		childWrapper	=
			GetWrapperFromNativeInstance
				<ChildClassId, ChildWrapper, ChildNative>(child);
	}
	if (parentWrapper)
		static_cast<wxWidgets::Wrapper*>(parentWrapper)->AddChild(static_cast<wxWidgets::Wrapper*>(childWrapper));
	else if (childWrapper)
		static_cast<wxWidgets::Wrapper*>(childWrapper)->SetParent(static_cast<wxWidgets::Wrapper*>(parentWrapper));
}

template<class ChildClassId, class ChildWrapper, class ChildNative>
void			SetWrapperChild (ChildNative* nativeInst)
{
	DASSERT(nativeInst);
	SetDependencyBetweenNativeInstances
		<DeltaWxWindowClassId, DeltaWxWindow, wxWindow,
		ChildClassId, ChildWrapper, ChildNative>
		(nativeInst->GetParent(), nativeInst);
}

////////////////////////////////////////////////////////////////

struct TopLevelWrapperDestroyEventHandler : public wxEvtHandler {
	template <class ClassId, class WrapperT, class Native>
	void		OnDestroy (wxWindowDestroyEvent& e)
	{
		Native* nativeInstance		= static_cast<Native*>(e.GetWindow());
		if (wxWidgets::NativeInstanceToWrapperMapGet().HasKey(nativeInstance)) {
			wxWidgets::WrapperInfo info			= wxWidgets::NativeInstanceToWrapperMapGet().GetValue(nativeInstance);
			DASSERT(info.classId == ClassId::ID);
			WrapperT* wrapper			= VGET_INST(WrapperT, info.serialNo, info.classId);
			DASSERT(wrapper && DPTR(wrapper)->IsValid());
			util_ui32 wrapperSerialNo	= DPTR(wrapper)->GetSerialNo();

			if (wxWidgets::WrapperToExternIdMapGet().HasKey(wrapperSerialNo)) {
				util_ui32 externIdSerialNo	= wxWidgets::WrapperToExternIdMapGet().GetValue(wrapperSerialNo);
				if (VALIDATABLE_VGET_INST(externIdSerialNo)) {
					DeltaValue externId;
					externId.FromExternIdBySerialNo(externIdSerialNo);
					ubag<DeltaValue*> referrers	= externId.GetExternId()->GetReferrers();
					for (ubag<DeltaValue*>::iterator i = referrers.begin(); i != referrers.end(); ++i) {
						DeltaValue* val			= i->first;
						val->Undefine();
					}
				}
			}

			DDELETE(wrapper);
		}
	}

	static TopLevelWrapperDestroyEventHandler*	Instance (void)
	{
		static TopLevelWrapperDestroyEventHandler instance;
		return &instance;
	}
};

////////////////////////////////////////////////////////////////

#endif
