#include "DeltaWxEvent.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(event, name)
#define WX_FUNC(name) WX_FUNC1(event, name)

WX_FUNC_DEF(geteventobject)
WX_FUNC_DEF(geteventtype)
WX_FUNC_DEF(getid)
WX_FUNC_DEF(getskipped)
WX_FUNC_DEF(gettimestamp)
WX_FUNC_DEF(iscommandevent)
WX_FUNC_DEF(resumepropagation)
WX_FUNC_DEF(seteventobject)
WX_FUNC_DEF(seteventtype)
WX_FUNC_DEF(setid)
WX_FUNC_DEF(settimestamp)
WX_FUNC_DEF(shouldpropagate)
WX_FUNC_DEF(skip)
WX_FUNC_DEF(stoppropagation)

WX_FUNCS_START
	WX_FUNC(geteventobject),
	WX_FUNC(geteventtype),
	WX_FUNC(getid),
	WX_FUNC(getskipped),
	WX_FUNC(gettimestamp),
	WX_FUNC(iscommandevent),
	WX_FUNC(resumepropagation),
	WX_FUNC(seteventobject),
	WX_FUNC(seteventtype),
	WX_FUNC(setid),
	WX_FUNC(settimestamp),
	WX_FUNC(shouldpropagate),
	WX_FUNC(skip),
	WX_FUNC(stoppropagation)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(0, uarraysize(funcs), "geteventobject", "stoppropagation")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Event, "event", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Event, val, event) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetEventObject (void* val, DeltaValue* at) 
{
	wxEvent *ev = DLIB_WXTYPECAST_BASE(Event, val, event);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Object, ev->GetEventObject())
	return true;
}

static bool GetEventType (void* val, DeltaValue* at) 
{
	wxEvent *ev = DLIB_WXTYPECAST_BASE(Event, val, event);
	WX_SETNUMBER_EX(*at, ev->GetEventType())
	return true;
}

static bool GetTimestamp (void* val, DeltaValue* at) 
{
	wxEvent *ev = DLIB_WXTYPECAST_BASE(Event, val, event);
	WX_SETNUMBER_EX(*at, ev->GetTimestamp())
	return true;
}

static bool GetId (void* val, DeltaValue* at) 
{
	wxEvent *ev = DLIB_WXTYPECAST_BASE(Event, val, event);
	WX_SETNUMBER_EX(*at, ev->GetId())
	return true;
}

static bool GetSkipped (void* val, DeltaValue* at) 
{
	wxEvent *ev = DLIB_WXTYPECAST_BASE(Event, val, event);
	WX_SETBOOL_EX(*at, ev->GetSkipped())
	return true;
}

static bool GetIsCommandEvent (void* val, DeltaValue* at) 
{
	wxEvent *ev = DLIB_WXTYPECAST_BASE(Event, val, event);
	WX_SETBOOL_EX(*at, ev->IsCommandEvent())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "eventObject",		&GetEventObject,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "eventType",			&GetEventType,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "timeStamp",			&GetTimestamp,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "id",					&GetId,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "skipped",			&GetSkipped,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isCommandEvent",		&GetIsCommandEvent,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Event,event)

////////////////////////////////////////////////////////////////

WX_FUNC_START(event_geteventobject, 1, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	wxObject* retval	= evt->GetEventObject();;
	WX_SETOBJECT(Object, retval)
}

WX_FUNC_START(event_geteventtype, 1, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_SETNUMBER(evt->GetEventType());
}

WX_FUNC_START(event_getid, 1, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_SETNUMBER(evt->GetId());
}

WX_FUNC_START(event_getskipped, 1, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_SETBOOL(evt->GetSkipped());
}

WX_FUNC_START(event_gettimestamp, 1, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_SETNUMBER(evt->GetTimestamp());
}

WX_FUNC_START(event_iscommandevent, 1, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_SETBOOL(evt->IsCommandEvent());
}

WX_FUNC_START(event_resumepropagation, 2, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_GETNUMBER(propagationLevel)
	evt->ResumePropagation(propagationLevel);
}

WX_FUNC_START(event_seteventobject, 2, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	DLIB_WXGET_BASE(object, Object, obj)
	evt->SetEventObject(obj);
}

WX_FUNC_START(event_seteventtype, 2, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_GETDEFINE(type)
	evt->SetEventType((wxEventType)type);
}

WX_FUNC_START(event_setid, 2, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_GETDEFINE(id)
	evt->SetId(id);
}

WX_FUNC_START(event_settimestamp, 2, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_GETNUMBER(timeStamp)
	evt->SetTimestamp(timeStamp);
}

WX_FUNC_START(event_shouldpropagate, 1, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_SETBOOL(evt->ShouldPropagate())
}

WX_FUNC_ARGRANGE_START(event_skip, 1, 2, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	if (n == 1) {
		evt->Skip();
	} else {
		WX_GETBOOL(skip)
		evt->Skip(skip);
	}
}

WX_FUNC_START(event_stoppropagation, 1, Nil)
	DLIB_WXGET_BASE(event, Event, evt)
	WX_SETNUMBER(evt->StopPropagation())
}
