#include "DeltaWxSysColourChangedEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(syscolourchangedevent, name)
#define WX_FUNC(name) WX_FUNC1(syscolourchangedevent, name)

WX_FUNC_DEF(construct)

WX_FUNCS_START
	WX_FUNC(construct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

//DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SysColourChangedEvent, "syscolourchangedevent", Event)
VCLASSID_IMPL(DeltaWxSysColourChangedEventClassId, "wx::syscolourchangedevent")
DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(SysColourChangedEvent, "syscolourchangedevent")
void SysColourChangedEventUtils::InstallAll(DeltaTable *methods)
{
	DPTR(methods)->DelegateInternal(EventUtils::GetMethods());
}

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SysColourChangedEvent, val, syscolourchangedevent) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(SysColourChangedEvent,syscolourchangedevent)

////////////////////////////////////////////////////////////////

WX_FUNC_START(syscolourchangedevent_construct, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SysColourChangedEvent, new wxSysColourChangedEvent())
}
