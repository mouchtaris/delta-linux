#include "DeltaWxEraseEvent.h"
#include "DeltaWxEvent.h"
#include "DeltaWxDC.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(eraseevent, name)
#define WX_FUNC(name) WX_FUNC1(eraseevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getdc)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getdc)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "getdc")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(EraseEvent, "eraseevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(EraseEvent, val, eraseevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetDC (void* val, DeltaValue* at) 
{
	wxEraseEvent *ev = DLIB_WXTYPECAST_BASE(EraseEvent, val, eraseevent);
	wxDC *dc = ev->GetDC();
	DeltaWxDC *retval = dc ? DNEWCLASS(DeltaWxDC, (dc)) : (DeltaWxDC*) 0;
	WX_SETOBJECT_EX(*at, DC, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "dc",					&GetDC,					DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(EraseEvent, eraseevent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(eraseevent_construct, 0, 2, Nil)
	int Id = 0;
	wxDC *dc = (wxDC*) 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(Id) }
	if (n >= 2) { DLIB_WXGET_BASE(dc, DC, _dc) dc = _dc; }
	DeltaWxEraseEvent* evt = DNEWCLASS(DeltaWxEraseEvent, (new wxEraseEvent(Id, dc)));
	WX_SETOBJECT(EraseEvent, evt)
}

DLIB_FUNC_START(eraseevent_destruct, 1, Nil)
	DLIB_WXDELETE(eraseevent, EraseEvent, evt)
}

DLIB_FUNC_START(eraseevent_getdc, 1, Nil)
	DLIB_WXGET_BASE(eraseevent, EraseEvent, evt)
	DeltaWxDC *retval = DNEWCLASS(DeltaWxDC, (evt->GetDC()));
	WX_SETOBJECT(DC, retval)
}
