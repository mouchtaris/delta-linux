#include "DeltaWxNotifyEvent.h"
#include "DeltaWxCommandEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(notifyevent, name)
#define WX_FUNC(name) WX_FUNC1(notifyevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(allow)
WX_FUNC_DEF(isallowed)
WX_FUNC_DEF(veto)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(allow),
	WX_FUNC(isallowed),
	WX_FUNC(veto)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "allow", "veto")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(NotifyEvent, "notifyevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(NotifyEvent, val, notifyevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetIsAllowed (void* val, DeltaValue* at) 
{
	wxNotifyEvent *ev = DLIB_WXTYPECAST_BASE(NotifyEvent, val, notifyevent);
	WX_SETBOOL_EX(*at, ev->IsAllowed())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isAllowed",			&GetIsAllowed,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(NotifyEvent,notifyevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(notifyevent_construct, 0, 2, Nil)
	wxNotifyEvent *notifyevt = (wxNotifyEvent*) 0;
	if (n == 0)
		notifyevt = new wxNotifyEvent();
	else if (n == 1) {
		WX_GETDEFINE(commandType)
		notifyevt = new wxNotifyEvent(commandType);
	} else {
		WX_GETDEFINE(commandType)
		WX_GETDEFINE(winid)
		notifyevt = new wxNotifyEvent(commandType, winid);
	}
	WX_SETOBJECT(NotifyEvent, notifyevt)
}

WX_FUNC_START(notifyevent_allow, 1, Nil)
	DLIB_WXGET_BASE(notifyevent, NotifyEvent, evt)
	evt->Allow();
}

WX_FUNC_START(notifyevent_isallowed, 1, Nil)
	DLIB_WXGET_BASE(notifyevent, NotifyEvent, evt)
	WX_SETBOOL(evt->IsAllowed())
}

WX_FUNC_START(notifyevent_veto, 1, Nil)
	DLIB_WXGET_BASE(notifyevent, NotifyEvent, evt)
	evt->Veto();
}
