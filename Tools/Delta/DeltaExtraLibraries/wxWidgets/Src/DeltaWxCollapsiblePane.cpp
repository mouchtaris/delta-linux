#include "DeltaWxCollapsiblePane.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxValidator.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(collapsiblepane, name)
#define WX_FUNC(name) WX_FUNC1(collapsiblepane, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(iscollapsed)
WX_FUNC_DEF(isexpanded)
WX_FUNC_DEF(collapse)
WX_FUNC_DEF(expand)
WX_FUNC_DEF(getpane)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(iscollapsed),
	WX_FUNC(isexpanded),
	WX_FUNC(collapse),
	WX_FUNC(expand),
	WX_FUNC(getpane)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "getpane")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CollapsiblePane, "collapsiblepane", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CollapsiblePane, val, collapsiblepane) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetIsCollapsed (void* val, DeltaValue* at) 
{
	wxCollapsiblePane *pane = DLIB_WXTYPECAST_BASE(CollapsiblePane, val, collapsiblepane);
	WX_SETBOOL_EX(*at, pane->IsCollapsed())
	return true;
}

static bool GetIsExpanded (void* val, DeltaValue* at) 
{
	wxCollapsiblePane *pane = DLIB_WXTYPECAST_BASE(CollapsiblePane, val, collapsiblepane);
	WX_SETBOOL_EX(*at, pane->IsExpanded())
	return true;
}

static bool GetPane (void* val, DeltaValue* at) 
{
	wxCollapsiblePane *pane = DLIB_WXTYPECAST_BASE(CollapsiblePane, val, collapsiblepane);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, pane->GetPane())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isCollapsed",		&GetIsCollapsed,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isExpanded",			&GetIsExpanded,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pane",				&GetPane,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(CollapsiblePane,collapsiblepane)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(collapsiblepane_construct, 0, 8, Nil)
	wxCollapsiblePane *pane = (wxCollapsiblePane*) 0;
	if (n == 0) {
		pane = new wxCollapsiblePane();
	} else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(winid)
		WX_GETSTRING(label)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxCP_DEFAULT_STYLE;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxCollapsiblePaneNameStr;
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		pane = new wxCollapsiblePane(parent, winid, label, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(CollapsiblePane, pane)
}

WX_FUNC_ARGRANGE_START(collapsiblepane_create, 4, 9, Nil)
	DLIB_WXGET_BASE(collapsiblepane, CollapsiblePane, pane)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(winid)
	WX_GETSTRING(label)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxCP_DEFAULT_STYLE;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxCollapsiblePaneNameStr;
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(pane->Create(parent, winid, label, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(pane);
}

WX_FUNC_START(collapsiblepane_iscollapsed, 1, Nil)
	DLIB_WXGET_BASE(collapsiblepane, CollapsiblePane, pane)
	WX_SETBOOL(pane->IsCollapsed())
}

WX_FUNC_START(collapsiblepane_isexpanded, 1, Nil)
	DLIB_WXGET_BASE(collapsiblepane, CollapsiblePane, pane)
	WX_SETBOOL(pane->IsExpanded())
}

WX_FUNC_ARGRANGE_START(collapsiblepane_collapse, 1, 2, Nil)
	DLIB_WXGET_BASE(collapsiblepane, CollapsiblePane, pane)
	bool collapse = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(collapse) }
	pane->Collapse(collapse);
}

WX_FUNC_START(collapsiblepane_expand, 1, Nil)
	DLIB_WXGET_BASE(collapsiblepane, CollapsiblePane, pane)
	pane->Expand();
}

WX_FUNC_START(collapsiblepane_getpane, 1, Nil)
	DLIB_WXGET_BASE(collapsiblepane, CollapsiblePane, pane)
	WX_SETOBJECT(Window, pane->GetPane())
}
