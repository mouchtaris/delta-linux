#include "DeltaWxCalendarEvent.h"
#include "DeltaWxDateEvent.h"
#include "DeltaWxCalendarCtrl.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(calendarevent, name)
#define WX_FUNC(name) WX_FUNC1(calendarevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getweekday)
WX_FUNC_DEF(setweekday)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getweekday),
	WX_FUNC(setweekday)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getweekday", "setweekday")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CalendarEvent, "calendarevent", DateEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CalendarEvent, val, calendarevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, DateEvent, val)
	return true;
}

static bool GetWeekDay (void* val, DeltaValue* at) 
{
	wxCalendarEvent *ev = DLIB_WXTYPECAST_BASE(CalendarEvent, val, calendarevent);
	at->FromNumber(ev->GetWeekDay());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "DateEvent",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "weekday",			&GetWeekDay,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(CalendarEvent,calendarevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(calendarevent_construct, 0, 2, Nil)
	wxCalendarEvent *evt = (wxCalendarEvent*) 0;
	if (n == 0) {
		evt = new wxCalendarEvent();
	} else if (n == 2) {
		DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, cal)
		WX_GETDEFINE(type)
		evt = new wxCalendarEvent(cal,
#if wxCHECK_VERSION(2, 9, 0)
		wxDateTime::Now(),
#endif
		type);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(CalendarEvent, evt)
}

WX_FUNC_START(calendarevent_getweekday, 1, Nil)
	DLIB_WXGET_BASE(calendarevent, CalendarEvent, evt)
	WX_SETNUMBER(evt->GetWeekDay())
}

WX_FUNC_START(calendarevent_setweekday, 2, Nil)
	DLIB_WXGET_BASE(calendarevent, CalendarEvent, evt)
	WX_GETDEFINE(day)
	evt->SetWeekDay((wxDateTime::WeekDay)day);
}
