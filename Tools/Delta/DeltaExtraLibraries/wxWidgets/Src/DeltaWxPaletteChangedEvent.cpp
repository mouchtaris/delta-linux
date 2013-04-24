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
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getchangedwindow)
WX_FUNC_DEF(setchangedwindow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getchangedwindow),
	WX_FUNC(setchangedwindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setchangedwindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PaletteChangedEvent, "palettechangedevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PaletteChangedEvent, val, palettechangedevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetChangedWindow (void* val, DeltaValue* at) 
{
	wxPaletteChangedEvent *ev = DLIB_WXTYPECAST_BASE(PaletteChangedEvent, val, palettechangedevent);
	wxWindow *win = ev->GetChangedWindow();
	DeltaWxWindow *retval = win ? DNEWCLASS(DeltaWxWindow, (win)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
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
	DeltaWxPaletteChangedEvent *evt = DNEWCLASS(DeltaWxPaletteChangedEvent,
		(new wxPaletteChangedEvent(winid)));
	WX_SETOBJECT(PaletteChangedEvent, evt)
}

DLIB_FUNC_START(palettechangedevent_destruct, 1, Nil)
	DLIB_WXDELETE(palettechangedevent, PaletteChangedEvent, evt)
}

DLIB_FUNC_START(palettechangedevent_getchangedwindow, 1, Nil)
	DLIB_WXGET_BASE(palettechangedevent, PaletteChangedEvent, evt)
	DeltaWxWindow *retval = DNEWCLASS(DeltaWxWindow, (evt->GetChangedWindow()));
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(palettechangedevent_setchangedwindow, 2, Nil)
	DLIB_WXGET_BASE(palettechangedevent, PaletteChangedEvent, evt)
	DLIB_WXGET_BASE(window, Window, window)
	evt->SetChangedWindow(window);
}
