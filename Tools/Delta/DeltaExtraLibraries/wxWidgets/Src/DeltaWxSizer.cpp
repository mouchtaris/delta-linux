#include "DeltaWxSizer.h"
#include "DeltaWxSizerItem.h"
#include "DeltaWxSizerFlags.h"
#include "DeltaWxObject.h"
#include "DeltaWxWindow.h"
#include "DeltaWxSize.h"
#include "DeltaWxPoint.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "ListLib.h"
#include "wxWrapperUtilFunctions.h"
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(sizer, name)
#define WX_FUNC(name) WX_FUNC1(sizer, name)

WX_FUNC_DEF(destruct)
WX_FUNC_DEF(add)
WX_FUNC_DEF(addspacer)
WX_FUNC_DEF(addstretchspacer)
WX_FUNC_DEF(clear)
WX_FUNC_DEF(detach)
WX_FUNC_DEF(fit)
WX_FUNC_DEF(fitinside)
WX_FUNC_DEF(getchildren)
WX_FUNC_DEF(getcontainingwindow)
WX_FUNC_DEF(getitem)
WX_FUNC_DEF(getsize)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(getminsize)
WX_FUNC_DEF(hide)
WX_FUNC_DEF(insert)
WX_FUNC_DEF(insertspacer)
WX_FUNC_DEF(insertstretchspacer)
WX_FUNC_DEF(isshown)
WX_FUNC_DEF(layout)
WX_FUNC_DEF(prepend)
WX_FUNC_DEF(prependspacer)
WX_FUNC_DEF(prependstretchspacer)
WX_FUNC_DEF(recalcsizes)
WX_FUNC_DEF(remove)
WX_FUNC_DEF(replace)
WX_FUNC_DEF(setdimension)
WX_FUNC_DEF(setminsize)
WX_FUNC_DEF(setitemminsize)
WX_FUNC_DEF(setsizehints)
WX_FUNC_DEF(setvirtualsizehints)
WX_FUNC_DEF(show)

WX_FUNCS_START
	WX_FUNC(destruct),
	WX_FUNC(add),
	WX_FUNC(addspacer),
	WX_FUNC(addstretchspacer),
	WX_FUNC(clear),
	WX_FUNC(detach),
	WX_FUNC(fit),
	WX_FUNC(fitinside),
	WX_FUNC(getchildren),
	WX_FUNC(getcontainingwindow),
	WX_FUNC(getitem),
	WX_FUNC(getsize),
	WX_FUNC(getposition),
	WX_FUNC(getminsize),
	WX_FUNC(hide),
	WX_FUNC(insert),
	WX_FUNC(insertspacer),
	WX_FUNC(insertstretchspacer),
	WX_FUNC(isshown),
	WX_FUNC(layout),
	WX_FUNC(prepend),
	WX_FUNC(prependspacer),
	WX_FUNC(prependstretchspacer),
	WX_FUNC(recalcsizes),
	WX_FUNC(remove),
	WX_FUNC(replace),
	WX_FUNC(setdimension),
	WX_FUNC(setminsize),
	WX_FUNC(setitemminsize),
	WX_FUNC(setsizehints),
	WX_FUNC(setvirtualsizehints),
	WX_FUNC(show)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(0, uarraysize(funcs), "destruct", "show")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Sizer, "sizer", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Sizer, val, sizer) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetSize (void* val, DeltaValue* at) 
{
	wxSizer *sizer = DLIB_WXTYPECAST_BASE(Sizer, val, sizer);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Size, new wxSize(sizer->GetSize()))
	return true;
}

static bool GetMinSize (void* val, DeltaValue* at) 
{
	wxSizer *sizer = DLIB_WXTYPECAST_BASE(Sizer, val, sizer);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Size, new wxSize(sizer->GetMinSize()))
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxSizer *sizer = DLIB_WXTYPECAST_BASE(Sizer, val, sizer);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Point, new wxPoint(sizer->GetPosition()))
	return true;
}

static bool GetChildren (void* val, DeltaValue* at) 
{
	wxSizer *sizer = DLIB_WXTYPECAST_BASE(Sizer, val, sizer);
	wxSizerItemList sizeritemlist = sizer->GetChildren();
	DeltaList_MakeForDebuggerOnly(*at);
	std::list<DeltaValue>* list = DeltaList_Get(*at);
	for (wxSizerItemList::iterator it = sizeritemlist.begin(); it != sizeritemlist.end(); ++it) {
		DeltaValue value;
		wxSizerItem *child = *it;
		WX_SETOBJECT_NO_CONTEXT_EX(value, SizerItem, child)
		list->push_back(value);
	}
	return true;
}

