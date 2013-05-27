#include "DeltaWxMenuBar.h"
#include "DeltaWxWindow.h"
#include "DeltaWxMenu.h"
#include "DeltaWxMenuItem.h"
#include "DeltaWxFrame.h"
#include "DeltaWxAcceleratorTable.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(menubar, name)
#define WX_FUNC(name) WX_FUNC1(menubar, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(append)
WX_FUNC_DEF(check)
WX_FUNC_DEF(enable)
WX_FUNC_DEF(enabletop)
WX_FUNC_DEF(findmenu)
WX_FUNC_DEF(findmenuitem)
WX_FUNC_DEF(finditem)
WX_FUNC_DEF(gethelpstring)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(getlabeltop)
WX_FUNC_DEF(getmenu)
WX_FUNC_DEF(getmenucount)
WX_FUNC_DEF(insert)
WX_FUNC_DEF(ischecked)
WX_FUNC_DEF(isenabled)
WX_FUNC_DEF(refresh)
WX_FUNC_DEF(remove)
WX_FUNC_DEF(replace)
WX_FUNC_DEF(sethelpstring)
WX_FUNC_DEF(setlabel)
WX_FUNC_DEF(setlabeltop)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(append),
	WX_FUNC(check),
	WX_FUNC(enable),
	WX_FUNC(enabletop),
	WX_FUNC(findmenu),
	WX_FUNC(findmenuitem),
	WX_FUNC(finditem),
	WX_FUNC(gethelpstring),
	WX_FUNC(getlabel),
	WX_FUNC(getlabeltop),
	WX_FUNC(getmenu),
	WX_FUNC(getmenucount),
	WX_FUNC(insert),
	WX_FUNC(ischecked),
	WX_FUNC(isenabled),
	WX_FUNC(refresh),
	WX_FUNC(remove),
	WX_FUNC(replace),
	WX_FUNC(sethelpstring),
	WX_FUNC(setlabel),
	WX_FUNC(setlabeltop),
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setlabeltop")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MenuBar, "menubar", Window)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MenuBar, val, menubar) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxWindow *_parent = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxWindow *parent = DNEWCLASS(DeltaWxWindow, (_parent));
	WX_SETOBJECT_EX(*at, Window, parent)
	return true;
}

