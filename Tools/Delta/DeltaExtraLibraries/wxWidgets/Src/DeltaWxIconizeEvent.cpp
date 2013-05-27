#include "DeltaWxIconizeEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(iconizeevent, name)
#define WX_FUNC(name) WX_FUNC1(iconizeevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(iconized)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(iconized)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "iconized")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(IconizeEvent, "iconizeevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(IconizeEvent, val, iconizeevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetIconized (void* val, DeltaValue* at) 
{
	wxIconizeEvent *ev = DLIB_WXTYPECAST_BASE(IconizeEvent, val, iconizeevent);
	WX_SETBOOL_EX(*at, ev->Iconized())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "iconized",			&GetIconized,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(IconizeEvent,iconizeevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(iconizeevent_construct, 0, 2, Nil)
	int winid = 0;
	bool iconized = true;
	if (n >= 1) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 2) { WX_GETBOOL_DEFINED(iconized) }
	DeltaWxIconizeEvent *evt = DNEWCLASS(DeltaWxIconizeEvent, (new wxIconizeEvent(winid, iconized)));
	WX_SETOBJECT(IconizeEvent, evt)
}

DLIB_FUNC_START(iconizeevent_destruct, 1, Nil)
	DLIB_WXDELETE(iconizeevent, IconizeEvent, evt)
}

DLIB_FUNC_START(iconizeevent_iconized, 1, Nil)
	DLIB_WXGET_BASE(iconizeevent, IconizeEvent, evt)
	WX_SETBOOL(evt->Iconized())
}
