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
	WX_SET_BASECLASS_GETTER(at, EvtHandler, val)
	return true;
}

static bool GetMenuBar (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, MenuBar, menu->GetMenuBar())
	return true;
}

static bool GetParent (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Menu, menu->GetParent())
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
		WX_SETOBJECT_NO_CONTEXT_EX(val, MenuItem, item)
		list->push_back(val);
	}
	return true;
}

static bool GetInvokingWindow (void* val, DeltaValue* at) 
{
	wxMenu *menu = DLIB_WXTYPECAST_BASE(Menu, val, menu);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, menu->GetInvokingWindow())
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
	WX_SETOBJECT_NO_CONTEXT_EX(*at, EvtHandler, menu->GetEventHandler())
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Menu,menu)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(menu_construct, 0, 2, Nil)
	wxMenu *menu = (wxMenu*) 0;
	if (n == 0)
		menu = new wxMenu();
	else if (n == 1) {
		WX_GETSTRING(title)
		menu = new wxMenu(title);
	} else {
		WX_GETSTRING(title)
		WX_GETDEFINE(style)
		menu = new wxMenu(title, style);
	}
	WX_SETOBJECT(Menu, menu)
}

WX_FUNC_START(menu_destruct, 1, Nil)
	DLIB_WXDELETE(menu, Menu, menu)
}

WX_FUNC_ARGRANGE_START(menu_append, 2, 5, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	if (n == 2 && (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId)) {
		DLIB_WXGET_BASE(menuitem, MenuItem, menuItem)
		retval	= menu->Append(menuItem);;
	} else {
		WX_GETDEFINE(id)
		if (n == 2) {
			retval	= menu->Append(id);;
		} else if (n == 3) {
			WX_GETSTRING(item)
			retval	= menu->Append(id, item);;
		} else if (n == 4) {
			WX_GETSTRING(item)
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
				DLIB_WXGET_BASE(menu, Menu, submenu)
				retval	= menu->Append(id, item, submenu);;
			} else
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
				WX_GETSTRING(helpString)
				retval	= menu->Append(id, item, helpString);;
			}
		} else {
			WX_GETSTRING(item)
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
				DLIB_WXGET_BASE(menu, Menu, submenu)
				WX_GETSTRING(helpString)
				retval	= menu->Append(id, item, submenu, helpString);;
			} else
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
				WX_GETSTRING(helpString)
				WX_GETDEFINE(kind)
				retval	= menu->Append(id, item, helpString, (wxItemKind)kind);;
			}
		}
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_appendcheckitem, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 3) {
		retval	= menu->AppendCheckItem(id, item);;
	} else {
		WX_GETSTRING(helpString)
		retval	= menu->AppendCheckItem(id, item, helpString);;
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_appendradioitem, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval;
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 3) {
		retval	= menu->AppendRadioItem(id, item);;
	} else {
		WX_GETSTRING(helpString)
		retval	= menu->AppendRadioItem(id, item, helpString);;
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_START(menu_appendseparator, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem* retval	= menu->AppendSeparator();;
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_appendsubmenu, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	DLIB_WXGET_BASE(menu, Menu, submenu)
	WX_GETSTRING(text)
	if (n == 3) {
		retval	= menu->AppendSubMenu(submenu, text);;
	} else {
		WX_GETSTRING(help)
		retval	= menu->AppendSubMenu(submenu, text, help);;
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_START(menu_break, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	menu->Break();
}

WX_FUNC_START(menu_check, 3, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_GETBOOL(check)
	menu->Check(id, check);
}

WX_FUNC_START(menu_delete, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		menu->Delete(item);
	} else {
		WX_GETDEFINE(id)
		menu->Delete(id);
	}
}

WX_FUNC_START(menu_destroy, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		menu->Destroy(item);
	} else {
		WX_GETDEFINE(id)
		menu->Destroy(id);
	}
}

WX_FUNC_START(menu_enable, 3, Nil)
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
		wxMenuItem *retval = (wxMenuItem*) 0;
		if (n == 2) {
			retval	= menu->FindItem(id);;
		} else {
			WX_GETTABLE(menu_table)
			wxMenu *arg;
			retval = menu->FindItem(id, &arg);
			WX_SETTABLE_RETVAL(menu_table, menu)
		}
		WX_SETOBJECT(MenuItem, retval)
	}
}

