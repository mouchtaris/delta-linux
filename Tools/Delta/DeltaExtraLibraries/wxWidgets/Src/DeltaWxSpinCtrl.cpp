#include "DeltaWxSpinCtrl.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(spinctrl, name)
#define WX_FUNC(name) WX_FUNC1(spinctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(setvalue)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(setrange)
WX_FUNC_DEF(setselection)
WX_FUNC_DEF(getmin)
WX_FUNC_DEF(getmax)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(setvalue),
	WX_FUNC(getvalue),
	WX_FUNC(setrange),
	WX_FUNC(setselection),
	WX_FUNC(getmin),
	WX_FUNC(getmax)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "getmax")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SpinCtrl, "spinctrl", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SpinCtrl, val, spinctrl) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(SpinCtrl,spinctrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(spinctrl_construct, 0, 10, Nil)
	wxSpinCtrl *spinctrl = (wxSpinCtrl*) 0;
	if (n == 0)
		spinctrl = new wxSpinCtrl();
	else {
		DLIB_WXGET_BASE(window, Window, parent)
		int id = wxID_ANY;
		wxString value = wxEmptyString;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxSP_ARROW_KEYS;
		int min = 0, max = 100, initial = 0;
		wxString name = _T("wxSpinCtrl");
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { WX_GETSTRING_DEFINED(value) }
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { WX_GETNUMBER_DEFINED(min) }
		if (n >= 8) { WX_GETNUMBER_DEFINED(max) }
		if (n >= 9) { WX_GETNUMBER_DEFINED(initial) }
		if (n >= 10) { WX_GETSTRING_DEFINED(name) }
		spinctrl = new wxSpinCtrl(parent, id, value, pos, size, style, min, max, initial, name);
	}
	WX_SET_WINDOW_OBJECT(SpinCtrl, spinctrl)
}

WX_FUNC_ARGRANGE_START(spinctrl_create, 2, 11, Nil)
	DLIB_WXGET_BASE(spinctrl, SpinCtrl, spinctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	int id = wxID_ANY;
	wxString value = wxEmptyString;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxSP_ARROW_KEYS;
	int min = 0, max = 100, initial = 0;
	wxString name = _T("wxSpinCtrl");
	if (n >= 3) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 4) { WX_GETSTRING_DEFINED(value) }
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { WX_GETNUMBER_DEFINED(min) }
	if (n >= 9) { WX_GETNUMBER_DEFINED(max) }
	if (n >= 10) { WX_GETNUMBER_DEFINED(initial) }
	if (n >= 11) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(spinctrl->Create(parent, id, value, pos, size, style, min, max, initial, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(spinctrl);
}

WX_FUNC_START(spinctrl_setvalue, 2, Nil)
	DLIB_WXGET_BASE(spinctrl, SpinCtrl, spinctrl)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(text)
		spinctrl->SetValue(text);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETNUMBER(value)
		spinctrl->SetValue(value);
	}
}

WX_FUNC_START(spinctrl_getvalue, 1, Nil)
	DLIB_WXGET_BASE(spinctrl, SpinCtrl, spinctrl)
	WX_SETNUMBER(spinctrl->GetValue())
}

WX_FUNC_START(spinctrl_setrange, 3, Nil)
	DLIB_WXGET_BASE(spinctrl, SpinCtrl, spinctrl)
	WX_GETNUMBER(minVal)
	WX_GETNUMBER(maxVal)
	spinctrl->SetRange(minVal, maxVal);
}

WX_FUNC_START(spinctrl_setselection, 3, Nil)
	DLIB_WXGET_BASE(spinctrl, SpinCtrl, spinctrl)
	WX_GETNUMBER(from)
	WX_GETNUMBER(to)
	spinctrl->SetSelection(from, to);
}

WX_FUNC_START(spinctrl_getmin, 1, Nil)
	DLIB_WXGET_BASE(spinctrl, SpinCtrl, spinctrl)
	WX_SETNUMBER(spinctrl->GetMin())
}

WX_FUNC_START(spinctrl_getmax, 1, Nil)
	DLIB_WXGET_BASE(spinctrl, SpinCtrl, spinctrl)
	WX_SETNUMBER(spinctrl->GetMax())
}
