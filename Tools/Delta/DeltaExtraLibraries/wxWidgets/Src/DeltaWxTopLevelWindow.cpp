#include "DeltaWxTopLevelWindow.h"
#include "DeltaWxWindow.h"
#include "DeltaWxSize.h"
#include "DeltaWxIcon.h"
#include "DeltaWxIconBundle.h"
#include "DeltaWxRegion.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(toplevelwindow, name)
#define WX_FUNC(name) WX_FUNC1(toplevelwindow, name)

WX_FUNC_DEF(cansettransparent)
WX_FUNC_DEF(enableclosebutton)
WX_FUNC_DEF(getdefaultitem)
WX_FUNC_DEF(geticon)
WX_FUNC_DEF(geticons)
WX_FUNC_DEF(gettitle)
WX_FUNC_DEF(isactive)
WX_FUNC_DEF(isalwaysmaximized)
WX_FUNC_DEF(iconize)
WX_FUNC_DEF(isfullscreen)
WX_FUNC_DEF(isiconized)
WX_FUNC_DEF(ismaximized)
WX_FUNC_DEF(maximize)
WX_FUNC_DEF(requestuserattention)
WX_FUNC_DEF(setdefaultitem)
WX_FUNC_DEF(seticon)
WX_FUNC_DEF(seticons)
WX_FUNC_DEF(setmaxsize)
WX_FUNC_DEF(setminsize)
WX_FUNC_DEF(setshape)
WX_FUNC_DEF(settitle)
WX_FUNC_DEF(settransparent)
WX_FUNC_DEF(shouldpreventappexit)
WX_FUNC_DEF(showfullscreen)

WX_FUNCS_START
	WX_FUNC(cansettransparent),
	WX_FUNC(enableclosebutton),
	WX_FUNC(getdefaultitem),
	WX_FUNC(geticon),
	WX_FUNC(geticons),
	WX_FUNC(gettitle),
	WX_FUNC(isactive),
	WX_FUNC(isalwaysmaximized),
	WX_FUNC(iconize),
	WX_FUNC(isfullscreen),
	WX_FUNC(isiconized),
	WX_FUNC(ismaximized),
	WX_FUNC(maximize),
	WX_FUNC(requestuserattention),
	WX_FUNC(setdefaultitem),
	WX_FUNC(seticon),
	WX_FUNC(seticons),
	WX_FUNC(setmaxsize),
	WX_FUNC(setminsize),
	WX_FUNC(setshape),
	WX_FUNC(settitle),
	WX_FUNC(settransparent),
	WX_FUNC(shouldpreventappexit),
	WX_FUNC(showfullscreen)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(0, uarraysize(funcs), "cansettransparent", "showfullscreen")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(TopLevelWindow, "toplevelwindow", Window)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TopLevelWindow, val, toplevelwindow) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Window, val)
	return true;
}

static bool GetDefaultItem (void* val, DeltaValue* at) 
{
	wxTopLevelWindow *win = DLIB_WXTYPECAST_BASE(TopLevelWindow, val, toplevelwindow);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, win->GetDefaultItem())
	return true;
}

static bool GetIsIconized (void* val, DeltaValue* at) 
{
	wxTopLevelWindow *win = DLIB_WXTYPECAST_BASE(TopLevelWindow, val, toplevelwindow);
	WX_SETBOOL_EX(*at, win->IsIconized())
	return true;
}

static bool GetIsMaximized (void* val, DeltaValue* at) 
{
	wxTopLevelWindow *win = DLIB_WXTYPECAST_BASE(TopLevelWindow, val, toplevelwindow);
	WX_SETBOOL_EX(*at, win->IsMaximized())
	return true;
}

static bool GetIsFullScreen (void* val, DeltaValue* at) 
{
	wxTopLevelWindow *win = DLIB_WXTYPECAST_BASE(TopLevelWindow, val, toplevelwindow);
	WX_SETBOOL_EX(*at, win->IsFullScreen())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Window",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "defaultItem",		&GetDefaultItem,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isIconized",			&GetIsIconized,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isMaximized",		&GetIsMaximized,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isFullscreen",		&GetIsFullScreen,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TopLevelWindow,toplevelwindow)

////////////////////////////////////////////////////////////////

WX_FUNC_START(toplevelwindow_cansettransparent, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->CanSetTransparent())
}

WX_FUNC_ARGRANGE_START(toplevelwindow_enableclosebutton, 1, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	bool enable = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(enable) }
	WX_SETBOOL(window->EnableCloseButton(enable))
}

WX_FUNC_START(toplevelwindow_getdefaultitem, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	wxWindow* retval	= window->GetDefaultItem();
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_START(toplevelwindow_geticon, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Icon, new wxIcon(window->GetIcon()))
}

WX_FUNC_START(toplevelwindow_geticons, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(IconBundle, new wxIconBundle(window->GetIcons()))
}

WX_FUNC_START(toplevelwindow_gettitle, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETSTRING(window->GetTitle())
}

WX_FUNC_START(toplevelwindow_isactive, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->IsActive())
}

WX_FUNC_START(toplevelwindow_isalwaysmaximized, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->IsAlwaysMaximized())
}

WX_FUNC_ARGRANGE_START(toplevelwindow_iconize, 1, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	bool iconize = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(iconize) }
	window->Iconize(iconize);
}

WX_FUNC_START(toplevelwindow_isfullscreen, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->IsFullScreen())
}

WX_FUNC_START(toplevelwindow_isiconized, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->IsIconized())
}

WX_FUNC_START(toplevelwindow_ismaximized, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->IsMaximized())
}

WX_FUNC_ARGRANGE_START(toplevelwindow_maximize, 1, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	bool maximize = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(maximize) }
	window->Maximize(maximize);
}

WX_FUNC_ARGRANGE_START(toplevelwindow_requestuserattention, 1, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	int flags = wxUSER_ATTENTION_INFO;
	if (n >= 2) { WX_GETDEFINE_DEFINED(flags) }
	window->RequestUserAttention(flags);
}

WX_FUNC_START(toplevelwindow_setdefaultitem, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(window, Window, win)
	wxWindow* retval	= window->SetDefaultItem(win);
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_START(toplevelwindow_seticon, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(icon, Icon, icon)
	window->SetIcon(*icon);
}

WX_FUNC_START(toplevelwindow_seticons, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(iconbundle, IconBundle, bundle)
	window->SetIcons(*bundle);
}

WX_FUNC_START(toplevelwindow_setmaxsize, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGETSIZE_BASE(size)
	window->SetMaxSize(*size);
}

WX_FUNC_START(toplevelwindow_setminsize, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGETSIZE_BASE(size)
	window->SetMinSize(*size);
}

WX_FUNC_START(toplevelwindow_setshape, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(region, Region, region)
	WX_SETBOOL(window->SetShape(*region))
}

WX_FUNC_START(toplevelwindow_settitle, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_GETSTRING(title)
	window->SetTitle(title);
}

WX_FUNC_START(toplevelwindow_settransparent, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_GETNUMBER(alpha)
	WX_SETBOOL(window->SetTransparent(alpha))
}

WX_FUNC_START(toplevelwindow_shouldpreventappexit, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->ShouldPreventAppExit())
}

WX_FUNC_ARGRANGE_START(toplevelwindow_showfullscreen, 2, 3, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_GETBOOL(show)
	long style = wxFULLSCREEN_ALL;
	if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
	WX_SETBOOL(window->ShowFullScreen(show, style))
}
