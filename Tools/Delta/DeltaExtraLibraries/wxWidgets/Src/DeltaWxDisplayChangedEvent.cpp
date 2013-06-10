#include "DeltaWxDisplayChangedEvent.h"
#include "DeltaWxEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(displaychangedevent, name)
#define WX_FUNC(name) WX_FUNC1(displaychangedevent, name)

WX_FUNC_DEF(construct)

WX_FUNCS_START
	WX_FUNC(construct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "destruct")

//DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DisplayChangedEvent, "displaychangedevent", Event)
VCLASSID_IMPL(DeltaWxDisplayChangedEventClassId, "wx::displaychangedevent")
DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(DisplayChangedEvent, "displaychangedevent")
void DisplayChangedEventUtils::InstallAll(DeltaTable *methods)
{
	DPTR(methods)->DelegateInternal(EventUtils::GetMethods());
}

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DisplayChangedEvent, val, displaychangedevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DisplayChangedEvent,displaychangedevent)

////////////////////////////////////////////////////////////////

WX_FUNC_START(displaychangedevent_construct, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(DisplayChangedEvent, new wxDisplayChangedEvent())
}
