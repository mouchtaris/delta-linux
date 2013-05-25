#include "DeltaWxKeyEvent.h"
#include "DeltaWxEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(keyevent, name)
#define WX_FUNC(name) WX_FUNC1(keyevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(altdown)
WX_FUNC_DEF(cmddown)
WX_FUNC_DEF(controldown)
WX_FUNC_DEF(getkeycode)
WX_FUNC_DEF(getmodifiers)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(getrawkeycode)
WX_FUNC_DEF(getrawkeyflags)
#if wxUSE_UNICODE
WX_FUNC_DEF(getunicodekey)
#endif // wxUSE_UNICODE
WX_FUNC_DEF(getx)
WX_FUNC_DEF(gety)
WX_FUNC_DEF(hasmodifiers)
WX_FUNC_DEF(metadown)
WX_FUNC_DEF(shiftdown)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(altdown),
	WX_FUNC(cmddown),
	WX_FUNC(controldown),
	WX_FUNC(getkeycode),
	WX_FUNC(getmodifiers),
	WX_FUNC(getposition),
	WX_FUNC(getrawkeycode),
	WX_FUNC(getrawkeyflags),
#if wxUSE_UNICODE
	WX_FUNC(getunicodekey),
#endif // wxUSE_UNICODE
	WX_FUNC(getx),
	WX_FUNC(gety),
	WX_FUNC(hasmodifiers),
	WX_FUNC(metadown),
	WX_FUNC(shiftdown)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "altdown", "shiftdown")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(KeyEvent, "keyevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetX (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETNUMBER_EX(*at, ev->GetX())
	return true;
}

static bool GetY (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETNUMBER_EX(*at, ev->GetY())
	return true;
}

static bool GetKeyCode (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETNUMBER_EX(*at, ev->GetKeyCode())
	return true;
}

static bool GetControlDown (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETBOOL_EX(*at, ev->ControlDown())
	return true;
}

static bool GetShiftDown (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETBOOL_EX(*at, ev->ShiftDown())
	return true;
}

static bool GetAltDown (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETBOOL_EX(*at, ev->AltDown())
	return true;
}

static bool GetMetaDown (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETBOOL_EX(*at, ev->MetaDown())
	return true;
}
#if wxUSE_UNICODE
static bool GetUnicodeKey (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETSTRING_EX(*at, wxString(ev->GetUnicodeKey()))
	return true;
}
#endif // wxUSE_UNICODE
static bool GetRawCode (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETNUMBER_EX(*at, ev->GetRawKeyCode())
	return true;
}

static bool GetRawFlags (void* val, DeltaValue* at) 
{
	wxKeyEvent *ev = DLIB_WXTYPECAST_BASE(KeyEvent, val, keyevent);
	WX_SETNUMBER_EX(*at, ev->GetRawKeyFlags())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "x",					&GetX,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "y",					&GetY,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "keyCode",			&GetKeyCode,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "controlDown",		&GetControlDown,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "shiftDown",			&GetShiftDown,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "altDown",			&GetAltDown,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "metaDown",			&GetMetaDown,			DELTA_GETBYSTRING_NO_PRECOND	},
#if wxUSE_UNICODE
	{ "uniChar",			&GetUnicodeKey,			DELTA_GETBYSTRING_NO_PRECOND	},
#endif // wxUSE_UNICODE
	{ "rawCode",			&GetRawCode,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rawFlags",			&GetRawFlags,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(KeyEvent,keyevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(keyevent_construct, 0, 1, Nil)
	int keyType = wxEVT_NULL;
	if (n >= 1) { WX_GETDEFINE_DEFINED(keyType) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(KeyEvent, new wxKeyEvent(keyType))
}

WX_FUNC_START(keyevent_altdown, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETBOOL(evt->AltDown())
}

WX_FUNC_START(keyevent_cmddown, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETBOOL(evt->CmdDown())
}

WX_FUNC_START(keyevent_controldown, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETBOOL(evt->ControlDown())
}

WX_FUNC_START(keyevent_getkeycode, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETNUMBER(evt->GetKeyCode())
}

WX_FUNC_START(keyevent_getmodifiers, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETNUMBER(evt->GetModifiers())
}

WX_FUNC_START(keyevent_getposition, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(evt->GetPosition()))
}

WX_FUNC_START(keyevent_getrawkeycode, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETNUMBER(evt->GetRawKeyCode())
}

WX_FUNC_START(keyevent_getrawkeyflags, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETNUMBER(evt->GetRawKeyFlags())
}
#if wxUSE_UNICODE
WX_FUNC_START(keyevent_getunicodekey, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETSTRING(wxString(evt->GetUnicodeKey()))
}
#endif // wxUSE_UNICODE
WX_FUNC_START(keyevent_getx, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETNUMBER(evt->GetX())
}

WX_FUNC_START(keyevent_gety, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETNUMBER(evt->GetY())
}

WX_FUNC_START(keyevent_hasmodifiers, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETBOOL(evt->HasModifiers())
}

WX_FUNC_START(keyevent_metadown, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETBOOL(evt->MetaDown())
}

WX_FUNC_START(keyevent_shiftdown, 1, Nil)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETBOOL(evt->ShiftDown())
}
