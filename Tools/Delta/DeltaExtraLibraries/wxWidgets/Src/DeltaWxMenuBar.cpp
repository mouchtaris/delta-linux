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

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "append", "setlabeltop")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MenuBar, "menubar", Window)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MenuBar, val, menubar) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Window, val)
	return true;
}

static bool GetMenus (void* val, DeltaValue* at) 
{
	wxMenuBar *mbar = DLIB_WXTYPECAST_BASE(MenuBar, val, menubar);
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, n = (int)mbar->GetMenuCount(); i < n; ++i) {
		DeltaValue value;
		wxMenu *menu = mbar->GetMenu(i);
		WX_SETOBJECT_NO_CONTEXT_EX(value, Menu, menu)
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static bool GetMenuBarFrame (void* val, DeltaValue* at) 
{
	wxMenuBar *mbar = DLIB_WXTYPECAST_BASE(MenuBar, val, menubar);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Frame, mbar->GetFrame())
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
	wxMenuBar *mbar = (wxMenuBar*) 0;
	if (n == 0)
		mbar = new wxMenuBar();
	else if (n == 1) {
		WX_GETDEFINE(style)
		mbar = new wxMenuBar(style);
	} else if (n >= 3) {
		WX_GETNUMBER(size)
		WX_GETTABLE(menus_table)
		wxMenu **menus = DNEWARR(wxMenu*, menus_table->Total());
		for (int i = 0; i < (int)menus_table->Total(); ++i) {
			DeltaValue value;
			menus_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
			if (value.Type() == DeltaValue_ExternId) {
				util_ui32 serial_no = (util_ui32)value.ToExternId();
				if (DLIB_WXISBASE(Menu, serial_no, menu, menu)) {
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
		mbar = new wxMenuBar(size, menus, titles, style);
		DDELARR(menus);
		DDELARR(titles);
	}
	WX_SET_WINDOW_OBJECT(MenuBar, mbar)
}

WX_FUNC_START(menubar_append, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETSTRING(title)
	WX_SETBOOL(mbar->Append(menu, title))
}

WX_FUNC_START(menubar_check, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_GETBOOL(check)
	mbar->Check(id, check);
}

WX_FUNC_START(menubar_enable, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_GETBOOL(enable)
	mbar->Enable(id, enable);
}

WX_FUNC_START(menubar_enabletop, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	WX_GETBOOL(enable)
	mbar->EnableTop(pos, enable);
}

WX_FUNC_START(menubar_findmenu, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETSTRING(title)
	WX_SETNUMBER(mbar->FindMenu(title));
}

WX_FUNC_START(menubar_findmenuitem, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETSTRING(menustring)
	WX_GETSTRING(itemstring)
	WX_SETNUMBER(mbar->FindMenuItem(menustring, itemstring));
}

WX_FUNC_ARGRANGE_START(menubar_finditem, 2, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	wxMenuItem *retval = (wxMenuItem*) 0;
	if (n == 2) {
		retval	= mbar->FindItem(id);;
	} else {
		WX_GETTABLE(menu_table)
		wxMenu *arg;
		retval = mbar->FindItem(id, &arg);
		DeltaValue menu;
		WX_SETOBJECT_EX(menu, Menu, arg)
		WX_SETTABLE_RETVAL(menu_table, menu)
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_START(menubar_gethelpstring, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_SETSTRING(mbar->GetHelpString(id))
}

WX_FUNC_START(menubar_getlabel, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_SETSTRING(mbar->GetLabel(id))
}

WX_FUNC_START(menubar_getlabeltop, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	WX_SETSTRING(mbar->GetLabelTop(pos))
}

WX_FUNC_START(menubar_getmenu, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(menuIndex)
	wxMenu* retval	= mbar->GetMenu(menuIndex);;
	WX_SETOBJECT(Menu, retval)
}

WX_FUNC_START(menubar_getmenucount, 1, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_SETNUMBER(mbar->GetMenuCount())
}

WX_FUNC_START(menubar_insert, 4, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETSTRING(title)
	WX_SETBOOL(mbar->Insert(pos, menu, title))
}

WX_FUNC_START(menubar_ischecked, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_SETBOOL(mbar->IsChecked(id))
}

WX_FUNC_START(menubar_isenabled, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_SETBOOL(mbar->IsEnabled(id))
}

WX_FUNC_START(menubar_refresh, 1, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	mbar->Refresh();
}

WX_FUNC_START(menubar_remove, 2, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	wxMenu* retval	= mbar->Remove(pos);;
	WX_SETOBJECT(Menu, retval)
}

WX_FUNC_START(menubar_replace, 4, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETSTRING(title)
	wxMenu* retval	= mbar->Replace(pos, menu, title);;
	WX_SETOBJECT(Menu, retval)
}

WX_FUNC_START(menubar_sethelpstring, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_GETSTRING(helpstring)
	mbar->SetHelpString(id, helpstring);
}

WX_FUNC_START(menubar_setlabel, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETDEFINE(id)
	WX_GETSTRING(label)
	mbar->SetLabel(id, label);
}

WX_FUNC_START(menubar_setlabeltop, 3, Nil)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	WX_GETNUMBER(pos)
	WX_GETSTRING(label)
	mbar->SetLabelTop(pos, label);
}
