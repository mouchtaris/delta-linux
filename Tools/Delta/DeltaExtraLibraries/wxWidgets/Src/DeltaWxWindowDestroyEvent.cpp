#include "DeltaWxWindowDestroyEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(windowdestroyevent, name)
#define WX_FUNC(name) WX_FUNC1(windowdestroyevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getwindow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getwindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getwindow", "getwindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(WindowDestroyEvent, "windowdestroyevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(WindowDestroyEvent, val, windowdestroyevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetWindow (void* val, DeltaValue* at) 
{
	wxWindowDestroyEvent *ev = DLIB_WXTYPECAST_BASE(WindowDestroyEvent, val, windowdestroyevent);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, ev->GetWindow())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "window",				&GetWindow,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(WindowDestroyEvent,windowdestroyevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(windowdestroyevent_construct, 0, 1, Nil)
	wxWindow *win = NULL;
	if (n >= 1) { DLIB_WXGET_BASE(window, Window, window) win = window; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(WindowDestroyEvent, new wxWindowDestroyEvent(win))
}

WX_FUNC_START(windowdestroyevent_getwindow, 1, Nil)
	DLIB_WXGET_BASE(windowdestroyevent, WindowDestroyEvent, evt)
	WX_SETOBJECT(Window, evt->GetWindow())
}
