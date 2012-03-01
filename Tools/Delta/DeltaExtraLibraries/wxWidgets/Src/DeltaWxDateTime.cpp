#include "DeltaWxDateTime.h"
#include "DeltaWxDateSpan.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(datetime, name)
#define WX_FUNC(name) WX_FUNC1(datetime, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(constructfromtimet)
WX_FUNC_DEF(constructfromjdn)
WX_FUNC_DEF(constructfromhms)
WX_FUNC_DEF(constructfromdmy)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(convertyeartobc)
WX_FUNC_DEF(getampmstrings)
WX_FUNC_DEF(getbegindst)
WX_FUNC_DEF(getcountry)
WX_FUNC_DEF(getcurrentyear)
WX_FUNC_DEF(getcurrentmonth)
WX_FUNC_DEF(getcentury)
WX_FUNC_DEF(getenddst)
WX_FUNC_DEF(getmonthname)
WX_FUNC_DEF(getnumberofdaysinyear)
WX_FUNC_DEF(getnumberofdaysinmonth)
WX_FUNC_DEF(gettimenow)
WX_FUNC_DEF(getweekdayname)
WX_FUNC_DEF(isleapyear)
WX_FUNC_DEF(iswesteuropeancountry)
WX_FUNC_DEF(isdstapplicable)
WX_FUNC_DEF(now)
WX_FUNC_DEF(setcountry)
WX_FUNC_DEF(today)
WX_FUNC_DEF(unow)
WX_FUNC_DEF(settocurrent)
WX_FUNC_DEF(settimet)
WX_FUNC_DEF(setjdn)
WX_FUNC_DEF(sethms)
WX_FUNC_DEF(set)
WX_FUNC_DEF(resettime)
WX_FUNC_DEF(setyear)
WX_FUNC_DEF(setmonth)
WX_FUNC_DEF(setday)
WX_FUNC_DEF(sethour)
WX_FUNC_DEF(setminute)
WX_FUNC_DEF(setsecond)
WX_FUNC_DEF(setmillisecond)
WX_FUNC_DEF(isvalid)
WX_FUNC_DEF(getdateonly)
WX_FUNC_DEF(getticks)
WX_FUNC_DEF(getyear)
WX_FUNC_DEF(getmonth)
WX_FUNC_DEF(getday)
WX_FUNC_DEF(getweekday)
WX_FUNC_DEF(gethour)
WX_FUNC_DEF(getminute)
WX_FUNC_DEF(getsecond)
WX_FUNC_DEF(getmillisecond)
WX_FUNC_DEF(getdayofyear)
WX_FUNC_DEF(getweekofyear)
WX_FUNC_DEF(getweekofmonth)
WX_FUNC_DEF(isworkday)
WX_FUNC_DEF(setfromdos)
WX_FUNC_DEF(getasdos)
WX_FUNC_DEF(isequalto)
WX_FUNC_DEF(isnotequalto)
WX_FUNC_DEF(isearlierthan)
WX_FUNC_DEF(islaterthan)
WX_FUNC_DEF(isstrictlybetween)
WX_FUNC_DEF(isbetween)
WX_FUNC_DEF(issamedate)
WX_FUNC_DEF(issametime)
WX_FUNC_DEF(isequalupto)
WX_FUNC_DEF(parserfc822date)
WX_FUNC_DEF(parseformat)
WX_FUNC_DEF(parsedatetime)
WX_FUNC_DEF(parsedate)
WX_FUNC_DEF(parsetime)
WX_FUNC_DEF(format)
WX_FUNC_DEF(formatdate)
WX_FUNC_DEF(formattime)
WX_FUNC_DEF(formatisodate)
WX_FUNC_DEF(formatisotime)
WX_FUNC_DEF(settoweekdayinsameweek)
WX_FUNC_DEF(getweekdayinsameweek)
WX_FUNC_DEF(settonextweekday)
WX_FUNC_DEF(getnextweekday)
WX_FUNC_DEF(settoprevweekday)
WX_FUNC_DEF(getprevweekday)
WX_FUNC_DEF(settoweekday)
WX_FUNC_DEF(settolastweekday)
WX_FUNC_DEF(getlastweekday)
WX_FUNC_DEF(settoweekofyear)
WX_FUNC_DEF(settolastmonthday)
WX_FUNC_DEF(getlastmonthday)
WX_FUNC_DEF(settoyearday)
WX_FUNC_DEF(getyearday)
WX_FUNC_DEF(getjuliandaynumber)
WX_FUNC_DEF(getjdn)
WX_FUNC_DEF(getmodifiedjuliandaynumber)
WX_FUNC_DEF(getmjd)
WX_FUNC_DEF(getratadie)
WX_FUNC_DEF(fromtimezone)
WX_FUNC_DEF(totimezone)
WX_FUNC_DEF(maketimezone)
WX_FUNC_DEF(makefromtimezone)
WX_FUNC_DEF(toutc)
WX_FUNC_DEF(makeutc)
WX_FUNC_DEF(isdst)
WX_FUNC_DEF(add)
WX_FUNC_DEF(subtract)
WX_FUNC_DEF(lessequalthan)
WX_FUNC_DEF(greaterequalthan)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(constructfromtimet),
	WX_FUNC(constructfromjdn),
	WX_FUNC(constructfromhms),
	WX_FUNC(constructfromdmy),
	WX_FUNC(convertyeartobc),
	WX_FUNC(getampmstrings),
	WX_FUNC(getbegindst),
	WX_FUNC(getcountry),
	WX_FUNC(getcurrentyear),
	WX_FUNC(getcurrentmonth),
	WX_FUNC(getcentury),
	WX_FUNC(getenddst),
	WX_FUNC(getmonthname),
	WX_FUNC(getnumberofdaysinyear),
	WX_FUNC(getnumberofdaysinmonth),
	WX_FUNC(gettimenow),
	WX_FUNC(getweekdayname),
	WX_FUNC(isleapyear),
	WX_FUNC(iswesteuropeancountry),
	WX_FUNC(isdstapplicable),
	WX_FUNC(now),
	WX_FUNC(setcountry),
	WX_FUNC(today),
	WX_FUNC(unow),
	WX_FUNC(settoweekofyear),
	WX_FUNC(destruct),
	WX_FUNC(settocurrent),
	WX_FUNC(settimet),
	WX_FUNC(setjdn),
	WX_FUNC(sethms),
	WX_FUNC(set),
	WX_FUNC(resettime),
	WX_FUNC(setyear),
	WX_FUNC(setmonth),
	WX_FUNC(setday),
	WX_FUNC(sethour),
	WX_FUNC(setminute),
	WX_FUNC(setsecond),
	WX_FUNC(setmillisecond),
	WX_FUNC(isvalid),
	WX_FUNC(getdateonly),
	WX_FUNC(getticks),
	WX_FUNC(getyear),
	WX_FUNC(getmonth),
	WX_FUNC(getday),
	WX_FUNC(getweekday),
	WX_FUNC(gethour),
	WX_FUNC(getminute),
	WX_FUNC(getsecond),
	WX_FUNC(getmillisecond),
	WX_FUNC(getdayofyear),
	WX_FUNC(getweekofyear),
	WX_FUNC(getweekofmonth),
	WX_FUNC(isworkday),
	WX_FUNC(setfromdos),
	WX_FUNC(getasdos),
	WX_FUNC(isequalto),
	WX_FUNC(isnotequalto),
	WX_FUNC(isearlierthan),
	WX_FUNC(islaterthan),
	WX_FUNC(isstrictlybetween),
	WX_FUNC(isbetween),
	WX_FUNC(issamedate),
	WX_FUNC(issametime),
	WX_FUNC(isequalupto),
	WX_FUNC(parserfc822date),
	WX_FUNC(parseformat),
	WX_FUNC(parsedatetime),
	WX_FUNC(parsedate),
	WX_FUNC(parsetime),
	WX_FUNC(format),
	WX_FUNC(formatdate),
	WX_FUNC(formattime),
	WX_FUNC(formatisodate),
	WX_FUNC(formatisotime),
	WX_FUNC(settoweekdayinsameweek),
	WX_FUNC(getweekdayinsameweek),
	WX_FUNC(settonextweekday),
	WX_FUNC(getnextweekday),
	WX_FUNC(settoprevweekday),
	WX_FUNC(getprevweekday),
	WX_FUNC(settoweekday),
	WX_FUNC(settolastweekday),
	WX_FUNC(getlastweekday),
	WX_FUNC(settolastmonthday),
	WX_FUNC(getlastmonthday),
	WX_FUNC(settoyearday),
	WX_FUNC(getyearday),
	WX_FUNC(getjuliandaynumber),
	WX_FUNC(getjdn),
	WX_FUNC(getmodifiedjuliandaynumber),
	WX_FUNC(getmjd),
	WX_FUNC(getratadie),
	WX_FUNC(fromtimezone),
	WX_FUNC(totimezone),
	WX_FUNC(maketimezone),
	WX_FUNC(makefromtimezone),
	WX_FUNC(toutc),
	WX_FUNC(makeutc),
	WX_FUNC(isdst),
	WX_FUNC(add),
	WX_FUNC(subtract),
	WX_FUNC(lessequalthan),
	WX_FUNC(greaterequalthan)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(26, uarraysize(funcs) - 26, "destruct", "greaterequalthan")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(DateTime, "datetime")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DateTime, val, datetime) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetTime (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetValue().ToDouble());
	return true;
}

