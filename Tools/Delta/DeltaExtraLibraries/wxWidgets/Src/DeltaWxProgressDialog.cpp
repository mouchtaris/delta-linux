#include "DeltaWxProgressDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(progressdialog, name)
#define WX_FUNC(name) WX_FUNC1(progressdialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(resume)
WX_FUNC_DEF(update)
WX_FUNC_DEF(pulse)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(resume),
	WX_FUNC(update),
	WX_FUNC(pulse)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "resume", "pulse")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ProgressDialog, "progressdialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ProgressDialog, val, progressdialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Dialog, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ProgressDialog,progressdialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(progressdialog_construct, 2, 5, Nil)
	WX_GETSTRING(title)
	WX_GETSTRING(message)
	int maximum = 100;
	wxWindow *parent = NULL;
	int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE;
	if (n >= 3) { WX_GETNUMBER_DEFINED(maximum) }
	if (n >= 4) { DLIB_WXGET_BASE(window, Window, window) parent = window; }
	if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
	wxProgressDialog* dialog = new wxProgressDialog(title, message, maximum, parent, style);
	WX_SET_TOPLEVELWINDOW_OBJECT(ProgressDialog, dialog)
}

WX_FUNC_START(progressdialog_resume, 1, Nil)
	DLIB_WXGET_BASE(progressdialog, ProgressDialog, dialog)
	dialog->Resume();
}

WX_FUNC_ARGRANGE_START(progressdialog_update, 2, 4, Nil)
	DLIB_WXGET_BASE(progressdialog, ProgressDialog, dialog)
	WX_GETNUMBER(value)
	wxString newmsg = wxEmptyString;
	bool skip;
	if (n >= 3) { WX_GETSTRING_DEFINED(newmsg) }
	WX_SETBOOL(dialog->Update(value, newmsg, &skip))
	if (n >= 4) {
		WX_GETTABLE(skip_table)
		WX_SETTABLE_RETVAL(skip_table, DeltaValue(skip))
	}
}

WX_FUNC_ARGRANGE_START(progressdialog_pulse, 1, 3, Nil)
	DLIB_WXGET_BASE(progressdialog, ProgressDialog, dialog)
	wxString newmsg = wxEmptyString;
	bool skip;
	if (n >= 2) { WX_GETSTRING_DEFINED(newmsg) }
	WX_SETBOOL(dialog->Pulse(newmsg, &skip))
	if (n >= 3) {
		WX_GETTABLE(skip_table)
		WX_SETTABLE_RETVAL(skip_table, DeltaValue(skip))
	}
}
