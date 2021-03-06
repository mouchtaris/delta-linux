#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxCommandEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(control, name)
#define WX_FUNC(name) WX_FUNC1(control, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(command)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(getlabeltext)
WX_FUNC_DEF(setlabel)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(command),
	WX_FUNC(getlabel),
	WX_FUNC(getlabeltext),
	WX_FUNC(setlabel)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "command", "setlabel")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Control, "control", Window)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Control, val, control) ? DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Window, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Window",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Control,control)

////////////////////////////////////////////////////////////////

WX_FUNC_START(control_construct, 0, Nil)
	wxControl* control		= new wxControl();
	WX_SET_WINDOW_OBJECT(Control, control)
}

WX_FUNC_START(control_command, 2, Nil)
	DLIB_WXGET_BASE(control, Control, control)
	DLIB_WXGET_BASE(commandevent, CommandEvent, command)
	control->Command(*command);
}

WX_FUNC_START(control_getlabel, 1, Nil)
	DLIB_WXGET_BASE(control, Control, control)
	WX_SETSTRING(control->GetLabel())
}

WX_FUNC_START(control_getlabeltext, 1, Nil)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(label)
		WX_SETSTRING(wxControl::GetLabelText(label))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(control, Control, control)
		WX_SETSTRING(control->GetLabelText())
	}
}

WX_FUNC_START(control_setlabel, 2, Nil)
	DLIB_WXGET_BASE(control, Control, control)
	WX_GETSTRING(label)
	control->SetLabel(label);
}
