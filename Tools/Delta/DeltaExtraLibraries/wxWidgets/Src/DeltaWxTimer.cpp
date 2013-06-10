#include "DeltaWxTimer.h"
#include "DeltaWxEvtHandler.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(timer, name)
#define WX_FUNC(name) WX_FUNC1(timer, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getinterval)
WX_FUNC_DEF(isoneshot)
WX_FUNC_DEF(isrunning)
WX_FUNC_DEF(setowner)
WX_FUNC_DEF(start)
WX_FUNC_DEF(stop)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getinterval),
	WX_FUNC(isoneshot),
	WX_FUNC(isrunning),
	WX_FUNC(setowner),
	WX_FUNC(start),
	WX_FUNC(stop)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "stop")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Timer, "timer", EvtHandler)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Timer, val, timer) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, EvtHandler, val)
	return true;
}

static bool GetInterval (void* val, DeltaValue* at) 
{
	wxTimer *timer = DLIB_WXTYPECAST_BASE(Timer, val, timer);
	at->FromNumber(timer->GetInterval());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "EvtHandler",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "interval",			&GetInterval,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Timer,timer)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(timer_construct, 0, 2, Nil)
	wxTimer *timer = (wxTimer*) 0;
	if (n == 0)
		timer = new wxTimer();
	else {
		DLIB_WXGET_BASE(evthandler, EvtHandler, handler)
		int id = wxID_ANY;
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		timer = new wxTimer(handler, id);
	}
	WX_SETOBJECT(Timer, timer)
}

WX_FUNC_START(timer_destruct, 1, Nil)
	DLIB_WXDELETE(timer, Timer, timer)
}

WX_FUNC_START(timer_getinterval, 1, Nil)
	DLIB_WXGET_BASE(timer, Timer, timer)
	WX_SETNUMBER(timer->GetInterval())
}

WX_FUNC_START(timer_isoneshot, 1, Nil)
	DLIB_WXGET_BASE(timer, Timer, timer)
	WX_SETBOOL(timer->IsOneShot())
}

WX_FUNC_START(timer_isrunning, 1, Nil)
	DLIB_WXGET_BASE(timer, Timer, timer)
	WX_SETBOOL(timer->IsRunning())
}

WX_FUNC_ARGRANGE_START(timer_setowner, 2, 3, Nil)
	DLIB_WXGET_BASE(timer, Timer, timer)
	DLIB_WXGET_BASE(evthandler, EvtHandler, owner)
	int timerid = wxID_ANY;
	if (n >= 3) { WX_GETDEFINE_DEFINED(timerid) }
	timer->SetOwner(owner, timerid);
}

WX_FUNC_ARGRANGE_START(timer_start, 1, 3, Nil)
	DLIB_WXGET_BASE(timer, Timer, timer)
	int milliseconds = -1;
	bool oneShot = false;
	if (n >= 2) { WX_GETNUMBER_DEFINED(milliseconds) }
	if (n >= 3) { WX_GETBOOL_DEFINED(oneShot) }
	WX_SETBOOL(timer->Start(milliseconds, oneShot))
}

WX_FUNC_START(timer_stop, 1, Nil)
	DLIB_WXGET_BASE(timer, Timer, timer)
	timer->Stop();
}
