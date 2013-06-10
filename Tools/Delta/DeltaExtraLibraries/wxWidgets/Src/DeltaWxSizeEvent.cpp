#include "DeltaWxSizeEvent.h"
#include "DeltaWxEvent.h"
#include "DeltaWxSize.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(sizeevent, name)
#define WX_FUNC(name) WX_FUNC1(sizeevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getsize)
WX_FUNC_DEF(getrect)
WX_FUNC_DEF(setrect)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getsize),
	WX_FUNC(getrect),
	WX_FUNC(setrect)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getsize", "setrect")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SizeEvent, "sizeevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SizeEvent, val, sizeevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetSize (void* val, DeltaValue* at) 
{
	wxSizeEvent *ev = DLIB_WXTYPECAST_BASE(SizeEvent, val, sizeevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Size, new wxSize(ev->GetSize()))
	return true;
}

static bool GetRect (void* val, DeltaValue* at) 
{
	wxSizeEvent *ev = DLIB_WXTYPECAST_BASE(SizeEvent, val, sizeevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Rect, new wxRect(ev->GetRect()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "size",				&GetSize,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rect",				&GetRect,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SizeEvent,sizeevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(sizeevent_construct, 0, 2, Nil)
	wxSizeEvent *evt = (wxSizeEvent*) 0;
	if (n == 0)
		evt = new wxSizeEvent();
	else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Size, serial_no, size, sz)) {
				int winid = 0;
				if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
				evt = new wxSizeEvent(*sz, winid);
			} else if (DLIB_WXISBASE(Rect, serial_no, rect, rect)) {
				int winid = 0;
				if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
				evt = new wxSizeEvent(*rect, winid);
			}
		}
	}
	WX_SETOBJECT(SizeEvent, evt)
}

WX_FUNC_START(sizeevent_getsize, 1, Nil)
	DLIB_WXGET_BASE(sizeevent, SizeEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(evt->GetSize()))
}

WX_FUNC_START(sizeevent_getrect, 1, Nil)
	DLIB_WXGET_BASE(sizeevent, SizeEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Rect, new wxRect(evt->GetRect()))
}

WX_FUNC_START(sizeevent_setrect, 2, Nil)
	DLIB_WXGET_BASE(sizeevent, SizeEvent, evt)
	DLIB_WXGET_BASE(rect, Rect, rect)
	evt->SetRect(*rect);
}
