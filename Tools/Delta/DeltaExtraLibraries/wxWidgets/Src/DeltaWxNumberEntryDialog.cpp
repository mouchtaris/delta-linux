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
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(numberentrydialog, name)
#define WX_FUNC(name) WX_FUNC1(numberentrydialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(onok)
WX_FUNC_DEF(oncancel)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getvalue),
	WX_FUNC(onok),
	WX_FUNC(oncancel)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "oncancel")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(NumberEntryDialog, "numberentrydialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(NumberEntryDialog, val, numberentrydialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDialog *_parent = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	DeltaWxDialog *parent = DNEWCLASS(DeltaWxDialog, (_parent));
	WX_SETOBJECT_EX(*at, Dialog, parent)
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(NumberEntryDialog, numberentrydialog);

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
	DeltaWxNumberEntryDialog *dialog = DNEWCLASS(DeltaWxNumberEntryDialog,
		(new wxNumberEntryDialog(parent, message, prompt, caption, value, min, max, pos)));
	WX_SETOBJECT(NumberEntryDialog, dialog)
}

DLIB_FUNC_START(numberentrydialog_destruct, 1, Nil)
	DLIB_WXDELETE(numberentrydialog, NumberEntryDialog, dialog)
}

DLIB_FUNC_START(numberentrydialog_getvalue, 1, Nil)
	DLIB_WXGET_BASE(numberentrydialog, NumberEntryDialog, dialog)
	WX_SETNUMBER(dialog->GetValue())
}

DLIB_FUNC_START(numberentrydialog_onok, 2, Nil)
	DLIB_WXGET_BASE(numberentrydialog, NumberEntryDialog, dialog)
	DLIB_WXGET_BASE(commandevent, CommandEvent, ev)
	dialog->OnOK(*ev);
}

DLIB_FUNC_START(numberentrydialog_oncancel, 2, Nil)
	DLIB_WXGET_BASE(numberentrydialog, NumberEntryDialog, dialog)
	DLIB_WXGET_BASE(commandevent, CommandEvent, ev)
	dialog->OnCancel(*ev);
}
