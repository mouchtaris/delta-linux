#include "DeltaWxPageSetupDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPageSetupDialogData.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(pagesetupdialog, name)
#define WX_FUNC(name) WX_FUNC1(pagesetupdialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getpagesetupdata)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getpagesetupdata),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getpagesetupdata", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PageSetupDialog, "pagesetupdialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PageSetupDialog, val, pagesetupdialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetPageSetupDialogData (void* val, DeltaValue* at) 
{
	wxPageSetupDialog *dlg = DLIB_WXTYPECAST_BASE(PageSetupDialog, val, pagesetupdialog);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, PageSetupDialogData, new wxPageSetupDialogData(dlg->GetPageSetupDialogData()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pageSetupDialogData",&GetPageSetupDialogData,DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PageSetupDialog,pagesetupdialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(pagesetupdialog_construct, 1, 2, Nil)
	DLIB_WXGET_BASE(window, Window, parent)
	wxPageSetupDialogData *data = NULL;
	if (n >= 2) { DLIB_WXGET_BASE(pagesetupdialogdata, PageSetupDialogData, _data) data = _data; }
	wxPageSetupDialog* dialog = new wxPageSetupDialog(parent, data);
	WX_SETOBJECT(PageSetupDialog, dialog)
}

WX_FUNC_START(pagesetupdialog_getpagesetupdata, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialog, PageSetupDialog, dialog)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(PageSetupDialogData, new wxPageSetupDialogData(dialog->GetPageSetupDialogData()))
}

WX_FUNC_START(pagesetupdialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(pagesetupdialog, PageSetupDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
