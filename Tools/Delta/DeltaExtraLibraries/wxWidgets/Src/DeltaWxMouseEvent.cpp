#include "DeltaWxMouseEvent.h"
#include "DeltaWxEvent.h"
#include "DeltaWxPoint.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(mouseevent, name)
#define WX_FUNC(name) WX_FUNC1(mouseevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(altdown)
WX_FUNC_DEF(button)
WX_FUNC_DEF(buttondclick)
WX_FUNC_DEF(buttondown)
WX_FUNC_DEF(buttonup)
WX_FUNC_DEF(cmddown)
WX_FUNC_DEF(controldown)
WX_FUNC_DEF(dragging)
WX_FUNC_DEF(entering)
WX_FUNC_DEF(getbutton)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(getlogicalposition)
WX_FUNC_DEF(getlinesperaction)
WX_FUNC_DEF(getwheelrotation)
WX_FUNC_DEF(getwheeldelta)
WX_FUNC_DEF(getx)
WX_FUNC_DEF(gety)
WX_FUNC_DEF(isbutton)
WX_FUNC_DEF(ispagescroll)
WX_FUNC_DEF(leaving)
WX_FUNC_DEF(leftdclick)
WX_FUNC_DEF(leftdown)
WX_FUNC_DEF(leftisdown)
WX_FUNC_DEF(leftup)
WX_FUNC_DEF(metadown)
WX_FUNC_DEF(middledclick)
WX_FUNC_DEF(middledown)
WX_FUNC_DEF(middleisdown)
WX_FUNC_DEF(middleup)
WX_FUNC_DEF(moving)
WX_FUNC_DEF(rightdclick)
WX_FUNC_DEF(rightdown)
WX_FUNC_DEF(rightisdown)
WX_FUNC_DEF(rightup)
WX_FUNC_DEF(shiftdown)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(altdown),
	WX_FUNC(button),
	WX_FUNC(buttondclick),
	WX_FUNC(buttondown),
	WX_FUNC(buttonup),
	WX_FUNC(cmddown),
	WX_FUNC(controldown),
	WX_FUNC(dragging),
	WX_FUNC(entering),
	WX_FUNC(getbutton),
	WX_FUNC(getposition),
	WX_FUNC(getlogicalposition),
	WX_FUNC(getlinesperaction),
	WX_FUNC(getwheelrotation),
	WX_FUNC(getwheeldelta),
	WX_FUNC(getx),
	WX_FUNC(gety),
	WX_FUNC(isbutton),
	WX_FUNC(ispagescroll),
	WX_FUNC(leaving),
	WX_FUNC(leftdclick),
	WX_FUNC(leftdown),
	WX_FUNC(leftisdown),
	WX_FUNC(leftup),
	WX_FUNC(metadown),
	WX_FUNC(middledclick),
	WX_FUNC(middledown),
	WX_FUNC(middleisdown),
	WX_FUNC(middleup),
	WX_FUNC(moving),
	WX_FUNC(rightdclick),
	WX_FUNC(rightdown),
	WX_FUNC(rightisdown),
	WX_FUNC(rightup),
	WX_FUNC(shiftdown)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "altdown", "shiftdown")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MouseEvent, "mouseevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetAltDown (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->AltDown())
	return true;
}

static bool GetCmdDown (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->CmdDown())
	return true;
}

static bool GetControlDown (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->ControlDown())
	return true;
}

static bool GetDragging (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->Dragging())
	return true;
}

static bool GetEntering (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->Entering())
	return true;
}

static bool GetButton (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETNUMBER_EX(*at, ev->GetButton())
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Point, new wxPoint(ev->GetPosition()))
	return true;
}

static bool GetLinesPerAction (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETNUMBER_EX(*at, ev->GetLinesPerAction())
	return true;
}

static bool GetWheelRotation (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETNUMBER_EX(*at, ev->GetWheelRotation())
	return true;
}

static bool GetWheelDelta (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->GetWheelDelta())
	return true;
}

static bool GetX (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETNUMBER_EX(*at, ev->GetX())
	return true;
}

static bool GetY (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETNUMBER_EX(*at, ev->GetY())
	return true;
}

static bool GetIsButton (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->IsButton())
	return true;
}

static bool GetIsPageScroll (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->IsPageScroll())
	return true;
}

static bool GetLeaving (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->Leaving())
	return true;
}

static bool GetMetaDown (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->MetaDown())
	return true;
}

static bool GetMoving (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->Moving())
	return true;
}

