#include "DeltaWxQueryNewPaletteEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(querynewpaletteevent, name)
#define WX_FUNC(name) WX_FUNC1(querynewpaletteevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getpaletterealized)
WX_FUNC_DEF(setpaletterealized)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getpaletterealized),
	WX_FUNC(setpaletterealized)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setpaletterealized")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(QueryNewPaletteEvent, "querynewpaletteevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(QueryNewPaletteEvent, val, querynewpaletteevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetPaletteRealized (void* val, DeltaValue* at) 
{
	wxQueryNewPaletteEvent *ev = DLIB_WXTYPECAST_BASE(QueryNewPaletteEvent, val, querynewpaletteevent);
	WX_SETBOOL_EX(*at, ev->GetPaletteRealized())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "paletteRealized",	&GetPaletteRealized,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(QueryNewPaletteEvent,querynewpaletteevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(querynewpaletteevent_construct, 0, 1, Nil)
	int winid = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(winid) }
	DeltaWxQueryNewPaletteEvent *evt = DNEWCLASS(DeltaWxQueryNewPaletteEvent,
		(new wxQueryNewPaletteEvent(winid)));
	WX_SETOBJECT(QueryNewPaletteEvent, evt)
}

DLIB_FUNC_START(querynewpaletteevent_destruct, 1, Nil)
	DLIB_WXDELETE(querynewpaletteevent, QueryNewPaletteEvent, evt)
}

DLIB_FUNC_START(querynewpaletteevent_getpaletterealized, 1, Nil)
	DLIB_WXGET_BASE(querynewpaletteevent, QueryNewPaletteEvent, evt)
	WX_SETBOOL(evt->GetPaletteRealized())
}

DLIB_FUNC_START(querynewpaletteevent_setpaletterealized, 2, Nil)
	DLIB_WXGET_BASE(querynewpaletteevent, QueryNewPaletteEvent, evt)
	WX_GETBOOL(realized)
	evt->SetPaletteRealized(realized);
}
