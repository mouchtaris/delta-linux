#include "DeltaWxDialog.h"
#include "DeltaWxTopLevelWindow.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxSizer.h"
#include "DeltaWxStdDialogButtonSizer.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(dialog, name)
#define WX_FUNC(name) WX_FUNC1(dialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(createbuttonsizer)
WX_FUNC_DEF(createseparatedbuttonsizer)
WX_FUNC_DEF(createstddialogbuttonsizer)
WX_FUNC_DEF(endmodal)
WX_FUNC_DEF(getaffirmativeid)
WX_FUNC_DEF(getescapeid)
WX_FUNC_DEF(getreturncode)
WX_FUNC_DEF(ismodal)
WX_FUNC_DEF(setaffirmativeid)
WX_FUNC_DEF(setescapeid)
WX_FUNC_DEF(setmodal)
WX_FUNC_DEF(setreturncode)
WX_FUNC_DEF(show)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(createbuttonsizer),
	WX_FUNC(createseparatedbuttonsizer),
	WX_FUNC(createstddialogbuttonsizer),
	WX_FUNC(endmodal),
	WX_FUNC(getaffirmativeid),
	WX_FUNC(getescapeid),
	WX_FUNC(getreturncode),
	WX_FUNC(ismodal),
	WX_FUNC(setaffirmativeid),
	WX_FUNC(setescapeid),
	WX_FUNC(setmodal),
	WX_FUNC(setreturncode),
	WX_FUNC(show),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Dialog, "dialog", TopLevelWindow)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Dialog, val, dialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, TopLevelWindow, val)
	return true;
}

static bool GetReturnCode (void* val, DeltaValue* at) 
{
	wxDialog *dialog = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	WX_SETNUMBER_EX(*at, dialog->GetReturnCode())
	return true;
}

static bool GetAffirmativeId (void* val, DeltaValue* at) 
{
	wxDialog *dialog = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	WX_SETNUMBER_EX(*at, dialog->GetAffirmativeId())
	return true;
}

static bool GetEscapeId (void* val, DeltaValue* at) 
{
	wxDialog *dialog = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	WX_SETNUMBER_EX(*at, dialog->GetEscapeId())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "TopLevelWindow",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "returnCode",			&GetReturnCode,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "affirmativeId",		&GetAffirmativeId,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "escapeId",			&GetEscapeId,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Dialog,dialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(dialog_construct, 0, 7, Nil)
	wxDialog *dialog = (wxDialog*) 0;
	if (n == 0)
		dialog = new wxDialog();
	else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		WX_GETSTRING(title)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxDEFAULT_DIALOG_STYLE;
		wxString name = wxDialogNameStr;
		if (n >= 4) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(sz) size = *sz; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { WX_GETSTRING_DEFINED(name) }
		dialog = new wxDialog(parent, id, title, pos, size, style, name);
	}
	WX_SET_TOPLEVELWINDOW_OBJECT(Dialog, dialog)
}

WX_FUNC_ARGRANGE_START(dialog_create, 4, 8, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	WX_GETSTRING(title)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxDEFAULT_DIALOG_STYLE;
	wxString name = wxDialogNameStr;
	if (n >= 5) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(sz) size = *sz; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(dialog->Create(parent, id, title, pos, size, style, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(dialog);
}

WX_FUNC_START(dialog_createbuttonsizer, 2, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_GETDEFINE(flag)
	wxSizer* retval	= dialog->CreateButtonSizer(flag);
	WX_SETOBJECT(Sizer, retval)
}

WX_FUNC_START(dialog_createseparatedbuttonsizer, 2, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_GETDEFINE(flag)
	wxSizer* retval	= dialog->CreateSeparatedButtonSizer(flag);
	WX_SETOBJECT(Sizer, retval)
}

WX_FUNC_START(dialog_createstddialogbuttonsizer, 2, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_GETDEFINE(flag)
	WX_SETOBJECT(StdDialogButtonSizer, dialog->CreateStdDialogButtonSizer(flag))
}

WX_FUNC_START(dialog_endmodal, 2, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_GETDEFINE(retCode)
	dialog->EndModal(retCode);
}

WX_FUNC_START(dialog_getaffirmativeid, 1, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_SETNUMBER(dialog->GetAffirmativeId())
}

WX_FUNC_START(dialog_getescapeid, 1, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_SETNUMBER(dialog->GetEscapeId())
}

WX_FUNC_START(dialog_getreturncode, 1, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_SETNUMBER(dialog->GetReturnCode())
}

WX_FUNC_START(dialog_ismodal, 1, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_SETBOOL(dialog->IsModal())
}

WX_FUNC_START(dialog_setaffirmativeid, 2, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_GETDEFINE(id)
	dialog->SetAffirmativeId(id);
}

WX_FUNC_START(dialog_setescapeid, 2, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_GETDEFINE(id)
	dialog->SetEscapeId(id);
}

WX_FUNC_START(dialog_setmodal, 2, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_GETBOOL(modal)
#if wxCHECK_VERSION(2, 9, 0)	//TODO: maybe remove completely?
	DPTR(vm)->PrimaryError("dialog_setmodal is deprecated, use dialog_showmodal instead");
	DLIB_RESET_RETURN;
#else
	dialog->SetModal(modal);
#endif
}

WX_FUNC_START(dialog_setreturncode, 2, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_GETDEFINE(code)
	dialog->SetReturnCode(code);
}

WX_FUNC_ARGRANGE_START(dialog_show, 1, 2, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	bool show = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(show) }
	WX_SETBOOL(dialog->Show(show))
}

WX_FUNC_START(dialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(dialog, Dialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
