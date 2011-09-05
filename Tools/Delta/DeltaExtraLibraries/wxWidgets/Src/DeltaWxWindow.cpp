#include "DeltaWxWindow.h"
#include "DeltaWxEvtHandler.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxAcceleratorTable.h"
#include "DeltaWxCaret.h"
#include "DeltaWxLayoutConstraints.h"
#include "DeltaWxColour.h"
#include "DeltaWxFont.h"
#include "DeltaWxRect.h"
#include "DeltaWxDropTarget.h"
#include "DeltaWxSizer.h"
#include "DeltaWxUpdateUIEvent.h"
#include "DeltaWxVisualAttributes.h"
#include "DeltaWxMenu.h"
#include "DeltaWxCursor.h"
#include "DeltaWxToolTip.h"
#include "DeltaWxRegion.h"
#include "DeltaWxPalette.h"
#include "DeltaWxValidator.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "udynamiclibloader.h"
#include "ListLib.h"
#include "wxWrapperUtilFunctions.h"
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(window, name)
#define WX_FUNC(name) WX_FUNC1(window, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(addchild)
WX_FUNC_DEF(cachebestsize)
WX_FUNC_DEF(capturemouse)
WX_FUNC_DEF(center)
WX_FUNC_DEF(centeronparent)
WX_FUNC_DEF(centre)
WX_FUNC_DEF(centreonparent)
WX_FUNC_DEF(clearbackground)
WX_FUNC_DEF(clienttoscreen)
WX_FUNC_DEF(close)
WX_FUNC_DEF(convertdialogtopixels)
WX_FUNC_DEF(convertpixelstodialog)
WX_FUNC_DEF(destroy)
WX_FUNC_DEF(destroychildren)
WX_FUNC_DEF(disable)
WX_FUNC_DEF(doupdatewindowui)
#if defined (__WXMSW__)
WX_FUNC_DEF(dragacceptfiles)
#endif //__WXMSW__
WX_FUNC_DEF(enable)
WX_FUNC_DEF(findfocus)
WX_FUNC_DEF(findwindow)
WX_FUNC_DEF(findwindowbyid)
WX_FUNC_DEF(findwindowbylabel)
WX_FUNC_DEF(findwindowbyname)
WX_FUNC_DEF(fit)
WX_FUNC_DEF(fitinside)
WX_FUNC_DEF(freeze)
WX_FUNC_DEF(getacceleratortable)
WX_FUNC_DEF(getbackgroundcolour)
WX_FUNC_DEF(getbackgroundstyle)
WX_FUNC_DEF(geteffectiveminsize)
WX_FUNC_DEF(getbestsize)
WX_FUNC_DEF(getcapture)
WX_FUNC_DEF(getcaret)
WX_FUNC_DEF(getcharheight)
WX_FUNC_DEF(getcharwidth)
WX_FUNC_DEF(getchildren)
WX_FUNC_DEF(getclassdefaultattributes)
WX_FUNC_DEF(getclientsize)
WX_FUNC_DEF(getconstraints)
WX_FUNC_DEF(getcontainingsizer)
WX_FUNC_DEF(getcursor)
WX_FUNC_DEF(getdefaultattributes)
WX_FUNC_DEF(getdroptarget)
WX_FUNC_DEF(geteventhandler)
WX_FUNC_DEF(getextrastyle)
WX_FUNC_DEF(getfont)
WX_FUNC_DEF(getforegroundcolour)
WX_FUNC_DEF(getgrandparent)
WX_FUNC_DEF(gethelptextatpoint)
WX_FUNC_DEF(gethelptext)
WX_FUNC_DEF(getid)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(getmaxsize)
WX_FUNC_DEF(getminsize)
WX_FUNC_DEF(getname)
WX_FUNC_DEF(getparent)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(getrect)
WX_FUNC_DEF(getscreenposition)
WX_FUNC_DEF(getscreenrect)
WX_FUNC_DEF(getscrollpos)
WX_FUNC_DEF(getscrollrange)
WX_FUNC_DEF(getscrollthumb)
WX_FUNC_DEF(getsize)
WX_FUNC_DEF(getsizer)
WX_FUNC_DEF(gettextextent)
WX_FUNC_DEF(gettooltip)
WX_FUNC_DEF(getupdateregion)
WX_FUNC_DEF(getvalidator)
WX_FUNC_DEF(getvirtualsize)
WX_FUNC_DEF(getwindowbordersize)
WX_FUNC_DEF(getwindowstyleflag)
WX_FUNC_DEF(getwindowvariant)
WX_FUNC_DEF(hascapture)
WX_FUNC_DEF(hasflag)
WX_FUNC_DEF(hasmultiplepages)
WX_FUNC_DEF(hasscrollbar)
WX_FUNC_DEF(hastransparentbackground)
WX_FUNC_DEF(hide)
WX_FUNC_DEF(inheritattributes)
WX_FUNC_DEF(initdialog)
WX_FUNC_DEF(invalidatebestsize)
WX_FUNC_DEF(isdoublebuffered)
WX_FUNC_DEF(isenabled)
WX_FUNC_DEF(isexposed)
WX_FUNC_DEF(isfrozen)
WX_FUNC_DEF(isretained)
WX_FUNC_DEF(isshown)
WX_FUNC_DEF(isshownonscreen)
WX_FUNC_DEF(istoplevel)
WX_FUNC_DEF(layout)
WX_FUNC_DEF(linedown)
WX_FUNC_DEF(lineup)
WX_FUNC_DEF(lower)
WX_FUNC_DEF(makemodal)
WX_FUNC_DEF(move)
WX_FUNC_DEF(moveafterintaborder)
WX_FUNC_DEF(movebeforeintaborder)
WX_FUNC_DEF(navigate)
WX_FUNC_DEF(oninternalidle)
WX_FUNC_DEF(pagedown)
WX_FUNC_DEF(pageup)
WX_FUNC_DEF(popeventhandler)
WX_FUNC_DEF(popupmenu)
WX_FUNC_DEF(pusheventhandler)
WX_FUNC_DEF(raise)
WX_FUNC_DEF(refresh)
WX_FUNC_DEF(refreshrect)
#if defined(__WXMSW__)
WX_FUNC_DEF(registerhotkey)
WX_FUNC_DEF(unregisterhotkey)
#endif //__WXMSW__
WX_FUNC_DEF(releasemouse)
WX_FUNC_DEF(removechild)
WX_FUNC_DEF(removeeventhandler)
WX_FUNC_DEF(reparent)
WX_FUNC_DEF(screentoclient)
WX_FUNC_DEF(scrolllines)
WX_FUNC_DEF(scrollpages)
WX_FUNC_DEF(scrollwindow)
WX_FUNC_DEF(setacceleratortable)
WX_FUNC_DEF(setautolayout)
WX_FUNC_DEF(setbackgroundcolour)
WX_FUNC_DEF(setbackgroundstyle)
WX_FUNC_DEF(setinitialsize)
WX_FUNC_DEF(setcaret)
WX_FUNC_DEF(setclientsize)
WX_FUNC_DEF(setconstraints)
WX_FUNC_DEF(setcontainingsizer)
WX_FUNC_DEF(setcursor)
WX_FUNC_DEF(setdroptarget)
WX_FUNC_DEF(seteventhandler)
WX_FUNC_DEF(setextrastyle)
WX_FUNC_DEF(setfocus)
WX_FUNC_DEF(setfocusfromkbd)
WX_FUNC_DEF(setfont)
WX_FUNC_DEF(setforegroundcolour)
WX_FUNC_DEF(sethelptext)
WX_FUNC_DEF(setid)
WX_FUNC_DEF(setlabel)
WX_FUNC_DEF(setmaxsize)
WX_FUNC_DEF(setminsize)
WX_FUNC_DEF(setname)
WX_FUNC_DEF(setownbackgroundcolour)
WX_FUNC_DEF(setownfont)
WX_FUNC_DEF(setownforegroundcolour)
WX_FUNC_DEF(setscrollbar)
WX_FUNC_DEF(setscrollpos)
WX_FUNC_DEF(setsize)
WX_FUNC_DEF(setsizer)
WX_FUNC_DEF(setsizerandfit)
WX_FUNC_DEF(setthemeenabled)
WX_FUNC_DEF(settooltip)
WX_FUNC_DEF(setvalidator)
WX_FUNC_DEF(setvirtualsize)
WX_FUNC_DEF(setvirtualsizehints)
WX_FUNC_DEF(setwindowstyle)
WX_FUNC_DEF(setwindowstyleflag)
WX_FUNC_DEF(setwindowvariant)
WX_FUNC_DEF(shouldinheritcolours)
WX_FUNC_DEF(show)
WX_FUNC_DEF(thaw)
WX_FUNC_DEF(togglewindowstyle)
WX_FUNC_DEF(transferdatafromwindow)
WX_FUNC_DEF(transferdatatowindow)
WX_FUNC_DEF(update)
WX_FUNC_DEF(updatewindowui)
WX_FUNC_DEF(validate)
WX_FUNC_DEF(warppointer)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(findfocus),
	WX_FUNC(findwindowbyid),
	WX_FUNC(findwindowbylabel),
	WX_FUNC(findwindowbyname),
	WX_FUNC(getcapture),
	WX_FUNC(getclassdefaultattributes),
	WX_FUNC(destruct),
	WX_FUNC(addchild),
	WX_FUNC(cachebestsize),
	WX_FUNC(capturemouse),
	WX_FUNC(center),
	WX_FUNC(centeronparent),
	WX_FUNC(centre),
	WX_FUNC(centreonparent),
	WX_FUNC(clearbackground),
	WX_FUNC(clienttoscreen),
	WX_FUNC(close),
	WX_FUNC(convertdialogtopixels),
	WX_FUNC(convertpixelstodialog),
	WX_FUNC(destroy),
	WX_FUNC(destroychildren),
	WX_FUNC(disable),
	WX_FUNC(doupdatewindowui),