static bool GetTicks (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetTicks());
	return true;
}

static bool GetCentury (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetCentury());
	return true;
}

static bool GetYear (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetYear());
	return true;
}

static bool GetMonth (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetMonth());
	return true;
}

static bool GetDay (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetDay());
	return true;
}

static bool GetWeekDay (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetWeekDay());
	return true;
}

static bool GetHour (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetHour());
	return true;
}

static bool GetMinute (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetMinute());
	return true;
}

static bool GetSecond(void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetSecond());
	return true;
}

static bool GetMillisecond (void* val, DeltaValue* at) 
{
	wxDateTime *datetime = DLIB_WXTYPECAST_BASE(DateTime, val, datetime);
	WX_SETNUMBER_EX(*at, datetime->GetMillisecond());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "time",				&GetTime,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "ticks",				&GetTicks,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "century",			&GetCentury,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "year",				&GetYear,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "month",				&GetMonth,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "day",				&GetDay,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "weekday",			&GetWeekDay,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hour",				&GetHour,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minute",				&GetMinute,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "second",				&GetSecond,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "millisecond",		&GetMillisecond,		DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxDateTimeInitFunc()
{
	DateTimeUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("+"), DeltaValue(&datetime_add_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("-"), DeltaValue(&datetime_subtract_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&datetime_isequalto_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&datetime_isnotequalto_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue(">"), DeltaValue(&datetime_islaterthan_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("<"), DeltaValue(&datetime_isearlierthan_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue(">="), DeltaValue(&datetime_greaterequalthan_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("<="), DeltaValue(&datetime_lessequalthan_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(DateTime, datetime, DeltaWxDateTimeInitFunc();, UEMPTY);

////////////////////////////////////////////////////////////////

#define WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, func)									\
	const wxDateTime& datetime##Ref = datetime->func;											\
	if (&datetime##Ref == datetime) {															\
		DLIB_RETVAL_REF = DPTR(vm)->GetActualArg(0);											\
	} else {																					\
		DeltaWxDateTime *retval = DNEWCLASS(DeltaWxDateTime, (new wxDateTime(datetime##Ref)));	\
		WX_SETOBJECT(DateTime, retval)															\
	}

DLIB_FUNC_START(datetime_construct, 0, Nil)
	DeltaWxDateTime *data = DNEWCLASS(DeltaWxDateTime, (new wxDateTime()));
	WX_SETOBJECT(DateTime, data)
}

DLIB_FUNC_START(datetime_constructfromtimet, 1, Nil)
	WX_GETNUMBER(timet)
	DeltaWxDateTime *data = DNEWCLASS(DeltaWxDateTime, (new wxDateTime((time_t)timet)));
	WX_SETOBJECT(DateTime, data)
}

DLIB_FUNC_START(datetime_constructfromjdn, 1, Nil)
	WX_GETNUMBER(jdn)
	DeltaWxDateTime *data = DNEWCLASS(DeltaWxDateTime, (new wxDateTime((double)jdn)));
	WX_SETOBJECT(DateTime, data)
}

WX_FUNC_ARGRANGE_START(datetime_constructfromhms, 1, 4, Nil)
	WX_GETNUMBER(hour)
	wxDateTime::wxDateTime_t minute = 0, second = 0, millisec = 0;
	if (n >= 2) { WX_GETNUMBER(_minute) minute = (wxDateTime::wxDateTime_t)_minute; }
	if (n >= 3) { WX_GETNUMBER(_second) second = (wxDateTime::wxDateTime_t)_second; }
	if (n >= 4) { WX_GETNUMBER(_millisec) millisec = (wxDateTime::wxDateTime_t)_millisec; }
	DeltaWxDateTime *data = DNEWCLASS(DeltaWxDateTime,
		(new wxDateTime((wxDateTime::wxDateTime_t)hour, minute, second, millisec)));
	WX_SETOBJECT(DateTime, data)
}

WX_FUNC_ARGRANGE_START(datetime_constructfromdmy, 2, 7, Nil)
	WX_GETDEFINE(day)
	WX_GETDEFINE(month)
	int year = wxDateTime::Inv_Year;
	wxDateTime::wxDateTime_t hour = 0, minute = 0, second = 0, millisec = 0;
	if (n >= 3) { WX_GETDEFINE_DEFINED(year) }
	if (n >= 4) { WX_GETNUMBER(_hour) hour = (wxDateTime::wxDateTime_t)_hour; }
	if (n >= 5) { WX_GETNUMBER(_minute) minute = (wxDateTime::wxDateTime_t)_minute; }
	if (n >= 6) { WX_GETNUMBER(_second) second = (wxDateTime::wxDateTime_t)_second; }
	if (n >= 7) { WX_GETNUMBER(_millisec) millisec = (wxDateTime::wxDateTime_t)_millisec; }
	DeltaWxDateTime *data = DNEWCLASS(DeltaWxDateTime, (new wxDateTime((wxDateTime::wxDateTime_t)day,
		(wxDateTime::Month)(int)month, year, hour, minute, second, millisec)));
	WX_SETOBJECT(DateTime, data)
}

DLIB_FUNC_START(datetime_destruct, 1, Nil)
	DLIB_WXDELETE(datetime, DateTime, datetime)
}

DLIB_FUNC_START(datetime_convertyeartobc, 1, Nil)
	WX_GETDEFINE(year)
	WX_SETNUMBER(wxDateTime::ConvertYearToBC(year))
}

DLIB_FUNC_START(datetime_getampmstrings, 2, Nil)
	WX_GETTABLE(am_table)
	WX_GETTABLE(pm_table)
	wxString am, pm;
	wxDateTime::GetAmPmStrings(&am, &pm);
	WX_SETTABLE_RETVAL(am_table, DeltaValue(std::string(am.mb_str(wxConvUTF8))))
	WX_SETTABLE_RETVAL(pm_table, DeltaValue(std::string(pm.mb_str(wxConvUTF8))))
}

WX_FUNC_ARGRANGE_START(datetime_getbegindst, 0, 2, Nil)
	int year = wxDateTime::Inv_Year;
	wxDateTime::Country country = wxDateTime::Country_Default;
	if (n >= 1) { WX_GETDEFINE_DEFINED(year) }
	if (n >= 2) { WX_GETDEFINE(_country) country = (wxDateTime::Country)_country; }
	DeltaWxDateTime *retval = DNEWCLASS(DeltaWxDateTime, (
		new wxDateTime(wxDateTime::GetBeginDST(year, country))));
	WX_SETOBJECT(DateTime, retval)
}

DLIB_FUNC_START(datetime_getcountry, 0, Nil)
	WX_SETNUMBER(wxDateTime::GetCountry())
}

WX_FUNC_ARGRANGE_START(datetime_getcurrentyear, 0, 1, Nil)
	wxDateTime::Calendar cal = wxDateTime::Gregorian;
	if (n >= 1) { WX_GETDEFINE(_cal) cal = (wxDateTime::Calendar)_cal; }
	WX_SETNUMBER(wxDateTime::GetCurrentYear(cal))
}

