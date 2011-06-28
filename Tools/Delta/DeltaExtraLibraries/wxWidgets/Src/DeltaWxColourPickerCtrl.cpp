#include "DeltaWxColourPickerCtrl.h"
#include "DeltaWxPickerBase.h"
#include "DeltaWxWindow.h"
#include "DeltaWxColour.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(colourpickerctrl, name)
#define WX_FUNC(name) WX_FUNC1(colourpickerctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getcolour)
WX_FUNC_DEF(setcolour)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getcolour),
	WX_FUNC(setcolour)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setcolour")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ColourPickerCtrl, "colourpickerctrl", PickerBase)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ColourPickerCtrl, val, colourpickerctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxPickerBase *_parent = DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase);
	DeltaWxPickerBase *parent = DNEWCLASS(DeltaWxPickerBase, (_parent));
	WX_SETOBJECT_EX(*at, PickerBase, parent)
	return true;
}

static bool GetColour (void* val, DeltaValue* at) 
{
	wxColourPickerCtrl *ctrl = DLIB_WXTYPECAST_BASE(ColourPickerCtrl, val, colourpickerctrl);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(ctrl->GetColour())));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "PickerBase",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "colour",				&GetColour,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ColourPickerCtrl, colourpickerctrl);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(colourpickerctrl_construct, 0, 8, Nil)
	wxColourPickerCtrl *wxctrl = (wxColourPickerCtrl*) 0;
	DeltaWxColourPickerCtrl *ctrl = (DeltaWxColourPickerCtrl*) 0;
	if (n == 0) {
		wxctrl = new wxColourPickerCtrl();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxColour col = *wxBLACK;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxCLRP_DEFAULT_STYLE;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxColourPickerCtrlNameStr;
		if (n >= 3) { DLIB_WXGET_BASE(colour, Colour, colour) col = *colour; }
		if (n >= 4) { DLIB_WXGET_BASE(point, Point, _pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGET_BASE(size, Size, _size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		wxctrl = new wxColourPickerCtrl(parent, id, col, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxctrl) ctrl = DNEWCLASS(DeltaWxColourPickerCtrl, (wxctrl));
	WX_SETOBJECT(ColourPickerCtrl, ctrl)
}

DLIB_FUNC_START(colourpickerctrl_destruct, 1, Nil)
	DLIB_WXDELETE(colourpickerctrl, ColourPickerCtrl, ctrl)
}

WX_FUNC_ARGRANGE_START(colourpickerctrl_create, 3, 9, Nil)
	DLIB_WXGET_BASE(colourpickerctrl, ColourPickerCtrl, ctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxColour col = *wxBLACK;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxCLRP_DEFAULT_STYLE;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxColourPickerCtrlNameStr;
	if (n >= 4) { DLIB_WXGET_BASE(colour, Colour, colour) col = *colour; }
	if (n >= 5) { DLIB_WXGET_BASE(point, Point, _pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGET_BASE(size, Size, _size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(ctrl->Create(parent, id, col, pos, size, style, *validator, name))
}

DLIB_FUNC_START(colourpickerctrl_getcolour, 1, Nil)
	DLIB_WXGET_BASE(colourpickerctrl, ColourPickerCtrl, ctrl)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(ctrl->GetColour())));
}

DLIB_FUNC_START(colourpickerctrl_setcolour, 2, Nil)
	DLIB_WXGET_BASE(colourpickerctrl, ColourPickerCtrl, ctrl)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(colour, Colour, col)
		ctrl->SetColour(*col);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(colourName)
		ctrl->SetColour(colourName);
	}
}