#if defined(__WXMSW__)
	WX_FUNC(dragacceptfiles),
#endif //__WXMSW__
	WX_FUNC(enable),
	WX_FUNC(findwindow),
	WX_FUNC(fit),
	WX_FUNC(fitinside),
	WX_FUNC(freeze),
	WX_FUNC(getacceleratortable),
	WX_FUNC(getbackgroundcolour),
	WX_FUNC(getbackgroundstyle),
	WX_FUNC(geteffectiveminsize),
	WX_FUNC(getbestsize),
	WX_FUNC(getcaret),
	WX_FUNC(getcharheight),
	WX_FUNC(getcharwidth),
	WX_FUNC(getchildren),
	WX_FUNC(getclientsize),
	WX_FUNC(getconstraints),
	WX_FUNC(getcontainingsizer),
	WX_FUNC(getcursor),
	WX_FUNC(getdefaultattributes),
	WX_FUNC(getdroptarget),
	WX_FUNC(geteventhandler),
	WX_FUNC(getextrastyle),
	WX_FUNC(getfont),
	WX_FUNC(getforegroundcolour),
	WX_FUNC(getgrandparent),
	WX_FUNC(gethelptextatpoint),
	WX_FUNC(gethelptext),
	WX_FUNC(getid),
	WX_FUNC(getlabel),
	WX_FUNC(getmaxsize),
	WX_FUNC(getminsize),
	WX_FUNC(getname),
	WX_FUNC(getparent),
	WX_FUNC(getposition),
	WX_FUNC(getrect),
	WX_FUNC(getscreenposition),
	WX_FUNC(getscreenrect),
	WX_FUNC(getscrollpos),
	WX_FUNC(getscrollrange),
	WX_FUNC(getscrollthumb),
	WX_FUNC(getsize),
	WX_FUNC(getsizer),
	WX_FUNC(gettextextent),
	WX_FUNC(gettooltip),
	WX_FUNC(getupdateregion),
	WX_FUNC(getvalidator),
	WX_FUNC(getvirtualsize),
	WX_FUNC(getwindowbordersize),
	WX_FUNC(getwindowstyleflag),
	WX_FUNC(getwindowvariant),
	WX_FUNC(hascapture),
	WX_FUNC(hasflag),
	WX_FUNC(hasmultiplepages),
	WX_FUNC(hasscrollbar),
	WX_FUNC(hastransparentbackground),
	WX_FUNC(hide),
	WX_FUNC(inheritattributes),
	WX_FUNC(initdialog),
	WX_FUNC(invalidatebestsize),
	WX_FUNC(isdoublebuffered),
	WX_FUNC(isenabled),
	WX_FUNC(isexposed),
	WX_FUNC(isfrozen),
	WX_FUNC(isretained),
	WX_FUNC(isshown),
	WX_FUNC(isshownonscreen),
	WX_FUNC(istoplevel),
	WX_FUNC(layout),
	WX_FUNC(linedown),
	WX_FUNC(lineup),
	WX_FUNC(lower),
	WX_FUNC(makemodal),
	WX_FUNC(move),
	WX_FUNC(moveafterintaborder),
	WX_FUNC(movebeforeintaborder),
	WX_FUNC(navigate),
	WX_FUNC(oninternalidle),
	WX_FUNC(pagedown),
	WX_FUNC(pageup),
	WX_FUNC(popeventhandler),
	WX_FUNC(popupmenu),
	WX_FUNC(pusheventhandler),
	WX_FUNC(raise),
	WX_FUNC(refresh),
	WX_FUNC(refreshrect),
#if defined(__WXMSW__)
	WX_FUNC(registerhotkey),
	WX_FUNC(unregisterhotkey),