WX_FUNC_ARGRANGE_START(datetime_getcurrentmonth, 0, 1, Nil)
	wxDateTime::Calendar cal = wxDateTime::Gregorian;
	if (n >= 1) { WX_GETDEFINE(_cal) cal = (wxDateTime::Calendar)_cal; }
	WX_SETNUMBER(wxDateTime::GetCurrentMonth(cal))
}

WX_FUNC_ARGRANGE_START(datetime_getcentury, 1, 2, Nil)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(datetime, DateTime, datetime)
		wxDateTime::TimeZone tz = wxDateTime::Local;
		if (n >= 2) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
		WX_SETNUMBER(datetime->GetCentury(tz))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number ||
			   DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETDEFINE(year)
		WX_SETNUMBER(wxDateTime::GetCentury(year))
	}
}

WX_FUNC_ARGRANGE_START(datetime_getenddst, 0, 2, Nil)
	int year = wxDateTime::Inv_Year;
	wxDateTime::Country country = wxDateTime::Country_Default;
	if (n >= 1) { WX_GETDEFINE_DEFINED(year) }
	if (n >= 2) { WX_GETDEFINE(_country) country = (wxDateTime::Country)_country; }
	DeltaWxDateTime *retval = DNEWCLASS(DeltaWxDateTime, (
		new wxDateTime(wxDateTime::GetEndDST(year, country))));
	WX_SETOBJECT(DateTime, retval)
}

