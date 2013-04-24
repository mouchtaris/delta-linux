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
WX_FUNC_DEF(destruct)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "destruct")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SysColourChangedEvent, "syscolourchangedevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SysColourChangedEvent, val, syscolourchangedevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SysColourChangedEvent,syscolourchangedevent)

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(syscolourchangedevent_construct, 0, Nil)
	DeltaWxSysColourChangedEvent *evt = DNEWCLASS(DeltaWxSysColourChangedEvent,
		(new wxSysColourChangedEvent()));
	WX_SETOBJECT(SysColourChangedEvent, evt)
}

DLIB_FUNC_START(syscolourchangedevent_destruct, 1, Nil)
	DLIB_WXDELETE(syscolourchangedevent, SysColourChangedEvent, evt)
}
