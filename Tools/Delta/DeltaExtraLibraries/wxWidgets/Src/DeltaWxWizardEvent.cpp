#include "DeltaWxWizardEvent.h"
#include "DeltaWxNotifyEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(wizardevent, name)
#define WX_FUNC(name) WX_FUNC1(wizardevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getdirection)
WX_FUNC_DEF(getpage)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getdirection),
	WX_FUNC(getpage)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getdirection", "getpage")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(WizardEvent, "wizardevent", NotifyEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(WizardEvent, val, wizardevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, NotifyEvent, val)
	return true;
}

static bool GetDirection (void* val, DeltaValue* at) 
{
	wxWizardEvent *ev = DLIB_WXTYPECAST_BASE(WizardEvent, val, wizardevent);
	WX_SETBOOL_EX(*at, ev->GetDirection())
	return true;
}

static bool GetPage (void* val, DeltaValue* at) 
{
	wxWizardEvent *ev = DLIB_WXTYPECAST_BASE(WizardEvent, val, wizardevent);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, WizardPage, ev->GetPage())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "NotifyEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(WizardEvent,wizardevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(wizardevent_construct, 0, 4, Nil)
	int type = wxEVT_NULL, id = wxID_ANY;
	bool direction = true;
	wxWizardPage* page = NULL;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 3) { WX_GETBOOL_DEFINED(direction) }
	if (n >= 4) { DLIB_WXGET_BASE(wizardpage, WizardPage, pg) page = pg; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(WizardEvent, new wxWizardEvent(type, id, direction, page))
}

WX_FUNC_START(wizardevent_getdirection, 1, Nil)
	DLIB_WXGET_BASE(wizardevent, WizardEvent, evt)
	WX_SETBOOL(evt->GetDirection())
}

WX_FUNC_START(wizardevent_getpage, 1, Nil)
	DLIB_WXGET_BASE(wizardevent, WizardEvent, evt)
	wxWizardPage* retval	= evt->GetPage();
	WX_SETOBJECT(WizardPage, retval)
}
