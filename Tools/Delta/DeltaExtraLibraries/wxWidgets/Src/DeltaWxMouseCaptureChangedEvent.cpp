#include "DeltaWxMouseCaptureChangedEvent.h"
#include "DeltaWxEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(mousecapturechangedevent, name)
#define WX_FUNC(name) WX_FUNC1(mousecapturechangedevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getcapturedwindow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getcapturedwindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getcapturedwindow", "getcapturedwindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MouseCaptureChangedEvent, "mousecapturechangedevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MouseCaptureChangedEvent, val, mousecapturechangedevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetCapturedWindow (void* val, DeltaValue* at) 
{
	wxMouseCaptureChangedEvent *ev = DLIB_WXTYPECAST_BASE(MouseCaptureChangedEvent, val, mousecapturechangedevent);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, ev->GetCapturedWindow())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "capturedWindow",		&GetCapturedWindow,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(MouseCaptureChangedEvent,mousecapturechangedevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(mousecapturechangedevent_construct, 0, 2, Nil)
	int winid = 0;
	wxWindow* gainedCapture = NULL;
	if (n >= 1) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 2) { DLIB_WXGET_BASE(window, Window, window) gainedCapture = window; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(MouseCaptureChangedEvent, new wxMouseCaptureChangedEvent(winid, gainedCapture))
}

WX_FUNC_START(mousecapturechangedevent_getcapturedwindow, 1, Nil)
	DLIB_WXGET_BASE(mousecapturechangedevent, MouseCaptureChangedEvent, evt)
	WX_SETOBJECT(Window, evt->GetCapturedWindow())
}