static bool GetShiftDown (void* val, DeltaValue* at) 
{
	wxMouseEvent *ev = DLIB_WXTYPECAST_BASE(MouseEvent, val, mouseevent);
	WX_SETBOOL_EX(*at, ev->ShiftDown())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "altDown",			&GetAltDown,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "cmdDown",			&GetCmdDown,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "controlDown",		&GetControlDown,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "dragging",			&GetDragging,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "entering",			&GetEntering,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "button",				&GetButton,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "position",			&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "linesPerAction",		&GetLinesPerAction,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "wheelRotation",		&GetWheelRotation,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "wheelDelta",			&GetWheelDelta,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "x",					&GetX,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "y",					&GetY,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isButton",			&GetIsButton,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isPageScroll",		&GetIsPageScroll,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "leaving",			&GetLeaving,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "metaDown",			&GetMetaDown,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "moving",				&GetMoving,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "shiftDown",			&GetShiftDown,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(MouseEvent,mouseevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(mouseevent_construct, 0, 1, Nil)
	int mouseType = wxEVT_NULL;
	if (n >= 1) { WX_GETDEFINE_DEFINED(mouseType) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(MouseEvent, new wxMouseEvent(mouseType))
}

WX_FUNC_START(mouseevent_altdown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->AltDown())
}

WX_FUNC_START(mouseevent_button, 2, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_GETDEFINE(button)
	WX_SETBOOL(evt->Button(button))
}

WX_FUNC_ARGRANGE_START(mouseevent_buttondclick, 1, 2, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	int button = wxMOUSE_BTN_ANY;
	if (n >= 2) { WX_GETDEFINE_DEFINED(button) }
	WX_SETBOOL(evt->ButtonDClick(button))
}

WX_FUNC_ARGRANGE_START(mouseevent_buttondown, 1, 2, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	int button = wxMOUSE_BTN_ANY;
	if (n >= 2) { WX_GETDEFINE_DEFINED(button) }
	WX_SETBOOL(evt->ButtonDown(button))
}

WX_FUNC_ARGRANGE_START(mouseevent_buttonup, 1, 2, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	int button = wxMOUSE_BTN_ANY;
	if (n >= 2) { WX_GETDEFINE_DEFINED(button) }
	WX_SETBOOL(evt->ButtonUp(button))
}

WX_FUNC_START(mouseevent_cmddown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->CmdDown())
}

WX_FUNC_START(mouseevent_controldown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->ControlDown())
}

WX_FUNC_START(mouseevent_dragging, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->Dragging())
}

WX_FUNC_START(mouseevent_entering, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->Entering())
}

WX_FUNC_START(mouseevent_getbutton, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETNUMBER(evt->GetButton())
}

WX_FUNC_ARGRANGE_START(mouseevent_getposition, 1, 3, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	if (n == 1) {
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(evt->GetPosition()))
	} else if (n == 3) {
		long x, y;
		evt->GetPosition(&x, &y);
		WX_GETTABLE(x_table)
		WX_GETTABLE(y_table)
		WX_SETTABLE_RETVAL(x_table, DeltaValue((DeltaNumberValueType)x))
		WX_SETTABLE_RETVAL(y_table, DeltaValue((DeltaNumberValueType)y))
	}
}

WX_FUNC_START(mouseevent_getlogicalposition, 2, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(evt->GetLogicalPosition(*dc)))
}

WX_FUNC_START(mouseevent_getlinesperaction, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETNUMBER(evt->GetLinesPerAction())
}

WX_FUNC_START(mouseevent_getwheelrotation, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETNUMBER(evt->GetWheelRotation())
}

WX_FUNC_START(mouseevent_getwheeldelta, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETNUMBER(evt->GetWheelDelta())
}

WX_FUNC_START(mouseevent_getx, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETNUMBER(evt->GetX())
}

WX_FUNC_START(mouseevent_gety, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETNUMBER(evt->GetY())
}

WX_FUNC_START(mouseevent_isbutton, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->IsButton())
}

WX_FUNC_START(mouseevent_ispagescroll, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->IsPageScroll())
}

WX_FUNC_START(mouseevent_leaving, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->Leaving())
}

WX_FUNC_START(mouseevent_leftdclick, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->LeftDClick())
}

WX_FUNC_START(mouseevent_leftdown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->LeftDown())
}

WX_FUNC_START(mouseevent_leftisdown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->LeftIsDown())
}

WX_FUNC_START(mouseevent_leftup, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->LeftUp())
}

WX_FUNC_START(mouseevent_metadown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->MetaDown())
}

WX_FUNC_START(mouseevent_middledclick, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->MiddleDClick())
}

WX_FUNC_START(mouseevent_middledown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->MiddleDown())
}

WX_FUNC_START(mouseevent_middleisdown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->MiddleIsDown())
}

WX_FUNC_START(mouseevent_middleup, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->MiddleUp())
}

WX_FUNC_START(mouseevent_moving, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->Moving())
}

WX_FUNC_START(mouseevent_rightdclick, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->RightDClick())
}

WX_FUNC_START(mouseevent_rightdown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->RightDown())
}

WX_FUNC_START(mouseevent_rightisdown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->RightIsDown())
}

WX_FUNC_START(mouseevent_rightup, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->RightUp())
}

WX_FUNC_START(mouseevent_shiftdown, 1, Nil)
	DLIB_WXGET_BASE(mouseevent, MouseEvent, evt)
	WX_SETBOOL(evt->ShiftDown())
}