WX_FUNC_START(menu_finditembyposition, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETNUMBER(position)
	wxMenuItem* retval	= menu->FindItemByPosition(position);;
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_START(menu_gethelpstring, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_SETSTRING(menu->GetHelpString(id))
}

WX_FUNC_START(menu_getlabel, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_SETSTRING(menu->GetLabel(id))
}

WX_FUNC_START(menu_getmenuitemcount, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_SETNUMBER(menu->GetMenuItemCount())
}

WX_FUNC_START(menu_getmenuitems, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItemList itemlist = menu->GetMenuItems();
	DeltaList_Make(DLIB_RETVAL_REF);
	std::list<DeltaValue>* list = DeltaList_Get(DLIB_RETVAL_REF);
	for (wxMenuItemList::iterator it = itemlist.begin(); it != itemlist.end(); ++it) {
		DeltaValue val;
		wxMenuItem *item = *it;
		WX_SETOBJECT_EX(val, MenuItem, item)
		list->push_back(val);
	}
}

WX_FUNC_START(menu_gettitle, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_SETSTRING(menu->GetTitle())
}

WX_FUNC_ARGRANGE_START(menu_insert, 3, 6, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	WX_GETNUMBER(pos)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId && n == 3) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		retval	= menu->Insert(pos, item);;
	} else if (n == 3) {
		WX_GETDEFINE(id)
		retval	= menu->Insert(pos, id);;
	} else if (n == 4) {
		WX_GETDEFINE(id)
		WX_GETSTRING(item)
		retval	= menu->Insert(pos, id, item);;
	} else if (n == 5) {
		WX_GETDEFINE(id)
		WX_GETSTRING(item)
		WX_GETSTRING(helpString)
		retval	= menu->Insert(pos, id, item, helpString);;
	} else {
		WX_GETDEFINE(id)
		WX_GETSTRING(item)
		WX_GETSTRING(helpString)
		WX_GETDEFINE(kind)
		retval	= menu->Insert(pos, id, item, helpString, (wxItemKind)kind);;
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_insertcheckitem, 4, 5, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	WX_GETNUMBER(pos)
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 4) {
		retval	= menu->InsertCheckItem(pos, id, item);;
	} else {
		WX_GETSTRING(helpString)
		retval	= menu->InsertCheckItem(pos, id, item, helpString);;
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_insertradioitem, 4, 5, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	WX_GETNUMBER(pos)
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 4) {
		retval	= menu->InsertRadioItem(pos, id, item);;
	} else {
		WX_GETSTRING(helpString)
		retval	= menu->InsertRadioItem(pos, id, item, helpString);;
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_START(menu_insertseparator, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETNUMBER(pos)
	wxMenuItem* retval	= menu->InsertSeparator(pos);;
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_START(menu_ischecked, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_SETBOOL(menu->IsChecked(id))
}

WX_FUNC_START(menu_isenabled, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_SETBOOL(menu->IsEnabled(id))
}

WX_FUNC_ARGRANGE_START(menu_prepend, 2, 5, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	if (n == 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		retval	= menu->Prepend(item);;
	} else {
		WX_GETDEFINE(id)
		if (n == 2) {
			retval	= menu->Prepend(id);;
		} else if (n == 3) {
			WX_GETSTRING(item)
			retval	= menu->Prepend(id, item);;
		} else if (n == 4) {
			WX_GETSTRING(item)
			WX_GETSTRING(helpString)
			retval	= menu->Prepend(id, item, helpString);;
		} else {
			WX_GETSTRING(item)
			WX_GETSTRING(helpString)
			WX_GETDEFINE(kind)
			retval	= menu->Prepend(id, item, helpString, (wxItemKind)kind);;
		}
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_prependcheckitem, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 3) {
		retval	= menu->PrependCheckItem(id, item);;
	} else {
		WX_GETSTRING(helpString)
		retval	= menu->PrependCheckItem(id, item, helpString);;
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_ARGRANGE_START(menu_prependradioitem, 3, 4, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	WX_GETDEFINE(id)
	WX_GETSTRING(item)
	if (n == 3) {
		retval	= menu->PrependRadioItem(id, item);;
	} else {
		WX_GETSTRING(helpString)
		retval	= menu->PrependRadioItem(id, item, helpString);;
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_START(menu_prependseparator, 1, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem* retval	= menu->PrependSeparator();;
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_START(menu_remove, 2, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	wxMenuItem *retval = (wxMenuItem*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(menuitem, MenuItem, item)
		retval	= menu->Remove(item);;
	} else {
		WX_GETDEFINE(id)
		retval	= menu->Remove(id);;
	}
	WX_SETOBJECT(MenuItem, retval)
}

WX_FUNC_START(menu_sethelpstring, 3, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_GETSTRING(helpString)
	menu->SetHelpString(id, helpString);
}

WX_FUNC_START(menu_setlabel, 3, Nil)
	DLIB_WXGET_BASE(menu, Menu, menu)
	WX_GETDEFINE(id)
	WX_GETSTRING(label)
	menu->SetLabel(id, label);
}

WX_FUNC_START(menu_settitle, 2, Nil)
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
