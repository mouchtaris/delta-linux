#include "DeltaWxMenu.h"
#include "DeltaWxMenuItem.h"
#include "DeltaWxMenuBar.h"
#include "DeltaWxEvtHandler.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(menu, name)
#define WX_FUNC(name) WX_FUNC1(menu, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(append)
WX_FUNC_DEF(appendcheckitem)
WX_FUNC_DEF(appendradioitem)
WX_FUNC_DEF(appendseparator)
WX_FUNC_DEF(appendsubmenu)
WX_FUNC_DEF(break)
WX_FUNC_DEF(check)
WX_FUNC_DEF(delete)
WX_FUNC_DEF(destroy)
WX_FUNC_DEF(enable)
WX_FUNC_DEF(finditem)
WX_FUNC_DEF(finditembyposition)
WX_FUNC_DEF(gethelpstring)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(getmenuitemcount)
WX_FUNC_DEF(getmenuitems)
WX_FUNC_DEF(gettitle)
WX_FUNC_DEF(insert)
WX_FUNC_DEF(insertcheckitem)
WX_FUNC_DEF(insertradioitem)
WX_FUNC_DEF(insertseparator)
WX_FUNC_DEF(ischecked)
WX_FUNC_DEF(isenabled)
WX_FUNC_DEF(prepend)
WX_FUNC_DEF(prependcheckitem)
WX_FUNC_DEF(prependradioitem)
WX_FUNC_DEF(prependseparator)
WX_FUNC_DEF(remove)
WX_FUNC_DEF(sethelpstring)
WX_FUNC_DEF(setlabel)
WX_FUNC_DEF(settitle)
WX_FUNC_DEF(updateui)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(append),
	WX_FUNC(appendcheckitem),
	WX_FUNC(appendradioitem),
	WX_FUNC(appendseparator),
	WX_FUNC(appendsubmenu),
	WX_FUNC(break),
	WX_FUNC(check),
	WX_FUNC(delete),
	WX_FUNC(destroy),
	WX_FUNC(enable),
	WX_FUNC(finditem),
	WX_FUNC(finditembyposition),
	WX_FUNC(gethelpstring),
	WX_FUNC(getlabel),
	WX_FUNC(getmenuitemcount),
	WX_FUNC(getmenuitems),
	WX_FUNC(gettitle),
	WX_FUNC(insert),
	WX_FUNC(insertcheckitem),
	WX_FUNC(insertradioitem),
	WX_FUNC(insertseparator),
	WX_FUNC(ischecked),
	WX_FUNC(isenabled),
	WX_FUNC(prepend),
	WX_FUNC(prependcheckitem),
	WX_FUNC(prependradioitem),
	WX_FUNC(prependseparator),
	WX_FUNC(remove),
	WX_FUNC(sethelpstring),
	WX_FUNC(setlabel),
	WX_FUNC(settitle),
	WX_FUNC(updateui)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "updateui")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Menu, "menu", EvtHandler)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Menu, val, menu) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvtHandler *_parent = DLIB_WXTYPECAST_BASE(EvtHandler, val, evthandler);
	DeltaWxEvtHandler *parent = DNEWCLASS(DeltaWxEvtHandler, (_parent));
	WX_SETOBJECT_EX(*at, EvtHandler, parent)
	return true;
}

static bool GetMenuBar (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	wxMenuBar *menubar = menu->GetMenuBar();
	DeltaWxMenuBar *retval = menubar ? DNEWCLASS(DeltaWxMenuBar, (menubar)) : (DeltaWxMenuBar*) 0;
	WX_SETOBJECT_EX(*at, MenuBar, retval)
	return true;
}

static bool GetParent (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	wxMenu *parent = menu->GetParent();
	DeltaWxMenu *retval = parent ? DNEWCLASS(DeltaWxMenu, (parent)) : (DeltaWxMenu*) 0;
	WX_SETOBJECT_EX(*at, Menu, retval)
	return true;
}