#endif //__WXMSW__
	WX_FUNC(releasemouse),
	WX_FUNC(removechild),
	WX_FUNC(removeeventhandler),
	WX_FUNC(reparent),
	WX_FUNC(screentoclient),
	WX_FUNC(scrolllines),
	WX_FUNC(scrollpages),
	WX_FUNC(scrollwindow),
	WX_FUNC(setacceleratortable),
	WX_FUNC(setautolayout),
	WX_FUNC(setbackgroundcolour),
	WX_FUNC(setbackgroundstyle),
	WX_FUNC(setinitialsize),
	WX_FUNC(setcaret),
	WX_FUNC(setclientsize),
	WX_FUNC(setconstraints),
	WX_FUNC(setcontainingsizer),
	WX_FUNC(setcursor),
	WX_FUNC(setdroptarget),
	WX_FUNC(seteventhandler),
	WX_FUNC(setextrastyle),
	WX_FUNC(setfocus),
	WX_FUNC(setfocusfromkbd),
	WX_FUNC(setfont),
	WX_FUNC(setforegroundcolour),
	WX_FUNC(sethelptext),
	WX_FUNC(setid),
	WX_FUNC(setlabel),
	WX_FUNC(setmaxsize),
	WX_FUNC(setminsize),
	WX_FUNC(setname),
	WX_FUNC(setownbackgroundcolour),
	WX_FUNC(setownfont),
	WX_FUNC(setownforegroundcolour),
	WX_FUNC(setscrollbar),
	WX_FUNC(setscrollpos),
	WX_FUNC(setsize),
	WX_FUNC(setsizer),
	WX_FUNC(setsizerandfit),
	WX_FUNC(setthemeenabled),
	WX_FUNC(settooltip),
	WX_FUNC(setvalidator),
	WX_FUNC(setvirtualsize),
	WX_FUNC(setvirtualsizehints),
	WX_FUNC(setwindowstyle),
	WX_FUNC(setwindowstyleflag),
	WX_FUNC(setwindowvariant),
	WX_FUNC(shouldinheritcolours),
	WX_FUNC(show),
	WX_FUNC(thaw),
	WX_FUNC(togglewindowstyle),
	WX_FUNC(transferdatafromwindow),
	WX_FUNC(transferdatatowindow),
	WX_FUNC(update),
	WX_FUNC(updatewindowui),
	WX_FUNC(validate),
	WX_FUNC(warppointer)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(7, uarraysize(funcs) - 7, "destruct", "warppointer")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Window, "window", EvtHandler)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at)
{
	return DLIB_WXTYPECAST_BASE(Window, val, window) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at)
{
	wxEvtHandler *_parent = DLIB_WXTYPECAST_BASE(EvtHandler, val, evthandler);
	DeltaWxEvtHandler *parent = DNEWCLASS(DeltaWxEvtHandler, (_parent));
	WX_SETOBJECT_EX(*at, EvtHandler, parent)
	return true;
}

static bool GetWindowId (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetId());
	return true;
}

static bool GetParent (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	wxWindow *parent = win->GetParent();
	DeltaWxWindow *retval = parent ? DNEWCLASS(DeltaWxWindow, (parent)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
	return true;
}

static bool GetChildren (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	wxWindowList windowlist = win->GetChildren();
	DeltaList_MakeForDebuggerOnly(*at);
	std::list<DeltaValue>* list = DeltaList_Get(*at);
	for (wxWindowList::iterator it = windowlist.begin(); it != windowlist.end(); ++it) {
		DeltaValue val;
		wxWindow *child = *it;
		DeltaWxWindow *deltawindow = DNEWCLASS(DeltaWxWindow, (child));
		WX_SETOBJECT_EX(val, Window, deltawindow)
		list->push_back(val);
	}
	return true;
}

static bool GetMinWidth (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetMinSize().x);
	return true;
}

static bool GetMinHeight (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetMinSize().y);
	return true;
}

static bool GetMaxWidth (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetMaxSize().x);
	return true;
}

static bool GetMaxHeight (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetMaxSize().y);
	return true;
}

static bool GetEventHandler (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	wxEvtHandler *evthandler = win->GetEventHandler();
	DeltaWxEvtHandler *retval = evthandler ? DNEWCLASS(DeltaWxEvtHandler, (evthandler)) :
		(DeltaWxEvtHandler*) 0;
	WX_SETOBJECT_EX(*at, EvtHandler, retval)
	return true;
}

static bool GetCursor (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxCursor *retval = DNEWCLASS(DeltaWxCursor, (new wxCursor(win->GetCursor())));
	WX_SETOBJECT_EX(*at, Cursor, retval)
	return true;
}

static bool GetFont (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(win->GetFont())));
	WX_SETOBJECT_EX(*at, Font, retval)
	return true;
}

static bool GetBackgroundColour (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(win->GetBackgroundColour())));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static bool GetForegroundColour (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(win->GetForegroundColour())));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static bool GetCaret (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	wxCaret *caret = win->GetCaret();
	DeltaWxCaret *retval = caret ? DNEWCLASS(DeltaWxCaret, (caret)) : (DeltaWxCaret*)0;
	WX_SETOBJECT_EX(*at, Caret, retval)
	return true;
}

static bool GetUpdateRegion (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxRegion *retval = DNEWCLASS(DeltaWxRegion, (new wxRegion(win->GetUpdateRegion())));
	WX_SETOBJECT_EX(*at, Region, retval)
	return true;
}

static bool GetAcceleratorTable (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxAcceleratorTable *retval = DNEWCLASS(DeltaWxAcceleratorTable,
		(win->GetAcceleratorTable()));
	WX_SETOBJECT_EX(*at, AcceleratorTable, retval)
	return true;
}

static bool GetToolTip (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	wxToolTip *tooltip = win->GetToolTip();
	DeltaWxToolTip *retval = tooltip ? DNEWCLASS(DeltaWxToolTip, (tooltip)) : (DeltaWxToolTip*)0;
	WX_SETOBJECT_EX(*at, ToolTip, retval)
	return true;
}

static bool GetConstraints (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	wxLayoutConstraints *constraints = win->GetConstraints();
	DeltaWxLayoutConstraints *retval = constraints
		? DNEWCLASS(DeltaWxLayoutConstraints, (constraints))
		: (DeltaWxLayoutConstraints*)0;
	WX_SETOBJECT_EX(*at, LayoutConstraints, retval)
	return true;
}

static bool GetWindowSizer (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	wxSizer *sizer = win->GetSizer();
	DeltaWxSizer *retval = sizer ? DNEWCLASS(DeltaWxSizer, (sizer)) : (DeltaWxSizer*)0;
	WX_SETOBJECT_EX(*at, Sizer, retval)
	return true;
}

static bool GetContainingSizer (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	wxSizer *sizer = win->GetContainingSizer();
	DeltaWxSizer *retval = sizer ? DNEWCLASS(DeltaWxSizer, (sizer)) : (DeltaWxSizer*)0;
	WX_SETOBJECT_EX(*at, Sizer, retval)
	return true;
}

static bool GetAutoLayout (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromBool(win->GetAutoLayout());
	return true;
}

static bool GetIsShown (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromBool(win->IsShown());
	return true;
}

static bool GetIsEnabled (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromBool(win->IsEnabled());
	return true;
}

static bool GetIsBeingDeleted (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromBool(win->IsBeingDeleted());
	return true;
}

static bool GetHasBgCol (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromBool(win->UseBgCol());
	return true;
}

static bool GetInheritBgCol (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromBool(win->InheritsBackgroundColour());
	return true;
}

static bool GetWindowStyle (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetWindowStyleFlag());
	return true;
}