static bool GetContainingWindow (void* val, DeltaValue* at) 
{
	wxSizer *sizer = DLIB_WXTYPECAST_BASE(Sizer, val, sizer);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, sizer->GetContainingWindow())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "size",				&GetSize,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minSize",			&GetMinSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "position",			&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "children",			&GetChildren,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "containigWindow",	&GetContainingWindow,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Sizer,sizer)

////////////////////////////////////////////////////////////////

WX_FUNC_START(sizer_destruct, 1, Nil)
	DLIB_WXDELETE(sizer, Sizer, sizer)
}

WX_FUNC_ARGRANGE_START(sizer_add, 2, 6, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	wxSizerItem *retval = (wxSizerItem*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			if (n == 2) {
				retval	= (sizer->Add(window));;
			} else {
				if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
					DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
					retval	=  (sizer->Add(window, *flags));;
				} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
					WX_GETNUMBER(proportion)
					int flag = 0, border = 0;
					if (n >= 4) { WX_GETDEFINE_DEFINED(flag) }
					if (n >= 5) { WX_GETDEFINE_DEFINED(border) }
					retval	=  (sizer->Add(window, proportion, flag, border));;
				}
			}
		} else
		if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			if (n == 2) {
				retval	= (sizer->Add(szr));;
			} else {
				if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
					DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
					retval	=  (sizer->Add(szr, *flags));;
				} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
					WX_GETNUMBER(proportion)
					int flag = 0, border = 0;
					if (n >= 4) { WX_GETDEFINE_DEFINED(flag) }
					if (n >= 5) { WX_GETDEFINE_DEFINED(border) }
					retval	=  (sizer->Add(szr, proportion, flag, border));;
				}
			}
		} else
		if (DLIB_WXISBASE(SizerItem, serial_no, sizeritem, szrItem)) {
			retval	= (sizer->Add(szrItem));;
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		int proportion = 0, flag = 0, border = 0;
		if (n >= 4) { WX_GETNUMBER_DEFINED(proportion) }
		if (n >= 5) { WX_GETDEFINE_DEFINED(flag) }
		if (n >= 6) { WX_GETDEFINE_DEFINED(border) }
		retval	=  (sizer->Add(width, height, proportion, flag, border));;
	}
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_START(sizer_addspacer, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_GETNUMBER(size)
	wxSizerItem* retval	= sizer->AddSpacer(size);
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_ARGRANGE_START(sizer_addstretchspacer, 1, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	int prop = 1;
	if (n >= 2) { WX_GETNUMBER_DEFINED(prop) }
	wxSizerItem* retval	= sizer->AddStretchSpacer(prop);
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_START(sizer_calcmin, 1, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(sizer->CalcMin()))
}

WX_FUNC_ARGRANGE_START(sizer_clear, 1, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	bool delete_windows = false;
	if (n >= 2) { WX_GETBOOL_DEFINED(delete_windows) }
	sizer->Clear(delete_windows);
}

WX_FUNC_START(sizer_detach, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			WX_SETBOOL(sizer->Detach(window))
		} else if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			WX_SETBOOL(sizer->Detach(szr))
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		WX_SETBOOL(sizer->Detach(index))
	}
}

WX_FUNC_START(sizer_fit, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(sizer->Fit(window)))
}

WX_FUNC_START(sizer_fitinside, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	DLIB_WXGET_BASE(window, Window, window)
	sizer->FitInside(window);
}

WX_FUNC_START(sizer_getchildren, 1, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	wxSizerItemList sizeritemlist = sizer->GetChildren();
	DeltaList_Make(DLIB_RETVAL_REF);
	std::list<DeltaValue>* list = DeltaList_Get(DLIB_RETVAL_REF);
	for (wxSizerItemList::iterator it = sizeritemlist.begin(); it != sizeritemlist.end(); ++it) {
		DeltaValue value;
		wxSizerItem *child = *it;
		WX_SETOBJECT_EX(value, SizerItem, child)
		list->push_back(value);
	}
}

WX_FUNC_START(sizer_getcontainingwindow, 1, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	wxWindow* retval	= (sizer->GetContainingWindow());;
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_ARGRANGE_START(sizer_getitem, 2, 3, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	wxSizerItem *retval = (wxSizerItem*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			bool recursive = false;
			if (n >= 3) { WX_GETBOOL_DEFINED(recursive) }
			retval	= sizer->GetItem(window, recursive);
		} else if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			bool recursive = false;
			if (n >= 3) { WX_GETBOOL_DEFINED(recursive) }
			retval	= sizer->GetItem(szr, recursive);
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		retval	= sizer->GetItem(index);
	}
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_START(sizer_getsize, 1, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(sizer->GetSize()))
}

