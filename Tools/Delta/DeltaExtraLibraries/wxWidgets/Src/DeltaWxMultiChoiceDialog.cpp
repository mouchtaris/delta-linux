#include "DeltaWxMultiChoiceDialog.h"
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
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(multichoicedialog, name)
#define WX_FUNC(name) WX_FUNC1(multichoicedialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getselections)
WX_FUNC_DEF(setselections)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getselections),
	WX_FUNC(setselections),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MultiChoiceDialog, "multichoicedialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MultiChoiceDialog, val, multichoicedialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDialog *_parent = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	DeltaWxDialog *parent = DNEWCLASS(DeltaWxDialog, (_parent));
	WX_SETOBJECT_EX(*at, Dialog, parent)
	return true;
}

static bool GetSelections (void* val, DeltaValue* at) 
{
	wxMultiChoiceDialog *dlg = DLIB_WXTYPECAST_BASE(MultiChoiceDialog, val, multichoicedialog);
	wxArrayInt selections = dlg->GetSelections();
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, n = (int)selections.GetCount(); i < n; ++i) {
		int selection = selections[i];
		DeltaValue value((DeltaNumberValueType)selection);
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "selections",			&GetSelections,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(MultiChoiceDialog,multichoicedialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(multichoicedialog_construct, 0, 6, Nil)
	wxMultiChoiceDialog *wxdialog = (wxMultiChoiceDialog*) 0;
	DeltaWxMultiChoiceDialog *dialog = (DeltaWxMultiChoiceDialog*) 0;
	if (n == 0)
		wxdialog = new wxMultiChoiceDialog();
	else if (n >= 4) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETSTRING(message)
		WX_GETSTRING(caption)
		WX_GETTABLE(choices_table)
		wxArrayString choices;
		for (int i = 0, num = choices_table->Total(); i < num; ++i) {
			DeltaValue value;
			choices_table->Get(DeltaValue(DeltaNumberValueType(i)), &value);
			if (value.Type() == DeltaValue_String) {
				choices.Add(wxString(value.ToString().c_str(), wxConvUTF8));
			}
		}
		long style = wxCHOICEDLG_STYLE;
		wxPoint pos = wxDefaultPosition;
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		wxdialog = new wxMultiChoiceDialog(parent, message, caption, choices, style, pos);
	}
	if (wxdialog) dialog = DNEWCLASS(DeltaWxMultiChoiceDialog, (wxdialog));
	WX_SETOBJECT(MultiChoiceDialog, dialog)
}

DLIB_FUNC_START(multichoicedialog_destruct, 1, Nil)
	DLIB_WXDELETE(multichoicedialog, MultiChoiceDialog, dialog)
}

DLIB_FUNC_START(multichoicedialog_getselections, 1, Nil)
	DLIB_WXGET_BASE(multichoicedialog, MultiChoiceDialog, dialog)
	wxArrayInt retval = dialog->GetSelections();
	DLIB_RETVAL_REF.FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, num = (int)retval.capacity(); i < num; ++i) {
		DLIB_RETVAL_REF.ToTable()->Set(DeltaValue(DeltaNumberValueType(i)), DeltaValue(DeltaNumberValueType(retval[i])));
	}
}

DLIB_FUNC_START(multichoicedialog_setselections, 2, Nil)
	DLIB_WXGET_BASE(multichoicedialog, MultiChoiceDialog, dialog)
	WX_GETTABLE(selections_table)
	wxArrayInt selections;
	for (int i = 0, num = (int)selections_table->Total(); i < num; ++i) {
		DeltaValue value;
		selections_table->Get(DeltaValue(DeltaNumberValueType(i)), &value);
		if (value.Type() == DeltaValue_Number) {
			selections.Insert((int)value.ToNumber(), i);
		}
	}
	dialog->SetSelections(selections);
}

DLIB_FUNC_START(multichoicedialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(multichoicedialog, MultiChoiceDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
