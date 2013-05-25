#include "DeltaWxMenuItem.h"
#include "DeltaWxObject.h"
#include "DeltaWxMenu.h"
#include "DeltaWxColour.h"
#include "DeltaWxFont.h"
#include "DeltaWxBitmap.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(menuitem, name)
#define WX_FUNC(name) WX_FUNC1(menuitem, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(check)
WX_FUNC_DEF(enable)
WX_FUNC_DEF(getbackgroundcolour)
WX_FUNC_DEF(getfont)
WX_FUNC_DEF(getmarginwidth)
WX_FUNC_DEF(gettextcolour)
WX_FUNC_DEF(setbackgroundcolour)
WX_FUNC_DEF(setfont)
WX_FUNC_DEF(setmarginwidth)
WX_FUNC_DEF(settextcolour)
WX_FUNC_DEF(setbitmap)
WX_FUNC_DEF(setbitmaps)
WX_FUNC_DEF(gethelp)
WX_FUNC_DEF(getkind)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(getlabelfromtext)
WX_FUNC_DEF(getmenu)
WX_FUNC_DEF(getname)
WX_FUNC_DEF(gettext)
WX_FUNC_DEF(getsubmenu)
WX_FUNC_DEF(ischeckable)
WX_FUNC_DEF(ischecked)
WX_FUNC_DEF(isenabled)
WX_FUNC_DEF(isseparator)
WX_FUNC_DEF(issubmenu)
WX_FUNC_DEF(sethelp)
WX_FUNC_DEF(setmenu)
WX_FUNC_DEF(setsubmenu)
WX_FUNC_DEF(settext)
WX_FUNC_DEF(getid)
WX_FUNC_DEF(setid)
WX_FUNC_DEF(setkind)
WX_FUNC_DEF(setcheckable)
WX_FUNC_DEF(toggle)
WX_FUNC_DEF(setname)
WX_FUNC_DEF(setitemlabel)
WX_FUNC_DEF(getitemlabeltext)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getlabelfromtext),
	WX_FUNC(destruct),
	WX_FUNC(check),
	WX_FUNC(enable),
	WX_FUNC(getbackgroundcolour),
	WX_FUNC(getfont),
	WX_FUNC(getmarginwidth),
	WX_FUNC(gettextcolour),
	WX_FUNC(gethelp),
	WX_FUNC(getid),
	WX_FUNC(getkind),
	WX_FUNC(getlabel),
	WX_FUNC(getmenu),
	WX_FUNC(getname),
	WX_FUNC(gettext),
	WX_FUNC(getsubmenu),
	WX_FUNC(ischeckable),
	WX_FUNC(ischecked),
	WX_FUNC(isenabled),
	WX_FUNC(isseparator),
	WX_FUNC(issubmenu),
	WX_FUNC(setbackgroundcolour),
	WX_FUNC(setfont),
	WX_FUNC(setmarginwidth),
	WX_FUNC(settextcolour),
	WX_FUNC(setbitmap),
	WX_FUNC(setbitmaps),
	WX_FUNC(sethelp),
	WX_FUNC(setmenu),
	WX_FUNC(setsubmenu),
	WX_FUNC(settext),
	WX_FUNC(setid),
	WX_FUNC(setkind),
	WX_FUNC(setcheckable),
	WX_FUNC(toggle),
	WX_FUNC(setname),
	WX_FUNC(setitemlabel),
	WX_FUNC(getitemlabeltext)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(2, uarraysize(funcs) - 2, "destruct", "getitemlabeltext")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MenuItem, "menuitem", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MenuItem, val, menuitem) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetId (void* val, DeltaValue* at) 
{
	wxMenuItem *mitem = DLIB_WXTYPECAST_BASE(MenuItem, val, menuitem);
	WX_SETNUMBER_EX(*at, mitem->GetId())
	return true;
}

static bool GetParentMenu (void* val, DeltaValue* at) 
{
	wxMenuItem *mitem = DLIB_WXTYPECAST_BASE(MenuItem, val, menuitem);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Menu, mitem->GetMenu())
	return true;
}

