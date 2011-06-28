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
WX_FUNC_DEF(destruct)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "destruct")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PasswordEntryDialog, "passwordentrydialog", TextEntryDialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PasswordEntryDialog, val, passwordentrydialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxTextEntryDialog *_parent = DLIB_WXTYPECAST_BASE(TextEntryDialog, val, textentrydialog);
	DeltaWxTextEntryDialog *parent = DNEWCLASS(DeltaWxTextEntryDialog, (_parent));
	WX_SETOBJECT_EX(*at, TextEntryDialog, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "TextEntryDialog",	&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PasswordEntryDialog, passwordentrydialog);

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
	if (n >= 6) { DLIB_WXGET_BASE(point, Point, pt) pos = *pt; }
	DeltaWxPasswordEntryDialog *dialog = DNEWCLASS(DeltaWxPasswordEntryDialog,
		(new wxPasswordEntryDialog(parent, message, caption, value, style, pos)));
	WX_SETOBJECT(PasswordEntryDialog, dialog)
}

DLIB_FUNC_START(passwordentrydialog_destruct, 1, Nil)
	DLIB_WXDELETE(passwordentrydialog, PasswordEntryDialog, dialog)
}
