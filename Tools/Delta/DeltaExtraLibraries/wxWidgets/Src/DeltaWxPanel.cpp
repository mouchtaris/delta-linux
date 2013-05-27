#include "DeltaWxPanel.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxSysColourChangedEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(panel, name)
#define WX_FUNC(name) WX_FUNC1(panel, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(initdialog)
WX_FUNC_DEF(setfocus)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(initdialog),
	WX_FUNC(setfocus)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setfocus")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Panel, "panel", Window)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Panel, val, panel) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxWindow *_parent = DLIB_WXTYPECAST_BASE(Window, val, window);
	DeltaWxWindow *parent = DNEWCLASS(DeltaWxWindow, (_parent));
	WX_SETOBJECT_EX(*at, Window, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Window",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Panel,panel)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(panel_construct, 0, 6, Nil)
	wxPanel* wxpanel = (wxPanel*) 0;
	DeltaWxPanel* panel = (DeltaWxPanel*) 0;
	if (n == 0)
		wxpanel = new wxPanel();
	else {
		DLIB_WXGET_BASE(window, Window, parent)
		int winid = wxID_ANY;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxTAB_TRAVERSAL | wxNO_BORDER;
		wxString name = wxPanelNameStr;
		if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
		if (n >= 3) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(sz) size = *sz; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { WX_GETSTRING_DEFINED(name) }
		wxpanel = new wxPanel(parent, winid, pos, size, style, name);
	}
	if (wxpanel) panel = DNEWCLASS(DeltaWxPanel, (wxpanel));
	WX_SETOBJECT(Panel, panel)
}

DLIB_FUNC_START(panel_destruct, 1, Nil)
	DLIB_WXDELETE(panel, Panel, panel)
}

WX_FUNC_ARGRANGE_START(panel_create, 2, 7, Nil)
	DLIB_WXGET_BASE(panel, Panel, panel)
	DLIB_WXGET_BASE(window, Window, parent)
	int winid = wxID_ANY;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxTAB_TRAVERSAL | wxNO_BORDER;
	wxString name = wxPanelNameStr;
	if (n >= 3) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 4) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(sz) size = *sz; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(panel->Create(parent, winid, pos, size, style, name))
}

DLIB_FUNC_START(panel_initdialog, 1, Nil)
	DLIB_WXGET_BASE(panel, Panel, panel)
	panel->InitDialog();
}

DLIB_FUNC_START(panel_setfocus, 1, Nil)
	DLIB_WXGET_BASE(panel, Panel, panel)
	panel->SetFocus();
}
