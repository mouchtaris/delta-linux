#include "DeltaWxListCtrl.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxImageList.h"
#include "DeltaWxListItem.h"
#include "DeltaWxTextCtrl.h"
#include "DeltaWxColour.h"
#include "DeltaWxFont.h"
#include "DeltaWxRect.h"
#include "DeltaWxValidator.h"
#include "DeltaWxListItemAttr.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(listctrl, name)
#define WX_FUNC(name) WX_FUNC1(listctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(arrange)
WX_FUNC_DEF(assignimagelist)
WX_FUNC_DEF(clearall)
WX_FUNC_DEF(create)
WX_FUNC_DEF(deleteallitems)
WX_FUNC_DEF(deletecolumn)
WX_FUNC_DEF(deleteitem)
WX_FUNC_DEF(editlabel)
WX_FUNC_DEF(ensurevisible)
WX_FUNC_DEF(finditem)
WX_FUNC_DEF(getcolumn)
WX_FUNC_DEF(getcolumncount)
WX_FUNC_DEF(getcolumnwidth)
WX_FUNC_DEF(getcountperpage)
WX_FUNC_DEF(geteditcontrol)
WX_FUNC_DEF(getimagelist)
WX_FUNC_DEF(getitem)
WX_FUNC_DEF(getitembackgroundcolour)
WX_FUNC_DEF(getitemcount)
WX_FUNC_DEF(getitemfont)
WX_FUNC_DEF(getitemposition)
WX_FUNC_DEF(getitemrect)
WX_FUNC_DEF(getsubitemrect)
WX_FUNC_DEF(getitemspacing)
WX_FUNC_DEF(getitemstate)
WX_FUNC_DEF(getitemtext)
WX_FUNC_DEF(getitemtextcolour)
WX_FUNC_DEF(getnextitem)
WX_FUNC_DEF(getselecteditemcount)
WX_FUNC_DEF(gettextcolour)
WX_FUNC_DEF(gettopitem)
WX_FUNC_DEF(getviewrect)
WX_FUNC_DEF(hittest)
WX_FUNC_DEF(insertcolumn)
WX_FUNC_DEF(insertitem)
WX_FUNC_DEF(refreshitem)
WX_FUNC_DEF(refreshitems)
WX_FUNC_DEF(scrolllist)
WX_FUNC_DEF(setbackgroundcolour)
WX_FUNC_DEF(setcolumn)
WX_FUNC_DEF(setcolumnwidth)
WX_FUNC_DEF(setimagelist)
WX_FUNC_DEF(setitem)
WX_FUNC_DEF(setitembackgroundcolour)
WX_FUNC_DEF(setitemcount)
WX_FUNC_DEF(setitemfont)
WX_FUNC_DEF(setitemimage)
WX_FUNC_DEF(setitemcolumnimage)
WX_FUNC_DEF(setitemposition)
WX_FUNC_DEF(setitemstate)
WX_FUNC_DEF(setitemtext)
WX_FUNC_DEF(setitemtextcolour)
WX_FUNC_DEF(setsinglestyle)
WX_FUNC_DEF(settextcolour)
WX_FUNC_DEF(setwindowstyleflag)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(arrange),
	WX_FUNC(assignimagelist),
	WX_FUNC(clearall),
	WX_FUNC(create),
	WX_FUNC(deleteallitems),
	WX_FUNC(deletecolumn),
	WX_FUNC(deleteitem),
	WX_FUNC(editlabel),
	WX_FUNC(ensurevisible),
	WX_FUNC(finditem),
	WX_FUNC(getcolumn),
	WX_FUNC(getcolumncount),
	WX_FUNC(getcolumnwidth),
	WX_FUNC(getcountperpage),
	WX_FUNC(geteditcontrol),
	WX_FUNC(getimagelist),
	WX_FUNC(getitem),
	WX_FUNC(getitembackgroundcolour),
	WX_FUNC(getitemcount),
	WX_FUNC(getitemfont),
	WX_FUNC(getitemposition),
	WX_FUNC(getitemrect),
	WX_FUNC(getsubitemrect),
	WX_FUNC(getitemspacing),
	WX_FUNC(getitemstate),
	WX_FUNC(getitemtext),
	WX_FUNC(getitemtextcolour),
	WX_FUNC(getnextitem),
	WX_FUNC(getselecteditemcount),
	WX_FUNC(gettextcolour),
	WX_FUNC(gettopitem),
	WX_FUNC(getviewrect),
	WX_FUNC(hittest),
	WX_FUNC(insertcolumn),
	WX_FUNC(insertitem),
	WX_FUNC(refreshitem),
	WX_FUNC(refreshitems),
	WX_FUNC(scrolllist),
	WX_FUNC(setbackgroundcolour),
	WX_FUNC(setcolumn),
	WX_FUNC(setcolumnwidth),
	WX_FUNC(setimagelist),
	WX_FUNC(setitem),
	WX_FUNC(setitembackgroundcolour),
	WX_FUNC(setitemcount),
	WX_FUNC(setitemfont),
	WX_FUNC(setitemimage),
	WX_FUNC(setitemcolumnimage),
	WX_FUNC(setitemposition),
	WX_FUNC(setitemstate),
	WX_FUNC(setitemtext),
	WX_FUNC(setitemtextcolour),
	WX_FUNC(setsinglestyle),
	WX_FUNC(settextcolour),
	WX_FUNC(setwindowstyleflag)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "arrange", "setwindowstyleflag")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ListCtrl, "listctrl", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ListCtrl, val, listctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetEditControl (void* val, DeltaValue* at) 
{
	wxListCtrl *ctrl = DLIB_WXTYPECAST_BASE(ListCtrl, val, listctrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, TextCtrl, ctrl->GetEditControl())
	return true;
}

