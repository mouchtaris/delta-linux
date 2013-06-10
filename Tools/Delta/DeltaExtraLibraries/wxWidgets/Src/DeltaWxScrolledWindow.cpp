#include "DeltaWxScrolledWindow.h"
#include "DeltaWxPanel.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxDC.h"
#include "DeltaWxRect.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(scrolledwindow, name)
#define WX_FUNC(name) WX_FUNC1(scrolledwindow, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(calcscrolledposition)
WX_FUNC_DEF(calcunscrolledposition)
WX_FUNC_DEF(create)
WX_FUNC_DEF(enablescrolling)
WX_FUNC_DEF(getscrollpixelsperunit)
WX_FUNC_DEF(getviewstart)
WX_FUNC_DEF(getvirtualsize)
WX_FUNC_DEF(isretained)
WX_FUNC_DEF(preparedc)
WX_FUNC_DEF(scroll)
WX_FUNC_DEF(setscrollbars)
WX_FUNC_DEF(setscrollrate)
WX_FUNC_DEF(settargetwindow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(calcscrolledposition),
	WX_FUNC(calcunscrolledposition),
	WX_FUNC(create),
	WX_FUNC(enablescrolling),
	WX_FUNC(getscrollpixelsperunit),
	WX_FUNC(getviewstart),
	WX_FUNC(getvirtualsize),
	WX_FUNC(isretained),
	WX_FUNC(preparedc),
	WX_FUNC(scroll),
	WX_FUNC(setscrollbars),
	WX_FUNC(setscrollrate),
	WX_FUNC(settargetwindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "calcscrolledposition", "settargetwindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ScrolledWindow, "scrolledwindow", Panel)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ScrolledWindow, val, scrolledwindow) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Panel, val)
	return true;
}

static bool GetScaleX (void* val, DeltaValue* at) 
{
	wxScrolledWindow *win = DLIB_WXTYPECAST_BASE(ScrolledWindow, val, scrolledwindow);
	WX_SETNUMBER_EX(*at, win->GetScaleX())
	return true;
}

static bool GetScaleY (void* val, DeltaValue* at) 
{
	wxScrolledWindow *win = DLIB_WXTYPECAST_BASE(ScrolledWindow, val, scrolledwindow);
	WX_SETNUMBER_EX(*at, win->GetScaleY())
	return true;
}

static bool GetTargetWindow (void* val, DeltaValue* at) 
{
	wxScrolledWindow *win = DLIB_WXTYPECAST_BASE(ScrolledWindow, val, scrolledwindow);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, win->GetTargetWindow())
	return true;
}

static bool GetRectToScroll (void* val, DeltaValue* at) 
{
	wxScrolledWindow *win = DLIB_WXTYPECAST_BASE(ScrolledWindow, val, scrolledwindow);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Rect, new wxRect(win->GetTargetRect()))
	return true;
}

static bool GetXScrollLines (void* val, DeltaValue* at) 
{
	wxScrolledWindow *win = DLIB_WXTYPECAST_BASE(ScrolledWindow, val, scrolledwindow);
	WX_SETNUMBER_EX(*at, win->GetScrollLines(wxHORIZONTAL))
	return true;
}

static bool GetYScrollLines (void* val, DeltaValue* at) 
{
	wxScrolledWindow *win = DLIB_WXTYPECAST_BASE(ScrolledWindow, val, scrolledwindow);
	WX_SETNUMBER_EX(*at, win->GetScrollLines(wxVERTICAL))
	return true;
}

static bool GetXScrollLinesPerPage (void* val, DeltaValue* at) 
{
	wxScrolledWindow *win = DLIB_WXTYPECAST_BASE(ScrolledWindow, val, scrolledwindow);
	WX_SETNUMBER_EX(*at, win->GetScrollPageSize(wxHORIZONTAL))
	return true;
}

static bool GetYScrollLinesPerPage (void* val, DeltaValue* at) 
{
	wxScrolledWindow *win = DLIB_WXTYPECAST_BASE(ScrolledWindow, val, scrolledwindow);
	WX_SETNUMBER_EX(*at, win->GetScrollPageSize(wxVERTICAL))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Panel",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "scaleX",				&GetScaleX,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "scaleY",				&GetScaleY,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "targetWindow",		&GetTargetWindow,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rectToScroll",		&GetRectToScroll,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "xScrollLines",		&GetXScrollLines,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "yScrollLines",		&GetYScrollLines,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "xScrollLinesPerPage",&GetXScrollLinesPerPage,DELTA_GETBYSTRING_NO_PRECOND	},
	{ "yScrollLinesPerPage",&GetYScrollLinesPerPage,DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ScrolledWindow,scrolledwindow)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(scrolledwindow_construct, 0, 6, Nil)
	wxScrolledWindow *win = (wxScrolledWindow*) 0;
	if (n == 0) {
		win = new wxScrolledWindow();
	} else {
		DLIB_WXGET_BASE(window, Window, parent)
		int winid = wxID_ANY;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxScrolledWindowStyle;
		wxString name = wxPanelNameStr;
		if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { WX_GETSTRING_DEFINED(name) }
		win = new wxScrolledWindow(parent, winid, pos, size, style, name);
	}
	WX_SET_WINDOW_OBJECT(ScrolledWindow, win)
}

