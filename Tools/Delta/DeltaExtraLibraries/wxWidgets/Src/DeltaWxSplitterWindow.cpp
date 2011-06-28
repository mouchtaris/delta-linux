#include "DeltaWxSplitterWindow.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(splitterwindow, name)
#define WX_FUNC(name) WX_FUNC1(splitterwindow, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getminimumpanesize)
WX_FUNC_DEF(getsashgravity)
WX_FUNC_DEF(getsashposition)
WX_FUNC_DEF(getsplitmode)
WX_FUNC_DEF(getwindow1)
WX_FUNC_DEF(getwindow2)
WX_FUNC_DEF(initialize)
WX_FUNC_DEF(issplit)
WX_FUNC_DEF(ondoubleclicksash)
WX_FUNC_DEF(onunsplit)
WX_FUNC_DEF(onsashpositionchange)
WX_FUNC_DEF(replacewindow)
WX_FUNC_DEF(setsashgravity)
WX_FUNC_DEF(setsashposition)
WX_FUNC_DEF(setsashsize)
WX_FUNC_DEF(setminimumpanesize)
WX_FUNC_DEF(setsplitmode)
WX_FUNC_DEF(splithorizontally)
WX_FUNC_DEF(splitvertically)
WX_FUNC_DEF(unsplit)
WX_FUNC_DEF(updatesize)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getminimumpanesize),
	WX_FUNC(getsashgravity),
	WX_FUNC(getsashposition),
	WX_FUNC(getsplitmode),
	WX_FUNC(getwindow1),
	WX_FUNC(getwindow2),
	WX_FUNC(initialize),
	WX_FUNC(issplit),
	WX_FUNC(ondoubleclicksash),
	WX_FUNC(onunsplit),
	WX_FUNC(onsashpositionchange),
	WX_FUNC(replacewindow),
	WX_FUNC(setsashgravity),
	WX_FUNC(setsashposition),
	WX_FUNC(setsashsize),
	WX_FUNC(setminimumpanesize),
	WX_FUNC(setsplitmode),
	WX_FUNC(splithorizontally),
	WX_FUNC(splitvertically),
	WX_FUNC(unsplit),
	WX_FUNC(updatesize)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "updatesize")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SplitterWindow, "splitterwindow", Window)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxWindow *_parent = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxWindow *parent = DNEWCLASS(DeltaWxWindow, (_parent));
	WX_SETOBJECT_EX(*at, Window, parent)
	return true;
}

static bool GetSplitMode (void* val, DeltaValue* at) 
{
	wxSplitterWindow *window = DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow);
	WX_SETNUMBER_EX(*at, window->GetSplitMode())
	return true;
}

static bool GetWindow1 (void* val, DeltaValue* at) 
{
	wxSplitterWindow *window = DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow);
	wxWindow *win = window->GetWindow1();
	DeltaWxWindow *retval = win ? DNEWCLASS(DeltaWxWindow, (win)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
	return true;
}

static bool GetWindow2 (void* val, DeltaValue* at) 
{
	wxSplitterWindow *window = DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow);
	wxWindow *win = window->GetWindow2();
	DeltaWxWindow *retval = win ? DNEWCLASS(DeltaWxWindow, (win)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
	return true;
}

static bool GetSashPosition (void* val, DeltaValue* at) 
{
	wxSplitterWindow *window = DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow);
	WX_SETNUMBER_EX(*at, window->GetSashPosition())
	return true;
}

static bool GetSashGravity (void* val, DeltaValue* at) 
{
	wxSplitterWindow *window = DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow);
	WX_SETNUMBER_EX(*at, window->GetSashGravity())
	return true;
}

static bool GetSashSize (void* val, DeltaValue* at) 
{
	wxSplitterWindow *window = DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow);
	WX_SETNUMBER_EX(*at, window->GetSashSize())
	return true;
}

static bool GetMinimumPaneSize (void* val, DeltaValue* at) 
{
	wxSplitterWindow *window = DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow);
	WX_SETNUMBER_EX(*at, window->GetMinimumPaneSize())
	return true;
}

static bool GetNeedUpdating (void* val, DeltaValue* at) 
{
	wxSplitterWindow *window = DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow);
	WX_SETNUMBER_EX(*at, window->GetNeedUpdating())
	return true;
}

static bool GetBorderSize (void* val, DeltaValue* at) 
{
	wxSplitterWindow *window = DLIB_WXTYPECAST_BASE(SplitterWindow, val, splitterwindow);
	WX_SETNUMBER_EX(*at, window->GetBorderSize())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Window",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "splitMode",			&GetSplitMode,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "window1",			&GetWindow1,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "window2",			&GetWindow2,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "sashPosition",		&GetSashPosition,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "sashGravity",		&GetSashGravity,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "sashSize",			&GetSashSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minimumPaneSize",	&GetMinimumPaneSize,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "needUpdating",		&GetNeedUpdating,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "borderSize",			&GetBorderSize,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SplitterWindow, splitterwindow);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(splitterwindow_construct, 0, 6, Nil)
	wxSplitterWindow* wxwindow = (wxSplitterWindow*) 0;
	DeltaWxSplitterWindow* window = (DeltaWxSplitterWindow*) 0;
	if (n == 0)
		wxwindow = new wxSplitterWindow();
	else {
		DLIB_WXGET_BASE(window, Window, parent)
		int id = wxID_ANY;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxSP_3D;
		wxString name = wxT("splitter");
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { DLIB_WXGET_BASE(point, Point, pt) pos = *pt; }
		if (n >= 4) { DLIB_WXGET_BASE(size, Size, sz) size = *sz; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { WX_GETSTRING_DEFINED(name) }
		wxwindow = new wxSplitterWindow(parent, id, pos, size, style, name);
	}
	if (wxwindow)
		window = DNEWCLASS(DeltaWxSplitterWindow, (wxwindow));
	WX_SETOBJECT(SplitterWindow, window)
}