static bool GetImageListNormal (void* val, DeltaValue* at) 
{
	wxListCtrl *ctrl = DLIB_WXTYPECAST_BASE(ListCtrl, val, listctrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ImageList, ctrl->GetImageList(wxIMAGE_LIST_NORMAL))
	return true;
}

static bool GetImageListSmall (void* val, DeltaValue* at) 
{
	wxListCtrl *ctrl = DLIB_WXTYPECAST_BASE(ListCtrl, val, listctrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ImageList, ctrl->GetImageList(wxIMAGE_LIST_SMALL))
	return true;
}

static bool GetImageListState (void* val, DeltaValue* at) 
{
	wxListCtrl *ctrl = DLIB_WXTYPECAST_BASE(ListCtrl, val, listctrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ImageList, ctrl->GetImageList(wxIMAGE_LIST_STATE))
	return true;
}

static bool GetColumnCount (void* val, DeltaValue* at) 
{
	wxListCtrl *ctrl = DLIB_WXTYPECAST_BASE(ListCtrl, val, listctrl);
	WX_SETNUMBER_EX(*at, ctrl->GetColumnCount())
	return true;
}

static bool GetItemCount (void* val, DeltaValue* at) 
{
	wxListCtrl *ctrl = DLIB_WXTYPECAST_BASE(ListCtrl, val, listctrl);
	WX_SETNUMBER_EX(*at, ctrl->GetItemCount())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textControl",		&GetEditControl,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "imageListNormal",	&GetImageListNormal,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "imageListSmall",		&GetImageListSmall,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "imageListState",		&GetImageListState,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "columnCount",		&GetColumnCount,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "count",				&GetItemCount,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ListCtrl,listctrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(listctrl_construct, 0, 7, Nil)
	wxListCtrl *ctrl = (wxListCtrl*) 0;
	if (n == 0) {
		ctrl = new wxListCtrl();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxLC_ICON;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxListCtrlNameStr;
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 7) { WX_GETSTRING_DEFINED(name) }
		ctrl = new wxListCtrl(parent, id, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(ListCtrl, ctrl)
}

WX_FUNC_ARGRANGE_START(listctrl_arrange, 1, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	int flag = wxLIST_ALIGN_DEFAULT;
	if (n >= 2) { WX_GETDEFINE_DEFINED(flag) }
	WX_SETBOOL(ctrl->Arrange(flag))
}

WX_FUNC_START(listctrl_assignimagelist, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	DLIB_WXGET_BASE(imagelist, ImageList, imagelist)
	WX_GETNUMBER(which)
	ctrl->AssignImageList(imagelist, which);
}

WX_FUNC_START(listctrl_clearall, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	ctrl->ClearAll();
}

WX_FUNC_ARGRANGE_START(listctrl_create, 3, 8, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxLC_ICON;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxListCtrlNameStr;
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 8) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(ctrl->Create(parent, id, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(ctrl);
}

WX_FUNC_START(listctrl_deleteallitems, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETBOOL(ctrl->DeleteAllItems())
}

WX_FUNC_START(listctrl_deletecolumn, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(col)
	WX_SETBOOL(ctrl->DeleteColumn(col))
}

WX_FUNC_START(listctrl_deleteitem, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_SETBOOL(ctrl->DeleteItem(item))
}

WX_FUNC_START(listctrl_editlabel, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	ctrl->EditLabel(item);
}

WX_FUNC_START(listctrl_ensurevisible, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_SETBOOL(ctrl->EnsureVisible(item))
}

WX_FUNC_ARGRANGE_START(listctrl_finditem, 3, 4, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(start)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(str)
		bool partial = false;
		if (n >= 4) { WX_GETBOOL_DEFINED(partial) }
		WX_SETNUMBER(ctrl->FindItem(start, str, partial))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId && n == 4) {
		DLIB_WXGETPOINT_BASE(pt)
		WX_GETDEFINE(direction)
		WX_SETNUMBER(ctrl->FindItem(start, *pt, direction))
	}
}

WX_FUNC_START(listctrl_getcolumn, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(col)
	wxListItem item;
	if (ctrl->GetColumn(col, item)) {
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ListItem, new wxListItem(item))
	} else {
		DLIB_RETVAL_REF.FromNil();
	}
}

