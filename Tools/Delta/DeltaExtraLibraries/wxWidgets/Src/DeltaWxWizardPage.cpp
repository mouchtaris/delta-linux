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

WX_FUNC_DEF(getprev)
WX_FUNC_DEF(getnext)
WX_FUNC_DEF(getbitmap)

WX_FUNCS_START
	WX_FUNC(getprev),
	WX_FUNC(getnext),
	WX_FUNC(getbitmap)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(0, uarraysize(funcs), "getprev", "getbitmap")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(WizardPage, "wizardpage", Panel)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(WizardPage, val, wizardpage) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Panel, val)
	return true;
}

static bool GetBitmap (void* val, DeltaValue* at) 
{
	wxWizardPage *page = DLIB_WXTYPECAST_BASE(WizardPage, val, wizardpage);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Bitmap, new wxBitmap(page->GetBitmap()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Panel",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bitmap",				&GetBitmap,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(WizardPage,wizardpage)

////////////////////////////////////////////////////////////////

WX_FUNC_START(wizardpage_getprev, 1, Nil)
	DLIB_WXGET_BASE(wizardpage, WizardPage, page)
	wxWizardPage* retval	= page->GetPrev();
	WX_SETOBJECT(WizardPage, retval)
}

WX_FUNC_START(wizardpage_getnext, 1, Nil)
	DLIB_WXGET_BASE(wizardpage, WizardPage, page)
	wxWizardPage* retval	= page->GetNext();
	WX_SETOBJECT(WizardPage, retval)
}

WX_FUNC_START(wizardpage_getbitmap, 1, Nil)
	DLIB_WXGET_BASE(wizardpage, WizardPage, page)
	WX_SETOBJECT(Bitmap, new wxBitmap(page->GetBitmap()))
}
