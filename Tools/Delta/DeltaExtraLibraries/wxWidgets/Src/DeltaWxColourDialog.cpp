#include "DeltaWxColourDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxColourData.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(colourdialog, name)
#define WX_FUNC(name) WX_FUNC1(colourdialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getcolourdata)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getcolourdata),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ColourDialog, "colourdialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ColourDialog, val, colourdialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDialog *_parent = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	DeltaWxDialog *parent = DNEWCLASS(DeltaWxDialog, (_parent));
	WX_SETOBJECT_EX(*at, Dialog, parent)
	return true;
}

static bool GetColourData (void* val, DeltaValue* at) 
{
	wxColourDialog *dlg = DLIB_WXTYPECAST_BASE(ColourDialog, val, colourdialog);
	DeltaWxColourData *retval = DNEWCLASS(DeltaWxColourData, (new wxColourData(dlg->GetColourData())));
	WX_SETOBJECT_EX(*at, ColourData, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "colourData",			&GetColourData,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ColourDialog,colourdialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(colourdialog_construct, 0, 2, Nil)
	wxColourDialog *wxdialog = (wxColourDialog*) 0;
	DeltaWxColourDialog *dialog = (DeltaWxColourDialog*) 0;
	if (n == 0)
		wxdialog = new wxColourDialog();
	else {
		DLIB_WXGET_BASE(window, Window, parent)
		wxColourData *data = NULL;
		if (n >= 2) { DLIB_WXGET_BASE(colourdata, ColourData, cdata) data = cdata; }
		wxdialog = new wxColourDialog(parent, data);
	}
	if (wxdialog) dialog = DNEWCLASS(DeltaWxColourDialog, (wxdialog));
	WX_SETOBJECT(ColourDialog, dialog)
}

DLIB_FUNC_START(colourdialog_destruct, 1, Nil)
	DLIB_WXDELETE(colourdialog, ColourDialog, dialog)
}

WX_FUNC_ARGRANGE_START(colourdialog_create, 2, 3, Nil)
	DLIB_WXGET_BASE(colourdialog, ColourDialog, dialog)
	DLIB_WXGET_BASE(window, Window, parent)
	wxColourData *data = NULL;
	if (n >= 3) { DLIB_WXGET_BASE(colourdata, ColourData, cdata) data = cdata; }
	WX_SETBOOL(dialog->Create(parent, data))
}

DLIB_FUNC_START(colourdialog_getcolourdata, 1, Nil)
	DLIB_WXGET_BASE(colourdialog, ColourDialog, dialog)
	DeltaWxColourData *retval = DNEWCLASS(DeltaWxColourData, (new wxColourData(dialog->GetColourData())));
	WX_SETOBJECT(ColourData, retval)
}

DLIB_FUNC_START(colourdialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(colourdialog, ColourDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
