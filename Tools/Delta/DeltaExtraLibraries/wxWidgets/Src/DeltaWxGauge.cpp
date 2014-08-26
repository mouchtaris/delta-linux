#include "DeltaWxGauge.h"
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
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild
//

#define WXGAUGE_GETBASE(name)										\
	DLIB_WXGET_BASE(gauge, Gauge, name)

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(gauge, name)
#define WX_FUNC(name) WX_FUNC1(gauge, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getbezelface)
WX_FUNC_DEF(getrange)
WX_FUNC_DEF(getshadowwidth)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(isvertical)
WX_FUNC_DEF(setbezelface)
WX_FUNC_DEF(setrange)
WX_FUNC_DEF(setshadowwidth)
WX_FUNC_DEF(setvalue)
WX_FUNC_DEF(pulse)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getbezelface),
	WX_FUNC(getrange),
	WX_FUNC(getshadowwidth),
	WX_FUNC(getvalue),
	WX_FUNC(isvertical),
	WX_FUNC(setbezelface),
	WX_FUNC(setrange),
	WX_FUNC(setshadowwidth),
	WX_FUNC(setvalue),
	WX_FUNC(pulse)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "pulse")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Gauge, "gauge", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Gauge, val, gauge) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetRangeMax (void* val, DeltaValue* at) 
{
	wxGauge *gauge = DLIB_WXTYPECAST_BASE(Gauge, val, gauge);
	WX_SETNUMBER_EX(*at, gauge->GetRange())
	return true;
}

static bool GetRangePos (void* val, DeltaValue* at) 
{
	wxGauge *gauge = DLIB_WXTYPECAST_BASE(Gauge, val, gauge);
	WX_SETNUMBER_EX(*at, gauge->GetValue())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rangeMax",			&GetRangeMax,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rangePos",			&GetRangePos,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Gauge,gauge)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(gauge_construct, 0, 8, Nil)
	wxGauge *gauge = (wxGauge*) 0;
	if (n == 0) {
		gauge = new wxGauge();
	} else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		WX_GETNUMBER(range)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxGA_HORIZONTAL;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxGaugeNameStr;
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		gauge = new wxGauge(parent, id, range, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(Gauge, gauge)
}

WX_FUNC_ARGRANGE_START(gauge_create, 4, 9, Nil)
	WXGAUGE_GETBASE(gauge)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	WX_GETNUMBER(range)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxGA_HORIZONTAL;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxGaugeNameStr;
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(gauge->Create(parent, id, range, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(gauge);
}

WX_FUNC_START(gauge_getbezelface, 1, Nil)
	WXGAUGE_GETBASE(gauge)
	WX_SETNUMBER(gauge->GetBezelFace())
}

WX_FUNC_START(gauge_getrange, 1, Nil)
	WXGAUGE_GETBASE(gauge)
	WX_SETNUMBER(gauge->GetRange())
}

WX_FUNC_START(gauge_getshadowwidth, 1, Nil)
	WXGAUGE_GETBASE(gauge)
	WX_SETNUMBER(gauge->GetShadowWidth())
}

WX_FUNC_START(gauge_getvalue, 1, Nil)
	WXGAUGE_GETBASE(gauge)
	WX_SETNUMBER(gauge->GetValue())
}

WX_FUNC_START(gauge_isvertical, 1, Nil)
	WXGAUGE_GETBASE(gauge)
	WX_SETBOOL(gauge->IsVertical())
}

WX_FUNC_START(gauge_setbezelface, 2, Nil)
	WXGAUGE_GETBASE(gauge)
	WX_GETNUMBER(width)
	gauge->SetBezelFace(width);
}

WX_FUNC_START(gauge_setrange, 2, Nil)
	WXGAUGE_GETBASE(gauge)
	WX_GETNUMBER(range)
	gauge->SetRange(range);
}

WX_FUNC_START(gauge_setshadowwidth, 2, Nil)
	WXGAUGE_GETBASE(gauge)
	WX_GETNUMBER(width)
	gauge->SetShadowWidth(width);
}

WX_FUNC_START(gauge_setvalue, 2, Nil)
	WXGAUGE_GETBASE(gauge)
	WX_GETNUMBER(pos)
	gauge->SetValue(pos);
}

WX_FUNC_START(gauge_pulse, 1, Nil)
	WXGAUGE_GETBASE(gauge)
	gauge->Pulse();
}
