#include "DeltaWxWizardPageSimple.h"
#include "DeltaWxWizardPage.h"
#include "DeltaWxWizard.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(wizardpagesimple, name)
#define WX_FUNC(name) WX_FUNC1(wizardpagesimple, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(setprev)
WX_FUNC_DEF(setnext)
WX_FUNC_DEF(chain)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(chain),
	WX_FUNC(destruct),
	WX_FUNC(setprev),
	WX_FUNC(setnext)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(2, uarraysize(funcs) - 2, "destruct", "setnext")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(WizardPageSimple, "wizardpagesimple", WizardPage)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(WizardPageSimple, val, wizardpagesimple) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxWizardPage *_parent = DLIB_WXTYPECAST_BASE(WizardPage, val, wizardpage);
	DeltaWxWizardPage *parent = DNEWCLASS(DeltaWxWizardPage, (_parent));
	WX_SETOBJECT_EX(*at, WizardPage, parent)
	return true;
}

static bool GetPrev (void* val, DeltaValue* at) 
{
	wxWizardPageSimple *page = DLIB_WXTYPECAST_BASE(WizardPageSimple, val, wizardpagesimple);
	wxWizardPage *prev = page->GetPrev();
	DeltaWxWizardPage *retval = prev ? DNEWCLASS(DeltaWxWizardPage, (prev)) : (DeltaWxWizardPage*) 0;
	WX_SETOBJECT_EX(*at, WizardPage, retval)
	return true;
}

static bool GetNext (void* val, DeltaValue* at) 
{
	wxWizardPageSimple *page = DLIB_WXTYPECAST_BASE(WizardPageSimple, val, wizardpagesimple);
	wxWizardPage *next = page->GetNext();
	DeltaWxWizardPage *retval = next ? DNEWCLASS(DeltaWxWizardPage, (next)) : (DeltaWxWizardPage*) 0;
	WX_SETOBJECT_EX(*at, WizardPage, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "WizardPage",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "prev",				&GetPrev,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "next",				&GetNext,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(WizardPageSimple,wizardpagesimple)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(wizardpagesimple_construct, 0, 5, Nil)
	wxWizardPageSimple *page = (wxWizardPageSimple*) 0;
	DeltaWxWizardPageSimple *_page = (DeltaWxWizardPageSimple*) 0;
	if (n == 0)
		page = new wxWizardPageSimple();
	else {
		DLIB_WXGET_BASE(wizard, Wizard, parent)
		wxWizardPage *prev = (wxWizardPage *)NULL, *next = (wxWizardPage *)NULL;
		wxBitmap bitmap = wxNullBitmap;
		const wxChar* resource = NULL;
		if (n >= 2) { DLIB_WXGET_BASE(wizardpage, WizardPage, pg) prev = pg; }
		if (n >= 3) { DLIB_WXGET_BASE(wizardpage, WizardPage, pg) next = pg; }
		if (n >= 4) { DLIB_WXGET_BASE(bitmap, Bitmap, bmp) bitmap = *bmp; }
		if (n >= 5) { WX_GETSTRING(str) resource = str.GetData(); }
		page = new wxWizardPageSimple(parent, prev, next, bitmap, resource);
	}
	if (page)
		_page = DNEWCLASS(DeltaWxWizardPageSimple, (page));
	WX_SETOBJECT(WizardPageSimple, _page)
}

DLIB_FUNC_START(wizardpagesimple_destruct, 1, Nil)
	DLIB_WXDELETE(wizardpagesimple, WizardPageSimple, page)
}

DLIB_FUNC_START(wizardpagesimple_setprev, 2, Nil)
	DLIB_WXGET_BASE(wizardpagesimple, WizardPageSimple, page)
	DLIB_WXGET_BASE(wizardpage, WizardPage, prev)
	page->SetPrev(prev);
}

DLIB_FUNC_START(wizardpagesimple_setnext, 2, Nil)
	DLIB_WXGET_BASE(wizardpagesimple, WizardPageSimple, page)
	DLIB_WXGET_BASE(wizardpage, WizardPage, next)
	page->SetNext(next);
}

DLIB_FUNC_START(wizardpagesimple_chain, 2, Nil)
	DLIB_WXGET_BASE(wizardpagesimple, WizardPageSimple, first)
	DLIB_WXGET_BASE(wizardpagesimple, WizardPageSimple, second)
	wxWizardPageSimple::Chain(first, second);
}
