#include "DeltaWxPasswordEntryDialog.h"
#include "DeltaWxTextEntryDialog.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(passwordentrydialog, name)
#define WX_FUNC(name) WX_FUNC1(passwordentrydialog, name)

WX_FUNC_DEF(construct)

WX_FUNCS_START
	WX_FUNC(construct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

//DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PasswordEntryDialog, "passwordentrydialog", TextEntryDialog)
VCLASSID_IMPL(DeltaWxPasswordEntryDialogClassId, "wx::passwordentrydialog")
DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(PasswordEntryDialog, "passwordentrydialog")
void PasswordEntryDialogUtils::InstallAll(DeltaTable *methods)
{
	DPTR(methods)->DelegateInternal(TextEntryDialogUtils::GetMethods());
}

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PasswordEntryDialog, val, passwordentrydialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, TextEntryDialog, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "TextEntryDialog",	&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PasswordEntryDialog,passwordentrydialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(passwordentrydialog_construct, 2, 6, Nil)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETSTRING(message)
	wxString caption = wxGetPasswordFromUserPromptStr, value = wxEmptyString;
	long style = wxTextEntryDialogStyle;
	wxPoint pos = wxDefaultPosition;
	if (n >= 3) { WX_GETSTRING_DEFINED(caption) }
	if (n >= 4) { WX_GETSTRING_DEFINED(value) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 6) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
	wxPasswordEntryDialog* dialog = new wxPasswordEntryDialog(parent, message, caption, value, style, pos);
	WX_SET_TOPLEVELWINDOW_OBJECT(PasswordEntryDialog, dialog)
}
