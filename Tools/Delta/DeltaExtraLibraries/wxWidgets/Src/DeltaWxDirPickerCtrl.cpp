#include "DeltaWxDirPickerCtrl.h"
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
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(dirpickerctrl, name)
#define WX_FUNC(name) WX_FUNC1(dirpickerctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getpath)
WX_FUNC_DEF(setpath)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getpath),
	WX_FUNC(setpath)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "setpath")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DirPickerCtrl, "dirpickerctrl", PickerBase)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DirPickerCtrl, val, dirpickerctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, PickerBase, val)
	return true;
}

static bool GetPath (void* val, DeltaValue* at) 
{
	wxDirPickerCtrl *ctrl = DLIB_WXTYPECAST_BASE(DirPickerCtrl, val, dirpickerctrl);
	WX_SETSTRING_EX(*at, ctrl->GetPath())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "PickerBase",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "path",				&GetPath,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DirPickerCtrl,dirpickerctrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(dirpickerctrl_construct, 0, 9, Nil)
	wxDirPickerCtrl *ctrl = (wxDirPickerCtrl*) 0;
	if (n == 0) {
		ctrl = new wxDirPickerCtrl();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxString path = wxEmptyString;
		wxString message = wxDirSelectorPromptStr;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxDIRP_DEFAULT_STYLE;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxDirPickerCtrlNameStr;
		if (n >= 3) { WX_GETSTRING_DEFINED(path) }
		if (n >= 4) { WX_GETSTRING_DEFINED(message) }
		if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 9) { WX_GETSTRING_DEFINED(name) }
		ctrl = new wxDirPickerCtrl(parent, id, path, message, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(DirPickerCtrl, ctrl)
}

WX_FUNC_ARGRANGE_START(dirpickerctrl_create, 3, 10, Nil)
	DLIB_WXGET_BASE(dirpickerctrl, DirPickerCtrl, ctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxString path = wxEmptyString;
	wxString message = wxDirSelectorPromptStr;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxDIRP_DEFAULT_STYLE;
	wxValidator *validator = (wxValidator*)&wxDirPickerCtrlNameStr;
	wxString name = wxDirPickerCtrlNameStr;
	if (n >= 4) { WX_GETSTRING_DEFINED(path) }
	if (n >= 5) { WX_GETSTRING_DEFINED(message) }
	if (n >= 6) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 7) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 8) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 9) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 10) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(ctrl->Create(parent, id, path, message, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(ctrl);
}

WX_FUNC_START(dirpickerctrl_getpath, 1, Nil)
	DLIB_WXGET_BASE(dirpickerctrl, DirPickerCtrl, ctrl)
	WX_SETSTRING(ctrl->GetPath())
}

WX_FUNC_START(dirpickerctrl_setpath, 2, Nil)
	DLIB_WXGET_BASE(dirpickerctrl, DirPickerCtrl, ctrl)
	WX_GETSTRING(dirname)
	ctrl->SetPath(dirname);
}