static bool GetExStyle (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetExtraStyle());
	return true;
}

static bool GetWindowName (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromString(std::string(win->GetName().mb_str(wxConvUTF8)));
	return true;
}

static bool GetThemeEnabled (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromBool(win->GetThemeEnabled());
	return true;
}

static bool GetBackgroundStyle (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetBackgroundStyle());
	return true;
}

static bool GetPalette (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxPalette *retval = DNEWCLASS(DeltaWxPalette, (new wxPalette(win->GetPalette())));
	WX_SETOBJECT_EX(*at, Palette, retval)
	return true;
}

static bool GetHasCustomPalette (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromBool(win->HasCustomPalette());
	return true;
}

static bool GetVirtualSize (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(win->GetVirtualSize())));
	WX_SETOBJECT_EX(*at, Size, retval)
	return true;
}

static bool GetWindowVariant (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetWindowVariant());
	return true;
}

static bool GetBestSizeCache (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(win->GetBestSize())));
	WX_SETOBJECT_EX(*at, Size, retval)
	return true;
}

static bool GetxThumbSize (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetScrollThumb(wxHORIZONTAL));
	return true;
}

static bool GetyThumbSize (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromNumber(win->GetScrollThumb(wxVERTICAL));
	return true;
}

static bool GetFrozenness (void* val, DeltaValue* at)
{
	wxWindow *win = DLIB_WXTYPECAST_BASE(Window, val, window);
	at->FromBool(win->IsFrozen());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "EvtHandler",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "windowId",			&GetWindowId,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "parent",				&GetParent,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "children",			&GetChildren,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minWidth",			&GetMinWidth,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minHeight",			&GetMinHeight,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maxWidth",			&GetMaxWidth,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maxHeight",			&GetMaxHeight,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "eventHandler",		&GetEventHandler,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "cursor",				&GetCursor,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "font",				&GetFont,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "backgroundColour",	&GetBackgroundColour,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "foregroundColour",	&GetForegroundColour,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "caret",				&GetCaret,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "updateRegion",		&GetUpdateRegion,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "acceleratorTable",	&GetAcceleratorTable,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "tooltip",			&GetToolTip,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "constraints",		&GetConstraints,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "windowSizer",		&GetWindowSizer,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "containingSizer",	&GetContainingSizer,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "autoLayout",			&GetAutoLayout,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isShown",			&GetIsShown,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isEnabled",			&GetIsEnabled,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isBeingDeleted",		&GetIsBeingDeleted,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasBgCol",			&GetHasBgCol,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "inheritBgCol",		&GetInheritBgCol,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "windowStyle",		&GetWindowStyle,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "exStyle",			&GetExStyle,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "windowName",			&GetWindowName,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "themeEnabled",		&GetThemeEnabled,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "backgroundStyle",	&GetBackgroundStyle,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "palette",			&GetPalette,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasCustomPalette",	&GetHasCustomPalette,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "virtualSize",		&GetVirtualSize,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "windowVariant",		&GetWindowVariant,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bestSizeCache",		&GetBestSizeCache,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "xThumbSize",			&GetxThumbSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "yThumbSize",			&GetyThumbSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "frozenness",			&GetFrozenness,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Window, window);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(window_construct, 0, 6, Nil)
	wxWindow *wxwindow = (wxWindow*) 0;
	DeltaWxWindow *window = (DeltaWxWindow*) 0;
	if (n == 0)
		wxwindow = new wxWindow();
	else if (n >= 2 && n <= 6) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxString name = wxPanelNameStr;
		if (n >= 3) {
			DLIB_WXGET_BASE(point, Point, point)
			pos = *point;
		}
		if (n >= 4) {
			DLIB_WXGET_BASE(size, Size, sz)
			size = *sz;
		}
		if (n >= 5) {
			WX_GETDEFINE(wxVar)
			style = (long) wxVar;
		}
		if (n == 6) {
			WX_GETSTRING(wxVar)
			name = wxVar;
		}
		wxwindow = new wxWindow(parent, id, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		return;
	}
	if (wxwindow)
		window = DNEWCLASS(DeltaWxWindow, (wxwindow));
	WX_SETOBJECT(Window, window)
}

DLIB_FUNC_START(window_destruct, 1, Nil)
	DLIB_WXDELETE(window, Window, window)
}

DLIB_FUNC_START(window_addchild, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(window, Window, child)
	window->AddChild(child);
}

DLIB_FUNC_START(window_cachebestsize, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(size, Size, size)
	window->CacheBestSize(*size);
}

DLIB_FUNC_START(window_capturemouse, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->CaptureMouse();
}

WX_FUNC_ARGRANGE_START(window_center, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1) {
		window->Centre();
	} else {
		WX_GETDEFINE(direction);
		window->Center((int)direction);
	}
}

WX_FUNC_ARGRANGE_START(window_centeronparent, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1) {
		window->CentreOnParent();
	} else {
		WX_GETDEFINE(direction);
		window->CenterOnParent((int)direction);
	}
}

WX_FUNC_ARGRANGE_START(window_centre, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1) {
		window->Centre();
	} else {
		WX_GETDEFINE(direction);
		window->Centre((int)direction);
	}
}

WX_FUNC_ARGRANGE_START(window_centreonparent, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1) {
		window->CentreOnParent();
	} else {
		WX_GETDEFINE(direction);
		window->CentreOnParent((int)direction);
	}
}

DLIB_FUNC_START(window_clearbackground, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->ClearBackground();
}

WX_FUNC_ARGRANGE_START(window_clienttoscreen, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxPoint *pt = (DeltaWxPoint*) 0;
	if (n == 2) {
		DLIB_WXGET_BASE(point, Point, point)
		pt = DNEWCLASS(DeltaWxPoint, (new wxPoint(window->ClientToScreen(*point))));
	} else {
		WX_GETNUMBER(db_x)
		WX_GETNUMBER(db_y)
		int x = (int) db_x;
		int y = (int) db_y;
		window->ClientToScreen(&x, &y);
		pt = DNEWCLASS(DeltaWxPoint, (new wxPoint(x, y)));
	}
	WX_SETOBJECT(Point, pt)
}

WX_FUNC_ARGRANGE_START(window_close, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1) {
		bool retval = window->Close();
		WX_SETBOOL(retval);
	} else {
		WX_GETBOOL(force);
		bool retval = window->Close(force);
		WX_SETBOOL(retval);
	}
}

DLIB_FUNC_START(window_convertdialogtopixels, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
	if (DLIB_WXISBASE(Point, serial_no, point, point_wr)) {
		wxPoint *pt = (wxPoint*) point_wr->GetCastToNativeInstance();
		DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint,
						(new wxPoint(window->ConvertDialogToPixels(*pt))));
		WX_SETOBJECT(Point, retval)
	} else
	if (DLIB_WXISBASE(Size, serial_no, size, size_wr)) {
		wxSize *sz = (wxSize*) size_wr->GetCastToNativeInstance();
		DeltaWxSize *retval = DNEWCLASS(DeltaWxSize,
						(new wxSize(window->ConvertDialogToPixels(*sz))));
		WX_SETOBJECT(Size, retval)
	}
}

