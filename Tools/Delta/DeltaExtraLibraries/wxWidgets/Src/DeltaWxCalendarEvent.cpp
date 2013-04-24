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
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getweekday)
WX_FUNC_DEF(setweekday)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getweekday),
	WX_FUNC(setweekday)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setweekday")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CalendarEvent, "calendarevent", DateEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CalendarEvent, val, calendarevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDateEvent *_parent = DLIB_WXTYPECAST_BASE(DateEvent, val, dateevent);
	DeltaWxDateEvent *parent = DNEWCLASS(DeltaWxDateEvent, (_parent));
	WX_SETOBJECT_EX(*at, DateEvent, parent)
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
	wxCalendarEvent *wxevt = (wxCalendarEvent*) 0;
	DeltaWxCalendarEvent *evt = (DeltaWxCalendarEvent*) 0;
	if (n == 0) {
		wxevt = new wxCalendarEvent();
	} else if (n == 2) {
		DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, cal)
		WX_GETDEFINE(type)
		wxevt = new wxCalendarEvent(cal, type);
	}
	if (wxevt) evt = DNEWCLASS(DeltaWxCalendarEvent, (wxevt));
	WX_SETOBJECT(CalendarEvent, evt)
}

DLIB_FUNC_START(calendarevent_destruct, 1, Nil)
	DLIB_WXDELETE(calendarevent, CalendarEvent, evt)
}

DLIB_FUNC_START(calendarevent_getweekday, 1, Nil)
	DLIB_WXGET_BASE(calendarevent, CalendarEvent, evt)
	WX_SETNUMBER(evt->GetWeekDay())
}

DLIB_FUNC_START(calendarevent_setweekday, 2, Nil)
	DLIB_WXGET_BASE(calendarevent, CalendarEvent, evt)
	WX_GETDEFINE(day)
	evt->SetWeekDay((wxDateTime::WeekDay)day);
}