WX_FUNC_START(listctrl_getcolumncount, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETNUMBER(ctrl->GetColumnCount())
}

WX_FUNC_START(listctrl_getcolumnwidth, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(col)
	WX_SETNUMBER(ctrl->GetColumnWidth(col))
}

WX_FUNC_START(listctrl_getcountperpage, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETNUMBER(ctrl->GetCountPerPage())
}

WX_FUNC_START(listctrl_geteditcontrol, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETOBJECT(TextCtrl, ctrl->GetEditControl())
}

WX_FUNC_START(listctrl_getimagelist, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETDEFINE(which)
	WX_SETOBJECT(ImageList, ctrl->GetImageList(which))
}

WX_FUNC_START(listctrl_getitem, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	wxListItem info;
	if (ctrl->GetItem(info)) {
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ListItem, new wxListItem(info))
	}
}

WX_FUNC_START(listctrl_getitembackgroundcolour, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(ctrl->GetItemBackgroundColour(item)))
}

WX_FUNC_START(listctrl_getitemcount, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETNUMBER(ctrl->GetItemCount())
}

WX_FUNC_START(listctrl_getitemfont, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(ctrl->GetItemFont(item)))
}

WX_FUNC_START(listctrl_getitemposition, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	wxPoint pos;
	if (ctrl->GetItemPosition(item, pos)) {
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(pos))
	}
}

WX_FUNC_ARGRANGE_START(listctrl_getitemrect, 2, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	int code = wxLIST_RECT_BOUNDS;
	if (n >= 3) { WX_GETDEFINE_DEFINED(code) }
	wxRect rect;
	if (ctrl->GetItemRect(item, rect, code)) {
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Rect, new wxRect(rect))
	}
}