DLIB_FUNC_START(window_convertpixelstodialog, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
	if (DLIB_WXISBASE(Point, serial_no, point, point_wr)) {
		wxPoint *pt = (wxPoint*) point_wr->GetCastToNativeInstance();
		DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint,
						(new wxPoint(window->ConvertPixelsToDialog(*pt))));
		WX_SETOBJECT(Point, retval)
	} else
	if (DLIB_WXISBASE(Size, serial_no, size, size_wr)) {
		wxSize *sz = (wxSize*) size_wr->GetCastToNativeInstance();
		DeltaWxSize *retval = DNEWCLASS(DeltaWxSize,
						(new wxSize(window->ConvertPixelsToDialog(*sz))));
		WX_SETOBJECT(Size, retval)
	}
}

DLIB_FUNC_START(window_destroy, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	bool retval = window->Destroy();
	WX_SETBOOL(retval);
}

DLIB_FUNC_START(window_destroychildren, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	bool retval = window->DestroyChildren();
	WX_SETBOOL(retval);
}

DLIB_FUNC_START(window_disable, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	bool retval = window->Disable();
	WX_SETBOOL(retval);
}

DLIB_FUNC_START(window_doupdatewindowui, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, ev)
	window->DoUpdateWindowUI(*ev);
}
#if defined(__WXMSW__)
DLIB_FUNC_START(window_dragacceptfiles, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETBOOL(accept)
	window->DragAcceptFiles(accept);
}
#endif //__WXMSW__
WX_FUNC_ARGRANGE_START(window_enable, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1) {
		window->Enable();
	} else {
		WX_GETBOOL(enable)
		window->Enable(enable);
	}
}

DLIB_FUNC_START(window_findfocus, 0, Nil)
	WXNEWCLASS(DeltaWxWindow, retval, wxWindow, wxWindow::FindFocus());
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(window_findwindow, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxWindow *retval = (DeltaWxWindow*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(id)
		WXNEWCLASS_DEFINED(DeltaWxWindow, retval, wxWindow, window->FindWindow((long)id));
	} else
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(name)
		WXNEWCLASS_DEFINED(DeltaWxWindow, retval, wxWindow, window->FindWindow(name));
	}
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_ARGRANGE_START(window_findwindowbyid, 1, 2, Nil)
	WX_GETNUMBER(id)
	DeltaWxWindow *retval = (DeltaWxWindow*) 0;
	if (n == 1) {
		WXNEWCLASS_DEFINED(DeltaWxWindow, retval, wxWindow, wxWindow::FindWindowById(id));
	} else {
		DLIB_WXGET_BASE(window, Window, parent)
		WXNEWCLASS_DEFINED(DeltaWxWindow, retval, wxWindow, wxWindow::FindWindowById(id, parent));
	}
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_ARGRANGE_START(window_findwindowbylabel, 1, 2, Nil)
	WX_GETSTRING(label)
	DeltaWxWindow *retval = (DeltaWxWindow*) 0;
	if (n == 1) {
		WXNEWCLASS_DEFINED(DeltaWxWindow, retval, wxWindow, wxWindow::FindWindowByLabel(label));
	} else {
		DLIB_WXGET_BASE(window, Window, parent)
		WXNEWCLASS_DEFINED(DeltaWxWindow, retval, wxWindow, wxWindow::FindWindowByLabel(label, parent));
	}
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_ARGRANGE_START(window_findwindowbyname, 1, 2, Nil)
	WX_GETSTRING(name)
	DeltaWxWindow *retval = (DeltaWxWindow*) 0;
	if (n == 1) {
		WXNEWCLASS_DEFINED(DeltaWxWindow, retval, wxWindow, wxWindow::FindWindowByName(name));
	} else {
		DLIB_WXGET_BASE(window, Window, parent)
		WXNEWCLASS_DEFINED(DeltaWxWindow, retval, wxWindow, wxWindow::FindWindowByName(name, parent));
	}
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(window_fit, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->Fit();
}

DLIB_FUNC_START(window_fitinside, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->FitInside();
}

DLIB_FUNC_START(window_freeze, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->Freeze();
}

DLIB_FUNC_START(window_getacceleratortable, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WXNEWCLASS(DeltaWxAcceleratorTable, retval, wxAcceleratorTable,	window->GetAcceleratorTable());
	WX_SETOBJECT(AcceleratorTable, retval)
}

DLIB_FUNC_START(window_getbackgroundcolour, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(window->GetBackgroundColour())));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(window_getbackgroundstyle, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETNUMBER(window->GetBackgroundStyle());
}

