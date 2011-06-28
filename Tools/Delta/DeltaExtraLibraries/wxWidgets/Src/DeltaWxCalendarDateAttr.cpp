#include "DeltaWxCalendarDateAttr.h"
#include "DeltaWxColour.h"
#include "DeltaWxFont.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(calendardateattr, name)
#define WX_FUNC(name) WX_FUNC1(calendardateattr, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(settextcolour)
WX_FUNC_DEF(setbackgroundcolour)
WX_FUNC_DEF(setbordercolour)
WX_FUNC_DEF(setfont)
WX_FUNC_DEF(setborder)
WX_FUNC_DEF(setholiday)
WX_FUNC_DEF(hastextcolour)
WX_FUNC_DEF(hasbackgroundcolour)
WX_FUNC_DEF(hasbordercolour)
WX_FUNC_DEF(hasfont)
WX_FUNC_DEF(hasborder)
WX_FUNC_DEF(isholiday)
WX_FUNC_DEF(gettextcolour)
WX_FUNC_DEF(getbackgroundcolour)
WX_FUNC_DEF(getbordercolour)
WX_FUNC_DEF(getfont)
WX_FUNC_DEF(getborder)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(settextcolour),
	WX_FUNC(setbackgroundcolour),
	WX_FUNC(setbordercolour),
	WX_FUNC(setfont),
	WX_FUNC(setborder),
	WX_FUNC(setholiday),
	WX_FUNC(hastextcolour),
	WX_FUNC(hasbackgroundcolour),
	WX_FUNC(hasbordercolour),
	WX_FUNC(hasfont),
	WX_FUNC(hasborder),
	WX_FUNC(isholiday),
	WX_FUNC(gettextcolour),
	WX_FUNC(getbackgroundcolour),
	WX_FUNC(getbordercolour),
	WX_FUNC(getfont),
	WX_FUNC(getborder)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "getborder")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(CalendarDateAttr, "calendardateattr")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CalendarDateAttr, val, calendardateattr) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetTextColour (void* val, DeltaValue* at) 
{
	wxCalendarDateAttr *attr = DLIB_WXTYPECAST_BASE(CalendarDateAttr, val, calendardateattr);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->GetTextColour())));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static bool GetBackgroundColour (void* val, DeltaValue* at) 
{
	wxCalendarDateAttr *attr = DLIB_WXTYPECAST_BASE(CalendarDateAttr, val, calendardateattr);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->GetBackgroundColour())));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static bool GetBorderColour (void* val, DeltaValue* at) 
{
	wxCalendarDateAttr *attr = DLIB_WXTYPECAST_BASE(CalendarDateAttr, val, calendardateattr);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->GetBorderColour())));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static bool GetFont (void* val, DeltaValue* at) 
{
	wxCalendarDateAttr *attr = DLIB_WXTYPECAST_BASE(CalendarDateAttr, val, calendardateattr);
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(attr->GetFont())));
	WX_SETOBJECT_EX(*at, Font, retval)
	return true;
}

static bool GetBorder (void* val, DeltaValue* at) 
{
	wxCalendarDateAttr *attr = DLIB_WXTYPECAST_BASE(CalendarDateAttr, val, calendardateattr);
	at->FromNumber(attr->GetBorder());
	return true;
}