WX_FUNC_ARGRANGE_START(listctrl_getsubitemrect, 3, 4, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_GETNUMBER(subItem)
	int code = wxLIST_RECT_BOUNDS;
	if (n >= 4) { WX_GETDEFINE_DEFINED(code) }
	wxRect rect;
	if (ctrl->GetSubItemRect(item, subItem, rect, code)) {
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Rect, new wxRect(rect))
	}
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(listctrl_getitemspacing, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(ctrl->GetItemSpacing()))
}

WX_FUNC_START(listctrl_getitemstate, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_GETDEFINE(stateMask)
	WX_SETNUMBER(ctrl->GetItemState(item, stateMask))
}

WX_FUNC_START(listctrl_getitemtext, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_SETSTRING(ctrl->GetItemText(item))
}

WX_FUNC_START(listctrl_getitemtextcolour, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(ctrl->GetItemTextColour(item)))
}

WX_FUNC_ARGRANGE_START(listctrl_getnextitem, 2, 4, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	int geometry = wxLIST_NEXT_ALL;
	int state = wxLIST_STATE_DONTCARE;
	if (n >= 3) { WX_GETDEFINE_DEFINED(geometry) }
	if (n >= 4) { WX_GETDEFINE_DEFINED(state) }
	WX_SETNUMBER(ctrl->GetNextItem(item, geometry, state))
}

WX_FUNC_START(listctrl_getselecteditemcount, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETNUMBER(ctrl->GetSelectedItemCount())
}

WX_FUNC_START(listctrl_gettextcolour, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(ctrl->GetTextColour()))
}

WX_FUNC_START(listctrl_gettopitem, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETNUMBER(ctrl->GetTopItem())
}

WX_FUNC_START(listctrl_getviewrect, 1, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Rect, new wxRect(ctrl->GetViewRect()))
}

WX_FUNC_ARGRANGE_START(listctrl_hittest, 3, 4, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	DLIB_WXGETPOINT_BASE(point)
	WX_GETDEFINE(flags)
	long subItem;
	WX_SETNUMBER(ctrl->HitTest(*point, flags, &subItem))
	if (n >= 4) {
		WX_GETTABLE(itemTable)
		WX_SETTABLE_RETVAL(itemTable, DeltaValue((DeltaNumberValueType)subItem))
	}
}

WX_FUNC_ARGRANGE_START(listctrl_insertcolumn, 3, 5, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(col)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(listitem, ListItem, info)
		WX_SETNUMBER(ctrl->InsertColumn(col, *info))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(heading)
		int format = wxLIST_FORMAT_LEFT, width = -1;
		if (n >= 4) { WX_GETDEFINE_DEFINED(format) }
		if (n >= 5) { WX_GETNUMBER_DEFINED(width) }
		WX_SETNUMBER(ctrl->InsertColumn(col, heading, format, width))
	}
}

WX_FUNC_ARGRANGE_START(listctrl_insertitem, 2, 4, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(listitem, ListItem, info)
		WX_SETNUMBER(ctrl->InsertItem(*info))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number && n >= 3) {
		WX_GETNUMBER(index)
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(imageIndex)
			WX_SETNUMBER(ctrl->InsertItem(index, imageIndex))
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETSTRING(label)
			if (n == 3) { WX_SETNUMBER(ctrl->InsertItem(index, label)) }
			if (n == 4) { WX_GETNUMBER(imageIndex) WX_SETNUMBER(ctrl->InsertItem(index, label, imageIndex)) }
		}
	}
}

WX_FUNC_START(listctrl_refreshitem, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	ctrl->RefreshItem(item);
}

WX_FUNC_START(listctrl_refreshitems, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(itemFrom)
	WX_GETNUMBER(itemTo)
	ctrl->RefreshItems(itemFrom, itemTo);
}

