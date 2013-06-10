#include "DeltaWxWizard.h"
#include "DeltaWxDialog.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxSizer.h"
#include "DeltaWxSize.h"
#include "DeltaWxWizardPage.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(wizard, name)
#define WX_FUNC(name) WX_FUNC1(wizard, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(fittopage)
WX_FUNC_DEF(getcurrentpage)
WX_FUNC_DEF(getpageareasizer)
WX_FUNC_DEF(getpagesize)
WX_FUNC_DEF(hasnextpage)
WX_FUNC_DEF(hasprevpage)
WX_FUNC_DEF(runwizard)
WX_FUNC_DEF(setpagesize)
WX_FUNC_DEF(setborder)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(fittopage),
	WX_FUNC(getcurrentpage),
	WX_FUNC(getpageareasizer),
	WX_FUNC(getpagesize),
	WX_FUNC(hasnextpage),
	WX_FUNC(hasprevpage),
	WX_FUNC(runwizard),
	WX_FUNC(setpagesize),
	WX_FUNC(setborder)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "setborder")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Wizard, "wizard", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Wizard, val, wizard) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Dialog, val)
	return true;
}

static bool GetPage (void* val, DeltaValue* at) 
{
	wxWizard *wizard = DLIB_WXTYPECAST_BASE(Wizard, val, wizard);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, WizardPage, wizard->GetCurrentPage())
	return true;
}

static bool GetBorder (void* val, DeltaValue* at) 
{
	wxWizard *wizard = DLIB_WXTYPECAST_BASE(Wizard, val, wizard);
	WX_SETNUMBER_EX(*at, wizard->GetBorder())
	return true;
}

static bool GetSizer (void* val, DeltaValue* at) 
{
	wxWizard *wizard = DLIB_WXTYPECAST_BASE(Wizard, val, wizard);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Sizer, wizard->GetPageAreaSizer())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "page",				&GetPage,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "border",				&GetBorder,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "sizerPage",			&GetSizer,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Wizard,wizard)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(wizard_construct, 0, 6, Nil)
	wxWizard *wizard = (wxWizard*) 0;
	if (n == 0)
		wizard = new wxWizard();
	else if (n >= 1) {
		DLIB_WXGET_BASE(window, Window, parent)
		int id = wxID_ANY;
		wxString title = wxEmptyString;
		wxBitmap bitmap = wxNullBitmap;
		wxPoint pos = wxDefaultPosition;
		long style = wxDEFAULT_DIALOG_STYLE;
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { WX_GETSTRING_DEFINED(title) }
		if (n >= 4) { DLIB_WXGET_BASE(bitmap, Bitmap, bmp) bitmap = *bmp; }
		if (n >= 5) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		wizard = new wxWizard(parent, id, title, bitmap, pos, style);
	}
	WX_SET_TOPLEVELWINDOW_OBJECT(Wizard, wizard)
}

WX_FUNC_ARGRANGE_START(wizard_create, 2, 7, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	DLIB_WXGET_BASE(window, Window, parent)
	int id = wxID_ANY;
	wxString title = wxEmptyString;
	wxBitmap bitmap = wxNullBitmap;
	wxPoint pos = wxDefaultPosition;
	long style = wxDEFAULT_DIALOG_STYLE;
	if (n >= 3) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 4) { WX_GETSTRING_DEFINED(title) }
	if (n >= 5) { DLIB_WXGET_BASE(bitmap, Bitmap, bmp) bitmap = *bmp; }
	if (n >= 6) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	WX_SETBOOL(wizard->Create(parent, id, title, bitmap, pos, style))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(wizard);
}

WX_FUNC_START(wizard_fittopage, 2, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	DLIB_WXGET_BASE(wizardpage, WizardPage, firstPage)
	wizard->FitToPage(firstPage);
}

WX_FUNC_START(wizard_getcurrentpage, 1, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	wxWizardPage* retval	= wizard->GetCurrentPage();
	WX_SETOBJECT(WizardPage, retval)
}

WX_FUNC_START(wizard_getpageareasizer, 1, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	wxSizer* retval	= wizard->GetPageAreaSizer();
	WX_SETOBJECT(Sizer, retval)
}

WX_FUNC_START(wizard_getpagesize, 1, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(wizard->GetPageSize()))
}

WX_FUNC_START(wizard_hasnextpage, 2, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	DLIB_WXGET_BASE(wizardpage, WizardPage, page)
	WX_SETBOOL(wizard->HasNextPage(page))
}

WX_FUNC_START(wizard_hasprevpage, 2, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	DLIB_WXGET_BASE(wizardpage, WizardPage, page)
	WX_SETBOOL(wizard->HasPrevPage(page))
}

WX_FUNC_START(wizard_runwizard, 2, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	DLIB_WXGET_BASE(wizardpage, WizardPage, firstPage)
	WX_SETBOOL(wizard->RunWizard(firstPage))
}

WX_FUNC_START(wizard_setpagesize, 2, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	DLIB_WXGETSIZE_BASE(size)
	wizard->SetPageSize(*size);
}

WX_FUNC_START(wizard_setborder, 2, Nil)
	DLIB_WXGET_BASE(wizard, Wizard, wizard)
	WX_GETNUMBER(border)
	wizard->SetBorder(border);
}
