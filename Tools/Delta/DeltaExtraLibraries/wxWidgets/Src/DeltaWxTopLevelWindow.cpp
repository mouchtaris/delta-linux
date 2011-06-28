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

WX_FUNC_DEF(destruct)
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
	WX_FUNC(destruct),
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

DELTALIBFUNC_DECLARECONSTS(0, uarraysize(funcs), "destruct", "showfullscreen")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(TopLevelWindow, "toplevelwindow", Window)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TopLevelWindow, val, toplevelwindow) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxWindow *_parent = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxWindow *parent = DNEWCLASS(DeltaWxWindow, (_parent));
	WX_SETOBJECT_EX(*at, Window, parent)
	return true;
}

static bool GetDefaultItem (void* val, DeltaValue* at) 
{
	wxTopLevelWindow *win = DLIB_WXTYPECAST_BASE(TopLevelWindow, val, toplevelwindow);
	wxWindow *item = win->GetDefaultItem();
	DeltaWxWindow *retval = item ? DNEWCLASS(DeltaWxWindow, (item)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(TopLevelWindow, toplevelwindow);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(toplevelwindow_destruct, 1, Nil)
	DLIB_WXDELETE(toplevelwindow, TopLevelWindow, window)
}

DLIB_FUNC_START(toplevelwindow_cansettransparent, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->CanSetTransparent())
}

WX_FUNC_ARGRANGE_START(toplevelwindow_enableclosebutton, 1, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	bool enable = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(enable) }
	WX_SETBOOL(window->EnableCloseButton(enable))
}

DLIB_FUNC_START(toplevelwindow_getdefaultitem, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WXNEWCLASS(DeltaWxWindow, retval, wxWindow, window->GetDefaultItem())
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(toplevelwindow_geticon, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DeltaWxIcon *retval = DNEWCLASS(DeltaWxIcon, (new wxIcon(window->GetIcon())));
	WX_SETOBJECT(Icon, retval)
}

DLIB_FUNC_START(toplevelwindow_geticons, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DeltaWxIconBundle *retval = DNEWCLASS(DeltaWxIconBundle, (new wxIconBundle(window->GetIcons())));
	WX_SETOBJECT(IconBundle, retval)
}

DLIB_FUNC_START(toplevelwindow_gettitle, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETSTRING(window->GetTitle())
}

DLIB_FUNC_START(toplevelwindow_isactive, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->IsActive())
}

DLIB_FUNC_START(toplevelwindow_isalwaysmaximized, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->IsAlwaysMaximized())
}

WX_FUNC_ARGRANGE_START(toplevelwindow_iconize, 1, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	bool iconize = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(iconize) }
	window->Iconize(iconize);
}

DLIB_FUNC_START(toplevelwindow_isfullscreen, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->IsFullScreen())
}

DLIB_FUNC_START(toplevelwindow_isiconized, 1, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_SETBOOL(window->IsIconized())
}

DLIB_FUNC_START(toplevelwindow_ismaximized, 1, Nil)
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

DLIB_FUNC_START(toplevelwindow_setdefaultitem, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(window, Window, win)
	WXNEWCLASS(DeltaWxWindow, retval, wxWindow, window->SetDefaultItem(win))
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(toplevelwindow_seticon, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(icon, Icon, icon)
	window->SetIcon(*icon);
}

DLIB_FUNC_START(toplevelwindow_seticons, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(iconbundle, IconBundle, bundle)
	window->SetIcons(*bundle);
}

DLIB_FUNC_START(toplevelwindow_setmaxsize, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(size, Size, size)
	window->SetMaxSize(*size);
}

DLIB_FUNC_START(toplevelwindow_setminsize, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(size, Size, size)
	window->SetMinSize(*size);
}

DLIB_FUNC_START(toplevelwindow_setshape, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	DLIB_WXGET_BASE(region, Region, region)
	WX_SETBOOL(window->SetShape(*region))
}

DLIB_FUNC_START(toplevelwindow_settitle, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_GETSTRING(title)
	window->SetTitle(title);
}

DLIB_FUNC_START(toplevelwindow_settransparent, 2, Nil)
	DLIB_WXGET_BASE(toplevelwindow, TopLevelWindow, window)
	WX_GETNUMBER(alpha)
	WX_SETBOOL(window->SetTransparent(alpha))
}

DLIB_FUNC_START(toplevelwindow_shouldpreventappexit, 1, Nil)
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
