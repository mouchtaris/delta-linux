#include "DeltaWxPrintDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPrintDialogData.h"
#include "DeltaWxPrintData.h"
#include "DeltaWxDC.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(printdialog, name)
#define WX_FUNC(name) WX_FUNC1(printdialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getprintdc)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getprintdc),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getprintdc", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PrintDialog, "printdialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PrintDialog, val, printdialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetPrintDialogData (void* val, DeltaValue* at) 
{
	wxPrintDialog *dlg = DLIB_WXTYPECAST_BASE(PrintDialog, val, printdialog);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, PrintDialogData, new wxPrintDialogData(dlg->GetPrintDialogData()))
	return true;
}

static bool GetPrintData (void* val, DeltaValue* at) 
{
	wxPrintDialog *dlg = DLIB_WXTYPECAST_BASE(PrintDialog, val, printdialog);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, PrintData, new wxPrintData(dlg->GetPrintData()))
	return true;
}

static bool GetPrintDC (void* val, DeltaValue* at) 
{
	wxPrintDialog *dlg = DLIB_WXTYPECAST_BASE(PrintDialog, val, printdialog);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, DC, dlg->GetPrintDC())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printDialogData",	&GetPrintDialogData,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printData",			&GetPrintData,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "printDC",			&GetPrintDC,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PrintDialog,printdialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(printdialog_construct, 1, 2, Nil)
	wxPrintDialog *dialog = (wxPrintDialog*) 0;
	DLIB_WXGET_BASE(window, Window, parent)
	if (n == 1) {
		dialog = new wxPrintDialog(parent);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(PrintDialogData, serial_no, printdialogdata, dialogdata)) {
			dialog = new wxPrintDialog(parent, dialogdata);
		} else if (DLIB_WXISBASE(PrintData, serial_no, printdata, printdata)) {
			dialog = new wxPrintDialog(parent, printdata);
		}
	}
	WX_SETOBJECT(PrintDialog, dialog)
}

WX_FUNC_START(printdialog_getprintdc, 1, Nil)
	DLIB_WXGET_BASE(printdialog, PrintDialog, dialog)
	WX_SETOBJECT(DC, dialog->GetPrintDC())
}

WX_FUNC_START(printdialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(printdialog, PrintDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
