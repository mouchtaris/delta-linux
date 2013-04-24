#include "DeltaWxFilePickerCtrl.h"
#include "DeltaWxPickerBase.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(filepickerctrl, name)
#define WX_FUNC(name) WX_FUNC1(filepickerctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getpath)
WX_FUNC_DEF(setpath)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getpath),
	WX_FUNC(setpath)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setpath")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FilePickerCtrl, "filepickerctrl", PickerBase)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FilePickerCtrl, val, filepickerctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxPickerBase *_parent = DLIB_WXTYPECAST_BASE(PickerBase, val, pickerbase);
	DeltaWxPickerBase *parent = DNEWCLASS(DeltaWxPickerBase, (_parent));
	WX_SETOBJECT_EX(*at, PickerBase, parent)
	return true;
}

static bool GetPath (void* val, DeltaValue* at) 
{
	wxFilePickerCtrl *ctrl = DLIB_WXTYPECAST_BASE(FilePickerCtrl, val, filepickerctrl);
	WX_SETSTRING_EX(*at, ctrl->GetPath())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "PickerBase",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "path",				&GetPath,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FilePickerCtrl,filepickerctrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(filepickerctrl_construct, 0, 10, Nil)
	wxFilePickerCtrl *wxctrl = (wxFilePickerCtrl*) 0;
	DeltaWxFilePickerCtrl *ctrl = (DeltaWxFilePickerCtrl*) 0;
	if (n == 0) {
		wxctrl = new wxFilePickerCtrl();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxString path = wxEmptyString;
		wxString message = wxFileSelectorPromptStr;
		wxString wildcard = wxFileSelectorDefaultWildcardStr;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxFLP_DEFAULT_STYLE;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxFilePickerCtrlNameStr;
		if (n >= 3) { WX_GETSTRING_DEFINED(path) }
		if (n >= 4) { WX_GETSTRING_DEFINED(message) }
		if (n >= 5) { WX_GETSTRING_DEFINED(wildcard) }
		if (n >= 6) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 7) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 8) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 9) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 10) { WX_GETSTRING_DEFINED(name) }
		wxctrl = new wxFilePickerCtrl(parent, id, path, message, wildcard, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxctrl) ctrl = DNEWCLASS(DeltaWxFilePickerCtrl, (wxctrl));
	WX_SETOBJECT(FilePickerCtrl, ctrl)
}

DLIB_FUNC_START(filepickerctrl_destruct, 1, Nil)
	DLIB_WXDELETE(filepickerctrl, FilePickerCtrl, ctrl)
}

WX_FUNC_ARGRANGE_START(filepickerctrl_create, 3, 11, Nil)
	DLIB_WXGET_BASE(filepickerctrl, FilePickerCtrl, ctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxString path = wxEmptyString;
	wxString message = wxFileSelectorPromptStr;
	wxString wildcard = wxFileSelectorDefaultWildcardStr;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxFLP_DEFAULT_STYLE;
	wxValidator *validator = (wxValidator*)&wxDirPickerCtrlNameStr;
	wxString name = wxFilePickerCtrlNameStr;
	if (n >= 4) { WX_GETSTRING_DEFINED(path) }
	if (n >= 5) { WX_GETSTRING_DEFINED(message) }
	if (n >= 6) { WX_GETSTRING_DEFINED(wildcard) }
	if (n >= 7) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 8) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 9) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 10) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 11) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(ctrl->Create(parent, id, path, message, wildcard, pos, size, style, *validator, name))
}

DLIB_FUNC_START(filepickerctrl_getpath, 1, Nil)
	DLIB_WXGET_BASE(filepickerctrl, FilePickerCtrl, ctrl)
	WX_SETSTRING(ctrl->GetPath())
}

DLIB_FUNC_START(filepickerctrl_setpath, 2, Nil)
	DLIB_WXGET_BASE(filepickerctrl, FilePickerCtrl, ctrl)
	WX_GETSTRING(dirname)
	ctrl->SetPath(dirname);
}