WX_FUNC_ARGRANGE_START(datetime_getmonthname, 1, 2, Nil)
	WX_GETDEFINE(month)
	wxDateTime::NameFlags flags = wxDateTime::Name_Full;
	if (n >= 2) { WX_GETDEFINE(_flags) flags = (wxDateTime::NameFlags)_flags; }
	WX_SETSTRING(wxDateTime::GetMonthName((wxDateTime::Month)month, flags))
}

WX_FUNC_ARGRANGE_START(datetime_getnumberofdaysinyear, 1, 2, Nil)
	WX_GETDEFINE(year)
	wxDateTime::Calendar cal = wxDateTime::Gregorian;
	if (n >= 2) { WX_GETDEFINE(_cal) cal = (wxDateTime::Calendar)_cal; }
	WX_SETNUMBER(wxDateTime::GetNumberOfDays(year, cal))
}

WX_FUNC_ARGRANGE_START(datetime_getnumberofdaysinmonth, 1, 3, Nil)
	WX_GETDEFINE(month)
	int year = wxDateTime::Inv_Year;
	wxDateTime::Calendar cal = wxDateTime::Gregorian;
	if (n >= 2) { WX_GETDEFINE_DEFINED(year) }
	if (n >= 3) { WX_GETDEFINE(_cal) cal = (wxDateTime::Calendar)_cal; }
	WX_SETNUMBER(wxDateTime::GetNumberOfDays((wxDateTime::Month)month, year, cal))
}

DLIB_FUNC_START(datetime_gettimenow, 0, Nil)
	WX_SETNUMBER(wxDateTime::GetTimeNow())
}

WX_FUNC_ARGRANGE_START(datetime_getweekdayname, 1, 2, Nil)
	WX_GETDEFINE(weekday)
	wxDateTime::NameFlags flags = wxDateTime::Name_Full;
	if (n >= 2) { WX_GETDEFINE(_flags) flags = (wxDateTime::NameFlags)_flags; }
	WX_SETSTRING(wxDateTime::GetWeekDayName((wxDateTime::WeekDay)weekday, flags))
}

WX_FUNC_ARGRANGE_START(datetime_isleapyear, 0, 2, Nil)
	int year = wxDateTime::Inv_Year;
	wxDateTime::Calendar cal = wxDateTime::Gregorian;
	if (n >= 1) { WX_GETDEFINE_DEFINED(year) }
	if (n >= 2) { WX_GETDEFINE(_cal) cal = (wxDateTime::Calendar)_cal; }
	WX_SETBOOL(wxDateTime::IsLeapYear(year, cal))
}

WX_FUNC_ARGRANGE_START(datetime_iswesteuropeancountry, 0, 1, Nil)
	wxDateTime::Country country = wxDateTime::Country_Default;
	if (n >= 1) { WX_GETDEFINE(_country) country = (wxDateTime::Country)_country; }
	WX_SETBOOL(wxDateTime::IsWestEuropeanCountry(country))
}

