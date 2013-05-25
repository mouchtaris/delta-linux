#include "DeltaWxSplitterEvent.h"
#include "DeltaWxSplitterWindow.h"
#include "DeltaWxNotifyEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(splitterevent, name)
#define WX_FUNC(name) WX_FUNC1(splitterevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getsashposition)
WX_FUNC_DEF(getx)
WX_FUNC_DEF(gety)
WX_FUNC_DEF(getwindowbeingremoved)
WX_FUNC_DEF(setsashposition)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getsashposition),
	WX_FUNC(getx),
	WX_FUNC(gety),
	WX_FUNC(getwindowbeingremoved),
	WX_FUNC(setsashposition)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getsashposition", "setsashposition")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SplitterEvent, "splitterevent", NotifyEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SplitterEvent, val, splitterevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, NotifyEvent, val)
	return true;
}

static bool GetSashPosition (void* val, DeltaValue* at) 
{
	wxSplitterEvent *ev = DLIB_WXTYPECAST_BASE(SplitterEvent, val, splitterevent);
	WX_SETNUMBER_EX(*at, ev->GetSashPosition());
	return true;
}

static bool GetX (void* val, DeltaValue* at) 
{
	wxSplitterEvent *ev = DLIB_WXTYPECAST_BASE(SplitterEvent, val, splitterevent);
	WX_SETNUMBER_EX(*at, ev->GetX());
	return true;
}

static bool GetY (void* val, DeltaValue* at) 
{
	wxSplitterEvent *ev = DLIB_WXTYPECAST_BASE(SplitterEvent, val, splitterevent);
	WX_SETNUMBER_EX(*at, ev->GetY());
	return true;
}

static bool GetWindowBeingRemoved (void* val, DeltaValue* at) 
{
	wxSplitterEvent *ev = DLIB_WXTYPECAST_BASE(SplitterEvent, val, splitterevent);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, ev->GetWindowBeingRemoved())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "NotifyEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "sashPosition",		&GetSashPosition,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "x",					&GetX,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "y",					&GetY,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "windowBeingRemoved",	&GetWindowBeingRemoved,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SplitterEvent,splitterevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(splitterevent_construct, 0, 2, Nil)
	wxEventType type = wxEVT_NULL;
	wxSplitterWindow *splitter = (wxSplitterWindow *)NULL;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { DLIB_WXGET_BASE(splitterwindow, SplitterWindow, _splitter) splitter = _splitter; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SplitterEvent, new wxSplitterEvent(type, splitter))
}

WX_FUNC_START(splitterevent_getsashposition, 1, Nil)
	DLIB_WXGET_BASE(splitterevent, SplitterEvent, evt)
	WX_SETNUMBER(evt->GetSashPosition())
}

WX_FUNC_START(splitterevent_getx, 1, Nil)
	DLIB_WXGET_BASE(splitterevent, SplitterEvent, evt)
	WX_SETNUMBER(evt->GetX())
}

WX_FUNC_START(splitterevent_gety, 1, Nil)
	DLIB_WXGET_BASE(splitterevent, SplitterEvent, evt)
	WX_SETNUMBER(evt->GetY())
}

WX_FUNC_START(splitterevent_getwindowbeingremoved, 1, Nil)
	DLIB_WXGET_BASE(splitterevent, SplitterEvent, evt)
	WX_SETOBJECT(Window, evt->GetWindowBeingRemoved())
}

WX_FUNC_START(splitterevent_setsashposition, 2, Nil)
	DLIB_WXGET_BASE(splitterevent, SplitterEvent, evt)
	WX_GETNUMBER(position)
	evt->SetSashPosition(position);
}