WX_FUNC_START(listctrl_scrolllist, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(dx)
	WX_GETNUMBER(dy)
	WX_SETBOOL(ctrl->ScrollList(dx, dy))
}

WX_FUNC_START(listctrl_setbackgroundcolour, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	DLIB_WXGET_BASE(colour, Colour, col)
	ctrl->SetBackgroundColour(*col);
}

WX_FUNC_START(listctrl_setcolumn, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(col)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETBOOL(ctrl->SetColumn(col, *item))
}

WX_FUNC_START(listctrl_setcolumnwidth, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(col)
	WX_GETNUMBER(width)
	WX_SETBOOL(ctrl->SetColumnWidth(col, width))
}

WX_FUNC_START(listctrl_setimagelist, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	WX_GETDEFINE(which)
	ctrl->SetImageList(imageList, which);
}

WX_FUNC_ARGRANGE_START(listctrl_setitem, 2, 5, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(listitem, ListItem, info)
		WX_SETBOOL(ctrl->SetItem(*info))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number && n >= 4) {
		WX_GETNUMBER(index)
		WX_GETNUMBER(col)
		WX_GETSTRING(label)
		int imageId = -1;
		if (n >= 5) { WX_GETNUMBER_DEFINED(imageId) }
		WX_SETNUMBER(ctrl->SetItem(index, col, label, imageId))
	}
}

WX_FUNC_START(listctrl_setitembackgroundcolour, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	DLIB_WXGET_BASE(colour, Colour, col)
	ctrl->SetItemBackgroundColour(item, *col);
}

WX_FUNC_START(listctrl_setitemcount, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(count)
	ctrl->SetItemCount(count);
}

WX_FUNC_START(listctrl_setitemfont, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	DLIB_WXGET_BASE(font, Font, font)
	ctrl->SetItemFont(item, *font);
}

WX_FUNC_ARGRANGE_START(listctrl_setitemimage, 3, 4, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_GETNUMBER(image)
	if (n == 3) { WX_SETBOOL(ctrl->SetItemImage(item, image)) }
	if (n == 4) { WX_GETNUMBER(selImage) WX_SETBOOL(ctrl->SetItemImage(item, image, selImage)) }
}

WX_FUNC_START(listctrl_setitemcolumnimage, 4, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_GETNUMBER(column)
	WX_GETNUMBER(image)
	WX_SETBOOL(ctrl->SetItemColumnImage(item, column, image))
}

WX_FUNC_START(listctrl_setitemposition, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	DLIB_WXGETPOINT_BASE(pos)
	WX_SETBOOL(ctrl->SetItemPosition(item, *pos))
}

WX_FUNC_START(listctrl_setitemstate, 4, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_GETDEFINE(state)
	WX_GETDEFINE(statemask)
	WX_SETBOOL(ctrl->SetItemState(item, state, statemask))
}

WX_FUNC_START(listctrl_setitemtext, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	WX_GETSTRING(text)
	ctrl->SetItemText(item, text);
}

WX_FUNC_START(listctrl_setitemtextcolour, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETNUMBER(item)
	DLIB_WXGET_BASE(colour, Colour, col)
	ctrl->SetItemTextColour(item, *col);
}

WX_FUNC_ARGRANGE_START(listctrl_setsinglestyle, 2, 3, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETDEFINE(style)
	bool add = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(add) }
	ctrl->SetSingleStyle(style, add);
}

WX_FUNC_START(listctrl_settextcolour, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	DLIB_WXGET_BASE(colour, Colour, col)
	ctrl->SetTextColour(*col);
}

WX_FUNC_START(listctrl_setwindowstyleflag, 2, Nil)
	DLIB_WXGET_BASE(listctrl, ListCtrl, ctrl)
	WX_GETDEFINE(style)
	ctrl->SetWindowStyleFlag(style);
}