DLIB_FUNC_START(window_geteffectiveminsize, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(window->GetEffectiveMinSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(window_getbestsize, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(window->GetBestSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(window_getcapture, 0, Nil)
	WXNEWCLASS(DeltaWxWindow, retval, wxWindow, wxWindow::GetCapture());
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(window_getcaret, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WXNEWCLASS(DeltaWxCaret, retval, wxCaret, window->GetCaret());
	WX_SETOBJECT(Caret, retval)
}

DLIB_FUNC_START(window_getcharheight, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETNUMBER(window->GetCharHeight());
}

DLIB_FUNC_START(window_getcharwidth, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETNUMBER(window->GetCharWidth());
}

DLIB_FUNC_START(window_getchildren, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	wxWindowList windowlist = window->GetChildren();
	DeltaList_Make(DLIB_RETVAL_REF);
	std::list<DeltaValue>* list = DeltaList_Get(DLIB_RETVAL_REF);
	for (wxWindowList::iterator it = windowlist.begin(); it != windowlist.end(); ++it) {
		DeltaValue val;
		wxWindow *child = *it;
		DeltaWxWindow *deltawindow = DNEWCLASS(DeltaWxWindow, (child));
		WX_SETOBJECT_EX(val, Window, deltawindow)
		list->push_back(val);
	}
}

WX_FUNC_ARGRANGE_START(window_getclassdefaultattributes, 0, 1, Nil)
	int variant = wxWINDOW_VARIANT_NORMAL;
	if (n >= 1) { WX_GETDEFINE_DEFINED(variant) }
	DeltaWxVisualAttributes *retval = DNEWCLASS(DeltaWxVisualAttributes,
		(new wxVisualAttributes(wxWindow::GetClassDefaultAttributes((wxWindowVariant)variant))));
	WX_SETOBJECT(VisualAttributes, retval)
}

DLIB_FUNC_START(window_getclientsize, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(window->GetClientSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(window_getconstraints, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WXNEWCLASS(DeltaWxLayoutConstraints, retval, wxLayoutConstraints, window->GetConstraints());
	WX_SETOBJECT(LayoutConstraints, retval)
}

DLIB_FUNC_START(window_getcontainingsizer, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WXNEWCLASS(DeltaWxSizer, retval, wxSizer, window->GetContainingSizer());
	WX_SETOBJECT(Sizer, retval)
}

DLIB_FUNC_START(window_getcursor, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxCursor *retval = DNEWCLASS(DeltaWxCursor, (new wxCursor(window->GetCursor())));
	WX_SETOBJECT(Cursor, retval)
}

DLIB_FUNC_START(window_getdefaultattributes, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxVisualAttributes *retval = DNEWCLASS(DeltaWxVisualAttributes, (
		new wxVisualAttributes(window->GetDefaultAttributes())));
	WX_SETOBJECT(VisualAttributes, retval)
}

DLIB_FUNC_START(window_getdroptarget, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxDropTarget *retval = DNEWCLASS(DeltaWxDropTarget, (window->GetDropTarget()));
	WX_SETOBJECT(DropTarget, retval)
}

DLIB_FUNC_START(window_geteventhandler, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxEvtHandler *retval = DNEWCLASS(DeltaWxEvtHandler, (window->GetEventHandler()));
	WX_SETNUMBER(window->GetExtraStyle());
}

DLIB_FUNC_START(window_getextrastyle, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETNUMBER(window->GetExtraStyle());
}

DLIB_FUNC_START(window_getfont, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(window->GetFont())));
	WX_SETOBJECT(Font, retval)
}

DLIB_FUNC_START(window_getforegroundcolour, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(window->GetForegroundColour())));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(window_getgrandparent, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WXNEWCLASS(DeltaWxWindow, retval, wxWindow, window->GetGrandParent());
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(window_gethelptextatpoint, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(point, Point, point)
	WX_GETDEFINE(origin)
	WX_SETSTRING(window->GetHelpTextAtPoint(*point, (wxHelpEvent::Origin)origin))
}

DLIB_FUNC_START(window_gethelptext, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETSTRING(window->GetHelpText())
}

DLIB_FUNC_START(window_getid, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETNUMBER(window->GetId());
}

DLIB_FUNC_START(window_getlabel, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETSTRING(window->GetLabel())
}

DLIB_FUNC_START(window_getmaxsize, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(window->GetMaxSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(window_getminsize, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(window->GetMinSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(window_getname, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETSTRING(window->GetName())
}

DLIB_FUNC_START(window_getparent, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WXNEWCLASS(DeltaWxWindow, retval, wxWindow, window->GetParent());
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(window_getposition, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(window->GetPosition())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(window_getrect, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxRect *retval = DNEWCLASS(DeltaWxRect, (new wxRect(window->GetRect())));
	WX_SETOBJECT(Rect, retval)
}

DLIB_FUNC_START(window_getscreenposition, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(window->GetScreenPosition())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(window_getscreenrect, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxRect *retval = DNEWCLASS(DeltaWxRect, (new wxRect(window->GetScreenRect())));
	WX_SETOBJECT(Rect, retval)
}

DLIB_FUNC_START(window_getscrollpos, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(orientation)
	WX_SETNUMBER(window->GetScrollPos(orientation));
}

DLIB_FUNC_START(window_getscrollrange, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(orientation)
	WX_SETNUMBER(window->GetScrollRange(orientation));
}

DLIB_FUNC_START(window_getscrollthumb, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(orientation)
	WX_SETNUMBER(window->GetScrollThumb(orientation));
}

DLIB_FUNC_START(window_getsize, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(window->GetSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(window_getsizer, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WXNEWCLASS(DeltaWxSizer, retval, wxSizer, (window->GetSizer()));
	WX_SETOBJECT(Sizer, retval)
}

WX_FUNC_ARGRANGE_START(window_gettextextent, 4, 7, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETSTRING(string)
	WX_GETTABLE(_x)
	WX_GETTABLE(_y)
	int x, y, descent, externalLeading;
	DeltaTable *_descent = (DeltaTable*) 0, *_externalLeading = (DeltaTable*) 0;
	wxFont *font = NULL;
	if (n >= 5) { WX_GETTABLE_DEFINED(_descent) }
	if (n >= 6) { WX_GETTABLE_DEFINED(_externalLeading) }
	if (n >= 7) { DLIB_WXGET_BASE(font, Font, _font) font = _font; }

	window->GetTextExtent(string, &x, &y, &descent, &externalLeading, font);
	WX_SETTABLE_RETVAL(_x, DeltaValue((DeltaNumberValueType)x));
	WX_SETTABLE_RETVAL(_y, DeltaValue((DeltaNumberValueType)y));
	if (n >= 5) { WX_SETTABLE_RETVAL(_descent, DeltaValue((DeltaNumberValueType)descent)); }
	if (n >= 6) { WX_SETTABLE_RETVAL(_externalLeading, DeltaValue((DeltaNumberValueType)externalLeading)); }
}

DLIB_FUNC_START(window_gettooltip, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxToolTip *retval = DNEWCLASS(DeltaWxToolTip, (window->GetToolTip()));
	WX_SETOBJECT(ToolTip, retval)
}

DLIB_FUNC_START(window_getupdateregion, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxRegion *retval = DNEWCLASS(DeltaWxRegion, (new wxRegion(window->GetUpdateRegion())));
	WX_SETOBJECT(Region, retval)
}

DLIB_FUNC_START(window_getvalidator, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxValidator *retval = DNEWCLASS(DeltaWxValidator, (window->GetValidator()));
	WX_SETOBJECT(Validator, retval)
}

DLIB_FUNC_START(window_getvirtualsize, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(window->GetVirtualSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(window_getwindowbordersize, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(window->GetWindowBorderSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(window_getwindowstyleflag, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETNUMBER((double)window->GetWindowStyleFlag());
}

DLIB_FUNC_START(window_getwindowvariant, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETNUMBER(window->GetWindowVariant())
}

DLIB_FUNC_START(window_hascapture, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->HasCapture());
}

DLIB_FUNC_START(window_hasflag, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(flag)
	WX_SETBOOL(window->HasFlag(flag));
}

DLIB_FUNC_START(window_hasmultiplepages, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->HasMultiplePages());
}

DLIB_FUNC_START(window_hasscrollbar, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(orientation)
	WX_SETBOOL(window->HasScrollbar(orientation));
}

DLIB_FUNC_START(window_hastransparentbackground, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->HasTransparentBackground());
}

DLIB_FUNC_START(window_hide, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->Hide());
}

DLIB_FUNC_START(window_inheritattributes, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->InheritAttributes();
}

DLIB_FUNC_START(window_initdialog, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->InitDialog();
}

DLIB_FUNC_START(window_invalidatebestsize, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->InvalidateBestSize();
}

DLIB_FUNC_START(window_isdoublebuffered, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->IsDoubleBuffered());
}

DLIB_FUNC_START(window_isenabled, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->IsEnabled());
}

WX_FUNC_ARGRANGE_START(window_isexposed, 1, 5, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 3) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_SETBOOL(window->IsExposed(x, y));
	} else if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(w)
		WX_GETNUMBER(h)
		WX_SETBOOL(window->IsExposed(x, y, w, h));
	} else if (n == 2) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Point, serial_no, point, point_wr)) {
			wxPoint *pt = (wxPoint*) point_wr->GetCastToNativeInstance();
			WX_SETBOOL(window->IsExposed(*pt));
		} else
		if (DLIB_WXISBASE(Rect, serial_no, rect, rect_wr)) {
			wxRect *rect = (wxRect*) rect_wr->GetCastToNativeInstance();
			WX_SETBOOL(window->IsExposed(*rect))
		}
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		return;
	}
}

DLIB_FUNC_START(window_isfrozen, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->IsFrozen());
}

DLIB_FUNC_START(window_isretained, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->IsRetained());
}

DLIB_FUNC_START(window_isshown, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->IsShown());
}

DLIB_FUNC_START(window_isshownonscreen, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->IsShownOnScreen());
}

