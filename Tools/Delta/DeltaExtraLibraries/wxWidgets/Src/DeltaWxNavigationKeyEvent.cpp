#include "DeltaWxNavigationKeyEvent.h"
#include "DeltaWxEvent.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(navigationkeyevent, name)
#define WX_FUNC(name) WX_FUNC1(navigationkeyevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getdirection)
WX_FUNC_DEF(setdirection)
WX_FUNC_DEF(iswindowchange)
WX_FUNC_DEF(setwindowchange)
WX_FUNC_DEF(isfromtab)
WX_FUNC_DEF(setfromtab)
WX_FUNC_DEF(getcurrentfocus)
WX_FUNC_DEF(setcurrentfocus)
WX_FUNC_DEF(setflags)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getdirection),
	WX_FUNC(setdirection),
	WX_FUNC(iswindowchange),
	WX_FUNC(setwindowchange),
	WX_FUNC(isfromtab),
	WX_FUNC(setfromtab),
	WX_FUNC(getcurrentfocus),
	WX_FUNC(setcurrentfocus),
	WX_FUNC(setflags)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getdirection", "setflags")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(NavigationKeyEvent, "navigationkeyevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(NavigationKeyEvent, val, navigationkeyevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetDirection (void* val, DeltaValue* at) 
{
	wxNavigationKeyEvent *ev = DLIB_WXTYPECAST_BASE(NavigationKeyEvent, val, navigationkeyevent);
	WX_SETBOOL_EX(*at, ev->GetDirection())
	return true;
}

static bool GetIsWindowChange (void* val, DeltaValue* at) 
{
	wxNavigationKeyEvent *ev = DLIB_WXTYPECAST_BASE(NavigationKeyEvent, val, navigationkeyevent);
	WX_SETBOOL_EX(*at, ev->IsWindowChange())
	return true;
}

static bool GetIsFromTab (void* val, DeltaValue* at) 
{
	wxNavigationKeyEvent *ev = DLIB_WXTYPECAST_BASE(NavigationKeyEvent, val, navigationkeyevent);
	WX_SETBOOL_EX(*at, ev->IsFromTab())
	return true;
}

static bool GetCurrentFocus (void* val, DeltaValue* at) 
{
	wxNavigationKeyEvent *ev = DLIB_WXTYPECAST_BASE(NavigationKeyEvent, val, navigationkeyevent);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, ev->GetCurrentFocus())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "direction",			&GetDirection,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isWindowChange",		&GetIsWindowChange,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isFromTab",			&GetIsFromTab,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "currentFocus",		&GetCurrentFocus,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(NavigationKeyEvent,navigationkeyevent)

////////////////////////////////////////////////////////////////

WX_FUNC_START(navigationkeyevent_construct, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(NavigationKeyEvent, new wxNavigationKeyEvent())
}

WX_FUNC_START(navigationkeyevent_getdirection, 1, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_SETBOOL(evt->GetDirection())
}

WX_FUNC_START(navigationkeyevent_setdirection, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_GETBOOL(forward)
	evt->SetDirection(forward);
}

WX_FUNC_START(navigationkeyevent_iswindowchange, 1, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_SETBOOL(evt->IsWindowChange())
}

WX_FUNC_START(navigationkeyevent_setwindowchange, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_GETBOOL(windowChange)
	evt->SetWindowChange(windowChange);
}

WX_FUNC_START(navigationkeyevent_isfromtab, 1, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_SETBOOL(evt->IsFromTab())
}

WX_FUNC_START(navigationkeyevent_setfromtab, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_GETBOOL(fromTab)
	evt->SetFromTab(fromTab);
}

WX_FUNC_START(navigationkeyevent_getcurrentfocus, 1, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_SETOBJECT(Window, evt->GetCurrentFocus())
}

WX_FUNC_START(navigationkeyevent_setcurrentfocus, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	DLIB_WXGET_BASE(window, Window, window)
	evt->SetCurrentFocus(window);
}

WX_FUNC_START(navigationkeyevent_setflags, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_GETDEFINE(flags)
	evt->SetFlags(flags);
}
