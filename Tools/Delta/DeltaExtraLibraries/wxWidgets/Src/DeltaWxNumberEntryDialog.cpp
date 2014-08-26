#include "DeltaWxNumberEntryDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(numberentrydialog, name)
#define WX_FUNC(name) WX_FUNC1(numberentrydialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(onok)
WX_FUNC_DEF(oncancel)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getvalue),
	WX_FUNC(onok),
	WX_FUNC(oncancel)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getvalue", "oncancel")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(NumberEntryDialog, "numberentrydialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(NumberEntryDialog, val, numberentrydialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Dialog, val)
	return true;
}

static bool GetValue (void* val, DeltaValue* at) 
{
	wxNumberEntryDialog *dlg = DLIB_WXTYPECAST_BASE(NumberEntryDialog, val, numberentrydialog);
	WX_SETNUMBER_EX(*at, dlg->GetValue())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "value",				&GetValue,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(NumberEntryDialog,numberentrydialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(numberentrydialog_construct, 7, 8, Nil)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETSTRING(message)
	WX_GETSTRING(prompt)
	WX_GETSTRING(caption)
	WX_GETNUMBER(value)
	WX_GETNUMBER(min)
	WX_GETNUMBER(max)
	wxPoint pos = wxDefaultPosition;
	if (n >= 8) { DLIB_WXGETPOINT_BASE(point) pos = *point; }
	wxNumberEntryDialog* dialog = new wxNumberEntryDialog(parent, message, prompt, caption, value, min, max, pos);
	WX_SET_TOPLEVELWINDOW_OBJECT(NumberEntryDialog, dialog)
}

WX_FUNC_START(numberentrydialog_getvalue, 1, Nil)
	DLIB_WXGET_BASE(numberentrydialog, NumberEntryDialog, dialog)
	WX_SETNUMBER(dialog->GetValue())
}

WX_FUNC_START(numberentrydialog_onok, 2, Nil)
	DLIB_WXGET_BASE(numberentrydialog, NumberEntryDialog, dialog)
	DLIB_WXGET_BASE(commandevent, CommandEvent, ev)
	dialog->OnOK(*ev);
}

WX_FUNC_START(numberentrydialog_oncancel, 2, Nil)
	DLIB_WXGET_BASE(numberentrydialog, NumberEntryDialog, dialog)
	DLIB_WXGET_BASE(commandevent, CommandEvent, ev)
	dialog->OnCancel(*ev);
}
