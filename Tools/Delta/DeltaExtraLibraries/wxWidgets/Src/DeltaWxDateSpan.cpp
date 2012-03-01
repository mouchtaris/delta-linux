#include "DeltaWxDateSpan.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(datespan, name)
#define WX_FUNC(name) WX_FUNC1(datespan, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(add)
WX_FUNC_DEF(day)
WX_FUNC_DEF(days)
WX_FUNC_DEF(getdays)
WX_FUNC_DEF(getmonths)
WX_FUNC_DEF(gettotaldays)
WX_FUNC_DEF(getweeks)
WX_FUNC_DEF(getyears)
WX_FUNC_DEF(month)
WX_FUNC_DEF(months)
WX_FUNC_DEF(multiply)
WX_FUNC_DEF(negate)
WX_FUNC_DEF(neg)
WX_FUNC_DEF(setdays)
WX_FUNC_DEF(setyears)
WX_FUNC_DEF(setmonths)
WX_FUNC_DEF(setweeks)
WX_FUNC_DEF(subtract)
WX_FUNC_DEF(week)
WX_FUNC_DEF(weeks)
WX_FUNC_DEF(year)
WX_FUNC_DEF(years)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)
WX_FUNC_DEF(assign)
WX_FUNC_DEF(minus)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(day),
	WX_FUNC(days),
	WX_FUNC(month),
	WX_FUNC(months),
	WX_FUNC(week),
	WX_FUNC(weeks),
	WX_FUNC(year),
	WX_FUNC(years),
	WX_FUNC(destruct),
	WX_FUNC(add),
	WX_FUNC(getdays),
	WX_FUNC(getmonths),
	WX_FUNC(gettotaldays),
	WX_FUNC(getweeks),
	WX_FUNC(getyears),
	WX_FUNC(multiply),
	WX_FUNC(negate),
	WX_FUNC(neg),
	WX_FUNC(setdays),
	WX_FUNC(setyears),
	WX_FUNC(setmonths),
	WX_FUNC(setweeks),
	WX_FUNC(subtract),
	WX_FUNC(equal),
	WX_FUNC(notequal),
	WX_FUNC(assign),
	WX_FUNC(minus)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(9, uarraysize(funcs) - 9, "destruct", "minus")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(DateSpan, "datespan")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DateSpan, val, datespan) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetYears (void* val, DeltaValue* at) 
{
	wxDateSpan *datespan = DLIB_WXTYPECAST_BASE(DateSpan, val, datespan);
	WX_SETNUMBER_EX(*at, datespan->GetYears());
	return true;
}

static bool GetMonths (void* val, DeltaValue* at) 
{
	wxDateSpan *datespan = DLIB_WXTYPECAST_BASE(DateSpan, val, datespan);
	WX_SETNUMBER_EX(*at, datespan->GetMonths());
	return true;
}

static bool GetWeeks (void* val, DeltaValue* at) 
{
	wxDateSpan *datespan = DLIB_WXTYPECAST_BASE(DateSpan, val, datespan);
	WX_SETNUMBER_EX(*at, datespan->GetWeeks());
	return true;
}

static bool GetDays (void* val, DeltaValue* at) 
{
	wxDateSpan *datespan = DLIB_WXTYPECAST_BASE(DateSpan, val, datespan);
	WX_SETNUMBER_EX(*at, datespan->GetDays());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "years",				&GetYears,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "months",				&GetMonths,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "weeks",				&GetWeeks,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "days",				&GetDays,				DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxDateSpanInitFunc()
{
	DateSpanUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&datespan_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&datespan_notequal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("+"), DeltaValue(&datespan_add_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("-"), DeltaValue(&datespan_minus_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("*"), DeltaValue(&datespan_multiply_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("="), DeltaValue(&datespan_assign_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(DateSpan, datespan, DeltaWxDateSpanInitFunc();, UEMPTY);

////////////////////////////////////////////////////////////////

#define WXDATESPAN_AVOID_UNNECESSARY_OBJECTS_NO_FUNC(datespan, datespanRef)						\
	if (&datespanRef == datespan) {																\
		DLIB_RETVAL_REF = DPTR(vm)->GetActualArg(0);											\
	} else {																					\
		DeltaWxDateSpan *retval = DNEWCLASS(DeltaWxDateSpan, (new wxDateSpan(datespanRef)));	\
		WX_SETOBJECT(DateSpan, retval)															\
	}

#define WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, func)									\
	const wxDateSpan& datespan##Ref = datespan->func;											\
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS_NO_FUNC(datespan, datespan##Ref)

WX_FUNC_ARGRANGE_START(datespan_construct, 0, 4, Nil)
	int years = 0, months = 0, weeks = 0, days = 0;
	if (n >= 1) { WX_GETNUMBER_DEFINED(years) }
	if (n >= 2) { WX_GETNUMBER_DEFINED(months) }
	if (n >= 3) { WX_GETNUMBER_DEFINED(weeks) }
	if (n >= 4) { WX_GETNUMBER_DEFINED(days) }
	DeltaWxDateSpan *datespan = DNEWCLASS(DeltaWxDateSpan,
		(new wxDateSpan(years, months, weeks, days)));
	WX_SETOBJECT(DateSpan, datespan)
}

DLIB_FUNC_START(datespan_destruct, 1, Nil)
	DLIB_WXDELETE(datespan, DateSpan, datespan)
}

DLIB_FUNC_START(datespan_add, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	DLIB_WXGET_BASE(datespan, DateSpan, other)
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, Add(*other))
}

DLIB_FUNC_START(datespan_day, 0, Nil)
	DeltaWxDateSpan *retval = DNEWCLASS(DeltaWxDateSpan, (new wxDateSpan(wxDateSpan::Day())));
	WX_SETOBJECT(DateSpan, retval)
}

DLIB_FUNC_START(datespan_days, 1, Nil)
	WX_GETNUMBER(days)
	DeltaWxDateSpan *retval = DNEWCLASS(DeltaWxDateSpan, (new wxDateSpan(wxDateSpan::Days(days))));
	WX_SETOBJECT(DateSpan, retval)
}

DLIB_FUNC_START(datespan_getdays, 1, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_SETNUMBER(datespan->GetDays())
}

DLIB_FUNC_START(datespan_getmonths, 1, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_SETNUMBER(datespan->GetMonths())
}

DLIB_FUNC_START(datespan_gettotaldays, 1, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_SETNUMBER(datespan->GetTotalDays())
}

DLIB_FUNC_START(datespan_getweeks, 1, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_SETNUMBER(datespan->GetWeeks())
}

DLIB_FUNC_START(datespan_getyears, 1, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_SETNUMBER(datespan->GetYears())
}

DLIB_FUNC_START(datespan_month, 0, Nil)
	DeltaWxDateSpan *retval = DNEWCLASS(DeltaWxDateSpan, (new wxDateSpan(wxDateSpan::Month())));
	WX_SETOBJECT(DateSpan, retval)
}

DLIB_FUNC_START(datespan_months, 1, Nil)
	WX_GETNUMBER(months)
	DeltaWxDateSpan *retval = DNEWCLASS(DeltaWxDateSpan, (new wxDateSpan(wxDateSpan::Months(months))));
	WX_SETOBJECT(DateSpan, retval)
}

