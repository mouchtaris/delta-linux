#include "DeltaWxShowEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(showevent, name)
#define WX_FUNC(name) WX_FUNC1(showevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getshow)
WX_FUNC_DEF(setshow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getshow),
	WX_FUNC(setshow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setshow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ShowEvent, "showevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ShowEvent, val, showevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetShow (void* val, DeltaValue* at) 
{
	wxShowEvent *ev = DLIB_WXTYPECAST_BASE(ShowEvent, val, showevent);
	WX_SETBOOL_EX(*at, ev->GetShow())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "show",				&GetShow,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ShowEvent, showevent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(showevent_construct, 0, 2, Nil)
	int winid = 0;
	bool show = false;
	if (n >= 1) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 2) { WX_GETBOOL_DEFINED(show) }
	DeltaWxShowEvent *evt = DNEWCLASS(DeltaWxShowEvent, (new wxShowEvent(winid, show)));
	WX_SETOBJECT(ShowEvent, evt)
}

DLIB_FUNC_START(showevent_destruct, 1, Nil)
	DLIB_WXDELETE(showevent, ShowEvent, evt)
}

DLIB_FUNC_START(showevent_getshow, 1, Nil)
	DLIB_WXGET_BASE(showevent, ShowEvent, evt)
	WX_SETBOOL(evt->GetShow())
}

DLIB_FUNC_START(showevent_setshow, 2, Nil)
	DLIB_WXGET_BASE(showevent, ShowEvent, evt)
	WX_GETBOOL(show)
	evt->SetShow(show);
}
