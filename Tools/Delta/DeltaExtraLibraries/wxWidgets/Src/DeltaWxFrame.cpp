#include "DeltaWxFrame.h"
#include "DeltaWxTopLevelWindow.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxStatusBar.h"
#include "DeltaWxMenuBar.h"
#include "DeltaWxToolBar.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(frame, name)
#define WX_FUNC(name) WX_FUNC1(frame, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(centre)
WX_FUNC_DEF(createstatusbar)
WX_FUNC_DEF(createtoolbar)
WX_FUNC_DEF(getclientareaorigin)
WX_FUNC_DEF(getmenubar)
WX_FUNC_DEF(getstatusbar)
WX_FUNC_DEF(getstatusbarpane)
WX_FUNC_DEF(gettoolbar)
WX_FUNC_DEF(processcommand)
WX_FUNC_DEF(sendsizeevent)
WX_FUNC_DEF(setmenubar)
WX_FUNC_DEF(setstatusbar)
WX_FUNC_DEF(setstatusbarpane)
WX_FUNC_DEF(setstatustext)
WX_FUNC_DEF(setstatuswidths)
WX_FUNC_DEF(settoolbar)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(centre),
	WX_FUNC(createstatusbar),
	WX_FUNC(createtoolbar),
	WX_FUNC(getclientareaorigin),
	WX_FUNC(getmenubar),
	WX_FUNC(getstatusbar),
	WX_FUNC(getstatusbarpane),
	WX_FUNC(gettoolbar),
	WX_FUNC(processcommand),
	WX_FUNC(sendsizeevent),
	WX_FUNC(setmenubar),
	WX_FUNC(setstatusbar),
	WX_FUNC(setstatusbarpane),
	WX_FUNC(setstatustext),
	WX_FUNC(setstatuswidths),
	WX_FUNC(settoolbar)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "settoolbar")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Frame, "frame", TopLevelWindow)

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Frame, val, frame) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxTopLevelWindow *_parent = DLIB_WXTYPECAST_BASE(TopLevelWindow, val, toplevelwindow);
	DeltaWxTopLevelWindow *parent = DNEWCLASS(DeltaWxTopLevelWindow, (_parent));
	WX_SETOBJECT_EX(*at, TopLevelWindow, parent)
	return true;
}

static bool GetMenuBar (void* val, DeltaValue* at) 
{
	wxFrame *frame = DLIB_WXTYPECAST_BASE(Frame, val, frame);
	wxMenuBar *menubar = frame->GetMenuBar();
	DeltaWxMenuBar *retval = menubar ? DNEWCLASS(DeltaWxMenuBar, (menubar)) : (DeltaWxMenuBar*) 0;
	WX_SETOBJECT_EX(*at, MenuBar, retval)
	return true;
}

static bool GetStatusBar (void* val, DeltaValue* at) 
{
	wxFrame *frame = DLIB_WXTYPECAST_BASE(Frame, val, frame);
	wxStatusBar *statusbar = frame->GetStatusBar();
	DeltaWxStatusBar *retval = statusbar ? DNEWCLASS(DeltaWxStatusBar, (statusbar)) : (DeltaWxStatusBar*) 0;
	WX_SETOBJECT_EX(*at, StatusBar, retval)
	return true;
}

static bool GetStatusBarPane (void* val, DeltaValue* at) 
{
	wxFrame *frame = DLIB_WXTYPECAST_BASE(Frame, val, frame);
	WX_SETNUMBER_EX(*at, frame->GetStatusBarPane())
	return true;
}