static bool GetTitle (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	WX_SETSTRING_EX(*at, menu->GetTitle())
	return true;
}

static bool GetItems (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	wxMenuItemList itemlist = menu->GetMenuItems();
	DeltaList_MakeForDebuggerOnly(*at);
	std::list<DeltaValue>* list = DeltaList_Get(*at);
	for (wxMenuItemList::iterator it = itemlist.begin(); it != itemlist.end(); ++it) {
		DeltaValue val;
		wxMenuItem *item = *it;
		DeltaWxMenuItem *dmenuitem = DNEWCLASS(DeltaWxMenuItem, (item));
		WX_SETOBJECT_EX(val, MenuItem, dmenuitem)
		list->push_back(val);
	}
	return true;
}

static bool GetInvokingWindow (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	wxWindow *win = menu->GetInvokingWindow();
	DeltaWxWindow *retval = win ? DNEWCLASS(DeltaWxWindow, (win)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
	return true;
}

static bool GetStyle (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	WX_SETNUMBER_EX(*at, menu->GetStyle())
	return true;
}

static bool GetEventHandler (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	wxEvtHandler *handler = menu->GetEventHandler();
	DeltaWxEvtHandler *retval = handler ? DNEWCLASS(DeltaWxEvtHandler, (handler)) : (DeltaWxEvtHandler*) 0;
	WX_SETOBJECT_EX(*at, EvtHandler, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "EvtHandler",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "menuBar",			&GetMenuBar,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "menuParent",			&GetParent,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "title",				&GetTitle,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "items",				&GetItems,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "invokingWindow",		&GetInvokingWindow,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "style",				&GetStyle,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "eventHandler",		&GetEventHandler,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Menu, menu);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(menu_construct, 0, 2, Nil)
	wxMenu *wxmenu = (wxMenu*) 0;
	DeltaWxMenu *menu = (DeltaWxMenu*) 0;
	if (n == 0)
		wxmenu = new wxMenu();
	else if (n == 1) {
		WX_GETSTRING(title)
		wxmenu = new wxMenu(title);
	} else {
		WX_GETSTRING(title)
		WX_GETDEFINE(style)
		wxmenu = new wxMenu(title, style);
	}
	if (wxmenu) menu = DNEWCLASS(DeltaWxMenu, (wxmenu));
	WX_SETOBJECT(Menu, menu)
}

DLIB_FUNC_START(menu_destruct, 1, Nil)
	DLIB_WXDELETE(menu, Menu, menu)
}

WX_FUNC_ARGRANGE_START(menu_append, 2, 5, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	if (n == 2 && (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId)) {
		DLIB_WXGET_BASE(menuitem, MenuItem, menuItem)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Append(menuItem));
	} else {
		WX_GETDEFINE(id)
		if (n == 2) {
			WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Append(id));
		} else if (n == 3) {
			WX_GETSTRING(item)
			WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Append(id, item));
		} else if (n == 4) {
			WX_GETSTRING(item)
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
				DLIB_WXGET_BASE(menu, Menu, submenu)
				WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Append(id, item, submenu));
			} else
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
				WX_GETSTRING(helpString)
				WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Append(id, item, helpString));
			}
		} else {
			WX_GETSTRING(item)
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
				DLIB_WXGET_BASE(menu, Menu, submenu)
				WX_GETSTRING(helpString)
				WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Append(id, item, submenu, helpString));
			} else
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
				WX_GETSTRING(helpString)
				WX_GETDEFINE(kind)
				WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Append(id, item, helpString, (wxItemKind)kind));
			}
		}
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_appendcheckitem, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 3) {
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->AppendCheckItem(id, item));
	} else {
		WX_GETSTRING(helpString)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->AppendCheckItem(id, item, helpString));
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_appendradioitem, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval;
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 3) {
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->AppendRadioItem(id, item));
	} else {
		WX_GETSTRING(helpString)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->AppendRadioItem(id, item, helpString));
	}
}

