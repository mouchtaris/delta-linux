#include "DeltaWxToggleButton.h"
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

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(togglebutton, name)
#define WX_FUNC(name) WX_FUNC1(togglebutton, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(setvalue)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getvalue),
	WX_FUNC(setvalue)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "setvalue")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ToggleButton, "togglebutton", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST(ToggleButton, val, togglebutton) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ToggleButton,togglebutton)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(togglebutton_construct, 0, 8, Nil)
	wxToggleButton *tglbtn = (wxToggleButton*) 0;
	if (n == 0)
		tglbtn = new wxToggleButton();
	else if (n >= 3) {
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
		tglbtn = new wxToggleButton(parent, id, label, pos, size, style, *validator, name);
	}
	WX_SET_WINDOW_OBJECT(ToggleButton, tglbtn)
}

WX_FUNC_ARGRANGE_START(togglebutton_create, 4, 9, Nil)
	DLIB_WXGET_BASE(togglebutton, ToggleButton, tglbtn)
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
	WX_SETBOOL(tglbtn->Create(parent, id, label, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(tglbtn);
}

WX_FUNC_START(togglebutton_getvalue, 1, Nil)
	DLIB_WXGET_BASE(togglebutton, ToggleButton, tglbtn)
	WX_SETBOOL(tglbtn->GetValue())
}

WX_FUNC_START(togglebutton_setvalue, 2, Nil)
	DLIB_WXGET_BASE(togglebutton, ToggleButton, tglbtn)
	WX_GETBOOL(state)
	tglbtn->SetValue(state);
}
