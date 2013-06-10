#include "DeltaWxFocusEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(focusevent, name)
#define WX_FUNC(name) WX_FUNC1(focusevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getwindow)
WX_FUNC_DEF(setwindow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getwindow),
	WX_FUNC(setwindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getwindow", "setwindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FocusEvent, "focusevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FocusEvent, val, focusevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetWindow (void* val, DeltaValue* at) 
{
	wxFocusEvent *ev = DLIB_WXTYPECAST_BASE(FocusEvent, val, focusevent);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, ev->GetWindow())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "window",				&GetWindow,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FocusEvent,focusevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(focusevent_construct, 0, 2, Nil)
	int type = wxEVT_NULL, winid = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(FocusEvent, new wxFocusEvent(type, winid))
}

WX_FUNC_START(focusevent_getwindow, 1, Nil)
	DLIB_WXGET_BASE(focusevent, FocusEvent, evt)
	WX_SETOBJECT(Window, evt->GetWindow())
}

WX_FUNC_START(focusevent_setwindow, 2, Nil)
	DLIB_WXGET_BASE(focusevent, FocusEvent, evt)
	DLIB_WXGET_BASE(window, Window, window)
	evt->SetWindow(window);
}
