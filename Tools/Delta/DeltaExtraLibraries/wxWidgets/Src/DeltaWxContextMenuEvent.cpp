#include "DeltaWxContextMenuEvent.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxPoint.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(contextmenuevent, name)
#define WX_FUNC(name) WX_FUNC1(contextmenuevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(setposition)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getposition),
	WX_FUNC(setposition)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getposition", "setposition")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ContextMenuEvent, "contextmenuevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ContextMenuEvent, val, contextmenuevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxContextMenuEvent *ev = DLIB_WXTYPECAST_BASE(ContextMenuEvent, val, contextmenuevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Point, new wxPoint(ev->GetPosition()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "position",			&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ContextMenuEvent,contextmenuevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(contextmenuevent_construct, 0, 3, Nil)
	int type = wxEVT_NULL;
	int winid = 0;
	wxPoint pt = wxDefaultPosition;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 3) { DLIB_WXGETPOINT_BASE(point) pt = *point; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ContextMenuEvent, new wxContextMenuEvent(type, winid, pt))
}

WX_FUNC_START(contextmenuevent_getposition, 1, Nil)
	DLIB_WXGET_BASE(contextmenuevent, ContextMenuEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(evt->GetPosition()))
}

WX_FUNC_START(contextmenuevent_setposition, 2, Nil)
	DLIB_WXGET_BASE(contextmenuevent, ContextMenuEvent, evt)
	DLIB_WXGETPOINT_BASE(point)
	evt->SetPosition(*point);
}
