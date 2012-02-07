#include "DeltaWxSingleChoiceDialog.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(singlechoicedialog, name)
#define WX_FUNC(name) WX_FUNC1(singlechoicedialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(getselectionclientdata)
WX_FUNC_DEF(getstringselection)
WX_FUNC_DEF(setselection)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getselection),
	WX_FUNC(getselectionclientdata),
	WX_FUNC(getstringselection),
	WX_FUNC(setselection),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SingleChoiceDialog, "singlechoicedialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SingleChoiceDialog, val, singlechoicedialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDialog *_parent = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	DeltaWxDialog *parent = DNEWCLASS(DeltaWxDialog, (_parent));
	WX_SETOBJECT_EX(*at, Dialog, parent)
	return true;
}

static bool GetSelection (void* val, DeltaValue* at) 
{
	wxSingleChoiceDialog *dlg = DLIB_WXTYPECAST_BASE(SingleChoiceDialog, val, singlechoicedialog);
	WX_SETNUMBER_EX(*at, dlg->GetSelection())
	return true;
}

static bool GetStringSelection (void* val, DeltaValue* at) 
{
	wxSingleChoiceDialog *dlg = DLIB_WXTYPECAST_BASE(SingleChoiceDialog, val, singlechoicedialog);
	WX_SETSTRING_EX(*at, dlg->GetStringSelection())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "selection",			&GetSelection,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "stringSelection",	&GetStringSelection,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SingleChoiceDialog, singlechoicedialog);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(singlechoicedialog_construct, 0, 7, Nil)
	wxSingleChoiceDialog *wxdialog = (wxSingleChoiceDialog*) 0;
	DeltaWxSingleChoiceDialog *dialog = (DeltaWxSingleChoiceDialog*) 0;
	if (n == 0)
		wxdialog = new wxSingleChoiceDialog();
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
		char **clientData = (char **)NULL;
		long style = wxCHOICEDLG_STYLE;
		wxPoint pos = wxDefaultPosition;
		if (n >= 5) {
			if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Nil)
				clientData = (char **)NULL;
			else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Table) {
				WX_GETTABLE(choices_table)
				int num = choices_table->Total();
				clientData = DNEWARR(char *, num);
				for (int i = 0; i < num; ++i) {
					DeltaValue value;
					choices_table->Get(DeltaValue(DeltaNumberValueType(i)), &value);
					if (value.Type() == DeltaValue_String) {
						clientData[i] = (char *)value.ToString().c_str();
					}
				}
			}
		}
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		wxdialog = new wxSingleChoiceDialog(parent, message, caption, choices, clientData, style, pos);
		if (clientData)
			DDELARR(clientData);
	}
	if (wxdialog) dialog = DNEWCLASS(DeltaWxSingleChoiceDialog, (wxdialog));
	WX_SETOBJECT(SingleChoiceDialog, dialog)
}

DLIB_FUNC_START(singlechoicedialog_destruct, 1, Nil)
	DLIB_WXDELETE(singlechoicedialog, SingleChoiceDialog, dialog)
}

DLIB_FUNC_START(singlechoicedialog_getselection, 1, Nil)
	DLIB_WXGET_BASE(singlechoicedialog, SingleChoiceDialog, dialog)
	WX_SETNUMBER(dialog->GetSelection())
}

DLIB_FUNC_START(singlechoicedialog_getselectionclientdata, 1, Nil)
	DLIB_WXGET_BASE(singlechoicedialog, SingleChoiceDialog, dialog)
	DLIB_RETVAL_REF.FromString(std::string(dialog->GetSelectionClientData()));
}

DLIB_FUNC_START(singlechoicedialog_getstringselection, 1, Nil)
	DLIB_WXGET_BASE(singlechoicedialog, SingleChoiceDialog, dialog)
	WX_SETSTRING(dialog->GetStringSelection())
}

DLIB_FUNC_START(singlechoicedialog_setselection, 2, Nil)
	DLIB_WXGET_BASE(singlechoicedialog, SingleChoiceDialog, dialog)
	WX_GETNUMBER(selection)
	dialog->SetSelection(selection);
}

DLIB_FUNC_START(singlechoicedialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(singlechoicedialog, SingleChoiceDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
