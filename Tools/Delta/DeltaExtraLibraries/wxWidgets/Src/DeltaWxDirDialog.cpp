#include "DeltaWxDirDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(dirdialog, name)
#define WX_FUNC(name) WX_FUNC1(dirdialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getpath)
WX_FUNC_DEF(getmessage)
WX_FUNC_DEF(setmessage)
WX_FUNC_DEF(setpath)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getpath),
	WX_FUNC(getmessage),
	WX_FUNC(setmessage),
	WX_FUNC(setpath),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DirDialog, "dirdialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DirDialog, val, dirdialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDialog *_parent = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	DeltaWxDialog *parent = DNEWCLASS(DeltaWxDialog, (_parent));
	WX_SETOBJECT_EX(*at, Dialog, parent)
	return true;
}

static bool GetMessage (void* val, DeltaValue* at) 
{
	wxDirDialog *dlg = DLIB_WXTYPECAST_BASE(DirDialog, val, dirdialog);
	WX_SETSTRING_EX(*at, dlg->GetMessage())
	return true;
}

static bool GetPath (void* val, DeltaValue* at) 
{
	wxDirDialog *dlg = DLIB_WXTYPECAST_BASE(DirDialog, val, dirdialog);
	WX_SETSTRING_EX(*at, dlg->GetPath())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "message",			&GetMessage,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "path",				&GetPath,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DirDialog,dirdialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(dirdialog_construct, 1, 7, Nil)
	DLIB_WXGET_BASE(window, Window, parent)
	wxString message = wxDirSelectorPromptStr, defaultPath = wxEmptyString, name = wxDirDialogNameStr;
	long style = wxDD_DEFAULT_STYLE;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	if (n >= 2) { WX_GETSTRING_DEFINED(message) }
	if (n >= 3) { WX_GETSTRING_DEFINED(defaultPath) }
	if (n >= 4) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETSTRING_DEFINED(name) }
	DeltaWxDirDialog *dialog = DNEWCLASS(DeltaWxDirDialog,
		(new wxDirDialog(parent, message, defaultPath, style, pos, size, name)));
	WX_SETOBJECT(DirDialog, dialog)
}

DLIB_FUNC_START(dirdialog_destruct, 1, Nil)
	DLIB_WXDELETE(dirdialog, DirDialog, dialog)
}

DLIB_FUNC_START(dirdialog_getpath, 1, Nil)
	DLIB_WXGET_BASE(dirdialog, DirDialog, dialog)
	WX_SETSTRING(dialog->GetPath())
}

DLIB_FUNC_START(dirdialog_getmessage, 1, Nil)
	DLIB_WXGET_BASE(dirdialog, DirDialog, dialog)
	WX_SETSTRING(dialog->GetMessage())
}

DLIB_FUNC_START(dirdialog_setmessage, 2, Nil)
	DLIB_WXGET_BASE(dirdialog, DirDialog, dialog)
	WX_GETSTRING(message)
	dialog->SetMessage(message);
}

DLIB_FUNC_START(dirdialog_setpath, 2, Nil)
	DLIB_WXGET_BASE(dirdialog, DirDialog, dialog)
	WX_GETSTRING(path)
	dialog->SetPath(path);
}

DLIB_FUNC_START(dirdialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(dirdialog, DirDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
