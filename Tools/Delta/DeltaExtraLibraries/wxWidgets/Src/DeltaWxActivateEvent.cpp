#include "DeltaWxActivateEvent.h"
#include "DeltaWxEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(activateevent, name)
#define WX_FUNC(name) WX_FUNC1(activateevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getactive)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getactive)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "getactive")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ActivateEvent, "activateevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ActivateEvent, val, activateevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetActive (void* val, DeltaValue* at) 
{
	wxActivateEvent *ev = DLIB_WXTYPECAST_BASE(ActivateEvent, val, activateevent);
	WX_SETBOOL_EX(*at, ev->GetActive())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "active",				&GetActive,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ActivateEvent,activateevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(activateevent_construct, 0, 3, Nil)
	int type = wxEVT_NULL;
	bool active = true;
	int Id = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETBOOL_DEFINED(active) }
	if (n >= 3) { WX_GETDEFINE_DEFINED(Id) }
	DeltaWxActivateEvent *evt = DNEWCLASS(DeltaWxActivateEvent,
		(new wxActivateEvent(type, active, Id)));
	WX_SETOBJECT(ActivateEvent, evt)
}

DLIB_FUNC_START(activateevent_destruct, 1, Nil)
	DLIB_WXDELETE(activateevent, ActivateEvent, evt)
}

DLIB_FUNC_START(activateevent_getactive, 1, Nil)
	DLIB_WXGET_BASE(activateevent, ActivateEvent, evt)
	WX_SETBOOL(evt->GetActive())
}
