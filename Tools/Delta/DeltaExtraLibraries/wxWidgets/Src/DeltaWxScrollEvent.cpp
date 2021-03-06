#include "DeltaWxScrollEvent.h"
#include "DeltaWxCommandEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(scrollevent, name)
#define WX_FUNC(name) WX_FUNC1(scrollevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getorientation)
WX_FUNC_DEF(getposition)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getorientation),
	WX_FUNC(getposition)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getorientation", "getposition")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ScrollEvent, "scrollevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ScrollEvent, val, scrollevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ScrollEvent,scrollevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(scrollevent_construct, 0, 4, Nil)
	int commandType = wxEVT_NULL, winid = 0, pos = 0, orient = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(commandType) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 3) { WX_GETNUMBER_DEFINED(pos) }
	if (n >= 4) { WX_GETDEFINE_DEFINED(orient) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ScrollEvent, new wxScrollEvent(commandType, winid, pos, orient))
}

WX_FUNC_START(scrollevent_getorientation, 1, Nil)
	DLIB_WXGET_BASE(scrollevent, ScrollEvent, evt)
	WX_SETNUMBER(evt->GetOrientation());
}

WX_FUNC_START(scrollevent_getposition, 1, Nil)
	DLIB_WXGET_BASE(scrollevent, ScrollEvent, evt)
	WX_SETNUMBER(evt->GetPosition());
}