static bool GetMenus (void* val, DeltaValue* at) 
{
	wxMenuBar *mbar = DLIB_WXTYPECAST_BASE(MenuBar, val, menubar);
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, n = (int)mbar->GetMenuCount(); i < n; ++i) {
		DeltaValue value;
		wxMenu *menu = mbar->GetMenu(i);
		DeltaWxMenu *retval = menu ? DNEWCLASS(DeltaWxMenu, (menu)) : (DeltaWxMenu*) 0;
		WX_SETOBJECT_EX(value, Menu, retval)
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static bool GetMenuBarFrame (void* val, DeltaValue* at) 
{
	wxMenuBar *mbar = DLIB_WXTYPECAST_BASE(MenuBar, val, menubar);
	wxFrame *frame = mbar->GetFrame();
	DeltaWxFrame *retval = frame ? DNEWCLASS(DeltaWxFrame, (frame)) : (DeltaWxFrame*) 0;
	WX_SETOBJECT_EX(*at, Frame, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Window",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "menus",				&GetMenus,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "menuBarFrame",		&GetMenuBarFrame,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(MenuBar,menubar)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(menubar_construct, 0, 4, Nil)
	wxMenuBar *wxmbar = (wxMenuBar*) 0;
	DeltaWxMenuBar *mbar = (DeltaWxMenuBar*) 0;
	if (n == 0)
		wxmbar = new wxMenuBar();
	else if (n == 1) {
		WX_GETDEFINE(style)
		wxmbar = new wxMenuBar(style);
	} else if (n >= 3) {
		WX_GETNUMBER(size)
		WX_GETTABLE(menus_table)
		wxMenu **menus = DNEWARR(wxMenu*, menus_table->Total());
		for (int i = 0; i < (int)menus_table->Total(); ++i) {
			DeltaValue value;
			menus_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
			if (value.Type() == DeltaValue_ExternId) {
				util_ui32 serial_no = (util_ui32)value.ToExternId();
				if (DLIB_WXISBASE(Menu, serial_no, menu, menu_wr)) {
					wxMenu *menu = (wxMenu*) menu_wr->GetCastToNativeInstance();
					menus[i] = menu;
				}
			}
		}
		WX_GETTABLE(titles_table)
		wxString *titles = DNEWARR(wxString, titles_table->Total());
		for (int i = 0; i < (int)titles_table->Total(); ++i) {
			DeltaValue value;
			titles_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
			if (value.Type() == DeltaValue_String) {
				wxString title = wxString(value.ToString().c_str(), wxConvUTF8);
				titles[i] = title;
			}
		}
		long style = 0;
		if (n >= 4) { WX_GETDEFINE_DEFINED(style) }
		wxmbar = new wxMenuBar(size, menus, titles, style);
		DDELARR(menus);
		DDELARR(titles);
	}
	if (wxmbar) mbar = DNEWCLASS(DeltaWxMenuBar, (wxmbar));
	WX_SETOBJECT(MenuBar, mbar)
}

DLIB_FUNC_START(menubar_destruct, 1, Nil)
	DLIB_WXDELETE(menubar, MenuBar, mbar)
}

DLIB_FUNC_START(menubar_append, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETSTRING(title)
	WX_SETBOOL(mbar->Append(menu, title))
}

DLIB_FUNC_START(menubar_check, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_GETBOOL(check)
	mbar->Check(id, check);
}

DLIB_FUNC_START(menubar_enable, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_GETBOOL(enable)
	mbar->Enable(id, enable);
}

DLIB_FUNC_START(menubar_enabletop, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	WX_GETBOOL(enable)
	mbar->EnableTop(pos, enable);
}

DLIB_FUNC_START(menubar_findmenu, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETSTRING(title)
	WX_SETNUMBER(mbar->FindMenu(title));
}

DLIB_FUNC_START(menubar_findmenuitem, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETSTRING(menustring)
	WX_GETSTRING(itemstring)
	WX_SETNUMBER(mbar->FindMenuItem(menustring, itemstring));
}

WX_FUNC_ARGRANGE_START(menubar_finditem, 2, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	if (n == 2) {
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, mbar->FindItem(id));
	} else {
		WX_GETTABLE(menu_table)
		wxMenu *arg;
		retval = DNEWCLASS(DeltaWxMenuItem, (mbar->FindItem(id, &arg)));
		WXNEWCLASS(DeltaWxMenu, menu, wxMenu, arg);
		WX_SETTABLE_RETVAL(menu_table, menu)
	}
	WX_SETOBJECT(MenuItem, retval)
}

DLIB_FUNC_START(menubar_gethelpstring, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_SETSTRING(mbar->GetHelpString(id))
}

DLIB_FUNC_START(menubar_getlabel, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_SETSTRING(mbar->GetLabel(id))
}

DLIB_FUNC_START(menubar_getlabeltop, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	WX_SETSTRING(mbar->GetLabelTop(pos))
}

DLIB_FUNC_START(menubar_getmenu, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(menuIndex)
	WXNEWCLASS(DeltaWxMenu, retval, wxMenu, mbar->GetMenu(menuIndex));
	WX_SETOBJECT(Menu, retval)
}

DLIB_FUNC_START(menubar_getmenucount, 1, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_SETNUMBER(mbar->GetMenuCount())
}

DLIB_FUNC_START(menubar_insert, 4, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETSTRING(title)
	WX_SETBOOL(mbar->Insert(pos, menu, title))
}

DLIB_FUNC_START(menubar_ischecked, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_SETBOOL(mbar->IsChecked(id))
}

DLIB_FUNC_START(menubar_isenabled, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_SETBOOL(mbar->IsEnabled(id))
}

DLIB_FUNC_START(menubar_refresh, 1, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	mbar->Refresh();
}

DLIB_FUNC_START(menubar_remove, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	WXNEWCLASS(DeltaWxMenu, retval, wxMenu, mbar->Remove(pos));
	WX_SETOBJECT(Menu, retval)
}

DLIB_FUNC_START(menubar_replace, 4, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETSTRING(title)
	WXNEWCLASS(DeltaWxMenu, retval, wxMenu, mbar->Replace(pos, menu, title));
	WX_SETOBJECT(Menu, retval)
}

DLIB_FUNC_START(menubar_sethelpstring, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_GETSTRING(helpstring)
	mbar->SetHelpString(id, helpstring);
}

DLIB_FUNC_START(menubar_setlabel, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_GETSTRING(label)
	mbar->SetLabel(id, label);
}

DLIB_FUNC_START(menubar_setlabeltop, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	WX_GETSTRING(label)
	mbar->SetLabelTop(pos, label);
}
