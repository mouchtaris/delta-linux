#include "DeltaWxSlider.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxValidator.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(slider, name)
#define WX_FUNC(name) WX_FUNC1(slider, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(clearsel)
WX_FUNC_DEF(clearticks)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getlinesize)
WX_FUNC_DEF(getmax)
WX_FUNC_DEF(getmin)
WX_FUNC_DEF(getpagesize)
WX_FUNC_DEF(getselend)
WX_FUNC_DEF(getselstart)
WX_FUNC_DEF(getthumblength)
WX_FUNC_DEF(gettickfreq)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(setlinesize)
WX_FUNC_DEF(setpagesize)
WX_FUNC_DEF(setrange)
WX_FUNC_DEF(setselection)
WX_FUNC_DEF(setthumblength)
WX_FUNC_DEF(settick)
WX_FUNC_DEF(settickfreq)
WX_FUNC_DEF(setvalue)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(clearsel),
	WX_FUNC(clearticks),
	WX_FUNC(create),
	WX_FUNC(getlinesize),
	WX_FUNC(getmax),
	WX_FUNC(getmin),
	WX_FUNC(getpagesize),
	WX_FUNC(getselend),
	WX_FUNC(getselstart),
	WX_FUNC(getthumblength),
	WX_FUNC(gettickfreq),
	WX_FUNC(getvalue),
	WX_FUNC(setlinesize),
	WX_FUNC(setpagesize),
	WX_FUNC(setrange),
	WX_FUNC(setselection),
	WX_FUNC(setthumblength),
	WX_FUNC(settick),
	WX_FUNC(settickfreq),
	WX_FUNC(setvalue)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "clearsel", "setvalue")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Slider, "slider", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Slider, val, slider) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetRangeMin (void* val, DeltaValue* at) 
{
	wxSlider *slider = DLIB_WXTYPECAST_BASE(Slider, val, slider);
	WX_SETNUMBER_EX(*at, slider->GetMin())
	return true;
}

static bool GetRangeMax (void* val, DeltaValue* at) 
{
	wxSlider *slider = DLIB_WXTYPECAST_BASE(Slider, val, slider);
	WX_SETNUMBER_EX(*at, slider->GetMax())
	return true;
}

static bool GetPageSize (void* val, DeltaValue* at) 
{
	wxSlider *slider = DLIB_WXTYPECAST_BASE(Slider, val, slider);
	WX_SETNUMBER_EX(*at, slider->GetPageSize())
	return true;
}

static bool GetLineSize (void* val, DeltaValue* at) 
{
	wxSlider *slider = DLIB_WXTYPECAST_BASE(Slider, val, slider);
	WX_SETNUMBER_EX(*at, slider->GetLineSize())
	return true;
}

static bool GetTickFrequency (void* val, DeltaValue* at) 
{
	wxSlider *slider = DLIB_WXTYPECAST_BASE(Slider, val, slider);
	WX_SETNUMBER_EX(*at, slider->GetTickFreq())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rangeMin",			&GetRangeMin,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rangeMax",			&GetRangeMax,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pageSize",			&GetPageSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "lineSize",			&GetLineSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "tickFrequency",		&GetTickFrequency,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Slider,slider)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(slider_construct, 0, 10, Nil)
	wxSlider *slider = (wxSlider*) 0;
	if (n == 0) {
		slider = new wxSlider();
	} else if (n >= 5) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		WX_GETNUMBER(value)
		WX_GETNUMBER(minValue)
		WX_GETNUMBER(maxValue)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxSL_HORIZONTAL;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxSliderNameStr;
		if (n >= 6) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 7) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 8) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 9) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 10) { WX_GETSTRING_DEFINED(name) }
		slider = new wxSlider(parent, id, value, minValue, maxValue, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(Slider, slider)
}

WX_FUNC_START(slider_clearsel, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	slider->ClearSel();
}

WX_FUNC_START(slider_clearticks, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	slider->ClearTicks();
}

WX_FUNC_ARGRANGE_START(slider_create, 6, 11, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	WX_GETNUMBER(value)
	WX_GETNUMBER(minValue)
	WX_GETNUMBER(maxValue)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxSL_HORIZONTAL;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxSliderNameStr;
	if (n >= 7) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 8) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 9) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 10) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 11) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(slider->Create(parent, id, value, minValue, maxValue,
		pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(slider);
}

WX_FUNC_START(slider_getlinesize, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_SETNUMBER(slider->GetLineSize())
}

WX_FUNC_START(slider_getmax, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_SETNUMBER(slider->GetMax())
}

WX_FUNC_START(slider_getmin, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_SETNUMBER(slider->GetMin())
}

WX_FUNC_START(slider_getpagesize, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_SETNUMBER(slider->GetPageSize())
}

WX_FUNC_START(slider_getselend, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_SETNUMBER(slider->GetSelEnd())
}

WX_FUNC_START(slider_getselstart, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_SETNUMBER(slider->GetSelStart())
}

WX_FUNC_START(slider_getthumblength, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_SETNUMBER(slider->GetThumbLength())
}

WX_FUNC_START(slider_gettickfreq, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_SETNUMBER(slider->GetTickFreq())
}

WX_FUNC_START(slider_getvalue, 1, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_SETNUMBER(slider->GetValue())
}

WX_FUNC_START(slider_setlinesize, 2, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_GETNUMBER(lineSize)
	slider->SetLineSize(lineSize);
}

WX_FUNC_START(slider_setpagesize, 2, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_GETNUMBER(pageSize)
	slider->SetPageSize(pageSize);
}

WX_FUNC_START(slider_setrange, 3, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_GETNUMBER(minValue)
	WX_GETNUMBER(maxValue)
	slider->SetRange(minValue, maxValue);
}

WX_FUNC_START(slider_setselection, 3, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_GETNUMBER(startPos)
	WX_GETNUMBER(endPos)
	slider->SetSelection(startPos, endPos);
}

WX_FUNC_START(slider_setthumblength, 2, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_GETNUMBER(len)
	slider->SetThumbLength(len);
}

WX_FUNC_START(slider_settick, 2, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_GETNUMBER(tickPos)
	slider->SetTick(tickPos);
}

WX_FUNC_START(slider_settickfreq, 3, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_GETNUMBER(num)
	WX_GETNUMBER(pos)
	slider->SetTickFreq(num, pos);
}

WX_FUNC_START(slider_setvalue, 2, Nil)
	DLIB_WXGET_BASE(slider, Slider, slider)
	WX_GETNUMBER(value)
	slider->SetValue(value);
}
