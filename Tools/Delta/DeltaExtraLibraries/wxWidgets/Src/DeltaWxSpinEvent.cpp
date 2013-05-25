#include "DeltaWxSpinEvent.h"
#include "DeltaWxNotifyEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(spinevent, name)
#define WX_FUNC(name) WX_FUNC1(spinevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(setposition)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getposition),
	WX_FUNC(setposition)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getposition", "setposition")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SpinEvent, "spinevent", NotifyEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SpinEvent, val, spinevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, NotifyEvent, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "NotifyEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SpinEvent,spinevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(spinevent_construct, 0, 2, Nil)
	int commandType = wxEVT_NULL, winid = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(commandType) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SpinEvent, new wxSpinEvent(commandType, winid))
}

WX_FUNC_START(spinevent_getposition, 1, Nil)
	DLIB_WXGET_BASE(spinevent, SpinEvent, evt)
	WX_SETNUMBER(evt->GetPosition())
}

WX_FUNC_START(spinevent_setposition, 2, Nil)
	DLIB_WXGET_BASE(spinevent, SpinEvent, evt)
	WX_GETNUMBER(pos)
	evt->SetPosition(pos);
}
