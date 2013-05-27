#include "DeltaWxFontPickerCtrl.h"
#include "DeltaWxPickerBase.h"
#include "DeltaWxWindow.h"
#include "DeltaWxFont.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(fontpickerctrl, name)
#define WX_FUNC(name) WX_FUNC1(fontpickerctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getselectedfont)
WX_FUNC_DEF(setselectedfont)
WX_FUNC_DEF(getmaxpointsize)
WX_FUNC_DEF(setmaxpointsize)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getselectedfont),
	WX_FUNC(setselectedfont),
	WX_FUNC(getmaxpointsize),
	WX_FUNC(setmaxpointsize)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setmaxpointsize")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FontPickerCtrl, "fontpickerctrl", PickerBase)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FontPickerCtrl, val, fontpickerctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxPickerBase *_parent = DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase);
	DeltaWxPickerBase *parent = DNEWCLASS(DeltaWxPickerBase, (_parent));
	WX_SETOBJECT_EX(*at, PickerBase, parent)
	return true;
}

static bool GetSelectedFont (void* val, DeltaValue* at) 
{
	wxFontPickerCtrl *ctrl = DLIB_WXTYPECAST_BASE(FontPickerCtrl, val, fontpickerctrl);
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(ctrl->GetSelectedFont())));
	WX_SETOBJECT_EX(*at, Font, retval)
	return true;
}

static bool GetMaxPointSize (void* val, DeltaValue* at) 
{
	wxFontPickerCtrl *ctrl = DLIB_WXTYPECAST_BASE(FontPickerCtrl, val, fontpickerctrl);
	WX_SETNUMBER_EX(*at, ctrl->GetMaxPointSize())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "PickerBase",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "selectedFont",		&GetSelectedFont,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maxPointSize",		&GetMaxPointSize,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FontPickerCtrl,fontpickerctrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(fontpickerctrl_construct, 0, 8, Nil)
	wxFontPickerCtrl *wxctrl = (wxFontPickerCtrl*) 0;
	DeltaWxFontPickerCtrl *ctrl = (DeltaWxFontPickerCtrl*) 0;
	if (n == 0) {
		wxctrl = new wxFontPickerCtrl();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxFont initial = wxNullFont;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxFNTP_DEFAULT_STYLE;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxFontPickerCtrlNameStr;
		if (n >= 3) { DLIB_WXGET_BASE(font, Font, font) initial = *font; }
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		wxctrl = new wxFontPickerCtrl(parent, id, initial, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxctrl) ctrl = DNEWCLASS(DeltaWxFontPickerCtrl, (wxctrl));
	WX_SETOBJECT(FontPickerCtrl, ctrl)
}

DLIB_FUNC_START(fontpickerctrl_destruct, 1, Nil)
	DLIB_WXDELETE(fontpickerctrl, FontPickerCtrl, ctrl)
}

WX_FUNC_ARGRANGE_START(fontpickerctrl_create, 3, 9, Nil)
	DLIB_WXGET_BASE(fontpickerctrl, FontPickerCtrl, ctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxFont initial = wxNullFont;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxFNTP_DEFAULT_STYLE;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxFontPickerCtrlNameStr;
	if (n >= 4) { DLIB_WXGET_BASE(font, Font, font) initial = *font; }
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(ctrl->Create(parent, id, initial, pos, size, style, *validator, name))
}

DLIB_FUNC_START(fontpickerctrl_getselectedfont, 1, Nil)
	DLIB_WXGET_BASE(fontpickerctrl, FontPickerCtrl, ctrl)
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(ctrl->GetSelectedFont())));
	WX_SETOBJECT(Font, retval)
}

DLIB_FUNC_START(fontpickerctrl_setselectedfont, 2, Nil)
	DLIB_WXGET_BASE(fontpickerctrl, FontPickerCtrl, ctrl)
	DLIB_WXGET_BASE(font, Font, font)
	ctrl->SetSelectedFont(*font);
}

DLIB_FUNC_START(fontpickerctrl_getmaxpointsize, 1, Nil)
	DLIB_WXGET_BASE(fontpickerctrl, FontPickerCtrl, ctrl)
	WX_SETNUMBER(ctrl->GetMaxPointSize())
}

DLIB_FUNC_START(fontpickerctrl_setmaxpointsize, 2, Nil)
	DLIB_WXGET_BASE(fontpickerctrl, FontPickerCtrl, ctrl)
	WX_GETNUMBER(max)
	ctrl->SetMaxPointSize(max);
}
