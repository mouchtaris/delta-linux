#include "DeltaWxFontDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxFontData.h"
#include "DeltaWxWindow.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(fontdialog, name)
#define WX_FUNC(name) WX_FUNC1(fontdialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getfontdata)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getfontdata),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FontDialog, "fontdialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FontDialog, val, fontdialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Dialog, val)
	return true;
}

static bool GetFontData (void* val, DeltaValue* at) 
{
	wxFontDialog *dialog = DLIB_WXTYPECAST_BASE(FontDialog, val, fontdialog);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, FontData, new wxFontData(dialog->GetFontData()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "fontData",			&GetFontData,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FontDialog,fontdialog)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(fontdialog_construct, 0, 2, Nil)
	wxFontDialog *dialog = (wxFontDialog*) 0;
	if (n == 0)
		dialog = new wxFontDialog();
	else {
		DLIB_WXGET_BASE(window, Window, parent)
		if (n == 1)
			dialog = new wxFontDialog(parent);
		else {
			DLIB_WXGET_BASE(fontdata, FontData, data)
			dialog = new wxFontDialog(parent, *data);
		}
	}
	WX_SET_TOPLEVELWINDOW_OBJECT(FontDialog, dialog)
}

WX_FUNC_ARGRANGE_START(fontdialog_create, 2, 3, Nil)
	DLIB_WXGET_BASE(fontdialog, FontDialog, dialog)
	DLIB_WXGET_BASE(window, Window, parent)
	if (n == 2) {
		WX_SETBOOL(dialog->Create(parent))
	} else {
		DLIB_WXGET_BASE(fontdata, FontData, data)
		WX_SETBOOL(dialog->Create(parent, *data))
	}
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(dialog);
}

WX_FUNC_START(fontdialog_getfontdata, 1, Nil)
	DLIB_WXGET_BASE(fontdialog, FontDialog, dialog)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(FontData, new wxFontData(dialog->GetFontData()))
}

WX_FUNC_START(fontdialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(fontdialog, FontDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