WX_FUNC_START(sizer_getposition, 1, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(sizer->GetPosition()))
}

WX_FUNC_START(sizer_getminsize, 1, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(sizer->GetMinSize()))
}

WX_FUNC_ARGRANGE_START(sizer_hide, 2, 3, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			bool recursive = false;
			if (n >= 3) { WX_GETBOOL_DEFINED(recursive) }
			WX_SETBOOL(sizer->Hide(window, recursive))
		} else if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			bool recursive = false;
			if (n >= 3) { WX_GETBOOL_DEFINED(recursive) }
			WX_SETBOOL(sizer->Hide(szr, recursive))
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		WX_SETBOOL(sizer->Hide(index))
	}
}

WX_FUNC_ARGRANGE_START(sizer_insert, 3, 7, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_GETNUMBER(index)
	wxSizerItem *retval = (wxSizerItem*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			if (n == 3) {
				retval	=  (sizer->Insert(index, window));;
			} else {
				if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
					DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
					retval	=  (sizer->Insert(index, window, *flags));;
				} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
					WX_GETNUMBER(proportion)
					int flag = 0, border = 0;
					if (n >= 5) { WX_GETDEFINE_DEFINED(flag) }
					if (n >= 6) { WX_GETDEFINE_DEFINED(border) }
					retval	=  (sizer->Insert(index, window, proportion, flag, border));;
				}
			}
		} else
		if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			if (n == 3) {
				retval	=  (sizer->Insert(index, szr));;
			} else {
				if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
					DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
					retval	=  (sizer->Insert(index, szr, *flags));;
				} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
					WX_GETNUMBER(proportion)
					int flag = 0, border = 0;
					wxObject* userData = NULL;
					if (n >= 5) { WX_GETDEFINE_DEFINED(flag) }
					if (n >= 6) { WX_GETDEFINE_DEFINED(border) }
					retval	=  (sizer->Insert(index, szr, proportion, flag, border));;
				}
			}
		} else
		if (DLIB_WXISBASE(SizerItem, serial_no, sizeritem, szrItem)) {
			retval	=  (sizer->Insert(index, szrItem));;
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		int proportion = 0, flag = 0, border = 0;
		wxObject* userData = NULL;
		if (n >= 5) { WX_GETNUMBER_DEFINED(proportion) }
		if (n >= 6) { WX_GETDEFINE_DEFINED(flag) }
		if (n >= 7) { WX_GETDEFINE_DEFINED(border) }
		retval	=  (sizer->Insert(index, width, height, proportion, flag, border));;
	}
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_START(sizer_insertspacer, 3, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_GETNUMBER(index)
	WX_GETNUMBER(size)
	wxSizerItem* retval	= sizer->InsertSpacer(index, size);
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_ARGRANGE_START(sizer_insertstretchspacer, 2, 3, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_GETNUMBER(index)
	int prop = 1;
	if (n >= 3) { WX_GETNUMBER_DEFINED(prop) }
	wxSizerItem* retval	= sizer->InsertStretchSpacer(index, prop);
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_START(sizer_isshown, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			WX_SETBOOL(sizer->IsShown(window))
		} else if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			WX_SETBOOL(sizer->IsShown(szr))
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		WX_SETBOOL(sizer->IsShown(index))
	}
}

WX_FUNC_START(sizer_layout, 1, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	sizer->Layout();
}

