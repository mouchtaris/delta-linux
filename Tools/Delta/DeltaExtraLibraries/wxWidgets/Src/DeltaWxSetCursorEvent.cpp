#include "DeltaWxSetCursorEvent.h"
#include "DeltaWxEvent.h"
#include "DeltaWxCursor.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(setcursorevent, name)
#define WX_FUNC(name) WX_FUNC1(setcursorevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getcursor)
WX_FUNC_DEF(getx)
WX_FUNC_DEF(gety)
WX_FUNC_DEF(hascursor)
WX_FUNC_DEF(setcursor)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getcursor),
	WX_FUNC(getx),
	WX_FUNC(gety),
	WX_FUNC(hascursor),
	WX_FUNC(setcursor)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getcursor", "setcursor")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SetCursorEvent, "setcursorevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SetCursorEvent, val, setcursorevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetCursor (void* val, DeltaValue* at) 
{
	wxSetCursorEvent *ev = DLIB_WXTYPECAST_BASE(SetCursorEvent, val, setcursorevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Cursor, new wxCursor(ev->GetCursor()))
	return true;
}

static bool GetX (void* val, DeltaValue* at) 
{
	wxSetCursorEvent *ev = DLIB_WXTYPECAST_BASE(SetCursorEvent, val, setcursorevent);
	WX_SETNUMBER_EX(*at, ev->GetX())
	return true;
}

static bool GetY (void* val, DeltaValue* at) 
{
	wxSetCursorEvent *ev = DLIB_WXTYPECAST_BASE(SetCursorEvent, val, setcursorevent);
	WX_SETNUMBER_EX(*at, ev->GetY())
	return true;
}

static bool GetHasCursor (void* val, DeltaValue* at) 
{
	wxSetCursorEvent *ev = DLIB_WXTYPECAST_BASE(SetCursorEvent, val, setcursorevent);
	WX_SETBOOL_EX(*at, ev->HasCursor())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "cursor",				&GetCursor,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "x",					&GetX,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "y",					&GetY,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasCursor",			&GetHasCursor,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SetCursorEvent,setcursorevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(setcursorevent_construct, 0, 2, Nil)
	int x = 0, y = 0;
	if (n >= 1) { WX_GETNUMBER_DEFINED(x) }
	if (n >= 2) { WX_GETNUMBER_DEFINED(y) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SetCursorEvent, new wxSetCursorEvent(x, y))
}

WX_FUNC_START(setcursorevent_getcursor, 1, Nil)
	DLIB_WXGET_BASE(setcursorevent, SetCursorEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Cursor, new wxCursor(evt->GetCursor()))
}

WX_FUNC_START(setcursorevent_getx, 1, Nil)
	DLIB_WXGET_BASE(setcursorevent, SetCursorEvent, evt)
	WX_SETNUMBER(evt->GetX())
}

WX_FUNC_START(setcursorevent_gety, 1, Nil)
	DLIB_WXGET_BASE(setcursorevent, SetCursorEvent, evt)
	WX_SETNUMBER(evt->GetY())
}

WX_FUNC_START(setcursorevent_hascursor, 1, Nil)
	DLIB_WXGET_BASE(setcursorevent, SetCursorEvent, evt)
	WX_SETBOOL(evt->HasCursor())
}

WX_FUNC_START(setcursorevent_setcursor, 2, Nil)
	DLIB_WXGET_BASE(setcursorevent, SetCursorEvent, evt)
	DLIB_WXGET_BASE(cursor, Cursor, cursor)
	evt->SetCursor(*cursor);
}
