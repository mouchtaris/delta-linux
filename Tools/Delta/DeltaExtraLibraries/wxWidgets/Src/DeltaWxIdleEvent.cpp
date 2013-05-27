#include "DeltaWxIdleEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(idleevent, name)
#define WX_FUNC(name) WX_FUNC1(idleevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(cansend)
WX_FUNC_DEF(getmode)
WX_FUNC_DEF(setmode)
WX_FUNC_DEF(requestmore)
WX_FUNC_DEF(morerequested)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(cansend),
	WX_FUNC(getmode),
	WX_FUNC(setmode),
	WX_FUNC(destruct),
	WX_FUNC(requestmore),
	WX_FUNC(morerequested)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(4, uarraysize(funcs) - 4, "destruct", "morerequested")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(IdleEvent, "idleevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(IdleEvent, val, idleevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetMoreRequested (void* val, DeltaValue* at) 
{
	wxIdleEvent *ev = DLIB_WXTYPECAST_BASE(IdleEvent, val, idleevent);
	WX_SETBOOL_EX(*at, ev->MoreRequested())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "moreRequested",		&GetMoreRequested,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(IdleEvent,idleevent)

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(idleevent_construct, 0, Nil)
	DeltaWxIdleEvent *evt = DNEWCLASS(DeltaWxIdleEvent, (new wxIdleEvent()));
	WX_SETOBJECT(IdleEvent, evt)
}

DLIB_FUNC_START(idleevent_destruct, 1, Nil)
	DLIB_WXDELETE(idleevent, IdleEvent, evt)
}

DLIB_FUNC_START(idleevent_cansend, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
#if wxCHECK_VERSION(2, 9, 0)	//does not provide the IdleEvent::CanSend function. TODO: maybe remove it totally?
	bool retval = true;
#else
	bool retval = wxIdleEvent::CanSend(window);
#endif
	WX_SETBOOL(retval)
}

DLIB_FUNC_START(idleevent_getmode, 0, Nil)
	WX_SETNUMBER(wxIdleEvent::GetMode())
}

DLIB_FUNC_START(idleevent_setmode, 1, Nil)
	WX_GETDEFINE(mode)
	wxIdleEvent::SetMode((wxIdleMode)mode);
}

WX_FUNC_ARGRANGE_START(idleevent_requestmore, 1, 2, Nil)
	DLIB_WXGET_BASE(idleevent, IdleEvent, evt)
	bool needMore = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(needMore) }
	evt->RequestMore(needMore);
}

DLIB_FUNC_START(idleevent_morerequested, 1, Nil)
	DLIB_WXGET_BASE(idleevent, IdleEvent, evt)
	WX_SETBOOL(evt->MoreRequested())
}
