#include "DeltaWxTimerEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(timerevent, name)
#define WX_FUNC(name) WX_FUNC1(timerevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getinterval)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getinterval)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getinterval", "getinterval")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(TimerEvent, "timerevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TimerEvent, val, timerevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetInterval (void* val, DeltaValue* at) 
{
	wxTimerEvent *ev = DLIB_WXTYPECAST_BASE(TimerEvent, val, timerevent);
	at->FromNumber(ev->GetInterval());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "interval",			&GetInterval,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TimerEvent,timerevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(timerevent_construct, 0, 2, Nil)
	int timerid = 0;
	int interval = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(timerid) }
	if (n >= 2) {
#if wxCHECK_VERSION(2, 9, 0)
		DPTR(vm)->Error("in wxWidgets 2.9+ timerevent_construct cannot take an interval argument");
		DLIB_RESET_RETURN;
#else
		WX_GETNUMBER_DEFINED(interval)
#endif
	}
	wxTimerEvent *wxtimerevt;
#if wxCHECK_VERSION(2, 9, 0)
	wxtimerevt = new wxTimerEvent();
	if (timerid)
		wxtimerevt->SetId(timerid);
#else
	wxtimerevt = new wxTimerEvent(timerid, interval);
#endif
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TimerEvent, wxtimerevt)
}

WX_FUNC_START(timerevent_getinterval, 1, Nil)
	DLIB_WXGET_BASE(timerevent, TimerEvent, ev)
	WX_SETNUMBER(ev->GetInterval())
}