static bool GetIsHoliday (void* val, DeltaValue* at) 
{
	wxCalendarDateAttr *attr = DLIB_WXTYPECAST_BASE(CalendarDateAttr, val, calendardateattr);
	at->FromBool(attr->IsHoliday());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textColour",			&GetTextColour,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "backgroundColour",	&GetBackgroundColour,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "borderColour",		&GetBorderColour,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "font",				&GetFont,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "border",				&GetBorder,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "holiday",			&GetIsHoliday,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(CalendarDateAttr, calendardateattr);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(calendardateattr_construct, 0, 5, Nil)
	wxCalendarDateAttr *wxattr = (wxCalendarDateAttr*) 0;
	DeltaWxCalendarDateAttr *attr = (DeltaWxCalendarDateAttr*) 0;
	if (n == 0) {
		wxattr = new wxCalendarDateAttr();
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(colour, Colour, colText)
			wxColour colBack = wxNullColour, colBorder = wxNullColour;
			wxFont font = wxNullFont;
			wxCalendarDateBorder border = wxCAL_BORDER_NONE;
			if (n >= 2) { DLIB_WXGET_BASE(colour, Colour, _colBack) colBack = *_colBack; }
			if (n >= 3) { DLIB_WXGET_BASE(colour, Colour, _colBorder) colBorder = *_colBorder; }
			if (n >= 4) { DLIB_WXGET_BASE(font, Font, _font) font = *_font; }
			if (n >= 5) { WX_GETDEFINE(_border) border = (wxCalendarDateBorder)_border; }
			wxattr = new wxCalendarDateAttr(*colText, colBack, colBorder, font, border);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String ||
				   DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETDEFINE(border)
			wxColour colBorder = wxNullColour;
			if (n >= 2) { DLIB_WXGET_BASE(colour, Colour, _colBorder) colBorder = *_colBorder; }
			wxattr = new wxCalendarDateAttr(border, colBorder);
		}
	}
	if (wxattr) attr = DNEWCLASS(DeltaWxCalendarDateAttr, (wxattr));
	WX_SETOBJECT(CalendarDateAttr, attr)
}

DLIB_FUNC_START(calendardateattr_destruct, 1, Nil)
	DLIB_WXDELETE(calendardateattr, CalendarDateAttr, attr)
}

DLIB_FUNC_START(calendardateattr_settextcolour, 2, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	DLIB_WXGET_BASE(colour, Colour, colText)
	attr->SetTextColour(*colText);
}

DLIB_FUNC_START(calendardateattr_setbackgroundcolour, 2, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	DLIB_WXGET_BASE(colour, Colour, colBack)
	attr->SetBackgroundColour(*colBack);
}

DLIB_FUNC_START(calendardateattr_setbordercolour, 2, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	DLIB_WXGET_BASE(colour, Colour, col)
	attr->SetBorderColour(*col);
}

DLIB_FUNC_START(calendardateattr_setfont, 2, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	DLIB_WXGET_BASE(font, Font, font)
	attr->SetFont(*font);
}

DLIB_FUNC_START(calendardateattr_setborder, 2, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	WX_GETDEFINE(border)
	attr->SetBorder((wxCalendarDateBorder)border);
}

DLIB_FUNC_START(calendardateattr_setholiday, 2, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	WX_GETBOOL(holiday)
	attr->SetHoliday(holiday);
}

DLIB_FUNC_START(calendardateattr_hastextcolour, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	WX_SETBOOL(attr->HasTextColour())
}

DLIB_FUNC_START(calendardateattr_hasbackgroundcolour, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	WX_SETBOOL(attr->HasBackgroundColour())
}

DLIB_FUNC_START(calendardateattr_hasbordercolour, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	WX_SETBOOL(attr->HasBorderColour())
}

DLIB_FUNC_START(calendardateattr_hasfont, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	WX_SETBOOL(attr->HasFont())
}

DLIB_FUNC_START(calendardateattr_hasborder, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	WX_SETBOOL(attr->HasBorder())
}

DLIB_FUNC_START(calendardateattr_isholiday, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	WX_SETBOOL(attr->IsHoliday())
}

DLIB_FUNC_START(calendardateattr_gettextcolour, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->GetTextColour())));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(calendardateattr_getbackgroundcolour, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->GetBackgroundColour())));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(calendardateattr_getbordercolour, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->GetBorderColour())));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(calendardateattr_getfont, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(attr->GetFont())));
	WX_SETOBJECT(Font, retval)
}

DLIB_FUNC_START(calendardateattr_getborder, 1, Nil)
	DLIB_WXGET_BASE(calendardateattr, CalendarDateAttr, attr)
	WX_SETNUMBER((wxCalendarDateBorder)attr->GetBorder())
}
