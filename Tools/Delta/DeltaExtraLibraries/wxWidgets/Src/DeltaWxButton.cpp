#include "DeltaWxButton.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(button, name)
#define WX_FUNC(name) WX_FUNC1(button, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(getdefaultsize)
WX_FUNC_DEF(setdefault)
WX_FUNC_DEF(setlabel)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getlabel),
	WX_FUNC(getdefaultsize),
	WX_FUNC(setdefault),
	WX_FUNC(setlabel)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "setlabel")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Button, "button", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Button, val, button) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Button,button)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(button_construct, 0, 8, Nil)
	wxButton *button = (wxButton*) 0;
	if (n == 0) {
		button = new wxButton();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxString label = wxEmptyString;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxButtonNameStr;
		if (n >= 3) { WX_GETSTRING_DEFINED(label) }
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		button = new wxButton(parent, id, label, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(Button, button)
}

WX_FUNC_ARGRANGE_START(button_create, 3, 9, Nil)
	DLIB_WXGET_BASE(button, Button, button)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxString label = wxEmptyString;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = 0;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxButtonNameStr;
	if (n >= 4) { WX_GETSTRING_DEFINED(label) }
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(button->Create(parent, id, label, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(button);
}

WX_FUNC_START(button_getlabel, 1, Nil)
	DLIB_WXGET_BASE(button, Button, button)
	WX_SETSTRING(button->GetLabel())
}

WX_FUNC_START(button_getdefaultsize, 1, Nil)
	DLIB_WXGET_BASE(button, Button, button)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(button->GetDefaultSize()))
}

WX_FUNC_START(button_setdefault, 1, Nil)
	DLIB_WXGET_BASE(button, Button, button)
	button->SetDefault();
}

WX_FUNC_START(button_setlabel, 2, Nil)
	DLIB_WXGET_BASE(button, Button, button)
	WX_GETSTRING(label)
	button->SetLabel(label);
}
