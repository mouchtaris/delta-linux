#include "DeltaWxMessageDialog.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(messagedialog, name)
#define WX_FUNC(name) WX_FUNC1(messagedialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MessageDialog, "messagedialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MessageDialog, val, messagedialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDialog *_parent = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	DeltaWxDialog *parent = DNEWCLASS(DeltaWxDialog, (_parent));
	WX_SETOBJECT_EX(*at, Dialog, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(MessageDialog,messagedialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(messagedialog_construct, 2, 5, Nil)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETSTRING(message)
	wxString caption = wxMessageBoxCaptionStr;
	long style = wxOK|wxCENTRE;
	wxPoint pos = wxDefaultPosition;
	if (n >= 3) { WX_GETSTRING_DEFINED(caption) }
	if (n >= 4) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 5) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
	DeltaWxMessageDialog *dialog = DNEWCLASS(DeltaWxMessageDialog,
		(new wxMessageDialog(parent, message, caption, style, pos)));
	WX_SETOBJECT(MessageDialog, dialog)
}

DLIB_FUNC_START(messagedialog_destruct, 1, Nil)
	DLIB_WXDELETE(messagedialog, MessageDialog, dialog)
}

DLIB_FUNC_START(messagedialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(messagedialog, MessageDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