static bool GetSubMenu (void* val, DeltaValue* at) 
{
	wxMenuItem *mitem = DLIB_WXTYPECAST_BASE(MenuItem, val, menuitem);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Menu, mitem->GetSubMenu())
	return true;
}

static bool GetText (void* val, DeltaValue* at) 
{
	wxMenuItem *mitem = DLIB_WXTYPECAST_BASE(MenuItem, val, menuitem);
	WX_SETSTRING_EX(*at, mitem->GetText())
	return true;
}

static bool GetHelp (void* val, DeltaValue* at) 
{
	wxMenuItem *mitem = DLIB_WXTYPECAST_BASE(MenuItem, val, menuitem);
	WX_SETSTRING_EX(*at, mitem->GetHelp())
	return true;
}

static bool GetKind (void* val, DeltaValue* at) 
{
	wxMenuItem *mitem = DLIB_WXTYPECAST_BASE(MenuItem, val, menuitem);
	WX_SETNUMBER_EX(*at, mitem->GetKind())
	return true;
}

static bool GetIsChecked (void* val, DeltaValue* at) 
{
	wxMenuItem *mitem = DLIB_WXTYPECAST_BASE(MenuItem, val, menuitem);
	WX_SETBOOL_EX(*at, mitem->IsChecked())
	return true;
}

static bool GetIsEnabled (void* val, DeltaValue* at) 
{
	wxMenuItem *mitem = DLIB_WXTYPECAST_BASE(MenuItem, val, menuitem);
	WX_SETBOOL_EX(*at, mitem->IsEnabled())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "id",					&GetId,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "parentMenu",			&GetParentMenu,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "subMenu",			&GetSubMenu,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "text",				&GetText,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "help",				&GetHelp,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "kind",				&GetKind,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isChecked",			&GetIsChecked,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isEnabled",			&GetIsEnabled,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(MenuItem,menuitem)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(menuitem_construct, 0, 6, Nil)
	wxMenuItem *mitem = (wxMenuItem*) 0;
	if (n == 0)
		mitem = new wxMenuItem();
	else {
		wxMenu *parentMenu = (wxMenu*) NULL;
		int id = wxID_SEPARATOR;
		wxString name = wxEmptyString;
		wxString help = wxEmptyString;
		wxItemKind kind = wxITEM_NORMAL;
		wxMenu *subMenu = (wxMenu*) NULL;
		if (n >= 1) { DLIB_WXGET_BASE(menu, Menu, menu) parentMenu = menu; }
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { WX_GETSTRING_DEFINED(name) }
		if (n >= 4) { WX_GETSTRING_DEFINED(help) }
		if (n >= 5) { WX_GETDEFINE(_kind) kind = (wxItemKind)_kind; }
		if (n >= 6) { DLIB_WXGET_BASE(menu, Menu, menu) subMenu = menu; }
		mitem = new wxMenuItem(parentMenu, id, name, help, (wxItemKind)kind, subMenu);
	}
	WX_SETOBJECT(MenuItem, mitem)
}

WX_FUNC_START(menuitem_destruct, 1, Nil)
	DLIB_WXDELETE(menuitem, MenuItem, mitem)
}

WX_FUNC_ARGRANGE_START(menuitem_check, 1, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	if (n == 1) {
		mitem->Check();
	} else {
		WX_GETBOOL(check)
		mitem->Check(check);
	}
}

WX_FUNC_ARGRANGE_START(menuitem_enable, 1, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	if (n == 1) {
		mitem->Enable();
	} else {
		WX_GETBOOL(enable)
		mitem->Enable(enable);
	}
}

WX_FUNC_START(menuitem_getbackgroundcolour, 1, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(mitem->GetBackgroundColour()))
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(menuitem_getfont, 1, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(mitem->GetFont()))
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(menuitem_getmarginwidth, 1, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETNUMBER(mitem->GetMarginWidth())
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(menuitem_gettextcolour, 1, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(mitem->GetTextColour()))
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(menuitem_gethelp, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETSTRING(mitem->GetHelp())
}

WX_FUNC_START(menuitem_getid, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETNUMBER(mitem->GetId())
}

WX_FUNC_START(menuitem_getkind, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETNUMBER(mitem->GetKind())
}

WX_FUNC_START(menuitem_getlabel, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETSTRING(mitem->GetLabel())
}

WX_FUNC_START(menuitem_getlabelfromtext, 1, Nil)
	WX_GETSTRING(text)
	WX_SETSTRING(wxMenuItem::GetLabelFromText(text))
}