WX_FUNC_ARGRANGE_START(datetime_isdstapplicable, 0, 2, Nil)
	int year = wxDateTime::Inv_Year;
	wxDateTime::Country country = wxDateTime::Country_Default;
	if (n >= 1) { WX_GETDEFINE_DEFINED(year) }
	if (n >= 2) { WX_GETDEFINE(_country) country = (wxDateTime::Country)_country; }
	WX_SETBOOL(wxDateTime::IsDSTApplicable(year, country))
}

DLIB_FUNC_START(datetime_now, 0, Nil)
	DeltaWxDateTime *retval = DNEWCLASS(DeltaWxDateTime, (new wxDateTime(wxDateTime::Now())));
	WX_SETOBJECT(DateTime, retval)
}

DLIB_FUNC_START(datetime_setcountry, 1, Nil)
	WX_GETDEFINE(country)
	wxDateTime::SetCountry((wxDateTime::Country)country);
}

DLIB_FUNC_START(datetime_today, 0, Nil)
	DeltaWxDateTime *retval = DNEWCLASS(DeltaWxDateTime, (new wxDateTime(wxDateTime::Today())));
	WX_SETOBJECT(DateTime, retval)
}

DLIB_FUNC_START(datetime_unow, 0, Nil)
	DeltaWxDateTime *retval = DNEWCLASS(DeltaWxDateTime, (new wxDateTime(wxDateTime::UNow())));
	WX_SETOBJECT(DateTime, retval)
}

DLIB_FUNC_START(datetime_settocurrent, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetToCurrent())
}

DLIB_FUNC_START(datetime_settimet, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(timet)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, Set((time_t)timet))
}

DLIB_FUNC_START(datetime_setjdn, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(jdn)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, Set((double)jdn))
}

WX_FUNC_ARGRANGE_START(datetime_sethms, 2, 5, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(hour)
	wxDateTime::wxDateTime_t minute = 0, second = 0, millisec = 0;
	if (n >= 3) { WX_GETNUMBER(_minute) minute = (wxDateTime::wxDateTime_t)_minute; }
	if (n >= 4) { WX_GETNUMBER(_second) second = (wxDateTime::wxDateTime_t)_second; }
	if (n >= 5) { WX_GETNUMBER(_millisec) millisec = (wxDateTime::wxDateTime_t)_millisec; }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, Set((wxDateTime::wxDateTime_t)hour,
		minute, second, millisec))
}

WX_FUNC_ARGRANGE_START(datetime_set, 3, 8, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(day)
	WX_GETDEFINE(month)
	int year = wxDateTime::Inv_Year;
	wxDateTime::wxDateTime_t hour = 0, minute = 0, second = 0, millisec = 0;
	if (n >= 4) { WX_GETDEFINE_DEFINED(year) }
	if (n >= 5) { WX_GETNUMBER(_hour) hour = (wxDateTime::wxDateTime_t)_hour; }
	if (n >= 6) { WX_GETNUMBER(_minute) minute = (wxDateTime::wxDateTime_t)_minute; }
	if (n >= 7) { WX_GETNUMBER(_second) second = (wxDateTime::wxDateTime_t)_second; }
	if (n >= 8) { WX_GETNUMBER(_millisec) millisec = (wxDateTime::wxDateTime_t)_millisec; }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, Set((wxDateTime::wxDateTime_t)day,
		(wxDateTime::Month)month, year, hour, minute, second, millisec))
}

DLIB_FUNC_START(datetime_resettime, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, ResetTime())
}

DLIB_FUNC_START(datetime_setyear, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(year)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetYear(year))
}

DLIB_FUNC_START(datetime_setmonth, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(month)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetMonth((wxDateTime::Month)month))
}

DLIB_FUNC_START(datetime_setday, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(day)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetDay((wxDateTime::wxDateTime_t)day))
}

DLIB_FUNC_START(datetime_sethour, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(hour)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetHour((wxDateTime::wxDateTime_t)hour))
}

DLIB_FUNC_START(datetime_setminute, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(minute)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetMinute((wxDateTime::wxDateTime_t)minute))
}

DLIB_FUNC_START(datetime_setsecond, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(second)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetSecond((wxDateTime::wxDateTime_t)second))
}

DLIB_FUNC_START(datetime_setmillisecond, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(millisecond)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetMillisecond((wxDateTime::wxDateTime_t)millisecond))
}

DLIB_FUNC_START(datetime_isvalid, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETBOOL(datetime->IsValid())
}

DLIB_FUNC_START(datetime_getdateonly, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, GetDateOnly())
}

DLIB_FUNC_START(datetime_getticks, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETNUMBER(datetime->GetTicks())
}

WX_FUNC_ARGRANGE_START(datetime_getyear, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetYear(tz))
}

WX_FUNC_ARGRANGE_START(datetime_getmonth, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetMonth(tz))
}

WX_FUNC_ARGRANGE_START(datetime_getday, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetDay(tz))
}

