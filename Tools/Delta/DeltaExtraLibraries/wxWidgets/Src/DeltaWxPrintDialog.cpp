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
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getprintdc)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getprintdc),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PrintDialog, "printdialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PrintDialog, val, printdialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetPrintDialogData (void* val, DeltaValue* at) 
{
	wxPrintDialog *dlg = DLIB_WXTYPECAST_BASE(PrintDialog, val, printdialog);
	DeltaWxPrintDialogData *retval = DNEWCLASS(DeltaWxPrintDialogData, (
		new wxPrintDialogData(dlg->GetPrintDialogData())));
	WX_SETOBJECT_EX(*at, PrintDialogData, retval)
	return true;
}

static bool GetPrintData (void* val, DeltaValue* at) 
{
	wxPrintDialog *dlg = DLIB_WXTYPECAST_BASE(PrintDialog, val, printdialog);
	DeltaWxPrintData *retval = DNEWCLASS(DeltaWxPrintData, (
		new wxPrintData(dlg->GetPrintData())));
	WX_SETOBJECT_EX(*at, PrintData, retval)
	return true;
}

static bool GetPrintDC (void* val, DeltaValue* at) 
{
	wxPrintDialog *dlg = DLIB_WXTYPECAST_BASE(PrintDialog, val, printdialog);
	wxDC *dc = dlg->GetPrintDC();
	DeltaWxDC *retval = dc ? DNEWCLASS(DeltaWxDC, (dc)) : (DeltaWxDC*) 0;
	WX_SETOBJECT_EX(*at, DC, retval)
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
	wxPrintDialog *wxdialog = (wxPrintDialog*) 0;
	DeltaWxPrintDialog *dialog = (DeltaWxPrintDialog*) 0;
	DLIB_WXGET_BASE(window, Window, parent)
	if (n == 1) {
		wxdialog = new wxPrintDialog(parent);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(PrintDialogData, serial_no, printdialogdata, data)) {
			wxPrintDialogData *dialogdata = (wxPrintDialogData*)data->GetCastToNativeInstance();
			wxdialog = new wxPrintDialog(parent, dialogdata);
		} else if (DLIB_WXISBASE(PrintData, serial_no, printdata, data)) {
			wxPrintData* printdata = (wxPrintData*)data->GetCastToNativeInstance();
			wxdialog = new wxPrintDialog(parent, printdata);
		}
	}
	if (wxdialog) dialog = DNEWCLASS(DeltaWxPrintDialog, (wxdialog));
	WX_SETOBJECT(PrintDialog, dialog)
}

DLIB_FUNC_START(printdialog_destruct, 1, Nil)
	DLIB_WXDELETE(printdialog, PrintDialog, dialog)
}

DLIB_FUNC_START(printdialog_getprintdc, 1, Nil)
	DLIB_WXGET_BASE(printdialog, PrintDialog, dialog)
	DeltaWxDC *retval = DNEWCLASS(DeltaWxDC, (dialog->GetPrintDC()));
	WX_SETOBJECT(DC, retval)
}

DLIB_FUNC_START(printdialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(printdialog, PrintDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