WX_FUNC_ARGRANGE_START(scrolledwindow_calcscrolledposition, 2, 5, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	if (n == 2) {
		DLIB_WXGETPOINT_BASE(point)
		wxPoint* newPoint = new wxPoint();
		win->CalcScrolledPosition(point->x, point->y, &newPoint->x, &newPoint->y);
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, newPoint)
	} else if (n == 3) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		int xx, yy;
		win->CalcScrolledPosition(x, y, &xx, &yy);
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(xx, yy))
	} else if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		int xx, yy;
		win->CalcScrolledPosition(x, y, &xx, &yy);
		WX_GETTABLE(xx_table)
		WX_GETTABLE(yy_table)
		WX_SETTABLE_RETVAL(xx_table, DeltaValue(DeltaNumberValueType(xx)))
		WX_SETTABLE_RETVAL(yy_table, DeltaValue(DeltaNumberValueType(yy)))
	}
}

WX_FUNC_ARGRANGE_START(scrolledwindow_calcunscrolledposition, 2, 5, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	if (n == 2) {
		DLIB_WXGETPOINT_BASE(point)
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(win->CalcUnscrolledPosition(*point)))
	} else if (n == 3) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		int xx, yy;
		win->CalcUnscrolledPosition(x, y, &xx, &yy);
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(xx, yy))
	} else if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		int xx, yy;
		win->CalcUnscrolledPosition(x, y, &xx, &yy);
		WX_GETTABLE(xx_table)
		WX_GETTABLE(yy_table)
		WX_SETTABLE_RETVAL(xx_table, DeltaValue(DeltaNumberValueType(xx)))
		WX_SETTABLE_RETVAL(yy_table, DeltaValue(DeltaNumberValueType(yy)))
	}
}

WX_FUNC_ARGRANGE_START(scrolledwindow_create, 2, 7, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	DLIB_WXGET_BASE(window, Window, parent)
	int winid = wxID_ANY;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxScrolledWindowStyle;
	wxString name = wxPanelNameStr;
	if (n >= 3) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(win->Create(parent, winid, pos, size, style, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(win);
}

WX_FUNC_START(scrolledwindow_enablescrolling, 3, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	WX_GETBOOL(xScrolling)
	WX_GETBOOL(yScrolling)
	win->EnableScrolling(xScrolling, yScrolling);
}

WX_FUNC_START(scrolledwindow_getscrollpixelsperunit, 3, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	int xUnit, yUnit;
	win->GetScrollPixelsPerUnit(&xUnit, &yUnit);
	WX_GETTABLE(xUnit_table)
	WX_GETTABLE(yUnit_table)
	WX_SETTABLE_RETVAL(xUnit_table, DeltaValue(DeltaNumberValueType(xUnit)))
	WX_SETTABLE_RETVAL(yUnit_table, DeltaValue(DeltaNumberValueType(yUnit)))
}

WX_FUNC_START(scrolledwindow_getviewstart, 3, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	int x, y;
	win->GetViewStart(&x, &y);
	WX_GETTABLE(x_table)
	WX_GETTABLE(y_table)
	WX_SETTABLE_RETVAL(x_table, DeltaValue(DeltaNumberValueType(x)))
	WX_SETTABLE_RETVAL(y_table, DeltaValue(DeltaNumberValueType(y)))
}

WX_FUNC_START(scrolledwindow_getvirtualsize, 3, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	int x, y;
	win->GetVirtualSize(&x, &y);
	WX_GETTABLE(x_table)
	WX_GETTABLE(y_table)
	WX_SETTABLE_RETVAL(x_table, DeltaValue(DeltaNumberValueType(x)))
	WX_SETTABLE_RETVAL(y_table, DeltaValue(DeltaNumberValueType(y)))
}

WX_FUNC_START(scrolledwindow_isretained, 1, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	WX_SETBOOL(win->IsRetained())
}

WX_FUNC_START(scrolledwindow_preparedc, 2, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	DLIB_WXGET_BASE(dc, DC, dc)
	win->PrepareDC(*dc);
}

WX_FUNC_START(scrolledwindow_scroll, 3, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	win->Scroll(x, y);
}

WX_FUNC_ARGRANGE_START(scrolledwindow_setscrollbars, 5, 8, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	WX_GETNUMBER(pixelsPerUnitX)
	WX_GETNUMBER(pixelsPerUnitY)
	WX_GETNUMBER(noUnitsX)
	WX_GETNUMBER(noUnitsY)
	int xPos = 0, yPos = 0;
	bool noRefresh = false;
	if (n >= 6) { WX_GETNUMBER_DEFINED(xPos) }
	if (n >= 7) { WX_GETNUMBER_DEFINED(yPos) }
	if (n >= 8) { WX_GETBOOL_DEFINED(noRefresh) }
	win->SetScrollbars(pixelsPerUnitX, pixelsPerUnitY, noUnitsX, noUnitsY, xPos, yPos, noRefresh);
}

WX_FUNC_START(scrolledwindow_setscrollrate, 3, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	WX_GETNUMBER(xstep)
	WX_GETNUMBER(ystep)
	win->SetScrollRate(xstep, ystep);
}

WX_FUNC_START(scrolledwindow_settargetwindow, 2, Nil)
	DLIB_WXGET_BASE(scrolledwindow, ScrolledWindow, win)
	DLIB_WXGET_BASE(window, Window, window)
	win->SetTargetWindow(window);
}
