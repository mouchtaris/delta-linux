#include "DeltaWxPickerBase.h"
#include "DeltaWxControl.h"
#include "DeltaWxTextCtrl.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(pickerbase, name)
#define WX_FUNC(name) WX_FUNC1(pickerbase, name)

WX_FUNC_DEF(setinternalmargin)
WX_FUNC_DEF(getinternalmargin)
WX_FUNC_DEF(settextctrlproportion)
WX_FUNC_DEF(setpickerctrlproportion)
WX_FUNC_DEF(gettextctrlproportion)
WX_FUNC_DEF(getpickerctrlproportion)
WX_FUNC_DEF(hastextctrl)
WX_FUNC_DEF(gettextctrl)
WX_FUNC_DEF(istextctrlgrowable)
WX_FUNC_DEF(setpickerctrlgrowable)
WX_FUNC_DEF(settextctrlgrowable)
WX_FUNC_DEF(ispickerctrlgrowable)

WX_FUNCS_START
	WX_FUNC(setinternalmargin),
	WX_FUNC(getinternalmargin),
	WX_FUNC(settextctrlproportion),
	WX_FUNC(setpickerctrlproportion),
	WX_FUNC(gettextctrlproportion),
	WX_FUNC(getpickerctrlproportion),
	WX_FUNC(hastextctrl),
	WX_FUNC(gettextctrl),
	WX_FUNC(istextctrlgrowable),
	WX_FUNC(setpickerctrlgrowable),
	WX_FUNC(settextctrlgrowable),
	WX_FUNC(ispickerctrlgrowable)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(0, uarraysize(funcs), "setinternalmargin", "ispickerctrlgrowable")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PickerBase, "pickerbase", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetInternalMargin (void* val, DeltaValue* at) 
{
	wxPickerBase *picker = DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase);
	WX_SETNUMBER_EX(*at, picker->GetInternalMargin())
	return true;
}

static bool GetTextCtrlProportion (void* val, DeltaValue* at) 
{
	wxPickerBase *picker = DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase);
	WX_SETNUMBER_EX(*at, picker->GetTextCtrlProportion())
	return true;
}

static bool GetPickerCtrlProportion (void* val, DeltaValue* at) 
{
	wxPickerBase *picker = DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase);
	WX_SETNUMBER_EX(*at, picker->GetPickerCtrlProportion())
	return true;
}

static bool GetTextCtrl (void* val, DeltaValue* at) 
{
	wxPickerBase *picker = DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, TextCtrl, picker->GetTextCtrl())
	return true;
}

static bool GetIsTextCtrlGrowable (void* val, DeltaValue* at) 
{
	wxPickerBase *picker = DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase);
	WX_SETBOOL_EX(*at, picker->IsTextCtrlGrowable())
	return true;
}

static bool GetIsPickerCtrlGrowable (void* val, DeltaValue* at) 
{
	wxPickerBase *picker = DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase);
	WX_SETBOOL_EX(*at, picker->IsPickerCtrlGrowable())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",					&GetKeys,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",				&GetBaseClass,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "internalMargin",			&GetInternalMargin,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textCtrlProportion",		&GetTextCtrlProportion,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pickerCtrlProportion",	&GetPickerCtrlProportion,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textCtrl",				&GetTextCtrl,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isTextCtrlGrowable",		&GetIsTextCtrlGrowable,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isPickerCtrlGrowable",	&GetIsPickerCtrlGrowable,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PickerBase,pickerbase)

////////////////////////////////////////////////////////////////

WX_FUNC_START(pickerbase_setinternalmargin, 2, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_GETNUMBER(margin)
	picker->SetInternalMargin(margin);
}

WX_FUNC_START(pickerbase_getinternalmargin, 1, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_SETNUMBER(picker->GetInternalMargin())
}

WX_FUNC_START(pickerbase_settextctrlproportion, 2, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_GETNUMBER(proportion)
	picker->SetTextCtrlProportion(proportion);
}

WX_FUNC_START(pickerbase_setpickerctrlproportion, 2, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_GETNUMBER(proportion)
	picker->SetPickerCtrlProportion(proportion);
}

WX_FUNC_START(pickerbase_gettextctrlproportion, 1, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_SETNUMBER(picker->GetTextCtrlProportion())
}

WX_FUNC_START(pickerbase_getpickerctrlproportion, 1, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_SETNUMBER(picker->GetPickerCtrlProportion())
}

WX_FUNC_START(pickerbase_hastextctrl, 1, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_SETBOOL(picker->HasTextCtrl())
}

WX_FUNC_START(pickerbase_gettextctrl, 1, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_SETOBJECT(TextCtrl, picker->GetTextCtrl())
}

WX_FUNC_START(pickerbase_istextctrlgrowable, 1, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_SETBOOL(picker->IsTextCtrlGrowable())
}

WX_FUNC_ARGRANGE_START(pickerbase_setpickerctrlgrowable, 1, 2, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	bool grow = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(grow) }
	picker->SetPickerCtrlGrowable(grow);
}

WX_FUNC_ARGRANGE_START(pickerbase_settextctrlgrowable, 1, 2, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	bool grow = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(grow) }
	picker->SetTextCtrlGrowable(grow);
}

WX_FUNC_START(pickerbase_ispickerctrlgrowable, 1, Nil)
	DLIB_WXGET_BASE(pickerbase, PickerBase, picker)
	WX_SETBOOL(picker->IsPickerCtrlGrowable())
}