WX_FUNC_ARGRANGE_START(datetime_getweekday, 1, 5, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	int tz = wxDateTime::Local;
	int num = 1, year = wxDateTime::Inv_Year;
	wxDateTime::Month month = wxDateTime::Inv_Month;
	if (n >= 2) { WX_GETDEFINE(_tz) tz = _tz; }
	if (n >= 3) { WX_GETNUMBER_DEFINED(num) }
	if (n >= 4) { WX_GETDEFINE(_month) month = (wxDateTime::Month)_month; }
	if (n >= 5) { WX_GETDEFINE_DEFINED(year) }
	if (n >= 3) {
		WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, GetWeekDay(
			(wxDateTime::WeekDay)tz, num, month, year))
	} else {
		WX_SETNUMBER(datetime->GetWeekDay((wxDateTime::TimeZone)tz))
	}
}

WX_FUNC_ARGRANGE_START(datetime_gethour, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetHour(tz))
}

WX_FUNC_ARGRANGE_START(datetime_getminute, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetMinute(tz))
}

WX_FUNC_ARGRANGE_START(datetime_getsecond, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetSecond(tz))
}

WX_FUNC_ARGRANGE_START(datetime_getmillisecond, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetMillisecond(tz))
}

WX_FUNC_ARGRANGE_START(datetime_getdayofyear, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetDayOfYear(tz))
}

WX_FUNC_ARGRANGE_START(datetime_getweekofyear, 1, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::WeekFlags flags = wxDateTime::Monday_First;
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_flags) flags = (wxDateTime::WeekFlags)_flags; }
	if (n >= 3) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetWeekOfYear(flags, tz))
}

WX_FUNC_ARGRANGE_START(datetime_getweekofmonth, 1, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::WeekFlags flags = wxDateTime::Monday_First;
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETDEFINE(_flags) flags = (wxDateTime::WeekFlags)_flags; }
	if (n >= 3) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETNUMBER(datetime->GetWeekOfMonth(flags, tz))
}

WX_FUNC_ARGRANGE_START(datetime_isworkday, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::Country country = wxDateTime::Country_Default;
	if (n >= 2) { WX_GETDEFINE(_country) country = (wxDateTime::Country)_country; }
	WX_SETBOOL(datetime->IsWorkDay(country))
}

DLIB_FUNC_START(datetime_setfromdos, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(ddt)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetFromDOS(ddt))
}

DLIB_FUNC_START(datetime_getasdos, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETNUMBER(datetime->GetAsDOS())
}

DLIB_FUNC_START(datetime_isequalto, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	WX_SETBOOL(datetime->IsEqualTo(*dt))
}

DLIB_FUNC_START(datetime_isnotequalto, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	WX_SETBOOL(datetime->operator!=(*dt))
}

DLIB_FUNC_START(datetime_isearlierthan, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	WX_SETBOOL(datetime->IsEarlierThan(*dt))
}

DLIB_FUNC_START(datetime_islaterthan, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	WX_SETBOOL(datetime->IsLaterThan(*dt))
}

DLIB_FUNC_START(datetime_isstrictlybetween, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, t1)
	DLIB_WXGET_BASE(datetime, DateTime, t2)
	WX_SETBOOL(datetime->IsStrictlyBetween(*t1, *t2))
}

DLIB_FUNC_START(datetime_isbetween, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, t1)
	DLIB_WXGET_BASE(datetime, DateTime, t2)
	WX_SETBOOL(datetime->IsBetween(*t1, *t2))
}

DLIB_FUNC_START(datetime_issamedate, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	WX_SETBOOL(datetime->IsSameDate(*dt))
}

DLIB_FUNC_START(datetime_issametime, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	WX_SETBOOL(datetime->IsSameTime(*dt))
}

DLIB_FUNC_START(datetime_isequalupto, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	DLIB_WXGET_BASE(timespan, TimeSpan, ts)
	WX_SETBOOL(datetime->IsEqualUpTo(*dt, *ts))
}

DLIB_FUNC_START(datetime_parserfc822date, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETSTRING(date)
	wxChar *retval = (wxChar*)datetime->ParseRfc822Date(date.c_str());
	if (retval == NULL) {
		DLIB_RETVAL_REF.FromNil();
	} else {
		WX_SETSTRING(wxString(retval))
	}
}

WX_FUNC_ARGRANGE_START(datetime_parseformat, 2, 4, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETSTRING(date)
	const wxChar *format = wxDefaultDateTimeFormat;
	wxDateTime dateDef = wxDefaultDateTime;
	if (n >= 3) { WX_GETSTRING(_format) format = _format.c_str(); }
	if (n >= 4) { DLIB_WXGET_BASE(datetime, DateTime, _dateDef) dateDef = *_dateDef; }
	wxChar *retval = (wxChar*)datetime->ParseFormat(date.c_str(), format, dateDef);
	if (retval == NULL) {
		DLIB_RETVAL_REF.FromNil();
	} else {
		WX_SETSTRING(wxString(retval))
	}
}

DLIB_FUNC_START(datetime_parsedatetime, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETSTRING(date)
	wxChar *retval = (wxChar*)datetime->ParseDateTime(date.c_str());
	if (retval == NULL) {
		DLIB_RETVAL_REF.FromNil();
	} else {
		WX_SETSTRING(wxString(retval))
	}
}

DLIB_FUNC_START(datetime_parsedate, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETSTRING(date)
	wxChar *retval = (wxChar*)datetime->ParseDate(date.c_str());
	if (retval == NULL) {
		DLIB_RETVAL_REF.FromNil();
	} else {
		WX_SETSTRING(wxString(retval))
	}
}

