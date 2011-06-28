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
WX_FUNC_DEF(destruct)
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
	WX_FUNC(destruct),
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

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setflags")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(NavigationKeyEvent, "navigationkeyevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(NavigationKeyEvent, val, navigationkeyevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
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
	wxWindow *win = ev->GetCurrentFocus();
	DeltaWxWindow *retval = win ? DNEWCLASS(DeltaWxWindow, (win)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(NavigationKeyEvent, navigationkeyevent);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(navigationkeyevent_construct, 0, Nil)
	DeltaWxNavigationKeyEvent *evt = DNEWCLASS(DeltaWxNavigationKeyEvent, (new wxNavigationKeyEvent()));
	WX_SETOBJECT(NavigationKeyEvent, evt)
}

DLIB_FUNC_START(navigationkeyevent_destruct, 1, Nil)
	DLIB_WXDELETE(navigationkeyevent, NavigationKeyEvent, evt)
}

DLIB_FUNC_START(navigationkeyevent_getdirection, 1, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_SETBOOL(evt->GetDirection())
}

DLIB_FUNC_START(navigationkeyevent_setdirection, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_GETBOOL(forward)
	evt->SetDirection(forward);
}

DLIB_FUNC_START(navigationkeyevent_iswindowchange, 1, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_SETBOOL(evt->IsWindowChange())
}

DLIB_FUNC_START(navigationkeyevent_setwindowchange, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_GETBOOL(windowChange)
	evt->SetWindowChange(windowChange);
}

DLIB_FUNC_START(navigationkeyevent_isfromtab, 1, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_SETBOOL(evt->IsFromTab())
}

DLIB_FUNC_START(navigationkeyevent_setfromtab, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_GETBOOL(fromTab)
	evt->SetFromTab(fromTab);
}

DLIB_FUNC_START(navigationkeyevent_getcurrentfocus, 1, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	DeltaWxWindow *retval = DNEWCLASS(DeltaWxWindow, (evt->GetCurrentFocus()));
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(navigationkeyevent_setcurrentfocus, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	DLIB_WXGET_BASE(window, Window, window)
	evt->SetCurrentFocus(window);
}

DLIB_FUNC_START(navigationkeyevent_setflags, 2, Nil)
	DLIB_WXGET_BASE(navigationkeyevent, NavigationKeyEvent, evt)
	WX_GETDEFINE(flags)
	evt->SetFlags(flags);
}
