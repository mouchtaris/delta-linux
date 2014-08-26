#include "DeltaWxDatePickerCtrl.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxDateTime.h"
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

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(datepickerctrl, name)
#define WX_FUNC(name) WX_FUNC1(datepickerctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getrange)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(setrange)
WX_FUNC_DEF(setvalue)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getrange),
	WX_FUNC(getvalue),
	WX_FUNC(setrange),
	WX_FUNC(setvalue)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "setvalue")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DatePickerCtrl, "datepickerctrl", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DatePickerCtrl, val, datepickerctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetDate (void* val, DeltaValue* at) 
{
	wxDatePickerCtrl *ctrl = DLIB_WXTYPECAST_BASE(DatePickerCtrl, val, datepickerctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, DateTime, new wxDateTime(ctrl->GetValue()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "date",				&GetDate,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DatePickerCtrl,datepickerctrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(datepickerctrl_construct, 0, 8, Nil)
	wxDatePickerCtrl *datectrl = (wxDatePickerCtrl*) 0;
	if (n == 0) {
		datectrl = new wxDatePickerCtrl();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxDateTime dt = wxDefaultDateTime;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxDP_DEFAULT | wxDP_SHOWCENTURY;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxDatePickerCtrlNameStr;
		if (n >= 3) { DLIB_WXGET_BASE(datetime, DateTime, _dt) dt = *_dt; }
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		datectrl = new wxDatePickerCtrl(parent, id, dt, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(DatePickerCtrl, datectrl)
}

WX_FUNC_ARGRANGE_START(datepickerctrl_create, 3, 8, Nil)
	DLIB_WXGET_BASE(datepickerctrl, DatePickerCtrl, datectrl)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxDateTime dt = wxDefaultDateTime;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxDP_DEFAULT | wxDP_SHOWCENTURY;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxDatePickerCtrlNameStr;
	if (n >= 4) { DLIB_WXGET_BASE(datetime, DateTime, _dt) dt = *_dt; }
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(datectrl->Create(parent, id, dt, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(datectrl);
}

WX_FUNC_START(datepickerctrl_getrange, 3, Nil)
	DLIB_WXGET_BASE(datepickerctrl, DatePickerCtrl, datectrl)
	wxDateTime dt1, dt2;
	WX_SETBOOL(datectrl->GetRange(&dt1, &dt2))
	WX_GETTABLE(dt1_table)
	{
		DeltaValue value;
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE_EX(value, DateTime, new wxDateTime(dt1))
		WX_SETTABLE_RETVAL(dt1_table, value)
	}
	WX_GETTABLE(dt2_table)
	{
		DeltaValue value;
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE_EX(value, DateTime, new wxDateTime(dt2))
		WX_SETTABLE_RETVAL(dt2_table, value)
	}
}

WX_FUNC_START(datepickerctrl_getvalue, 1, Nil)
	DLIB_WXGET_BASE(datepickerctrl, DatePickerCtrl, datectrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(DateTime, new wxDateTime(datectrl->GetValue()))
}

WX_FUNC_START(datepickerctrl_setrange, 3, Nil)
	DLIB_WXGET_BASE(datepickerctrl, DatePickerCtrl, datectrl)
	DLIB_WXGET_BASE(datetime, DateTime, dt1)
	DLIB_WXGET_BASE(datetime, DateTime, dt2)
	datectrl->SetRange(*dt1, *dt2);
}

WX_FUNC_START(datepickerctrl_setvalue, 2, Nil)
	DLIB_WXGET_BASE(datepickerctrl, DatePickerCtrl, datectrl)
	DLIB_WXGET_BASE(datetime, DateTime, dt)
	datectrl->SetValue(*dt);
}
