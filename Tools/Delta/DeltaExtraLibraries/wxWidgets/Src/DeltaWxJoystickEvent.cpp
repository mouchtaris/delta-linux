#include "DeltaWxJoystickEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(joystickevent, name)
#define WX_FUNC(name) WX_FUNC1(joystickevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(buttondown)
WX_FUNC_DEF(buttonisdown)
WX_FUNC_DEF(buttonup)
WX_FUNC_DEF(getbuttonchange)
WX_FUNC_DEF(getbuttonstate)
WX_FUNC_DEF(getjoystick)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(getzposition)
WX_FUNC_DEF(isbutton)
WX_FUNC_DEF(ismove)
WX_FUNC_DEF(iszmove)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(buttondown),
	WX_FUNC(buttonisdown),
	WX_FUNC(buttonup),
	WX_FUNC(getbuttonchange),
	WX_FUNC(getbuttonstate),
	WX_FUNC(getjoystick),
	WX_FUNC(getposition),
	WX_FUNC(getzposition),
	WX_FUNC(isbutton),
	WX_FUNC(ismove),
	WX_FUNC(iszmove)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "iszmove")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(JoystickEvent, "joystickevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(JoystickEvent, val, joystickevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxEvent *_parent = DLIB_WXTYPECAST_BASE(Event, val, event);
	DeltaWxEvent *parent = DNEWCLASS(DeltaWxEvent, (_parent));
	WX_SETOBJECT_EX(*at, Event, parent)
	return true;
}

static bool GetButtonChange (void* val, DeltaValue* at) 
{
	wxJoystickEvent *ev = DLIB_WXTYPECAST_BASE(JoystickEvent, val, joystickevent);
	WX_SETNUMBER_EX(*at, ev->GetButtonChange())
	return true;
}

static bool GetButtonState (void* val, DeltaValue* at) 
{
	wxJoystickEvent *ev = DLIB_WXTYPECAST_BASE(JoystickEvent, val, joystickevent);
	WX_SETNUMBER_EX(*at, ev->GetButtonState())
	return true;
}

static bool GetJoystick (void* val, DeltaValue* at) 
{
	wxJoystickEvent *ev = DLIB_WXTYPECAST_BASE(JoystickEvent, val, joystickevent);
	WX_SETNUMBER_EX(*at, ev->GetJoystick())
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxJoystickEvent *ev = DLIB_WXTYPECAST_BASE(JoystickEvent, val, joystickevent);
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(ev->GetPosition())));
	WX_SETOBJECT_EX(*at, Point, retval)
	return true;
}

static bool GetZPosition (void* val, DeltaValue* at) 
{
	wxJoystickEvent *ev = DLIB_WXTYPECAST_BASE(JoystickEvent, val, joystickevent);
	WX_SETNUMBER_EX(*at, ev->GetZPosition())
	return true;
}

static bool GetIsButton (void* val, DeltaValue* at) 
{
	wxJoystickEvent *ev = DLIB_WXTYPECAST_BASE(JoystickEvent, val, joystickevent);
	WX_SETBOOL_EX(*at, ev->IsButton())
	return true;
}

static bool GetIsMove (void* val, DeltaValue* at) 
{
	wxJoystickEvent *ev = DLIB_WXTYPECAST_BASE(JoystickEvent, val, joystickevent);
	WX_SETBOOL_EX(*at, ev->IsMove())
	return true;
}

static bool GetIsZMove (void* val, DeltaValue* at) 
{
	wxJoystickEvent *ev = DLIB_WXTYPECAST_BASE(JoystickEvent, val, joystickevent);
	WX_SETBOOL_EX(*at, ev->IsZMove())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "buttonChange",		&GetButtonChange,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "buttonState",		&GetButtonState,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "joystick",			&GetJoystick,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "position",			&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "zPosition",			&GetZPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isButton",			&GetIsButton,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isMove",				&GetIsMove,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isZMove",			&GetIsZMove,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(JoystickEvent,joystickevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(joystickevent_construct, 0, 4, Nil)
	int type = wxEVT_NULL;
	int state = 0;
	int joystick = wxJOYSTICK1;
	int change = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(state) }
	if (n >= 3) { WX_GETDEFINE_DEFINED(joystick) }
	if (n >= 4) { WX_GETDEFINE_DEFINED(change) }
	DeltaWxJoystickEvent *evt = DNEWCLASS(DeltaWxJoystickEvent,
		(new wxJoystickEvent(type, state, joystick, change)));
	WX_SETOBJECT(JoystickEvent, evt)
}

DLIB_FUNC_START(joystickevent_destruct, 1, Nil)
	DLIB_WXDELETE(joystickevent, JoystickEvent, evt)
}

WX_FUNC_ARGRANGE_START(joystickevent_buttondown, 1, 2, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	int but = wxJOY_BUTTON_ANY;
	if (n >= 1) { WX_GETDEFINE_DEFINED(but) }
	WX_SETBOOL(evt->ButtonDown(but))
}

WX_FUNC_ARGRANGE_START(joystickevent_buttonisdown, 1, 2, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	int but = wxJOY_BUTTON_ANY;
	if (n >= 1) { WX_GETDEFINE_DEFINED(but) }
	WX_SETBOOL(evt->ButtonIsDown(but))
}

WX_FUNC_ARGRANGE_START(joystickevent_buttonup, 1, 2, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	int but = wxJOY_BUTTON_ANY;
	if (n >= 1) { WX_GETDEFINE_DEFINED(but) }
	WX_SETBOOL(evt->ButtonUp(but))
}

DLIB_FUNC_START(joystickevent_getbuttonchange, 1, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	WX_SETNUMBER(evt->GetButtonChange())
}

DLIB_FUNC_START(joystickevent_getbuttonstate, 1, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	WX_SETNUMBER(evt->GetButtonState())
}

DLIB_FUNC_START(joystickevent_getjoystick, 1, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	WX_SETNUMBER(evt->GetJoystick())
}

DLIB_FUNC_START(joystickevent_getposition, 1, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(evt->GetPosition())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(joystickevent_getzposition, 1, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	WX_SETNUMBER(evt->GetZPosition())
}

DLIB_FUNC_START(joystickevent_isbutton, 1, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	WX_SETBOOL(evt->IsButton())
}

DLIB_FUNC_START(joystickevent_ismove, 1, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	WX_SETBOOL(evt->IsMove())
}

DLIB_FUNC_START(joystickevent_iszmove, 1, Nil)
	DLIB_WXGET_BASE(joystickevent, JoystickEvent, evt)
	WX_SETBOOL(evt->IsZMove())
}
