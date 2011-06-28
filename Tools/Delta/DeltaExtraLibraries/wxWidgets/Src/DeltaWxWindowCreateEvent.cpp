#include "DeltaWxWindowCreateEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(windowcreateevent, name)
#define WX_FUNC(name) WX_FUNC1(windowcreateevent, name)

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

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(WindowCreateEvent, "windowcreateevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(WindowCreateEvent, val, windowcreateevent) ?
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
	wxWindowCreateEvent *ev = DLIB_WXTYPECAST_BASE(WindowCreateEvent, val, windowcreateevent);
	wxWindow *win = ev->GetWindow();
	DeltaWxWindow *retval = win ? DNEWCLASS(DeltaWxWindow, (win)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "window",				&GetWindow,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(WindowCreateEvent, windowcreateevent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(windowcreateevent_construct, 0, 1, Nil)
	wxWindow *win = NULL;
	if (n >= 1) { DLIB_WXGET_BASE(window, Window, window) win = window; }
	DeltaWxWindowCreateEvent *evt = DNEWCLASS(DeltaWxWindowCreateEvent,
		(new wxWindowCreateEvent(win)));
	WX_SETOBJECT(WindowCreateEvent, evt)
}

DLIB_FUNC_START(windowcreateevent_destruct, 1, Nil)
	DLIB_WXDELETE(windowcreateevent, WindowCreateEvent, evt)
}

DLIB_FUNC_START(windowcreateevent_getwindow, 1, Nil)
	DLIB_WXGET_BASE(windowcreateevent, WindowCreateEvent, evt)
	DeltaWxWindow *retval = DNEWCLASS(DeltaWxWindow, (evt->GetWindow()));
	WX_SETOBJECT(Window, retval)
}
