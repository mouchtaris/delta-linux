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
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(checkbox, name)
#define WX_FUNC(name) WX_FUNC1(checkbox, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
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
	WX_FUNC(destruct),
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

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "set3statevalue")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CheckBox, "checkbox", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CheckBox, val, checkbox) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxControl *_parent = DLIB_WXTYPECAST_BASE(Control, val, control);
	DeltaWxControl *parent = DNEWCLASS(DeltaWxControl, (_parent));
	WX_SETOBJECT_EX(*at, Control, parent)
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(CheckBox, checkbox);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(checkbox_construct, 0, 8, Nil)
	wxCheckBox *wxchbox = (wxCheckBox*) 0;
	DeltaWxCheckBox *chbox = (DeltaWxCheckBox*) 0;
	if (n == 0) {
		wxchbox = new wxCheckBox();
	} else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		WX_GETSTRING(label)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxCheckBoxNameStr;
		if (n >= 4) { DLIB_WXGET_BASE(point, Point, _pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGET_BASE(size, Size, _size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		wxchbox = new wxCheckBox(parent, id, label, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxchbox) chbox = DNEWCLASS(DeltaWxCheckBox, (wxchbox));
	WX_SETOBJECT(CheckBox, chbox)
}

DLIB_FUNC_START(checkbox_destruct, 1, Nil)
	DLIB_WXDELETE(checkbox, CheckBox, chbox)
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
	if (n >= 5) { DLIB_WXGET_BASE(point, Point, _pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGET_BASE(size, Size, _size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(chbox->Create(parent, id, label, pos, size, style, *validator, name))
}

DLIB_FUNC_START(checkbox_getvalue, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETBOOL(chbox->GetValue())
}

DLIB_FUNC_START(checkbox_get3statevalue, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETNUMBER(chbox->Get3StateValue())
}

DLIB_FUNC_START(checkbox_is3rdstateallowedforuser, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETBOOL(chbox->Is3rdStateAllowedForUser())
}

DLIB_FUNC_START(checkbox_is3state, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETBOOL(chbox->Is3State())
}

DLIB_FUNC_START(checkbox_ischecked, 1, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_SETBOOL(chbox->IsChecked())
}

DLIB_FUNC_START(checkbox_setvalue, 2, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_GETBOOL(state)
	chbox->SetValue(state);
}

DLIB_FUNC_START(checkbox_set3statevalue, 2, Nil)
	DLIB_WXGET_BASE(checkbox, CheckBox, chbox)
	WX_GETDEFINE(state)
	chbox->Set3StateValue((wxCheckBoxState) state);
}