DLIB_FUNC_START(datetime_parsetime, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETSTRING(date)
	wxChar *retval = (wxChar*)datetime->ParseTime(date.c_str());
	if (retval == NULL) {
		DLIB_RETVAL_REF.FromNil();
	} else {
		WX_SETSTRING(wxString(retval))
	}
}

WX_FUNC_ARGRANGE_START(datetime_format, 1, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	const wxChar *format = wxDefaultDateTimeFormat;
	wxDateTime::TimeZone tz = wxDateTime::Local;
	if (n >= 2) { WX_GETSTRING(_format) format = _format.c_str(); }
	if (n >= 3) { WX_GETDEFINE(_tz) tz = (wxDateTime::TimeZone)_tz; }
	WX_SETSTRING(datetime->Format(format, tz))
}

DLIB_FUNC_START(datetime_formatdate, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETSTRING(datetime->FormatDate())
}

DLIB_FUNC_START(datetime_formattime, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETSTRING(datetime->FormatTime())
}

DLIB_FUNC_START(datetime_formatisodate, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETSTRING(datetime->FormatISODate())
}

DLIB_FUNC_START(datetime_formatisotime, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETSTRING(datetime->FormatISOTime())
}

WX_FUNC_ARGRANGE_START(datetime_settoweekdayinsameweek, 2, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(weekday)
	wxDateTime::WeekFlags flags = wxDateTime::Monday_First;
	if (n >= 3) { WX_GETDEFINE(_flags) flags = (wxDateTime::WeekFlags)_flags; }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetToWeekDayInSameWeek((wxDateTime::WeekDay)weekday, flags))
}

WX_FUNC_ARGRANGE_START(datetime_getweekdayinsameweek, 2, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(weekday)
	wxDateTime::WeekFlags flags = wxDateTime::Monday_First;
	if (n >= 3) { WX_GETDEFINE(_flags) flags = (wxDateTime::WeekFlags)_flags; }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, GetWeekDayInSameWeek((wxDateTime::WeekDay)weekday, flags))
}

DLIB_FUNC_START(datetime_settonextweekday, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(weekday)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetToNextWeekDay((wxDateTime::WeekDay)weekday))
}

DLIB_FUNC_START(datetime_getnextweekday, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(weekday)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, GetNextWeekDay((wxDateTime::WeekDay)weekday))
}

DLIB_FUNC_START(datetime_settoprevweekday, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(weekday)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetToPrevWeekDay((wxDateTime::WeekDay)weekday))
}

DLIB_FUNC_START(datetime_getprevweekday, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(weekday)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, GetPrevWeekDay((wxDateTime::WeekDay)weekday))
}

WX_FUNC_ARGRANGE_START(datetime_settoweekday, 2, 5, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(weekday)
	int num = 1, year = wxDateTime::Inv_Year;
	wxDateTime::Month month = wxDateTime::Inv_Month;
	if (n >= 3) { WX_GETNUMBER_DEFINED(num) }
	if (n >= 4) { WX_GETDEFINE(_month) month = (wxDateTime::Month)_month; }
	if (n >= 5) { WX_GETDEFINE_DEFINED(year) }
	WX_SETBOOL(datetime->SetToWeekDay((wxDateTime::WeekDay)weekday, num, month, year))
}

WX_FUNC_ARGRANGE_START(datetime_settolastweekday, 2, 4, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(weekday)
	wxDateTime::Month month = wxDateTime::Inv_Month;
	int year = wxDateTime::Inv_Year;
	if (n >= 3) { WX_GETDEFINE(_month) month = (wxDateTime::Month)_month; }
	if (n >= 4) { WX_GETDEFINE_DEFINED(year) }
	WX_SETBOOL(datetime->SetToLastWeekDay((wxDateTime::WeekDay)weekday, month, year))
}

WX_FUNC_ARGRANGE_START(datetime_getlastweekday, 2, 4, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(weekday)
	wxDateTime::Month month = wxDateTime::Inv_Month;
	int year = wxDateTime::Inv_Year;
	if (n >= 3) { WX_GETDEFINE(_month) month = (wxDateTime::Month)_month; }
	if (n >= 4) { WX_GETDEFINE_DEFINED(year) }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, GetLastWeekDay((wxDateTime::WeekDay)weekday, month, year))
}

WX_FUNC_ARGRANGE_START(datetime_settoweekofyear, 2, 3, Nil)
	WX_GETDEFINE(year)
	WX_GETNUMBER(numWeek)
	wxDateTime::WeekDay weekday = wxDateTime::Mon;
	if (n >= 3) { WX_GETDEFINE(_weekday) weekday = (wxDateTime::WeekDay)_weekday; }
	DeltaWxDateTime *retval = DNEWCLASS(DeltaWxDateTime, (
		new wxDateTime(wxDateTime::SetToWeekOfYear(year, numWeek, weekday))));
	WX_SETOBJECT(DateTime, retval)
}

