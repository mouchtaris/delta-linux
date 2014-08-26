#include "DeltaWxCheckBox.h"
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
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(checkbox, name)
#define WX_FUNC(name) WX_FUNC1(checkbox, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(get3statevalue)
WX_FUNC_DEF(is3rdstateallowedforuser)
WX_FUNC_DEF(is3state)
WX_FUNC_DEF(ischecked)
WX_FUNC_DEF(setvalue)
WX_FUNC_DEF(set3statevalue)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getvalue),
	WX_FUNC(get3statevalue),
	WX_FUNC(is3rdstateallowedforuser),
	WX_FUNC(is3state),
	WX_FUNC(ischecked),
	WX_FUNC(setvalue),
	WX_FUNC(set3statevalue)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "set3statevalue")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CheckBox, "checkbox", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CheckBox, val, checkbox) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetState (void* val, DeltaValue* at) 
{
	wxCheckBox *chbox = DLIB_WXTYPECAST_BASE(CheckBox, val, checkbox);
	at->FromNumber(chbox->Get3StateValue());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "state",				&GetState,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(CheckBox,checkbox)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(checkbox_construct, 0, 8, Nil)
	wxCheckBox *chbox = (wxCheckBox*) 0;
	if (n == 0) {
		chbox = new wxCheckBox();
	} else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		WX_GETSTRING(label)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxCheckBoxNameStr;
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		chbox = new wxCheckBox(parent, id, label, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(CheckBox, chbox)
}

WX_FUNC_ARGRANGE_START(checkbox_create, 4, 9, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	WX_GETSTRING(label)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = 0;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxCheckBoxNameStr;
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(chbox->Create(parent, id, label, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(chbox);
}

WX_FUNC_START(checkbox_getvalue, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETBOOL(chbox->GetValue())
}

WX_FUNC_START(checkbox_get3statevalue, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETNUMBER(chbox->Get3StateValue())
}

WX_FUNC_START(checkbox_is3rdstateallowedforuser, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETBOOL(chbox->Is3rdStateAllowedForUser())
}

WX_FUNC_START(checkbox_is3state, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETBOOL(chbox->Is3State())
}

WX_FUNC_START(checkbox_ischecked, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETBOOL(chbox->IsChecked())
}

WX_FUNC_START(checkbox_setvalue, 2, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_GETBOOL(state)
	chbox->SetValue(state);
}

WX_FUNC_START(checkbox_set3statevalue, 2, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_GETDEFINE(state)
	chbox->Set3StateValue((wxCheckBoxState) state);
}
