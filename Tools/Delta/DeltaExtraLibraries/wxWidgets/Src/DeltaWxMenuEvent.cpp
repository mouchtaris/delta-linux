#include "DeltaWxMenuEvent.h"
#include "DeltaWxEvent.h"
#include "DeltaWxMenu.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(menuevent, name)
#define WX_FUNC(name) WX_FUNC1(menuevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getmenu)
WX_FUNC_DEF(getmenuid)
WX_FUNC_DEF(ispopup)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getmenu),
	WX_FUNC(getmenuid),
	WX_FUNC(ispopup)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getmenu", "ispopup")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MenuEvent, "menuevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MenuEvent, val, menuevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetMenu (void* val, DeltaValue* at) 
{
	wxMenuEvent *ev = DLIB_WXTYPECAST_BASE(MenuEvent, val, menuevent);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Menu, ev->GetMenu())
	return true;
}

static bool GetMenuId (void* val, DeltaValue* at) 
{
	wxMenuEvent *ev = DLIB_WXTYPECAST_BASE(MenuEvent, val, menuevent);
	WX_SETNUMBER_EX(*at, ev->GetMenuId())
	return true;
}

static bool GetIsPopup (void* val, DeltaValue* at) 
{
	wxMenuEvent *ev = DLIB_WXTYPECAST_BASE(MenuEvent, val, menuevent);
	WX_SETBOOL_EX(*at, ev->IsPopup())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "menu",				&GetMenu,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "menuId",				&GetMenuId,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isPopup",			&GetIsPopup,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(MenuEvent,menuevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(menuevent_construct, 0, 3, Nil)
	int type = wxEVT_NULL;
	int winid = 0;
	wxMenu* menu = NULL;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 3) { DLIB_WXGET_BASE(menu, Menu, _menu) menu = _menu; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(MenuEvent, new wxMenuEvent(type, winid, menu))
}

WX_FUNC_START(menuevent_getmenu, 1, Nil)
	DLIB_WXGET_BASE(menuevent, MenuEvent, evt)
	WX_SETOBJECT(Menu, evt->GetMenu())
}

WX_FUNC_START(menuevent_getmenuid, 1, Nil)
	DLIB_WXGET_BASE(menuevent, MenuEvent, evt)
	WX_SETNUMBER(evt->GetMenuId())
}

WX_FUNC_START(menuevent_ispopup, 1, Nil)
	DLIB_WXGET_BASE(menuevent, MenuEvent, evt)
	WX_SETBOOL(evt->IsPopup())
}