DLIB_FUNC_START(datespan_multiply, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_GETNUMBER(factor)
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, Multiply(factor))
}

DLIB_FUNC_START(datespan_negate, 1, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, Negate())
}

DLIB_FUNC_START(datespan_neg, 1, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, Neg())
}

DLIB_FUNC_START(datespan_setdays, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_GETNUMBER(days)
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, SetDays(days))
}

DLIB_FUNC_START(datespan_setyears, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_GETNUMBER(years)
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, SetYears(years))
}

DLIB_FUNC_START(datespan_setmonths, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_GETNUMBER(months)
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, SetMonths(months))
}

DLIB_FUNC_START(datespan_setweeks, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	WX_GETNUMBER(weeks)
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, SetWeeks(weeks))
}

DLIB_FUNC_START(datespan_subtract, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	DLIB_WXGET_BASE(datespan, DateSpan, other)
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS(datespan, Subtract(*other))
}

DLIB_FUNC_START(datespan_week, 0, Nil)
	DeltaWxDateSpan *retval = DNEWCLASS(DeltaWxDateSpan, (new wxDateSpan(wxDateSpan::Week())));
	WX_SETOBJECT(DateSpan, retval)
}

DLIB_FUNC_START(datespan_weeks, 1, Nil)
	WX_GETNUMBER(weeks)
	DeltaWxDateSpan *retval = DNEWCLASS(DeltaWxDateSpan, (new wxDateSpan(wxDateSpan::Weeks(weeks))));
	WX_SETOBJECT(DateSpan, retval)
}

DLIB_FUNC_START(datespan_year, 0, Nil)
	DeltaWxDateSpan *retval = DNEWCLASS(DeltaWxDateSpan, (new wxDateSpan(wxDateSpan::Year())));
	WX_SETOBJECT(DateSpan, retval)
}

DLIB_FUNC_START(datespan_years, 1, Nil)
	WX_GETNUMBER(years)
	DeltaWxDateSpan *retval = DNEWCLASS(DeltaWxDateSpan, (new wxDateSpan(wxDateSpan::Years(years))));
	WX_SETOBJECT(DateSpan, retval)
}

DLIB_FUNC_START(datespan_equal, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	DLIB_WXGET_BASE(datespan, DateSpan, other)
	WX_SETBOOL(datespan->operator==(*other))
}

DLIB_FUNC_START(datespan_notequal, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	DLIB_WXGET_BASE(datespan, DateSpan, other)
	WX_SETBOOL(datespan->operator!=(*other))
}

DLIB_FUNC_START(datespan_assign, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	DLIB_WXGET_BASE(datespan, DateSpan, other)
	datespan->operator=(*other);
}

WX_FUNC_ARGRANGE_START(datespan_minus, 1, 2, Nil)
	DLIB_WXGET_BASE(datespan, DateSpan, datespan)
	wxDateSpan datespanRef;
	if (n == 1) {
		datespanRef = datespan->operator-();
	} else {
		DLIB_WXGET_BASE(datespan, DateSpan, other)
		datespanRef = datespan->operator-(*other);
	}
	WXDATESPAN_AVOID_UNNECESSARY_OBJECTS_NO_FUNC(datespan, datespanRef)
}
