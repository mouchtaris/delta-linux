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
WX_FUNC_DEF(create)
WX_FUNC_DEF(getcolourdata)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getcolourdata),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ColourDialog, "colourdialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ColourDialog, val, colourdialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Dialog, val)
	return true;
}

static bool GetColourData (void* val, DeltaValue* at) 
{
	wxColourDialog *dlg = DLIB_WXTYPECAST_BASE(ColourDialog, val, colourdialog);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, ColourData, new wxColourData(dlg->GetColourData()))
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
	wxColourDialog *dialog = (wxColourDialog*) 0;
	if (n == 0)
		dialog = new wxColourDialog();
	else {
		DLIB_WXGET_BASE(window, Window, parent)
		wxColourData *data = NULL;
		if (n >= 2) { DLIB_WXGET_BASE(colourdata, ColourData, cdata) data = cdata; }
		dialog = new wxColourDialog(parent, data);
	}
	WX_SET_TOPLEVELWINDOW_OBJECT(ColourDialog, dialog)
}

WX_FUNC_ARGRANGE_START(colourdialog_create, 2, 3, Nil)
	DLIB_WXGET_BASE(colourdialog, ColourDialog, dialog)
	DLIB_WXGET_BASE(window, Window, parent)
	wxColourData *data = NULL;
	if (n >= 3) { DLIB_WXGET_BASE(colourdata, ColourData, cdata) data = cdata; }
	WX_SETBOOL(dialog->Create(parent, data))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(dialog);
}

WX_FUNC_START(colourdialog_getcolourdata, 1, Nil)
	DLIB_WXGET_BASE(colourdialog, ColourDialog, dialog)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ColourData, new wxColourData(dialog->GetColourData()))
}

WX_FUNC_START(colourdialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(colourdialog, ColourDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
