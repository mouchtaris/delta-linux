#include "DeltaWxWizardPage.h"
#include "DeltaWxPanel.h"
#include "DeltaWxBitmap.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(wizardpage, name)
#define WX_FUNC(name) WX_FUNC1(wizardpage, name)

WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getprev)
WX_FUNC_DEF(getnext)
WX_FUNC_DEF(getbitmap)

WX_FUNCS_START
	WX_FUNC(destruct),
	WX_FUNC(getprev),
	WX_FUNC(getnext),
	WX_FUNC(getbitmap)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(0, uarraysize(funcs), "destruct", "getbitmap")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(WizardPage, "wizardpage", Panel)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(WizardPage, val, wizardpage) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxPanel *_parent = DLIB_WXTYPECAST_BASE(Panel, val, panel);
	DeltaWxPanel *parent = DNEWCLASS(DeltaWxPanel, (_parent));
	WX_SETOBJECT_EX(*at, Panel, parent)
	return true;
}

static bool GetBitmap (void* val, DeltaValue* at) 
{
	wxWizardPage *page = DLIB_WXTYPECAST_BASE(WizardPage, val, wizardpage);
	DeltaWxBitmap *retval = DNEWCLASS(DeltaWxBitmap, (new wxBitmap(page->GetBitmap())));
	WX_SETOBJECT_EX(*at, Bitmap, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Panel",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bitmap",				&GetBitmap,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(WizardPage,wizardpage)

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(wizardpage_destruct, 1, Nil)
	DLIB_WXDELETE(wizardpage, WizardPage, page)
}

DLIB_FUNC_START(wizardpage_getprev, 1, Nil)
	DLIB_WXGET_BASE(wizardpage, WizardPage, page)
	WXNEWCLASS(DeltaWxWizardPage, retval, wxWizardPage, page->GetPrev())
	WX_SETOBJECT(WizardPage, retval)
}

DLIB_FUNC_START(wizardpage_getnext, 1, Nil)
	DLIB_WXGET_BASE(wizardpage, WizardPage, page)
	WXNEWCLASS(DeltaWxWizardPage, retval, wxWizardPage, page->GetNext())
	WX_SETOBJECT(WizardPage, retval)
}

DLIB_FUNC_START(wizardpage_getbitmap, 1, Nil)
	DLIB_WXGET_BASE(wizardpage, WizardPage, page)
	DeltaWxBitmap *retval = DNEWCLASS(DeltaWxBitmap, (new wxBitmap(page->GetBitmap())));
	WX_SETOBJECT(Bitmap, retval)
}