DLIB_FUNC_START(window_istoplevel, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->IsTopLevel());
}

DLIB_FUNC_START(window_layout, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->Layout();
}

DLIB_FUNC_START(window_linedown, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->LineDown());
}

DLIB_FUNC_START(window_lineup, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->LineUp());
}

DLIB_FUNC_START(window_lower, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->Lower();
}

DLIB_FUNC_START(window_makemodal, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETBOOL(flag)
	window->MakeModal(flag);
}

WX_FUNC_ARGRANGE_START(window_move, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 2) {
		DLIB_WXGET_BASE(point, Point, pt)
		window->Move(*pt);
	} else {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		window->Move(x, y);
	}
}

DLIB_FUNC_START(window_moveafterintaborder, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(window, Window, win)
	window->MoveAfterInTabOrder(win);
}

DLIB_FUNC_START(window_movebeforeintaborder, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(window, Window, win)
	window->MoveBeforeInTabOrder(win);
}

WX_FUNC_ARGRANGE_START(window_navigate, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1) {
		WX_SETBOOL(window->Navigate());
	} else {
		WX_GETDEFINE(flags)
		WX_SETBOOL(window->Navigate(flags));
	}
}

DLIB_FUNC_START(window_oninternalidle, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->OnInternalIdle();
}

DLIB_FUNC_START(window_pagedown, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->PageDown());
}

DLIB_FUNC_START(window_pageup, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->PageUp());
}

WX_FUNC_ARGRANGE_START(window_popeventhandler, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	bool deleteHandler = false;
	if (n >= 2) { WX_GETBOOL_DEFINED(deleteHandler) }
	DeltaWxEvtHandler *retval = DNEWCLASS(DeltaWxEvtHandler, (window->PopEventHandler(deleteHandler)));
	WX_SETOBJECT(EvtHandler, retval)
}

WX_FUNC_ARGRANGE_START(window_popupmenu, 2, 4, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(menu, Menu, menu)
	if (n == 4) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_SETBOOL(window->PopupMenu(menu, x, y))
	} else {
		wxPoint pos = wxDefaultPosition;
		if (n == 3) { DLIB_WXGET_BASE(point, Point, pt) pos = *pt; }
		WX_SETBOOL(window->PopupMenu(menu, pos))
	}
}

DLIB_FUNC_START(window_pusheventhandler, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	window->PushEventHandler(handler);
}

DLIB_FUNC_START(window_raise, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->Raise();
}

WX_FUNC_ARGRANGE_START(window_refresh, 1, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1) {
		window->Refresh();
	} else if (n == 2) {
		WX_GETBOOL(eraseBackground)
		window->Refresh(eraseBackground);
	} else {
		WX_GETBOOL(eraseBackground)
		DLIB_WXGET_BASE(rect, Rect, rect)
		window->Refresh(eraseBackground, rect);
	}
}

WX_FUNC_ARGRANGE_START(window_refreshrect, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(rect, Rect, rect)
	if (n == 2) {
		window->RefreshRect(*rect);
	} else {
		WX_GETBOOL(eraseBackground)
		window->RefreshRect(*rect, eraseBackground);
	}
}
#if defined(__WXMSW__)
DLIB_FUNC_START(window_registerhotkey, 4, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETNUMBER(hotkeyId)
	WX_GETDEFINE(modifiers)
	WX_GETDEFINE(virtualKeyCode)
	bool retval = window->RegisterHotKey(hotkeyId, modifiers, virtualKeyCode);
	WX_SETBOOL(retval);
}

DLIB_FUNC_START(window_unregisterhotkey, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(hotkeyId)
	WX_SETBOOL(window->UnregisterHotKey(hotkeyId));
}
#endif //__WXMSW__
DLIB_FUNC_START(window_releasemouse, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->ReleaseMouse();
}

DLIB_FUNC_START(window_removechild, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(window, Window, child)
	window->RemoveChild(child);
}

DLIB_FUNC_START(window_removeeventhandler, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	WX_SETBOOL(window->RemoveEventHandler(handler))
}

DLIB_FUNC_START(window_reparent, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(window, Window, newParent)
	window->Reparent(newParent);
}

WX_FUNC_ARGRANGE_START(window_screentoclient, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DeltaWxPoint *retval = (DeltaWxPoint*) 0;
	if (n == 2) {
		DLIB_WXGET_BASE(point, Point, pt)
		retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(window->ScreenToClient(*pt))));
	} else {
		WX_GETNUMBER(db_x)
		WX_GETNUMBER(db_y)
		int x = (int) db_x;
		int y = (int) db_y;
		window->ScreenToClient(&x, &y);
		retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(x, y)));
	}
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(window_scrolllines, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETNUMBER(lines)
	WX_SETBOOL(window->ScrollLines(lines));
}

DLIB_FUNC_START(window_scrollpages, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETNUMBER(pages)
	WX_SETBOOL(window->ScrollPages(pages));
}

WX_FUNC_ARGRANGE_START(window_scrollwindow, 3, 4, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	if (n == 3) {
		window->ScrollWindow(x, y);
	} else {
		DLIB_WXGET_BASE(rect, Rect, rect)
		window->ScrollWindow(x, y, rect);
	}
}

DLIB_FUNC_START(window_setacceleratortable, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(acceleratortable, AcceleratorTable, accel)
	window->SetAcceleratorTable(*accel);
}

DLIB_FUNC_START(window_setautolayout, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETBOOL(autoLayout)
	window->SetAutoLayout(autoLayout);
}

DLIB_FUNC_START(window_setbackgroundcolour, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(colour, Colour, colour)
	WX_SETBOOL(window->SetBackgroundColour(*colour));
}

DLIB_FUNC_START(window_setbackgroundstyle, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(style)
	window->SetBackgroundStyle((wxBackgroundStyle) style);
}

WX_FUNC_ARGRANGE_START(window_setinitialsize, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1)
		window->SetInitialSize();
	else {
		DLIB_WXGET_BASE(size, Size, size)
		window->SetInitialSize(*size);
	}
}

DLIB_FUNC_START(window_setcaret, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(caret, Caret, caret)
	window->SetCaret(caret);
}

WX_FUNC_ARGRANGE_START(window_setclientsize, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 2) {
		DLIB_WXGET_BASE(size, Size, size)
		window->SetClientSize(*size);
	} else {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		window->SetClientSize(width, height);
	}
}

DLIB_FUNC_START(window_setconstraints, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, constraints)
	window->SetConstraints(constraints);
}

DLIB_FUNC_START(window_setcontainingsizer, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	window->SetContainingSizer(sizer);
}

DLIB_FUNC_START(window_setcursor, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(cursor, Cursor, cursor)
	WX_SETBOOL(window->SetCursor(*cursor));
}

