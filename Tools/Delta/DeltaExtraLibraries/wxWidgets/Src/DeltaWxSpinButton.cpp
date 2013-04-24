#include "DeltaWxSpinButton.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(spinbutton, name)
#define WX_FUNC(name) WX_FUNC1(spinbutton, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getmax)
WX_FUNC_DEF(getmin)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(setrange)
WX_FUNC_DEF(setvalue)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getmax),
	WX_FUNC(getmin),
	WX_FUNC(getvalue),
	WX_FUNC(setrange),
	WX_FUNC(setvalue)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setvalue")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SpinButton, "spinbutton", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SpinButton, val, spinbutton) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxControl *_parent = DLIB_WXTYPECAST_BASE(Control, val, control);
	DeltaWxControl *parent = DNEWCLASS(DeltaWxControl, (_parent));
	WX_SETOBJECT_EX(*at, Control, parent)
	return true;
}

static bool GetMin (void* val, DeltaValue* at) 
{
	wxSpinButton *button = DLIB_WXTYPECAST_BASE(SpinButton, val, spinbutton);
	WX_SETNUMBER_EX(*at, button->GetMin())
	return true;
}

static bool GetMax (void* val, DeltaValue* at) 
{
	wxSpinButton *button = DLIB_WXTYPECAST_BASE(SpinButton, val, spinbutton);
	WX_SETNUMBER_EX(*at, button->GetMax())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "min",				&GetMin,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "max",				&GetMax,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SpinButton,spinbutton)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(spinbutton_construct, 0, 6, Nil)
	wxSpinButton *wxspinbtn = (wxSpinButton*) 0;
	DeltaWxSpinButton *spinbtn = (DeltaWxSpinButton*) 0;
	if (n == 0)
		wxspinbtn = new wxSpinButton();
	else {
		DLIB_WXGET_BASE(window, Window, parent)
		int id = wxID_ANY;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxSP_VERTICAL | wxSP_ARROW_KEYS;
		wxString name = wxSPIN_BUTTON_NAME;
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { WX_GETSTRING_DEFINED(name) }
		wxspinbtn = new wxSpinButton(parent, id, pos, size, style, name);
	}
	if (wxspinbtn) spinbtn = DNEWCLASS(DeltaWxSpinButton, (wxspinbtn));
	WX_SETOBJECT(SpinButton, spinbtn)
}

DLIB_FUNC_START(spinbutton_destruct, 1, Nil)
	DLIB_WXDELETE(spinbutton, SpinButton, spinbtn)
}

WX_FUNC_ARGRANGE_START(spinbutton_create, 2, 7, Nil)
	DLIB_WXGET_BASE(spinbutton, SpinButton, spinbtn)
	DLIB_WXGET_BASE(window, Window, parent)
	int id = wxID_ANY;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxSP_VERTICAL | wxSP_ARROW_KEYS;
	wxString name = wxSPIN_BUTTON_NAME;
	if (n >= 3) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(spinbtn->Create(parent, id, pos, size, style, name))
}

DLIB_FUNC_START(spinbutton_getmax, 1, Nil)
	DLIB_WXGET_BASE(spinbutton, SpinButton, spinbtn)
	WX_SETNUMBER(spinbtn->GetMax())
}

DLIB_FUNC_START(spinbutton_getmin, 1, Nil)
	DLIB_WXGET_BASE(spinbutton, SpinButton, spinbtn)
	WX_SETNUMBER(spinbtn->GetMin())
}

DLIB_FUNC_START(spinbutton_getvalue, 1, Nil)
	DLIB_WXGET_BASE(spinbutton, SpinButton, spinbtn)
	WX_SETNUMBER(spinbtn->GetValue())
}

DLIB_FUNC_START(spinbutton_setrange, 3, Nil)
	DLIB_WXGET_BASE(spinbutton, SpinButton, spinbtn)
	WX_GETNUMBER(min)
	WX_GETNUMBER(max)
	spinbtn->SetRange(min, max);
}

DLIB_FUNC_START(spinbutton_setvalue, 2, Nil)
	DLIB_WXGET_BASE(spinbutton, SpinButton, spinbtn)
	WX_GETNUMBER(value)
	spinbtn->SetValue(value);
}
