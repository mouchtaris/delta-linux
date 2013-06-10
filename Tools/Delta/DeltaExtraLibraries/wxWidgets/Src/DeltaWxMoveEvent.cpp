#include "DeltaWxMoveEvent.h"
#include "DeltaWxEvent.h"
#include "DeltaWxPoint.h"
#include "DeltaWxRect.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(moveevent, name)
#define WX_FUNC(name) WX_FUNC1(moveevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(getrect)
WX_FUNC_DEF(setposition)
WX_FUNC_DEF(setrect)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getposition),
	WX_FUNC(getrect),
	WX_FUNC(setposition),
	WX_FUNC(setrect)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getposition", "setrect")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MoveEvent, "moveevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MoveEvent, val, moveevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxMoveEvent *ev = DLIB_WXTYPECAST_BASE(MoveEvent, val, moveevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Point, new wxPoint(ev->GetPosition()))
	return true;
}

static bool GetRect (void* val, DeltaValue* at) 
{
	wxMoveEvent *ev = DLIB_WXTYPECAST_BASE(MoveEvent, val, moveevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Rect, new wxRect(ev->GetRect()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "position",			&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rect",				&GetRect,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(MoveEvent,moveevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(moveevent_construct, 0, 2, Nil)
	wxMoveEvent *evt = (wxMoveEvent*) 0;
	if (n == 0)
		evt = new wxMoveEvent();
	else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Point, serial_no, point, pt)) {
				int winid = 0;
				if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
				evt = new wxMoveEvent(*pt, winid);
			} else if (DLIB_WXISBASE(Rect, serial_no, rect, rect)) {
				int winid = 0;
				if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
				evt = new wxMoveEvent(*rect, winid);
			}
		}
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(MoveEvent, evt)
}

WX_FUNC_START(moveevent_getposition, 1, Nil)
	DLIB_WXGET_BASE(moveevent, MoveEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(evt->GetPosition()))
}

WX_FUNC_START(moveevent_getrect, 1, Nil)
	DLIB_WXGET_BASE(moveevent, MoveEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Rect, new wxRect(evt->GetRect()))
}

WX_FUNC_START(moveevent_setposition, 2, Nil)
	DLIB_WXGET_BASE(moveevent, MoveEvent, evt)
	DLIB_WXGETPOINT_BASE(pt)
	evt->SetPosition(*pt);
}

WX_FUNC_START(moveevent_setrect, 2, Nil)
	DLIB_WXGET_BASE(moveevent, MoveEvent, evt)
	DLIB_WXGET_BASE(rect, Rect, rect)
	evt->SetRect(*rect);
}