DLIB_FUNC_START(menu_appendseparator, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WXNEWCLASS(DeltaWxMenuItem, retval, wxMenuItem, menu->AppendSeparator());
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_appendsubmenu, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	DLIB_WXGET_BASE(menu, Menu, submenu)
	WX_GETSTRING(text)
	if (n == 3) {
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->AppendSubMenu(submenu, text));
	} else {
		WX_GETSTRING(help)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->AppendSubMenu(submenu, text, help));
	}
	WX_SETOBJECT(MenuItem, retval)
}

DLIB_FUNC_START(menu_break, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	menu->Break();
}

DLIB_FUNC_START(menu_check, 3, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_GETBOOL(check)
	menu->Check(id, check);
}

DLIB_FUNC_START(menu_delete, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		menu->Delete(item);
	} else {
		WX_GETDEFINE(id)
		menu->Delete(id);
	}
}

DLIB_FUNC_START(menu_destroy, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		menu->Destroy(item);
	} else {
		WX_GETDEFINE(id)
		menu->Destroy(id);
	}
}

DLIB_FUNC_START(menu_enable, 3, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_GETBOOL(enable)
	menu->Enable(id, enable);
}

WX_FUNC_ARGRANGE_START(menu_finditem, 2, 3, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	if (n == 2 && (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String)) {
		WX_GETSTRING(itemString)
		WX_SETNUMBER(menu->FindItem(itemString))
	} else {
		WX_GETDEFINE(id)
		DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
		if (n == 2) {
			WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->FindItem(id));
		} else {
			WX_GETTABLE(menu_table)
			wxMenu *arg;
			retval = DNEWCLASS(DeltaWxMenuItem, (menu->FindItem(id, &arg)));
			WXNEWCLASS(DeltaWxMenu, menu, wxMenu, arg)
			WX_SETTABLE_RETVAL(menu_table, menu)
		}
		WX_SETOBJECT(MenuItem, retval)
	}
}

DLIB_FUNC_START(menu_finditembyposition, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETNUMBER(position)
	WXNEWCLASS(DeltaWxMenuItem, retval, wxMenuItem, menu->FindItemByPosition(position));
	WX_SETOBJECT(MenuItem, retval)
}

DLIB_FUNC_START(menu_gethelpstring, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_SETSTRING(menu->GetHelpString(id))
}

DLIB_FUNC_START(menu_getlabel, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_SETSTRING(menu->GetLabel(id))
}

DLIB_FUNC_START(menu_getmenuitemcount, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_SETNUMBER(menu->GetMenuItemCount())
}

DLIB_FUNC_START(menu_getmenuitems, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItemList itemlist = menu->GetMenuItems();
	DeltaList_Make(DLIB_RETVAL_REF);
	std::list<DeltaValue>* list = DeltaList_Get(DLIB_RETVAL_REF);
	for (wxMenuItemList::iterator it = itemlist.begin(); it != itemlist.end(); ++it) {
		DeltaValue val;
		wxMenuItem *item = *it;
		DeltaWxMenuItem *dmenuitem = DNEWCLASS(DeltaWxMenuItem, (item));
		WX_SETOBJECT_EX(val, MenuItem, dmenuitem)
		list->push_back(val);
	}
}

DLIB_FUNC_START(menu_gettitle, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_SETSTRING(menu->GetTitle())
}

