#include "DeltaWxCloseEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(closeevent, name)
#define WX_FUNC(name) WX_FUNC1(closeevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(canveto)
WX_FUNC_DEF(getloggingoff)
WX_FUNC_DEF(setcanveto)
WX_FUNC_DEF(setloggingoff)
WX_FUNC_DEF(veto)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(canveto),
	WX_FUNC(getloggingoff),
	WX_FUNC(setcanveto),
	WX_FUNC(setloggingoff),
	WX_FUNC(veto)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "veto")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CloseEvent, "closeevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CloseEvent, val, closeevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetLoggingOff (void* val, DeltaValue* at) 
{
	wxCloseEvent *ev = DLIB_WXTYPECAST_BASE(CloseEvent, val, closeevent);
	at->FromBool(ev->GetLoggingOff());
	return true;
}

static bool GetVeto (void* val, DeltaValue* at) 
{
	wxCloseEvent *ev = DLIB_WXTYPECAST_BASE(CloseEvent, val, closeevent);
	at->FromBool(ev->GetVeto());
	return true;
}

static bool GetCanVeto (void* val, DeltaValue* at) 
{
	wxCloseEvent *ev = DLIB_WXTYPECAST_BASE(CloseEvent, val, closeevent);
	at->FromBool(ev->CanVeto());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "loggingOff",			&GetLoggingOff,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "veto",				&GetVeto,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "canVeto",			&GetCanVeto,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(CloseEvent, closeevent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(closeevent_construct, 0, 2, Nil)
	int type = wxEVT_NULL, winid = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
	DeltaWxCloseEvent *evt = DNEWCLASS(DeltaWxCloseEvent, (new wxCloseEvent(type, winid)));
	WX_SETOBJECT(CloseEvent, evt)
}

DLIB_FUNC_START(closeevent_destruct, 1, Nil)
	DLIB_WXDELETE(closeevent, CloseEvent, evt)
}

DLIB_FUNC_START(closeevent_canveto, 1, Nil)
	DLIB_WXGET_BASE(closeevent, CloseEvent, evt)
	WX_SETBOOL(evt->CanVeto())
}

DLIB_FUNC_START(closeevent_getloggingoff, 1, Nil)
	DLIB_WXGET_BASE(closeevent, CloseEvent, evt)
	WX_SETBOOL(evt->GetLoggingOff())
}

DLIB_FUNC_START(closeevent_setcanveto, 2, Nil)
	DLIB_WXGET_BASE(closeevent, CloseEvent, evt)
	WX_GETBOOL(canVeto)
	evt->SetCanVeto(canVeto);
}

DLIB_FUNC_START(closeevent_setloggingoff, 2, Nil)
	DLIB_WXGET_BASE(closeevent, CloseEvent, evt)
	WX_GETBOOL(loggingOff)
	evt->SetLoggingOff(loggingOff);
}

WX_FUNC_ARGRANGE_START(closeevent_veto, 1, 2, Nil)
	DLIB_WXGET_BASE(closeevent, CloseEvent, evt)
	bool veto = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(veto) }
	evt->Veto(veto);
}