WX_FUNC_ARGRANGE_START(datetime_settolastmonthday, 1, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::Month month = wxDateTime::Inv_Month;
	int year = wxDateTime::Inv_Year;
	if (n >= 2) { WX_GETDEFINE(_month) month = (wxDateTime::Month)_month; }
	if (n >= 3) { WX_GETDEFINE_DEFINED(year) }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetToLastMonthDay(month, year))
}

WX_FUNC_ARGRANGE_START(datetime_getlastmonthday, 1, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::Month month = wxDateTime::Inv_Month;
	int year = wxDateTime::Inv_Year;
	if (n >= 2) { WX_GETDEFINE(_month) month = (wxDateTime::Month)_month; }
	if (n >= 3) { WX_GETDEFINE_DEFINED(year) }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, GetLastMonthDay(month, year))
}

DLIB_FUNC_START(datetime_settoyearday, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(yday)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, SetToYearDay(yday))
}

DLIB_FUNC_START(datetime_getyearday, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETNUMBER(yday)
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, GetYearDay(yday))
}

DLIB_FUNC_START(datetime_getjuliandaynumber, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETNUMBER(datetime->GetJulianDayNumber())
}

DLIB_FUNC_START(datetime_getjdn, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETNUMBER(datetime->GetJDN())
}

DLIB_FUNC_START(datetime_getmodifiedjuliandaynumber, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETNUMBER(datetime->GetModifiedJulianDayNumber())
}

DLIB_FUNC_START(datetime_getmjd, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETNUMBER(datetime->GetMJD())
}

DLIB_FUNC_START(datetime_getratadie, 1, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_SETNUMBER(datetime->GetRataDie())
}

WX_FUNC_ARGRANGE_START(datetime_fromtimezone, 2, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(tz)
	bool noDST = false;
	if (n >= 3) { WX_GETBOOL_DEFINED(noDST) }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, FromTimezone((wxDateTime::TimeZone)tz, noDST))
}

WX_FUNC_ARGRANGE_START(datetime_totimezone, 2, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(tz)
	bool noDST = false;
	if (n >= 3) { WX_GETBOOL_DEFINED(noDST) }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, ToTimezone((wxDateTime::TimeZone)tz, noDST))
}

WX_FUNC_ARGRANGE_START(datetime_maketimezone, 2, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(tz)
	bool noDST = false;
	if (n >= 3) { WX_GETBOOL_DEFINED(noDST) }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, MakeTimezone((wxDateTime::TimeZone)tz, noDST))
}

WX_FUNC_ARGRANGE_START(datetime_makefromtimezone, 2, 3, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	WX_GETDEFINE(tz)
	bool noDST = false;
	if (n >= 3) { WX_GETBOOL_DEFINED(noDST) }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, MakeFromTimezone((wxDateTime::TimeZone)tz, noDST))
}

WX_FUNC_ARGRANGE_START(datetime_toutc, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	bool noDST = false;
	if (n >= 2) { WX_GETBOOL_DEFINED(noDST) }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, ToUTC(noDST))
}

WX_FUNC_ARGRANGE_START(datetime_makeutc, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	bool noDST = false;
	if (n >= 2) { WX_GETBOOL_DEFINED(noDST) }
	WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, MakeUTC(noDST))
}

WX_FUNC_ARGRANGE_START(datetime_isdst, 1, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	wxDateTime::Country country = wxDateTime::Country_Default;
	if (n >= 2) { WX_GETDEFINE(_country) country = (wxDateTime::Country)_country; }
	WX_SETNUMBER(datetime->IsDST(country))
}

DLIB_FUNC_START(datetime_add, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(TimeSpan, serial_no, timespan, timespan)) {
			wxTimeSpan *diff = (wxTimeSpan*) timespan->GetCastToNativeInstance();
			WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, Add(*diff))
		} else
		if (DLIB_WXISBASE(DateSpan, serial_no, datespan, datespan)) {
			wxDateSpan *diff = (wxDateSpan*) datespan->GetCastToNativeInstance();
			WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, Add(*diff))
		}
	}
}

DLIB_FUNC_START(datetime_subtract, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(TimeSpan, serial_no, timespan, timespan)) {
			wxTimeSpan *diff = (wxTimeSpan*) timespan->GetCastToNativeInstance();
			WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, Subtract(*diff))
		} else
		if (DLIB_WXISBASE(DateSpan, serial_no, datespan, datespan)) {
			wxDateSpan *diff = (wxDateSpan*) datespan->GetCastToNativeInstance();
			WXDATETIME_AVOID_UNNECESSARY_OBJECTS(datetime, Subtract(*diff))
		} else
		if (DLIB_WXISBASE(DateTime, serial_no, datetime, datetime_wr)) {
			wxDateTime *dt = (wxDateTime*) datetime_wr->GetCastToNativeInstance();
			DeltaWxTimeSpan *retval = DNEWCLASS(DeltaWxTimeSpan, (
				new wxTimeSpan(datetime->Subtract(*dt))));
			WX_SETOBJECT(TimeSpan, retval)
		}
	}
}

DLIB_FUNC_START(datetime_lessequalthan, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	WX_SETBOOL(datetime->operator<=(*dt))
}

DLIB_FUNC_START(datetime_greaterequalthan, 2, Nil)
	DLIB_WXGET_BASE(datetime, DateTime, datetime)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	WX_SETBOOL(datetime->operator>=(*dt))
}
