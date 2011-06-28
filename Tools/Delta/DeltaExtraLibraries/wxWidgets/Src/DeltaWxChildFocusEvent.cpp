#include "DeltaWxChildFocusEvent.h"
#include "DeltaWxCommandEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(childfocusevent, name)
#define WX_FUNC(name) WX_FUNC1(childfocusevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getwindow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getwindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "getwindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ChildFocusEvent, "childfocusevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ChildFocusEvent, val, childfocusevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxCommandEvent *_parent = DLIB_WXTYPECAST_BASE(CommandEvent, val, commandevent);
	DeltaWxCommandEvent *parent = DNEWCLASS(DeltaWxCommandEvent, (_parent));
	WX_SETOBJECT_EX(*at, CommandEvent, parent)
	return true;
}

static bool GetWindow (void* val, DeltaValue* at) 
{
	wxChildFocusEvent *ev = DLIB_WXTYPECAST_BASE(ChildFocusEvent, val, childfocusevent);
	wxWindow *win = ev->GetWindow();
	DeltaWxWindow *retval = win ? DNEWCLASS(DeltaWxWindow, (win)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "window",				&GetWindow,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ChildFocusEvent, childfocusevent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(childfocusevent_construct, 0, 1, Nil)
	wxWindow *win = NULL;
	if (n >= 1) { DLIB_WXGET_BASE(window, Window, window) win = window; }
	DeltaWxChildFocusEvent *evt = DNEWCLASS(DeltaWxChildFocusEvent, (new wxChildFocusEvent(win)));
	WX_SETOBJECT(ChildFocusEvent, evt)
}

DLIB_FUNC_START(childfocusevent_destruct, 1, Nil)
	DLIB_WXDELETE(childfocusevent, ChildFocusEvent, evt)
}

DLIB_FUNC_START(childfocusevent_getwindow, 1, Nil)
	DLIB_WXGET_BASE(childfocusevent, ChildFocusEvent, evt)
	DeltaWxWindow *retval = DNEWCLASS(DeltaWxWindow, (evt->GetWindow()));
	WX_SETOBJECT(Window, retval)
}
