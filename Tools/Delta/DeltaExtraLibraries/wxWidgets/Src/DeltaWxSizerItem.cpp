#include "DeltaWxSizerItem.h"
#include "DeltaWxSizer.h"
#include "DeltaWxWindow.h"
#include "DeltaWxObject.h"
#include "DeltaWxSizerFlags.h"
#include "DeltaWxSize.h"
#include "DeltaWxPoint.h"
#include "DeltaWxRect.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(sizeritem, name)
#define WX_FUNC(name) WX_FUNC1(sizeritem, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(calcmin)
WX_FUNC_DEF(deletewindows)
WX_FUNC_DEF(detachsizer)
WX_FUNC_DEF(getborder)
WX_FUNC_DEF(getflag)
WX_FUNC_DEF(getminsize)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(getproportion)
WX_FUNC_DEF(getratio)
WX_FUNC_DEF(getrect)
WX_FUNC_DEF(getsize)
WX_FUNC_DEF(getsizer)
WX_FUNC_DEF(getspacer)
WX_FUNC_DEF(getuserdata)
WX_FUNC_DEF(getwindow)
WX_FUNC_DEF(issizer)
WX_FUNC_DEF(isshown)
WX_FUNC_DEF(isspacer)
WX_FUNC_DEF(iswindow)
WX_FUNC_DEF(setborder)
WX_FUNC_DEF(setdimension)
WX_FUNC_DEF(setflag)
WX_FUNC_DEF(setinitsize)
WX_FUNC_DEF(setproportion)
WX_FUNC_DEF(setratio)
WX_FUNC_DEF(setsizer)
WX_FUNC_DEF(setspacer)
WX_FUNC_DEF(setwindow)
WX_FUNC_DEF(show)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(calcmin),
	WX_FUNC(deletewindows),
	WX_FUNC(detachsizer),
	WX_FUNC(getborder),
	WX_FUNC(getflag),
	WX_FUNC(getminsize),
	WX_FUNC(getposition),
	WX_FUNC(getproportion),
	WX_FUNC(getratio),
	WX_FUNC(getrect),
	WX_FUNC(getsize),
	WX_FUNC(getsizer),
	WX_FUNC(getspacer),
	WX_FUNC(getuserdata),
	WX_FUNC(getwindow),
	WX_FUNC(issizer),
	WX_FUNC(isshown),
	WX_FUNC(isspacer),
	WX_FUNC(iswindow),
	WX_FUNC(setborder),
	WX_FUNC(setdimension),
	WX_FUNC(setflag),
	WX_FUNC(setinitsize),
	WX_FUNC(setproportion),
	WX_FUNC(setratio),
	WX_FUNC(setsizer),
	WX_FUNC(setspacer),
	WX_FUNC(setwindow),
	WX_FUNC(show)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "show")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SizerItem, "sizeritem", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetWindow (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, item->GetWindow())
	return true;
}

static bool GetSizer (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Sizer, item->GetSizer())
	return true;
}

static bool GetSpacer (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Size, new wxSize(item->GetSpacer()))
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Point, new wxPoint(item->GetPosition()))
	return true;
}

static bool GetMinSize (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Size, new wxSize(item->GetMinSize()))
	return true;
}

static bool GetProportion (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETNUMBER_EX(*at, item->GetProportion())
	return true;
}

static bool GetBorder (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETNUMBER_EX(*at, item->GetBorder())
	return true;
}

static bool GetFlag (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETNUMBER_EX(*at, item->GetFlag())
	return true;
}

static bool GetRect (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Rect, new wxRect(item->GetRect()))
	return true;
}

static bool GetRatio (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	WX_SETNUMBER_EX(*at, item->GetRatio())
	return true;
}

////////////////////////////////////////////////////////////////

static bool IsWindow (void* val) {
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	return item->IsWindow();
}

static bool IsSizer (void* val) {
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	return item->IsSizer();
}

