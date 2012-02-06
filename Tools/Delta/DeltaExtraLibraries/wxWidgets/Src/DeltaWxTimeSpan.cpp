#include "DeltaWxTimeSpan.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(timespan, name)
#define WX_FUNC(name) WX_FUNC1(timespan, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(abs)
WX_FUNC_DEF(add)
WX_FUNC_DEF(days)
WX_FUNC_DEF(day)
WX_FUNC_DEF(format)
WX_FUNC_DEF(getdays)
WX_FUNC_DEF(gethours)
WX_FUNC_DEF(getmilliseconds)
WX_FUNC_DEF(getminutes)
WX_FUNC_DEF(getseconds)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(getweeks)
WX_FUNC_DEF(hours)
WX_FUNC_DEF(hour)
WX_FUNC_DEF(isequalto)
WX_FUNC_DEF(islongerthan)
WX_FUNC_DEF(isnegative)
WX_FUNC_DEF(isnull)
WX_FUNC_DEF(ispositive)
WX_FUNC_DEF(isshorterthan)
WX_FUNC_DEF(minutes)
WX_FUNC_DEF(minute)
WX_FUNC_DEF(multiply)
WX_FUNC_DEF(negate)
WX_FUNC_DEF(neg)
WX_FUNC_DEF(milliseconds)
WX_FUNC_DEF(millisecond)
WX_FUNC_DEF(seconds)
WX_FUNC_DEF(second)
WX_FUNC_DEF(subtract)
WX_FUNC_DEF(weeks)
WX_FUNC_DEF(week)
WX_FUNC_DEF(minus)
WX_FUNC_DEF(isnotequalto)
WX_FUNC_DEF(lessequalthan)
WX_FUNC_DEF(lessthan)
WX_FUNC_DEF(greaterequalthan)
WX_FUNC_DEF(greaterthan)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(days),
	WX_FUNC(day),
	WX_FUNC(hours),
	WX_FUNC(hour),
	WX_FUNC(minutes),
	WX_FUNC(minute),
	WX_FUNC(milliseconds),
	WX_FUNC(millisecond),
	WX_FUNC(seconds),
	WX_FUNC(second),
	WX_FUNC(weeks),
	WX_FUNC(week),
	WX_FUNC(destruct),
	WX_FUNC(abs),
	WX_FUNC(add),
	WX_FUNC(format),
	WX_FUNC(getdays),
	WX_FUNC(gethours),
	WX_FUNC(getmilliseconds),
	WX_FUNC(getminutes),
	WX_FUNC(getseconds),
	WX_FUNC(getvalue),
	WX_FUNC(getweeks),
	WX_FUNC(isequalto),
	WX_FUNC(islongerthan),
	WX_FUNC(isnegative),
	WX_FUNC(isnull),
	WX_FUNC(ispositive),
	WX_FUNC(isshorterthan),
	WX_FUNC(multiply),
	WX_FUNC(negate),
	WX_FUNC(neg),
	WX_FUNC(subtract),
	WX_FUNC(minus),
	WX_FUNC(isnotequalto),
	WX_FUNC(lessequalthan),
	WX_FUNC(lessthan),
	WX_FUNC(greaterequalthan),
	WX_FUNC(greaterthan)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(13, uarraysize(funcs) - 13, "destruct", "greaterthan")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(TimeSpan, "timespan")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TimeSpan, val, timespan) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetDays (void* val, DeltaValue* at) 
{
	wxTimeSpan *timespan = DLIB_WXTYPECAST_BASE(TimeSpan, val, timespan);
	WX_SETNUMBER_EX(*at, timespan->GetDays());
	return true;
}

static bool GetHours (void* val, DeltaValue* at) 
{
	wxTimeSpan *timespan = DLIB_WXTYPECAST_BASE(TimeSpan, val, timespan);
	WX_SETNUMBER_EX(*at, timespan->GetHours());
	return true;
}

static bool GetMilliseconds (void* val, DeltaValue* at) 
{
	wxTimeSpan *timespan = DLIB_WXTYPECAST_BASE(TimeSpan, val, timespan);
	WX_SETNUMBER_EX(*at, timespan->GetMilliseconds().ToDouble());
	return true;
}

static bool GetMinutes (void* val, DeltaValue* at) 
{
	wxTimeSpan *timespan = DLIB_WXTYPECAST_BASE(TimeSpan, val, timespan);
	WX_SETNUMBER_EX(*at, timespan->GetMinutes());
	return true;
}

