#include "DeltaWxInitDialogEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(initdialogevent, name)
#define WX_FUNC(name) WX_FUNC1(initdialogevent, name)

WX_FUNC_DEF(construct)

WX_FUNCS_START
	WX_FUNC(construct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

//DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(InitDialogEvent, "initdialogevent", Event)
VCLASSID_IMPL(DeltaWxInitDialogEventClassId, "wx::initdialogevent")
DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(InitDialogEvent, "initdialogevent")
void InitDialogEventUtils::InstallAll(DeltaTable *methods)
{
	DPTR(methods)->DelegateInternal(EventUtils::GetMethods());
}

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(InitDialogEvent, val, initdialogevent) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(InitDialogEvent,initdialogevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(initdialogevent_construct, 0, 1, Nil)
	int Id = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(Id) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(InitDialogEvent, new wxInitDialogEvent(Id))
}