static bool IsSpacer (void* val) {
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	return item->IsSpacer();
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "window",				&GetWindow,				&IsWindow						},
	{ "sizer",				&GetSizer,				&IsSizer						},
	{ "spacer",				&GetSpacer,				&IsSpacer						},
	{ "position",			&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minSize",			&GetMinSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "proportion",			&GetProportion,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "border",				&GetBorder,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "flag",				&GetFlag,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rect",				&GetRect,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "ratio",				&GetRatio,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SizerItem,sizeritem)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(sizeritem_construct, 0, 5, Nil)
	wxSizerItem *item = (wxSizerItem*) 0;
	if (n == 0)
		item = new wxSizerItem();
	else if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Window, serial_no, window, window)) {
				DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
				item = new wxSizerItem(window, *flags);
			} else
			if (DLIB_WXISBASE(Sizer, serial_no, sizer, sizer)) {
				DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
				item = new wxSizerItem(sizer, *flags);
			}
		}
	} else if (n == 4) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Window, serial_no, window, window)) {
				WX_GETNUMBER(proportion)
				WX_GETDEFINE(flag)
				WX_GETDEFINE(border)
				item = new wxSizerItem(window, proportion, flag, border, NULL);
			} else
			if (DLIB_WXISBASE(Sizer, serial_no, sizer, sizer)) {
				WX_GETNUMBER(proportion)
				WX_GETDEFINE(flag)
				WX_GETDEFINE(border)
				item = new wxSizerItem(sizer, proportion, flag, border, NULL);
			}
		}
	} else if (n == 5) {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		WX_GETNUMBER(proportion)
		WX_GETDEFINE(flag)
		WX_GETDEFINE(border)
		item = new wxSizerItem(width, height, proportion, flag, border, NULL);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			DPTR(vm)->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SETOBJECT(SizerItem, item)
}

WX_FUNC_START(sizeritem_destruct, 1, Nil)
	DLIB_WXDELETE(sizeritem, SizerItem, item)
}

WX_FUNC_START(sizeritem_calcmin, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(item->CalcMin()))
}

WX_FUNC_START(sizeritem_deletewindows, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	item->DeleteWindows();
}

WX_FUNC_START(sizeritem_detachsizer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	item->DetachSizer();
}

WX_FUNC_START(sizeritem_getborder, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETNUMBER(item->GetBorder())
}

WX_FUNC_START(sizeritem_getflag, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETNUMBER(item->GetFlag())
}

WX_FUNC_START(sizeritem_getminsize, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(item->GetMinSize()))
}

WX_FUNC_START(sizeritem_getposition, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(item->GetPosition()))
}

WX_FUNC_START(sizeritem_getproportion, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETNUMBER(item->GetProportion())
}

WX_FUNC_START(sizeritem_getratio, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETNUMBER(item->GetRatio())
}

WX_FUNC_START(sizeritem_getrect, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Rect, new wxRect(item->GetRect()))
}

WX_FUNC_START(sizeritem_getsize, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(item->GetSize()))
}

WX_FUNC_START(sizeritem_getsizer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETOBJECT(Sizer, item->GetSizer())
}

WX_FUNC_START(sizeritem_getspacer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(item->GetSpacer()))
}

WX_FUNC_START(sizeritem_getuserdata, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETOBJECT(Object, item->GetUserData())
}

WX_FUNC_START(sizeritem_getwindow, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETOBJECT(Window, item->GetWindow())
}

WX_FUNC_START(sizeritem_issizer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETBOOL(item->IsSizer())
}

WX_FUNC_START(sizeritem_isshown, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETBOOL(item->IsShown())
}

WX_FUNC_START(sizeritem_isspacer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETBOOL(item->IsSpacer())
}

WX_FUNC_START(sizeritem_iswindow, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETBOOL(item->IsWindow())
}

WX_FUNC_START(sizeritem_setborder, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_GETDEFINE(border)
	item->SetBorder(border);
}

WX_FUNC_START(sizeritem_setdimension, 3, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DLIB_WXGETPOINT_BASE(pos)
	DLIB_WXGETSIZE_BASE(size)
	item->SetDimension(*pos, *size);
}

WX_FUNC_START(sizeritem_setflag, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_GETDEFINE(flag)
	item->SetFlag(flag);
}

WX_FUNC_START(sizeritem_setinitsize, 3, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	item->SetInitSize(x, y);
}

WX_FUNC_START(sizeritem_setproportion, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_GETNUMBER(proportion)
	item->SetProportion(proportion);
}

WX_FUNC_ARGRANGE_START(sizeritem_setratio, 2, 3, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	if (n == 3) {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		item->SetRatio(width, height);
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGETSIZE_BASE(size)
			item->SetRatio(*size);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(ratio)
			item->SetRatio(ratio);
		}
	}
}

WX_FUNC_START(sizeritem_setsizer, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	item->SetSizer(sizer);
}

WX_FUNC_START(sizeritem_setspacer, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DLIB_WXGETSIZE_BASE(size)
	item->SetSpacer(*size);
}

WX_FUNC_START(sizeritem_setwindow, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DLIB_WXGET_BASE(window, Window, window)
	item->SetWindow(window);
}

WX_FUNC_START(sizeritem_show, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_GETBOOL(show)
	item->Show(show);
}
