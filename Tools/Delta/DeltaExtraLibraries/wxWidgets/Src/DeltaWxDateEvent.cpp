#include "DeltaWxDateEvent.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxWindow.h"
#include "DeltaWxDateTime.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(dateevent, name)
#define WX_FUNC(name) WX_FUNC1(dateevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getdate)
WX_FUNC_DEF(setdate)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getdate),
	WX_FUNC(setdate)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getdate", "setdate")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DateEvent, "dateevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DateEvent, val, dateevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetDate (void* val, DeltaValue* at) 
{
	wxDateEvent *ev = DLIB_WXTYPECAST_BASE(DateEvent, val, dateevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, DateTime, new wxDateTime(ev->GetDate()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "date",				&GetDate,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DateEvent,dateevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(dateevent_construct, 0, 3, Nil)
	wxDateEvent *evt = (wxDateEvent*) 0;
	if (n == 0) {
		evt = new wxDateEvent();
	} else if (n == 3) {
		DLIB_WXGET_BASE(window, Window, win)
		DLIB_WXGET_BASE(datetime, DateTime, dt)
		WX_GETDEFINE(type)
		evt = new wxDateEvent(win, *dt, type);
	}
	WX_SETOBJECT(DateEvent, evt)
}

WX_FUNC_START(dateevent_getdate, 1, Nil)
	DLIB_WXGET_BASE(dateevent, DateEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(DateTime, new wxDateTime(evt->GetDate()))
}

WX_FUNC_START(dateevent_setdate, 2, Nil)
	DLIB_WXGET_BASE(dateevent, DateEvent, evt)
	DLIB_WXGET_BASE(datetime, DateTime, date)
	evt->SetDate(*date);
}
