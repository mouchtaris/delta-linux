#include "DeltaWxCalendarCtrl.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxDateTime.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxColour.h"
#include "DeltaWxCalendarDateAttr.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(calendarctrl, name)
#define WX_FUNC(name) WX_FUNC1(calendarctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(setdate)
WX_FUNC_DEF(getdate)
WX_FUNC_DEF(enableyearchange)
WX_FUNC_DEF(enablemonthchange)
WX_FUNC_DEF(enableholidaydisplay)
WX_FUNC_DEF(setheadercolours)
WX_FUNC_DEF(getheadercolourfg)
WX_FUNC_DEF(getheadercolourbg)
WX_FUNC_DEF(sethighlightcolours)
WX_FUNC_DEF(gethighlightcolourfg)
WX_FUNC_DEF(gethighlightcolourbg)
WX_FUNC_DEF(setholidaycolours)
WX_FUNC_DEF(getholidaycolourfg)
WX_FUNC_DEF(getholidaycolourbg)
WX_FUNC_DEF(getattr)
WX_FUNC_DEF(setattr)
WX_FUNC_DEF(setholiday)
WX_FUNC_DEF(resetattr)
WX_FUNC_DEF(hittest)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(setdate),
	WX_FUNC(getdate),
	WX_FUNC(enableyearchange),
	WX_FUNC(enablemonthchange),
	WX_FUNC(enableholidaydisplay),
	WX_FUNC(setheadercolours),
	WX_FUNC(getheadercolourfg),
	WX_FUNC(getheadercolourbg),
	WX_FUNC(sethighlightcolours),
	WX_FUNC(gethighlightcolourfg),
	WX_FUNC(gethighlightcolourbg),
	WX_FUNC(setholidaycolours),
	WX_FUNC(getholidaycolourfg),
	WX_FUNC(getholidaycolourbg),
	WX_FUNC(getattr),
	WX_FUNC(setattr),
	WX_FUNC(setholiday),
	WX_FUNC(resetattr),
	WX_FUNC(hittest)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "hittest")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CalendarCtrl, "calendarctrl", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

#if !wxCHECK_VERSION(2, 9, 0)
static bool GetMonth (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Control, ctrl->GetMonthControl())
	return true;
}

static bool GetYear (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Control, ctrl->GetYearControl())
	return true;
}
#endif

static bool GetDate (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, DateTime, new wxDateTime(ctrl->GetDate()))
	return true;
}

#if !wxCHECK_VERSION(2, 9, 0)
static bool GetLowDate (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, DateTime, new wxDateTime(ctrl->GetLowerDateLimit()))
	return true;
}

static bool GetHighDate (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, DateTime, new wxDateTime(ctrl->GetUpperDateLimit()))
	return true;
}
#endif

static bool GetColHighlightFg (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(ctrl->GetHighlightColourFg()))
	return true;
}

static bool GetColHighlightBg (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(ctrl->GetHighlightColourBg()))
	return true;
}

static bool GetColHolidayFg (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(ctrl->GetHolidayColourFg()))
	return true;
}

static bool GetColHolidayBg (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(ctrl->GetHolidayColourBg()))
	return true;
}

static bool GetColHeaderFg (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(ctrl->GetHeaderColourFg()))
	return true;
}

static bool GetColHeaderBg (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(ctrl->GetHeaderColourBg()))
	return true;
}

