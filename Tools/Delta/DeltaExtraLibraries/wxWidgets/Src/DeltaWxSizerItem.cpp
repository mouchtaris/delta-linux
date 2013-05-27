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
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetWindow (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	wxWindow *win = item->GetWindow();
	DeltaWxWindow *retval = win ? DNEWCLASS(DeltaWxWindow, (win)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
	return true;
}

static bool GetSizer (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	wxSizer *sizer = item->GetSizer();
	DeltaWxSizer *retval = sizer ? DNEWCLASS(DeltaWxSizer, (sizer)) : (DeltaWxSizer*) 0;
	WX_SETOBJECT_EX(*at, Sizer, retval)
	return true;
}

static bool GetSpacer (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(item->GetSpacer())));
	WX_SETOBJECT_EX(*at, Size, retval)
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(item->GetPosition())));
	WX_SETOBJECT_EX(*at, Point, retval)
	return true;
}

static bool GetMinSize (void* val, DeltaValue* at) 
{
	wxSizerItem *item = DLIB_WXTYPECAST_BASE(SizerItem, val, sizeritem);
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(item->GetMinSize())));
	WX_SETOBJECT_EX(*at, Size, retval)
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
	DeltaWxRect *retval = DNEWCLASS(DeltaWxRect, (new wxRect(item->GetRect())));
	WX_SETOBJECT_EX(*at, Rect, retval)
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
	wxSizerItem *wxitem = (wxSizerItem*) 0;
	DeltaWxSizerItem *item = (DeltaWxSizerItem*) 0;
	if (n == 0)
		wxitem = new wxSizerItem();
	else if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Window, serial_no, window, window_wr)) {
				wxWindow *window = (wxWindow*) window_wr->GetCastToNativeInstance();
				DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
				wxitem = new wxSizerItem(window, *flags);
			} else
			if (DLIB_WXISBASE(Sizer, serial_no, sizer, sizer_wr)) {
				wxSizer *sizer = (wxSizer*) sizer_wr->GetCastToNativeInstance();
				DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
				wxitem = new wxSizerItem(sizer, *flags);
			}
		}
	} else if (n == 4) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Window, serial_no, window, window_wr)) {
				wxWindow *window = (wxWindow*) window_wr->GetCastToNativeInstance();
				WX_GETNUMBER(proportion)
				WX_GETDEFINE(flag)
				WX_GETDEFINE(border)
				wxitem = new wxSizerItem(window, proportion, flag, border, NULL);
			} else
			if (DLIB_WXISBASE(Sizer, serial_no, sizer, sizer_wr)) {
				wxSizer *sizer = (wxSizer*) sizer_wr->GetCastToNativeInstance();
				WX_GETNUMBER(proportion)
				WX_GETDEFINE(flag)
				WX_GETDEFINE(border)
				wxitem = new wxSizerItem(sizer, proportion, flag, border, NULL);
			}
		}
	} else if (n == 5) {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		WX_GETNUMBER(proportion)
		WX_GETDEFINE(flag)
		WX_GETDEFINE(border)
		wxitem = new wxSizerItem(width, height, proportion, flag, border, NULL);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			DPTR(vm)->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxitem) item = DNEWCLASS(DeltaWxSizerItem, (wxitem));
	WX_SETOBJECT(SizerItem, item)
}

DLIB_FUNC_START(sizeritem_destruct, 1, Nil)
	DLIB_WXDELETE(sizeritem, SizerItem, item)
}

DLIB_FUNC_START(sizeritem_calcmin, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(item->CalcMin())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(sizeritem_deletewindows, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	item->DeleteWindows();
}

DLIB_FUNC_START(sizeritem_detachsizer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	item->DetachSizer();
}

DLIB_FUNC_START(sizeritem_getborder, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETNUMBER(item->GetBorder())
}

DLIB_FUNC_START(sizeritem_getflag, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETNUMBER(item->GetFlag())
}

DLIB_FUNC_START(sizeritem_getminsize, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(item->GetMinSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(sizeritem_getposition, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(item->GetPosition())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(sizeritem_getproportion, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETNUMBER(item->GetProportion())
}

DLIB_FUNC_START(sizeritem_getratio, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETNUMBER(item->GetRatio())
}

DLIB_FUNC_START(sizeritem_getrect, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DeltaWxRect *retval = DNEWCLASS(DeltaWxRect, (new wxRect(item->GetRect())));
	WX_SETOBJECT(Rect, retval)
}

DLIB_FUNC_START(sizeritem_getsize, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(item->GetSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(sizeritem_getsizer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DeltaWxSizer *retval = DNEWCLASS(DeltaWxSizer, (item->GetSizer()));
	WX_SETOBJECT(Sizer, retval)
}

DLIB_FUNC_START(sizeritem_getspacer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(item->GetSpacer())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(sizeritem_getuserdata, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DeltaWxObject *retval = DNEWCLASS(DeltaWxObject, (item->GetUserData()));
	WX_SETOBJECT(Object, retval)
}

DLIB_FUNC_START(sizeritem_getwindow, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DeltaWxWindow *retval = DNEWCLASS(DeltaWxWindow, (item->GetWindow()));
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(sizeritem_issizer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETBOOL(item->IsSizer())
}

DLIB_FUNC_START(sizeritem_isshown, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETBOOL(item->IsShown())
}

DLIB_FUNC_START(sizeritem_isspacer, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETBOOL(item->IsSpacer())
}

DLIB_FUNC_START(sizeritem_iswindow, 1, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_SETBOOL(item->IsWindow())
}

DLIB_FUNC_START(sizeritem_setborder, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_GETDEFINE(border)
	item->SetBorder(border);
}

DLIB_FUNC_START(sizeritem_setdimension, 3, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DLIB_WXGETPOINT_BASE(pos)
	DLIB_WXGETSIZE_BASE(size)
	item->SetDimension(*pos, *size);
}

DLIB_FUNC_START(sizeritem_setflag, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_GETDEFINE(flag)
	item->SetFlag(flag);
}

DLIB_FUNC_START(sizeritem_setinitsize, 3, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	item->SetInitSize(x, y);
}

DLIB_FUNC_START(sizeritem_setproportion, 2, Nil)
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

DLIB_FUNC_START(sizeritem_setsizer, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	item->SetSizer(sizer);
}

DLIB_FUNC_START(sizeritem_setspacer, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DLIB_WXGETSIZE_BASE(size)
	item->SetSpacer(*size);
}

DLIB_FUNC_START(sizeritem_setwindow, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	DLIB_WXGET_BASE(window, Window, window)
	item->SetWindow(window);
}

DLIB_FUNC_START(sizeritem_show, 2, Nil)
	DLIB_WXGET_BASE(sizeritem, SizerItem, item)
	WX_GETBOOL(show)
	item->Show(show);
}