static bool GetSeconds (void* val, DeltaValue* at) 
{
	wxTimeSpan *timespan = DLIB_WXTYPECAST_BASE(TimeSpan, val, timespan);
	WX_SETNUMBER_EX(*at, timespan->GetSeconds().ToDouble());
	return true;
}

static bool GetValue (void* val, DeltaValue* at) 
{
	wxTimeSpan *timespan = DLIB_WXTYPECAST_BASE(TimeSpan, val, timespan);
	WX_SETNUMBER_EX(*at, timespan->GetValue().ToDouble());
	return true;
}

static bool GetWeeks (void* val, DeltaValue* at) 
{
	wxTimeSpan *timespan = DLIB_WXTYPECAST_BASE(TimeSpan, val, timespan);
	WX_SETNUMBER_EX(*at, timespan->GetWeeks());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "days",				&GetDays,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hours",				&GetHours,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "milliseconds",		&GetMilliseconds,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minutes",			&GetMinutes,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "seconds",			&GetSeconds,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "value",				&GetValue,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "weeks",				&GetWeeks,				DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxTimeSpanInitFunc()
{
	TimeSpanUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&timespan_isequalto_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&timespan_isnotequalto_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("<="), DeltaValue(&timespan_lessequalthan_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("<"), DeltaValue(&timespan_lessthan_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue(">="), DeltaValue(&timespan_greaterequalthan_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue(">"), DeltaValue(&timespan_greaterthan_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("+"), DeltaValue(&timespan_add_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("-"), DeltaValue(&timespan_minus_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("*"), DeltaValue(&timespan_multiply_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(TimeSpan, timespan, DeltaWxTimeSpanInitFunc();, );

////////////////////////////////////////////////////////////////

#define WXTIMESPAN_AVOID_UNNECESSARY_OBJECTS(timespan, func)									\
	wxTimeSpan *timespan##Ref = &(timespan->func);												\
	if (timespan##Ref == timespan) {															\
		DLIB_RETVAL_REF = DPTR(vm)->GetActualArg(0);											\
	} else {																					\
		DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(*timespan##Ref)));	\
		WX_SETOBJECT(TimeSpan, retval)															\
	}

WX_FUNC_ARGRANGE_START(timespan_construct, 0, 4, Nil)
	wxTimeSpan *wxtimespan = (wxTimeSpan*) 0;
	DeltaWxTimeSpan *timespan = (DeltaWxTimeSpan*) 0;
	if (n == 0)
		wxtimespan = new wxTimeSpan();
	else {
		double minutes = 0, seconds = 0, milliseconds = 0;
		WX_GETNUMBER(hours)
		if (n >= 2) { WX_GETNUMBER_DEFINED(minutes) }
		if (n >= 3) { WX_GETNUMBER_DEFINED(seconds) }
		if (n >= 4) { WX_GETNUMBER_DEFINED(milliseconds) }
		wxtimespan = new wxTimeSpan(hours, minutes, seconds, milliseconds);
	}
	if (wxtimespan) timespan = DNEWCLASS(DeltaWxTimeSpan, (wxtimespan));
	WX_SETOBJECT(TimeSpan, timespan)
}

DLIB_FUNC_START(timespan_destruct, 1, Nil)
	DLIB_WXDELETE(timespan, TimeSpan, timespan)
}

DLIB_FUNC_START(timespan_abs, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WXTIMESPAN_AVOID_UNNECESSARY_OBJECTS(timespan, Abs())
}

DLIB_FUNC_START(timespan_add, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, diff)
	WXTIMESPAN_AVOID_UNNECESSARY_OBJECTS(timespan, Add(*diff))
}

DLIB_FUNC_START(timespan_days, 1, Nil)
	WX_GETNUMBER(days)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Days(days))));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_day, 0, Nil)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Day())));
	WX_SETOBJECT(TimeSpan, retval)
}

WX_FUNC_ARGRANGE_START(timespan_format, 1, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	if (n == 1)
		WX_SETSTRING(timespan->Format())
	else {
		WX_GETSTRING(format)
		WX_SETSTRING(timespan->Format(format.c_str()))
	}
}

DLIB_FUNC_START(timespan_getdays, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETNUMBER(timespan->GetDays())
}