WX_FUNC_ARGRANGE_START(sizer_prepend, 2, 6, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	wxSizerItem *retval = (wxSizerItem*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			if (n == 2) {
				retval	= (sizer->Prepend(window));;
			} else {
				if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
					DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
					retval	=  (sizer->Prepend(window, *flags));;
				} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
					WX_GETNUMBER(proportion)
					int flag = 0, border = 0;
					if (n >= 4) { WX_GETDEFINE_DEFINED(flag) }
					if (n >= 5) { WX_GETDEFINE_DEFINED(border) }
					retval	=  (sizer->Prepend(window, proportion, flag, border));;
				}
			}
		} else
		if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			if (n == 2) {
				retval	= (sizer->Prepend(szr));;
			} else {
				if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
					DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
					retval	=  (sizer->Prepend(szr, *flags));;
				} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
					WX_GETNUMBER(proportion)
					int flag = 0, border = 0;
					if (n >= 4) { WX_GETDEFINE_DEFINED(flag) }
					if (n >= 5) { WX_GETDEFINE_DEFINED(border) }
					retval	=  (sizer->Prepend(szr, proportion, flag, border));;
				}
			}
		} else
		if (DLIB_WXISBASE(SizerItem, serial_no, sizeritem, szrItem)) {
			retval	= (sizer->Prepend(szrItem));;
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		int proportion = 0, flag = 0, border = 0;
		if (n >= 4) { WX_GETNUMBER_DEFINED(proportion) }
		if (n >= 5) { WX_GETDEFINE_DEFINED(flag) }
		if (n >= 6) { WX_GETDEFINE_DEFINED(border) }
		retval	=  (sizer->Prepend(width, height, proportion, flag, border));;
	}
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_START(sizer_prependspacer, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_GETNUMBER(size)
	wxSizerItem* retval	= sizer->PrependSpacer(size);
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_ARGRANGE_START(sizer_prependstretchspacer, 1, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	int prop = 1;
	if (n >= 2) { WX_GETNUMBER_DEFINED(prop) }
	wxSizerItem* retval	= sizer->PrependStretchSpacer(prop);
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_START(sizer_recalcsizes, 1, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	sizer->RecalcSizes();
}

WX_FUNC_START(sizer_remove, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			bool retval;
#if wxCHECK_VERSION(2, 9, 0)
			retval = sizer->Detach(window);
#else
			retval = sizer->Remove(window);
#endif
			WX_SETBOOL(retval)
		} else if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			WX_SETBOOL(sizer->Remove(szr))
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		WX_SETBOOL(sizer->Remove(index))
	}
}

WX_FUNC_ARGRANGE_START(sizer_replace, 3, 4, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, oldwin)) {
			DLIB_WXGET_BASE(window, Window, newwin)
			bool recursive = false;
			if (n >= 4) { WX_GETBOOL_DEFINED(recursive) }
			WX_SETBOOL(sizer->Replace(oldwin, newwin, recursive))
		} else if (DLIB_WXISBASE(Sizer, serial_no, sizer, oldsz)) {
			DLIB_WXGET_BASE(sizer, Sizer, newsz)
			bool recursive = false;
			if (n >= 4) { WX_GETBOOL_DEFINED(recursive) }
			WX_SETBOOL(sizer->Replace(oldsz, newsz, recursive))
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(oldindex)
		DLIB_WXGET_BASE(sizeritem, SizerItem, newitem)
		WX_SETBOOL(sizer->Replace(oldindex, newitem))
	}
}

WX_FUNC_START(sizer_setdimension, 5, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	WX_GETNUMBER(width)
	WX_GETNUMBER(height)
	sizer->SetDimension(x, y, width, height);
}

WX_FUNC_ARGRANGE_START(sizer_setminsize, 2, 3, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	if (n == 2) {
		DLIB_WXGETSIZE_BASE(size)
		sizer->SetMinSize(*size);
	} else {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		sizer->SetMinSize(width, height);
	}
}

WX_FUNC_START(sizer_setitemminsize, 4, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			WX_GETNUMBER(width)
			WX_GETNUMBER(height)
			sizer->SetItemMinSize(window, width, height);
		} else if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			WX_GETNUMBER(width)
			WX_GETNUMBER(height)
			sizer->SetItemMinSize(szr, width, height);
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		sizer->SetItemMinSize(index, width, height);
	}
}

WX_FUNC_START(sizer_setsizehints, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	DLIB_WXGET_BASE(window, Window, window)
	sizer->SetSizeHints(window);
}

WX_FUNC_START(sizer_setvirtualsizehints, 2, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	DLIB_WXGET_BASE(window, Window, window)
	sizer->SetVirtualSizeHints(window);
}

WX_FUNC_ARGRANGE_START(sizer_show, 2, 4, Nil)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			bool show = true, recursive = false;
			if (n >= 3) { WX_GETBOOL_DEFINED(show) }
			if (n >= 4) { WX_GETBOOL_DEFINED(recursive) }
			WX_SETBOOL(sizer->Show(window, show, recursive))
		} else if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			bool show = true, recursive = false;
			if (n >= 3) { WX_GETBOOL_DEFINED(show) }
			if (n >= 4) { WX_GETBOOL_DEFINED(recursive) }
			WX_SETBOOL(sizer->Show(szr, show, recursive))
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		bool show = true;
		if (n >= 3) { WX_GETBOOL_DEFINED(show) }
		WX_SETBOOL(sizer->Show(index, show))
	}
}
