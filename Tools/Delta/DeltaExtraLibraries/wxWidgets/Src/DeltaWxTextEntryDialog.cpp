#include "DeltaWxTextEntryDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(textentrydialog, name)
#define WX_FUNC(name) WX_FUNC1(textentrydialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(setvalue)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getvalue),
	WX_FUNC(setvalue),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getvalue", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(TextEntryDialog, "textentrydialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TextEntryDialog, val, textentrydialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Dialog, val)
	return true;
}

static bool GetValue (void* val, DeltaValue* at) 
{
	wxTextEntryDialog *dlg = DLIB_WXTYPECAST_BASE(TextEntryDialog, val, textentrydialog);
	WX_SETSTRING_EX(*at, dlg->GetValue())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "value",				&GetValue,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TextEntryDialog,textentrydialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(textentrydialog_construct, 2, 6, Nil)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETSTRING(message)
	wxString caption = wxGetTextFromUserPromptStr, value = wxEmptyString;
	long style = wxTextEntryDialogStyle;
	wxPoint pos = wxDefaultPosition;
	if (n >= 3) { WX_GETSTRING_DEFINED(caption) }
	if (n >= 4) { WX_GETSTRING_DEFINED(value) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 6) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
	wxTextEntryDialog* dialog = new wxTextEntryDialog(parent, message, caption, value, style, pos);
	WX_SET_TOPLEVELWINDOW_OBJECT(TextEntryDialog, dialog)
}

WX_FUNC_START(textentrydialog_getvalue, 1, Nil)
	DLIB_WXGET_BASE(textentrydialog, TextEntryDialog, dialog)
	WX_SETSTRING(dialog->GetValue())
}

WX_FUNC_START(textentrydialog_setvalue, 2, Nil)
	DLIB_WXGET_BASE(textentrydialog, TextEntryDialog, dialog)
	WX_GETSTRING(value)
	dialog->SetValue(value);
}

WX_FUNC_START(textentrydialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(textentrydialog, TextEntryDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