DLIB_FUNC_START(timespan_gethours, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETNUMBER(timespan->GetHours())
}

DLIB_FUNC_START(timespan_getmilliseconds, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETNUMBER(timespan->GetMilliseconds().ToDouble())
}

DLIB_FUNC_START(timespan_getminutes, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETNUMBER(timespan->GetMinutes())
}

DLIB_FUNC_START(timespan_getseconds, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETNUMBER(timespan->GetSeconds().ToDouble())
}

DLIB_FUNC_START(timespan_getvalue, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETNUMBER(timespan->GetValue().ToDouble())
}

DLIB_FUNC_START(timespan_getweeks, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETNUMBER(timespan->GetWeeks())
}

DLIB_FUNC_START(timespan_hours, 1, Nil)
	WX_GETNUMBER(hours)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Hours(hours))));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_hour, 0, Nil)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Hour())));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_isequalto, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, ts)
	WX_SETBOOL(timespan->IsEqualTo(*ts))
}

DLIB_FUNC_START(timespan_islongerthan, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, ts)
	WX_SETBOOL(timespan->IsLongerThan(*ts))
}

DLIB_FUNC_START(timespan_isnegative, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETBOOL(timespan->IsNegative())
}

DLIB_FUNC_START(timespan_isnull, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETBOOL(timespan->IsNull())
}

DLIB_FUNC_START(timespan_ispositive, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_SETBOOL(timespan->IsPositive())
}

DLIB_FUNC_START(timespan_isshorterthan, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, ts)
	WX_SETBOOL(timespan->IsShorterThan(*ts))
}

DLIB_FUNC_START(timespan_minutes, 1, Nil)
	WX_GETNUMBER(minutes)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Minutes(minutes))));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_minute, 0, Nil)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Minute())));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_multiply, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WX_GETNUMBER(factor)
	WXTIMESPAN_AVOID_UNNECESSARY_OBJECTS(timespan, Multiply(factor))
}

DLIB_FUNC_START(timespan_negate, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WXTIMESPAN_AVOID_UNNECESSARY_OBJECTS(timespan, Negate())
}

DLIB_FUNC_START(timespan_neg, 1, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	WXTIMESPAN_AVOID_UNNECESSARY_OBJECTS(timespan, Neg())
}

DLIB_FUNC_START(timespan_milliseconds, 1, Nil)
	WX_GETNUMBER(milliseconds)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Milliseconds(milliseconds))));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_millisecond, 0, Nil)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Millisecond())));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_seconds, 1, Nil)
	WX_GETNUMBER(seconds)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Seconds(seconds))));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_second, 0, Nil)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Second())));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_subtract, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, diff)
	WXTIMESPAN_AVOID_UNNECESSARY_OBJECTS(timespan, Subtract(*diff))
}

DLIB_FUNC_START(timespan_weeks, 1, Nil)
	WX_GETNUMBER(weeks)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Weeks(weeks))));
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_week, 0, Nil)
	DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(wxTimeSpan::Week())));
	WX_SETOBJECT(TimeSpan, retval)
}

WX_FUNC_ARGRANGE_START(timespan_minus, 1, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DeltaWxTimeSpan *retval = (DeltaWxTimeSpan*) 0;
	if (n == 1)
		retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(timespan->operator-())));
	else {
		DLIB_WXGET_BASE(timespan, TimeSpan, diff)
		retval = DNEWCLASS(DeltaWxTimeSpan, (new wxTimeSpan(timespan->operator-(*diff))));
	}
	WX_SETOBJECT(TimeSpan, retval)
}

DLIB_FUNC_START(timespan_isnotequalto, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, diff)
	WX_SETBOOL(timespan->operator!=(*diff))
}

DLIB_FUNC_START(timespan_lessequalthan, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, diff)
	WX_SETBOOL(timespan->operator<=(*diff))
}

DLIB_FUNC_START(timespan_lessthan, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, diff)
	WX_SETBOOL(timespan->operator<(*diff))
}

DLIB_FUNC_START(timespan_greaterequalthan, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, diff)
	WX_SETBOOL(timespan->operator>=(*diff))
}

DLIB_FUNC_START(timespan_greaterthan, 2, Nil)
	DLIB_WXGET_BASE(timespan, TimeSpan, timespan)
	DLIB_WXGET_BASE(timespan, TimeSpan, diff)
	WX_SETBOOL(timespan->operator>(*diff))
}
