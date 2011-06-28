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
WX_FUNC_DEF(destruct)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "destruct")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DisplayChangedEvent, "displaychangedevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DisplayChangedEvent, val, displaychangedevent) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(DisplayChangedEvent, displaychangedevent);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(displaychangedevent_construct, 0, Nil)
	DeltaWxDisplayChangedEvent *evt = DNEWCLASS(DeltaWxDisplayChangedEvent, (new wxDisplayChangedEvent()));
	WX_SETOBJECT(DisplayChangedEvent, evt)
}

DLIB_FUNC_START(displaychangedevent_destruct, 1, Nil)
	DLIB_WXDELETE(displaychangedevent, DisplayChangedEvent, evt)
}