DLIB_FUNC_START(splitterwindow_destruct, 1, Nil)
	DLIB_WXDELETE(splitterwindow, SplitterWindow, window)
}

WX_FUNC_ARGRANGE_START(splitterwindow_create, 2, 6, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	DLIB_WXGET_BASE(window, Window, parent)
	int id = wxID_ANY;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxSP_3D;
	wxString name = wxT("splitter");
	if (n >= 3) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 4) { DLIB_WXGET_BASE(point, Point, pt) pos = *pt; }
	if (n >= 5) { DLIB_WXGET_BASE(size, Size, sz) size = *sz; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(window->Create(parent, id, pos, size, style, name))
}

DLIB_FUNC_START(splitterwindow_getminimumpanesize, 1, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_SETNUMBER(window->GetMinimumPaneSize())
}

DLIB_FUNC_START(splitterwindow_getsashgravity, 1, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_SETNUMBER(window->GetSashGravity())
}

DLIB_FUNC_START(splitterwindow_getsashposition, 1, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_SETNUMBER(window->GetSashPosition())
}

DLIB_FUNC_START(splitterwindow_getsplitmode, 1, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_SETNUMBER(window->GetSplitMode())
}

DLIB_FUNC_START(splitterwindow_getwindow1, 1, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	DeltaWxWindow *retval = DNEWCLASS(DeltaWxWindow, (window->GetWindow1()));
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(splitterwindow_getwindow2, 1, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	DeltaWxWindow *retval = DNEWCLASS(DeltaWxWindow, (window->GetWindow2()));
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(splitterwindow_initialize, 2, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	DLIB_WXGET_BASE(window, Window, win)
	window->Initialize(win);
}

DLIB_FUNC_START(splitterwindow_issplit, 1, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_SETBOOL(window->IsSplit())
}

DLIB_FUNC_START(splitterwindow_ondoubleclicksash, 3, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	window->OnDoubleClickSash(x, y);
}

DLIB_FUNC_START(splitterwindow_onunsplit, 2, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	DLIB_WXGET_BASE(window, Window, removed)
	window->OnUnsplit(removed);
}

DLIB_FUNC_START(splitterwindow_onsashpositionchange, 2, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_GETNUMBER(newSashPosition)
	WX_SETBOOL(window->OnSashPositionChange(newSashPosition))
}

DLIB_FUNC_START(splitterwindow_replacewindow, 3, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	DLIB_WXGET_BASE(window, Window, windowOld)
	DLIB_WXGET_BASE(window, Window, windowNew)
	WX_SETBOOL(window->ReplaceWindow(windowOld, windowNew))
}

DLIB_FUNC_START(splitterwindow_setsashgravity, 2, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_GETNUMBER(gravity)
	window->SetSashGravity(gravity);
}

WX_FUNC_ARGRANGE_START(splitterwindow_setsashposition, 2, 3, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_GETNUMBER(position)
	bool redraw = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(redraw) }
	window->SetSashPosition(position, redraw);
}

DLIB_FUNC_START(splitterwindow_setsashsize, 2, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_GETNUMBER(size)
	window->SetSashSize(size);
}

DLIB_FUNC_START(splitterwindow_setminimumpanesize, 2, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_GETNUMBER(paneSize)
	window->SetMinimumPaneSize(paneSize);
}

DLIB_FUNC_START(splitterwindow_setsplitmode, 2, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	WX_GETDEFINE(mode)
	window->SetSplitMode(mode);
}

WX_FUNC_ARGRANGE_START(splitterwindow_splithorizontally, 3, 4, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	DLIB_WXGET_BASE(window, Window, window1)
	DLIB_WXGET_BASE(window, Window, window2)
	int sashPosition = 0;
	if (n >= 4) { WX_GETNUMBER_DEFINED(sashPosition) }
	WX_SETBOOL(window->SplitHorizontally(window1, window2, sashPosition))
}

WX_FUNC_ARGRANGE_START(splitterwindow_splitvertically, 3, 4, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	DLIB_WXGET_BASE(window, Window, window1)
	DLIB_WXGET_BASE(window, Window, window2)
	int sashPosition = 0;
	if (n >= 4) { WX_GETNUMBER_DEFINED(sashPosition) }
	WX_SETBOOL(window->SplitVertically(window1, window2, sashPosition))
}

WX_FUNC_ARGRANGE_START(splitterwindow_unsplit, 1, 2, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	wxWindow *toRemove = (wxWindow *) NULL;
	if (n >= 2) { DLIB_WXGET_BASE(splitterwindow, SplitterWindow, win) toRemove = win; }
	WX_SETBOOL(window->Unsplit(toRemove))
}

DLIB_FUNC_START(splitterwindow_updatesize, 1, Nil)
	DLIB_WXGET_BASE(splitterwindow, SplitterWindow, window)
	window->UpdateSize();
}