static bool GetAttrs (void* val, DeltaValue* at) 
{
	wxCalendarCtrl *ctrl = DLIB_WXTYPECAST_BASE(CalendarCtrl, val, calendarctrl);
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 1; i < 32; ++i) {
		DeltaValue value;
		wxCalendarDateAttr *attr = ctrl->GetAttr(i);
		WX_SETOBJECT_NO_CONTEXT_EX(value, CalendarDateAttr, attr)
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
#if !wxCHECK_VERSION(2, 9, 0)
	{ "month",				&GetMonth,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "year",				&GetYear,				DELTA_GETBYSTRING_NO_PRECOND	},
#endif
	{ "date",				&GetDate,				DELTA_GETBYSTRING_NO_PRECOND	},
#if !wxCHECK_VERSION(2, 9, 0)
	{ "lowdate",			&GetLowDate,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "highdate",			&GetHighDate,			DELTA_GETBYSTRING_NO_PRECOND	},
#endif
	{ "colHightlightFg",	&GetColHighlightFg,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "colHightlightBg",	&GetColHighlightBg,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "colHolidayFg",		&GetColHolidayFg,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "colHolidayBg",		&GetColHolidayBg,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "colHeaderFg",		&GetColHeaderFg,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "colHeaderBg",		&GetColHeaderBg,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "attrs",				&GetAttrs,				DELTA_GETBYSTRING_NO_PRECOND	},
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(CalendarCtrl,calendarctrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(calendarctrl_construct, 0, 7, Nil)
	wxCalendarCtrl *calctrl = (wxCalendarCtrl*) 0;
	if (n == 0) {
		calctrl = new wxCalendarCtrl();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxDateTime date = wxDefaultDateTime;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxCAL_SHOW_HOLIDAYS | wxWANTS_CHARS;
		wxString name = wxCalendarNameStr;
		if (n >= 3) { DLIB_WXGET_BASE(datetime, DateTime, _date) date = *_date; }
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { WX_GETSTRING_DEFINED(name) }
		calctrl = new wxCalendarCtrl(parent, id, date, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(CalendarCtrl, calctrl)
}

WX_FUNC_ARGRANGE_START(calendarctrl_create, 3, 8, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxDateTime date = wxDefaultDateTime;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxCAL_SHOW_HOLIDAYS | wxWANTS_CHARS;
	wxString name = wxCalendarNameStr;
	if (n >= 4) { DLIB_WXGET_BASE(datetime, DateTime, _date) date = *_date; }
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(calctrl->Create(parent, id, date, pos, size, style, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(calctrl);
}

WX_FUNC_START(calendarctrl_setdate, 2, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	DLIB_WXGET_BASE(datetime, DateTime, date)
	calctrl->SetDate(*date);
}

WX_FUNC_START(calendarctrl_getdate, 1, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(DateTime, new wxDateTime(calctrl->GetDate()))
}

WX_FUNC_ARGRANGE_START(calendarctrl_enableyearchange, 1, 2, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	bool enable = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(enable) }
#if wxCHECK_VERSION(2, 9, 0)
	calctrl->EnableMonthChange(enable);	//Used for both month and year
#else
	calctrl->EnableYearChange(enable);
#endif
}

WX_FUNC_ARGRANGE_START(calendarctrl_enablemonthchange, 1, 2, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	bool enable = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(enable) }
	calctrl->EnableMonthChange(enable);
}

WX_FUNC_ARGRANGE_START(calendarctrl_enableholidaydisplay, 1, 2, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	bool enable = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(enable) }
	calctrl->EnableHolidayDisplay(enable);
}

WX_FUNC_START(calendarctrl_setheadercolours, 3, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	DLIB_WXGET_BASE(colour, Colour, colFg)
	DLIB_WXGET_BASE(colour, Colour, colBg)
	calctrl->SetHeaderColours(*colFg, *colBg);
}

WX_FUNC_START(calendarctrl_getheadercolourfg, 1, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(calctrl->GetHeaderColourFg()))
}

WX_FUNC_START(calendarctrl_getheadercolourbg, 1, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(calctrl->GetHeaderColourBg()))
}

WX_FUNC_START(calendarctrl_sethighlightcolours, 3, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	DLIB_WXGET_BASE(colour, Colour, colFg)
	DLIB_WXGET_BASE(colour, Colour, colBg)
	calctrl->SetHighlightColours(*colFg, *colBg);
}

WX_FUNC_START(calendarctrl_gethighlightcolourfg, 1, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(calctrl->GetHighlightColourFg()))
}

WX_FUNC_START(calendarctrl_gethighlightcolourbg, 1, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(calctrl->GetHighlightColourBg()))
}

WX_FUNC_START(calendarctrl_setholidaycolours, 3, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	DLIB_WXGET_BASE(colour, Colour, colFg)
	DLIB_WXGET_BASE(colour, Colour, colBg)
	calctrl->SetHolidayColours(*colFg, *colBg);
}

WX_FUNC_START(calendarctrl_getholidaycolourfg, 1, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(calctrl->GetHolidayColourFg()))
}

WX_FUNC_START(calendarctrl_getholidaycolourbg, 1, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(calctrl->GetHolidayColourBg()))
}

WX_FUNC_START(calendarctrl_getattr, 2, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_GETNUMBER(day)
	WX_SETOBJECT(CalendarDateAttr, calctrl->GetAttr(day))
}

WX_FUNC_START(calendarctrl_setattr, 3, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_GETNUMBER(day)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	calctrl->SetAttr(day, attr);
}

WX_FUNC_START(calendarctrl_setholiday, 2, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_GETNUMBER(day)
	calctrl->SetHoliday(day);
}

WX_FUNC_START(calendarctrl_resetattr, 2, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	WX_GETNUMBER(day)
	calctrl->ResetAttr(day);
}

WX_FUNC_ARGRANGE_START(calendarctrl_hittest, 2, 4, Nil)
	DLIB_WXGET_BASE(calendarctrl, CalendarCtrl, calctrl)
	DLIB_WXGETPOINT_BASE(pos)
	wxDateTime date;
	wxDateTime::WeekDay wd;
	calctrl->HitTest(*pos, &date, &wd);
	if (n >= 3) {
		WX_GETTABLE(date_table)
		DeltaValue value;
		WX_SETOBJECT_EX(value, DateTime, &date)
		WX_SETTABLE_RETVAL(date_table, value)
	}
	if (n >= 4) {
		WX_GETTABLE(wd_table)
		WX_SETTABLE_RETVAL(wd_table, DeltaValue(DeltaNumberValueType((double)wd)))
	}
}
