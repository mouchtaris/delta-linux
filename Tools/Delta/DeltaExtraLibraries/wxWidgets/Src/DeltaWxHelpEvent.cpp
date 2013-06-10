#include "DeltaWxHelpEvent.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxPoint.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(helpevent, name)
#define WX_FUNC(name) WX_FUNC1(helpevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getorigin)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(setorigin)
WX_FUNC_DEF(setposition)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getorigin),
	WX_FUNC(getposition),
	WX_FUNC(setorigin),
	WX_FUNC(setposition)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getorigin", "setposition")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(HelpEvent, "helpevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(HelpEvent, val, helpevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetOrigin (void* val, DeltaValue* at) 
{
	wxHelpEvent *ev = DLIB_WXTYPECAST_BASE(HelpEvent, val, helpevent);
	WX_SETNUMBER_EX(*at, (int)ev->GetOrigin())
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxHelpEvent *ev = DLIB_WXTYPECAST_BASE(HelpEvent, val, helpevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Point, new wxPoint(ev->GetPosition()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "origin",				&GetOrigin,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "position",			&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(HelpEvent,helpevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(helpevent_construct, 0, 4, Nil)
	int type = wxEVT_NULL;
	int winid = 0;
	wxPoint pt = wxDefaultPosition;
	wxHelpEvent::Origin origin = wxHelpEvent::Origin_Unknown;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 3) { DLIB_WXGETPOINT_BASE(point) pt = *point; }
	if (n >= 4) { WX_GETDEFINE(_origin) origin = (wxHelpEvent::Origin)_origin; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(HelpEvent, new wxHelpEvent(type, winid, pt, origin))
}

WX_FUNC_START(helpevent_getorigin, 1, Nil)
	DLIB_WXGET_BASE(helpevent, HelpEvent, evt)
	WX_SETNUMBER((int)evt->GetOrigin())
}

WX_FUNC_START(helpevent_getposition, 1, Nil)
	DLIB_WXGET_BASE(helpevent, HelpEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(evt->GetPosition()))
}

WX_FUNC_START(helpevent_setorigin, 2, Nil)
	DLIB_WXGET_BASE(helpevent, HelpEvent, evt)
	WX_GETDEFINE(origin)
	evt->SetOrigin((wxHelpEvent::Origin)origin);
}

WX_FUNC_START(helpevent_setposition, 1, Nil)
	DLIB_WXGET_BASE(helpevent, HelpEvent, evt)
	DLIB_WXGETPOINT_BASE(point)
	evt->SetPosition(*point);
}
