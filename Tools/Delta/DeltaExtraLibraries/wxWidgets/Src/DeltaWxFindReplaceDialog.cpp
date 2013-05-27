#include "DeltaWxFindReplaceDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxFindReplaceData.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(findreplacedialog, name)
#define WX_FUNC(name) WX_FUNC1(findreplacedialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getdata)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getdata)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "getdata")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FindReplaceDialog, "findreplacedialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FindReplaceDialog, val, findreplacedialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDialog *_parent = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	DeltaWxDialog *parent = DNEWCLASS(DeltaWxDialog, (_parent));
	WX_SETOBJECT_EX(*at, Dialog, parent)
	return true;
}

static bool GetFindReplaceData (void* val, DeltaValue* at) 
{
	wxFindReplaceDialog *dialog = DLIB_WXTYPECAST_BASE(FindReplaceDialog, val, findreplacedialog);
	DeltaWxFindReplaceData *retval = DNEWCLASS(DeltaWxFindReplaceData,
		((wxFindReplaceData*)dialog->GetData()));
	WX_SETOBJECT_EX(*at, FindReplaceData, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "findReplaceData",	&GetFindReplaceData,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FindReplaceDialog,findreplacedialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(findreplacedialog_construct, 0, 4, Nil)
	wxFindReplaceDialog *wxdialog = (wxFindReplaceDialog*) 0;
	DeltaWxFindReplaceDialog *dialog = (DeltaWxFindReplaceDialog*) 0;
	if (n == 0)
		wxdialog = new wxFindReplaceDialog();
	else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		DLIB_WXGET_BASE(findreplacedata, FindReplaceData, data)
		WX_GETSTRING(title)
		int style = 0;
		if (n >= 4) { WX_GETDEFINE_DEFINED(style) }
		wxdialog = new wxFindReplaceDialog(parent, data, title, style);
	}
	if (wxdialog) dialog = DNEWCLASS(DeltaWxFindReplaceDialog, (wxdialog));
	WX_SETOBJECT(FindReplaceDialog, dialog)
}

DLIB_FUNC_START(findreplacedialog_destruct, 1, Nil)
	DLIB_WXDELETE(findreplacedialog, FindReplaceDialog, dialog)
}

WX_FUNC_ARGRANGE_START(findreplacedialog_create, 4, 5, Nil)
	DLIB_WXGET_BASE(findreplacedialog, FindReplaceDialog, dialog)
	DLIB_WXGET_BASE(window, Window, parent)
	DLIB_WXGET_BASE(findreplacedata, FindReplaceData, data)
	WX_GETSTRING(title)
	int style = 0;
	if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
	WX_SETBOOL(dialog->Create(parent, data, title, style))
}

DLIB_FUNC_START(findreplacedialog_getdata, 1, Nil)
	DLIB_WXGET_BASE(findreplacedialog, FindReplaceDialog, dialog)
	DeltaWxFindReplaceData *retval = DNEWCLASS(DeltaWxFindReplaceData,
		((wxFindReplaceData*)dialog->GetData()));
	WX_SETOBJECT(FindReplaceData, retval)
}