DLIB_FUNC_START(window_setdroptarget, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(droptarget, DropTarget, target)
	window->SetDropTarget(target);
}

DLIB_FUNC_START(window_seteventhandler, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
	window->SetEventHandler(handler);
}

DLIB_FUNC_START(window_setextrastyle, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(exStyle)
	window->SetExtraStyle(exStyle);
}

DLIB_FUNC_START(window_setfocus, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->SetFocus();
}

DLIB_FUNC_START(window_setfocusfromkbd, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->SetFocusFromKbd();
}

DLIB_FUNC_START(window_setfont, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(font, Font, font)
	window->SetFont(*font);
}

DLIB_FUNC_START(window_setforegroundcolour, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(colour, Colour, colour)
	window->SetForegroundColour(*colour);
}

DLIB_FUNC_START(window_sethelptext, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETSTRING(helpText)
	window->SetHelpText(helpText);
}

DLIB_FUNC_START(window_setid, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(id)
	window->SetId(id);
}

DLIB_FUNC_START(window_setlabel, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETSTRING(label)
	window->SetLabel(label);
}

DLIB_FUNC_START(window_setmaxsize, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(size, Size, size)
	window->SetMaxSize(*size);
}

DLIB_FUNC_START(window_setminsize, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(size, Size, size)
	window->SetMinSize(*size);
}

DLIB_FUNC_START(window_setname, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETSTRING(name)
	window->SetName(name);
}

DLIB_FUNC_START(window_setownbackgroundcolour, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(colour, Colour, colour)
	window->SetOwnBackgroundColour(*colour);
}

DLIB_FUNC_START(window_setownfont, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(font, Font, font)
	window->SetOwnFont(*font);
}

DLIB_FUNC_START(window_setownforegroundcolour, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(colour, Colour, colour)
	window->SetOwnForegroundColour(*colour);
}

WX_FUNC_ARGRANGE_START(window_setscrollbar, 5, 6, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(orientation)
	WX_GETNUMBER(position)
	WX_GETNUMBER(thumbSize)
	WX_GETNUMBER(range)
	if (n == 5) {
		window->SetScrollbar(orientation, position, thumbSize, range);
	} else {
		WX_GETBOOL(refresh)
		window->SetScrollbar(orientation, position, thumbSize, range, refresh);
	}
}

WX_FUNC_ARGRANGE_START(window_setscrollpos, 3, 4, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(orientation)
	WX_GETNUMBER(position)
	if (n == 3) {
		window->SetScrollPos(orientation, position);
	} else {
		WX_GETBOOL(refresh)
		window->SetScrollPos(orientation, position, refresh);
	}
}

WX_FUNC_ARGRANGE_START(window_setsize, 2, 6, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 3) {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		window->SetSize(width, height);
	} else if (n >= 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		if (n == 5)
			window->SetSize(x, y, width, height);
		else {
			WX_GETDEFINE(sizeFlags)
			window->SetSize(x, y, width, height, sizeFlags);
		}
	} else if (n == 2) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Size, serial_no, size, size_wr)) {
			wxSize *size = (wxSize*) size_wr->GetCastToNativeInstance();
			window->SetSize(*size);
		} else
		if (DLIB_WXISBASE(Rect, serial_no, rect, rect_wr)) {
			wxRect *rect = (wxRect*) rect_wr->GetCastToNativeInstance();
			window->SetSize(*rect);
		}
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		return;
	}
}

WX_FUNC_ARGRANGE_START(window_setsizer, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	bool deleteOld = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(deleteOld) }
	window->SetSizer(sizer, deleteOld);
}

WX_FUNC_ARGRANGE_START(window_setsizerandfit, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(sizer, Sizer, sizer)
	bool deleteOld = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(deleteOld) }
	window->SetSizerAndFit(sizer, deleteOld);
}

DLIB_FUNC_START(window_setthemeenabled, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETBOOL(enable)
	window->SetThemeEnabled(enable);
}

DLIB_FUNC_START(window_settooltip, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(tip)
		window->SetToolTip(tip);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(tooltip, ToolTip, tip)
		window->SetToolTip(tip);
	}
}

DLIB_FUNC_START(window_setvalidator, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	DLIB_WXGET_BASE(validator, Validator, validator)
	window->SetValidator(*validator);
}

WX_FUNC_ARGRANGE_START(window_setvirtualsize, 2, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 2) {
		DLIB_WXGET_BASE(size, Size, size)
		window->SetVirtualSize(*size);
	} else {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		window->SetVirtualSize(width, height);
	}
}

WX_FUNC_ARGRANGE_START(window_setvirtualsizehints, 2, 5, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (DPTR(vm)->GetActualArg(1)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(size, Size, minSize)
		if (n == 2)
			window->SetVirtualSizeHints(*minSize);
		else if (n == 3) {
			DLIB_WXGET_BASE(size, Size, maxSize)
			window->SetVirtualSizeHints(*minSize, *maxSize);
		}
	} else if (DPTR(vm)->GetActualArg(1)->Type() ==DeltaValue_Number) {
		WX_GETNUMBER(minW)
		WX_GETNUMBER(minH)
		if (n == 3)
			window->SetVirtualSizeHints(minW, minH);
		else if (n == 4) {
			WX_GETNUMBER(maxW)
			window->SetVirtualSizeHints(minW, minH, maxW);
		} else if (n == 5) {
			WX_GETNUMBER(maxW)
			WX_GETNUMBER(maxH)
			window->SetVirtualSizeHints(minW, minH, maxW, maxH);
		}
	}
}

DLIB_FUNC_START(window_setwindowstyle, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(style)
	window->SetWindowStyle(style);
}

DLIB_FUNC_START(window_setwindowstyleflag, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(style)
	window->SetWindowStyleFlag(style);
}

DLIB_FUNC_START(window_setwindowvariant, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(variant)
	window->SetWindowVariant((wxWindowVariant) variant);
}

DLIB_FUNC_START(window_shouldinheritcolours, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->ShouldInheritColours());
}

WX_FUNC_ARGRANGE_START(window_show, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1) {
		WX_SETBOOL(window->Show());
	} else {
		WX_GETBOOL(show)
		WX_SETBOOL(window->Show(show));
	}
}

DLIB_FUNC_START(window_thaw, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->Thaw();
}

DLIB_FUNC_START(window_togglewindowstyle, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETDEFINE(flag)
	WX_SETBOOL(window->ToggleWindowStyle(flag));
}

DLIB_FUNC_START(window_transferdatafromwindow, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->TransferDataFromWindow());
}

DLIB_FUNC_START(window_transferdatatowindow, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->TransferDataToWindow());
}

DLIB_FUNC_START(window_update, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	window->Update();
}

WX_FUNC_ARGRANGE_START(window_updatewindowui, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 1)
		window->UpdateWindowUI();
	else {
		WX_GETDEFINE(flags)
		window->UpdateWindowUI(flags);
	}
}

DLIB_FUNC_START(window_validate, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(window->Validate());
}

DLIB_FUNC_START(window_warppointer, 3, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	window->WarpPointer(x, y);
}
