#include "DeltaWxScrollWinEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(scrollwinevent, name)
#define WX_FUNC(name) WX_FUNC1(scrollwinevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getorientation)
WX_FUNC_DEF(getposition)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getorientation),
	WX_FUNC(getposition)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "getposition")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ScrollWinEvent, "scrollwinevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ScrollWinEvent, val, scrollwinevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetCommandInt (void* val, DeltaValue* at) 
{
	wxScrollWinEvent *ev = DLIB_WXTYPECAST_BASE(ScrollWinEvent, val, scrollwinevent);
	WX_SETNUMBER_EX(*at, ev->GetPosition())
	return true;
}

static bool GetOrientation (void* val, DeltaValue* at) 
{
	wxScrollWinEvent *ev = DLIB_WXTYPECAST_BASE(ScrollWinEvent, val, scrollwinevent);
	WX_SETNUMBER_EX(*at, ev->GetOrientation())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "commandInt",			&GetCommandInt,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "orientation",		&GetOrientation,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ScrollWinEvent, scrollwinevent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(scrollwinevent_construct, 0, 3, Nil)
	int commandType = wxEVT_NULL, pos = 0, orient = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(commandType) }
	if (n >= 2) { WX_GETNUMBER_DEFINED(pos) }
	if (n >= 3) { WX_GETDEFINE_DEFINED(orient) }
	DeltaWxScrollWinEvent *evt = DNEWCLASS(DeltaWxScrollWinEvent,
		(new wxScrollWinEvent(commandType, pos, orient)));
	WX_SETOBJECT(ScrollWinEvent, evt)
}

DLIB_FUNC_START(scrollwinevent_destruct, 1, Nil)
	DLIB_WXDELETE(scrollwinevent, ScrollWinEvent, evt)
}

DLIB_FUNC_START(scrollwinevent_getorientation, 1, Nil)
	DLIB_WXGET_BASE(scrollwinevent, ScrollWinEvent, evt)
	WX_SETNUMBER(evt->GetOrientation());
}

DLIB_FUNC_START(scrollwinevent_getposition, 1, Nil)
	DLIB_WXGET_BASE(scrollwinevent, ScrollWinEvent, evt)
	WX_SETNUMBER(evt->GetPosition());
}
