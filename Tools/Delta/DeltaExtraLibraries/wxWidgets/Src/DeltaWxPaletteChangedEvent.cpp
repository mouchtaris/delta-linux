#include "DeltaWxPaletteChangedEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(palettechangedevent, name)
#define WX_FUNC(name) WX_FUNC1(palettechangedevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getchangedwindow)
WX_FUNC_DEF(setchangedwindow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getchangedwindow),
	WX_FUNC(setchangedwindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getchangedwindow", "setchangedwindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PaletteChangedEvent, "palettechangedevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PaletteChangedEvent, val, palettechangedevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetChangedWindow (void* val, DeltaValue* at) 
{
	wxPaletteChangedEvent *ev = DLIB_WXTYPECAST_BASE(PaletteChangedEvent, val, palettechangedevent);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, ev->GetChangedWindow())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "changedWindow",		&GetChangedWindow,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PaletteChangedEvent,palettechangedevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(palettechangedevent_construct, 0, 1, Nil)
	int winid = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(winid) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(PaletteChangedEvent, new wxPaletteChangedEvent(winid))
}

WX_FUNC_START(palettechangedevent_getchangedwindow, 1, Nil)
	DLIB_WXGET_BASE(palettechangedevent, PaletteChangedEvent, evt)
	WX_SETOBJECT(Window, evt->GetChangedWindow())
}

WX_FUNC_START(palettechangedevent_setchangedwindow, 2, Nil)
	DLIB_WXGET_BASE(palettechangedevent, PaletteChangedEvent, evt)
	DLIB_WXGET_BASE(window, Window, window)
	evt->SetChangedWindow(window);
}