WX_FUNC_ARGRANGE_START(menu_insert, 3, 6, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	WX_GETNUMBER(pos)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId && n == 3) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Insert(pos, item));
	} else if (n == 3) {
		WX_GETDEFINE(id)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Insert(pos, id));
	} else if (n == 4) {
		WX_GETDEFINE(id)
		WX_GETSTRING(item)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Insert(pos, id, item));
	} else if (n == 5) {
		WX_GETDEFINE(id)
		WX_GETSTRING(item)
		WX_GETSTRING(helpString)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Insert(pos, id, item, helpString));
	} else {
		WX_GETDEFINE(id)
		WX_GETSTRING(item)
		WX_GETSTRING(helpString)
		WX_GETDEFINE(kind)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Insert(pos, id, item, helpString, (wxItemKind)kind));
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_insertcheckitem, 4, 5, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	WX_GETNUMBER(pos)
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 4) {
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->InsertCheckItem(pos, id, item));
	} else {
		WX_GETSTRING(helpString)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->InsertCheckItem(pos, id, item, helpString));
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_insertradioitem, 4, 5, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	WX_GETNUMBER(pos)
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 4) {
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->InsertRadioItem(pos, id, item));
	} else {
		WX_GETSTRING(helpString)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->InsertRadioItem(pos, id, item, helpString));
	}
	WX_SETOBJECT(MenuItem, retval)
}

DLIB_FUNC_START(menu_insertseparator, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETNUMBER(pos)
	WXNEWCLASS(DeltaWxMenuItem, retval, wxMenuItem, menu->InsertSeparator(pos));
	WX_SETOBJECT(MenuItem, retval)
}

DLIB_FUNC_START(menu_ischecked, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_SETBOOL(menu->IsChecked(id))
}

DLIB_FUNC_START(menu_isenabled, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_SETBOOL(menu->IsEnabled(id))
}

WX_FUNC_ARGRANGE_START(menu_prepend, 2, 5, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	if (n == 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Prepend(item));
	} else {
		WX_GETDEFINE(id)
		if (n == 2) {
			WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Prepend(id));
		} else if (n == 3) {
			WX_GETSTRING(item)
			WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Prepend(id, item));
		} else if (n == 4) {
			WX_GETSTRING(item)
			WX_GETSTRING(helpString)
			WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Prepend(id, item, helpString));
		} else {
			WX_GETSTRING(item)
			WX_GETSTRING(helpString)
			WX_GETDEFINE(kind)
			WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Prepend(id, item, helpString, (wxItemKind)kind));
		}
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_prependcheckitem, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 3) {
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->PrependCheckItem(id, item));
	} else {
		WX_GETSTRING(helpString)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->PrependCheckItem(id, item, helpString));
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_prependradioitem, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 3) {
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->PrependRadioItem(id, item));
	} else {
		WX_GETSTRING(helpString)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->PrependRadioItem(id, item, helpString));
	}
	WX_SETOBJECT(MenuItem, retval)
}

DLIB_FUNC_START(menu_prependseparator, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WXNEWCLASS(DeltaWxMenuItem, retval, wxMenuItem, menu->PrependSeparator());
	WX_SETOBJECT(MenuItem, retval)
}

DLIB_FUNC_START(menu_remove, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	DeltaWxMenuItem *retval = (DeltaWxMenuItem*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Remove(item));
	} else {
		WX_GETDEFINE(id)
		WXNEWCLASS_DEFINED(DeltaWxMenuItem, retval, wxMenuItem, menu->Remove(id));
	}
	WX_SETOBJECT(MenuItem, retval)
}

DLIB_FUNC_START(menu_sethelpstring, 3, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_GETSTRING(helpString)
	menu->SetHelpString(id, helpString);
}

DLIB_FUNC_START(menu_setlabel, 3, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_GETSTRING(label)
	menu->SetLabel(id, label);
}

DLIB_FUNC_START(menu_settitle, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETSTRING(title)
	menu->SetTitle(title);
}

WX_FUNC_ARGRANGE_START(menu_updateui, 1, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxEvtHandler* source = (wxEvtHandler*)NULL;
	if (n >= 2) { DLIB_WXGET_BASE(evthandler, EvtHandler, handler) source = handler; }
	menu->UpdateUI(source);
}