WX_FUNC_START(menuitem_getmenu, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	wxMenu* retval	= mitem->GetMenu();;
	WX_SETOBJECT(Menu, retval)
}

WX_FUNC_START(menuitem_getname, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETSTRING(mitem->wxMenuItemBase::GetName())
}

WX_FUNC_START(menuitem_gettext, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETSTRING(mitem->GetText())
}

WX_FUNC_START(menuitem_getsubmenu, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	wxMenu* retval	= mitem->GetSubMenu();;
	WX_SETOBJECT(Menu, retval)
}

WX_FUNC_START(menuitem_ischeckable, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETBOOL(mitem->IsCheckable())
}

WX_FUNC_START(menuitem_ischecked, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETBOOL(mitem->IsChecked())
}

WX_FUNC_START(menuitem_isenabled, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETBOOL(mitem->IsEnabled())
}

WX_FUNC_START(menuitem_isseparator, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETBOOL(mitem->IsSeparator())
}

WX_FUNC_START(menuitem_issubmenu, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETBOOL(mitem->IsSubMenu())
}

WX_FUNC_START(menuitem_setbackgroundcolour, 2, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	DLIB_WXGET_BASE(colour, Colour, colour)
	mitem->SetBackgroundColour(*colour);
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(menuitem_setfont, 2, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	DLIB_WXGET_BASE(font, Font, font)
	mitem->SetFont(*font);
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(menuitem_setmarginwidth, 2, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_GETNUMBER(width)
	mitem->SetMarginWidth(width);
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(menuitem_settextcolour, 2, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	DLIB_WXGET_BASE(colour, Colour, colour)
	mitem->SetTextColour(*colour);
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(menuitem_setbitmap, 2, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
	mitem->SetBitmap(*bmp);
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_ARGRANGE_START(menuitem_setbitmaps, 2, 3, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	DLIB_WXGET_BASE(bitmap, Bitmap, checked)
	wxBitmap *unchecked = &wxNullBitmap;
	if (n >= 3) { DLIB_WXGET_BASE(bitmap, Bitmap, _unchecked) unchecked = _unchecked; }
	mitem->SetBitmaps(*checked, *unchecked);
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(menuitem_sethelp, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_GETSTRING(helpString)
	mitem->SetHelp(helpString);
}

WX_FUNC_START(menuitem_setmenu, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	DLIB_WXGET_BASE(menu, Menu, menu)
	mitem->SetMenu(menu);
}

WX_FUNC_START(menuitem_setsubmenu, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	DLIB_WXGET_BASE(menu, Menu, menu)
	mitem->SetSubMenu(menu);
}

WX_FUNC_START(menuitem_settext, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_GETSTRING(text)
	mitem->SetText(text);
}

WX_FUNC_START(menuitem_setid, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_GETDEFINE(id)
	mitem->SetId(id);
}

WX_FUNC_START(menuitem_setkind, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_GETDEFINE(kind)
	mitem->SetKind((wxItemKind)kind);
}

WX_FUNC_START(menuitem_setcheckable, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_GETBOOL(checkable)
	mitem->SetCheckable(checkable);
}

WX_FUNC_START(menuitem_toggle, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	mitem->Toggle();
}

WX_FUNC_START(menuitem_setname, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_GETSTRING(name)
	mitem->wxMenuItemBase::SetName(name);
}

WX_FUNC_START(menuitem_setitemlabel, 2, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_GETSTRING(str)
	mitem->SetItemLabel(str);
}

WX_FUNC_START(menuitem_getitemlabeltext, 1, Nil)
	DLIB_WXGET_BASE(menuitem, MenuItem, mitem)
	WX_SETSTRING(mitem->GetItemLabelText())
}