static bool GetToolBar (void* val, DeltaValue* at) 
{
	wxFrame *frame = DLIB_WXTYPECAST_BASE(Frame, val, frame);
	wxToolBar *toolbar = frame->GetToolBar();
	DeltaWxToolBar *retval = toolbar ? DNEWCLASS(DeltaWxToolBar, (toolbar)) : (DeltaWxToolBar*) 0;
	WX_SETOBJECT_EX(*at, ToolBar, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "TopLevelWindow",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "frameMenuBar",		&GetMenuBar,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "frameStatusBar",		&GetStatusBar,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "statusBarPane",		&GetStatusBarPane,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "frameToolBar",		&GetToolBar,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Frame,frame)

////////////////////////////////////////////////////////////////
// TODO: move this proper location and use the 
// WX_SET_WINDOW_OBJECT macro to create adapter-aware objects
template<typename T>
class WrapperDestroyEventHandler : public wxEvtHandler {
public:
	void OnDestroy(wxWindowDestroyEvent& event)
		{ DDELETE(static_cast<T*>(event.GetWindow()->GetClientData())); }
	static WrapperDestroyEventHandler* Instance(void) {
		static WrapperDestroyEventHandler<T> instance;
		return &instance;
	}
};
 #define WX_SET_WINDOW_OBJECT(_wxclass, _var, _wxvar)												\
	if (_wxvar) {																					\
		_var = DNEWCLASS(DeltaWx##_wxclass, (_wxvar));												\
		_wxvar->SetClientData(_var);																\
		_wxvar->Connect(																			\
			wxEVT_DESTROY,																			\
			wxWindowDestroyEventHandler(WrapperDestroyEventHandler<DeltaWx##_wxclass>::OnDestroy),	\
			NULL,																					\
			WrapperDestroyEventHandler<DeltaWx##_wxclass>::Instance()								\
		);																							\
	}																								\
	WX_SETOBJECT(_wxclass, _var)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(frame_construct, 0, 7, Nil)
	wxFrame *wxframe = (wxFrame*) 0;
	DeltaWxFrame *frame = (DeltaWxFrame*) 0;
	if (n == 0)
		wxframe = new wxFrame();
	else if (n >= 3 && n <= 7) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		WX_GETSTRING(title)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxDEFAULT_FRAME_STYLE;
		wxString name = wxFrameNameStr;
		if (n >= 4) {
			DLIB_WXGETPOINT_BASE(point)
			pos = *point;
		}
		if (n >= 5) {
			DLIB_WXGETSIZE_BASE(sz)
			size = *sz;
		}
		if (n >= 6) {
			WX_GETDEFINE(wxVar)
			style = (long) wxVar;
		}
		if (n == 7) {
			WX_GETSTRING(wxVar)
			name = wxVar;
		}
		wxframe = new wxFrame(parent, id, title, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			DPTR(vm)->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(Frame, frame, wxframe)
}

DLIB_FUNC_START(frame_destruct, 1, Nil)
	DLIB_WXDELETE(frame, Frame, frame)
}

WX_FUNC_ARGRANGE_START(frame_create, 4, 8, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	WX_GETSTRING(title)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxDEFAULT_FRAME_STYLE;
	wxString name = wxFrameNameStr;
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(frame->Create(parent, id, title, pos, size, style, name))
}

WX_FUNC_ARGRANGE_START(frame_centre, 1, 2, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	if (n == 1) {
		frame->Centre();
	} else {
		WX_GETDEFINE(direction)
		frame->Centre(direction);
	}
}

WX_FUNC_ARGRANGE_START(frame_createstatusbar, 1, 5, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	int number = 1;
	long style = 0x0010 | 0x00010000;
	int id = 0;
	DeltaWxStatusBar *retval = (DeltaWxStatusBar*) 0;
	if (n >= 2) { WX_GETNUMBER_DEFINED(number) }
	if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 4) { WX_GETDEFINE_DEFINED(id) }
	if (n < 5) {
		retval = DNEWCLASS(DeltaWxStatusBar, (frame->CreateStatusBar(
			number, style, id)));
	} else {
		WX_GETSTRING(name)
		retval = DNEWCLASS(DeltaWxStatusBar, (frame->CreateStatusBar(
			number, style, id, name)));
	}
	WX_SETOBJECT(StatusBar, retval)
}

WX_FUNC_ARGRANGE_START(frame_createtoolbar, 1, 4, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	long style = -1;
	int winid = wxID_ANY;
	wxString name = wxToolBarNameStr;
	if (n >= 2) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 3) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 4) { WX_GETSTRING_DEFINED(name) }
	DeltaWxToolBar *retval = DNEWCLASS(DeltaWxToolBar, (frame->CreateToolBar(style, winid, name)));
	WX_SETOBJECT(ToolBar, retval)
}

DLIB_FUNC_START(frame_getclientareaorigin, 1, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(frame->GetClientAreaOrigin())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(frame_getmenubar, 1, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	WXNEWCLASS(DeltaWxMenuBar, retval, wxMenuBar, frame->GetMenuBar());
	WX_SETOBJECT(MenuBar, retval)
}

DLIB_FUNC_START(frame_getstatusbar, 1, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	WXNEWCLASS(DeltaWxStatusBar, retval, wxStatusBar, frame->GetStatusBar());
	WX_SETOBJECT(StatusBar, retval)
}

DLIB_FUNC_START(frame_getstatusbarpane, 1, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	WX_SETNUMBER(frame->GetStatusBarPane())
}

DLIB_FUNC_START(frame_gettoolbar, 1, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	WXNEWCLASS(DeltaWxToolBar, retval, wxToolBar, frame->GetToolBar());
	WX_SETOBJECT(ToolBar, retval)
}

DLIB_FUNC_START(frame_processcommand, 2, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	WX_GETNUMBER(id)
	frame->ProcessCommand(id);
}

DLIB_FUNC_START(frame_sendsizeevent, 1, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	frame->SendSizeEvent();
}

DLIB_FUNC_START(frame_setmenubar, 2, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	DLIB_WXGET_BASE(menubar, MenuBar, mbar)
	frame->SetMenuBar(mbar);
}

DLIB_FUNC_START(frame_setstatusbar, 2, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	frame->SetStatusBar(stbar);
}

DLIB_FUNC_START(frame_setstatusbarpane, 2, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	WX_GETNUMBER(n)
	frame->SetStatusBarPane(n);
}

WX_FUNC_ARGRANGE_START(frame_setstatustext, 2, 3, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	WX_GETSTRING(text)
	if (n == 2) {
		frame->SetStatusText(text);
	} else {
		WX_GETNUMBER(n)
		frame->SetStatusText(text, n);
	}
}

DLIB_FUNC_START(frame_setstatuswidths, 3, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	WX_GETNUMBER(n)
	WX_GETTABLE(d_widths)
	int d_widths_size = d_widths->Total();
	int *widths = DNEWARR(int, d_widths_size);
	DeltaValue content;
	for (int i = 0; i < d_widths_size; ++i) {
		d_widths->Get(DeltaValue((DeltaNumberValueType)i), &content);
		if (content.Type() == DeltaValue_Number)
			widths[i] = (int)content.ToNumber();
	}
	frame->SetStatusWidths(n, widths);
	DDELARR(widths);
}

DLIB_FUNC_START(frame_settoolbar, 2, Nil)
	DLIB_WXGET_BASE(frame, Frame, frame)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	frame->SetToolBar(toolbar);
}
