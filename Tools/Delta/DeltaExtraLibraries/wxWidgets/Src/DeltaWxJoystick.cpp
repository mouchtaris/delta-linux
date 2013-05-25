#include "DeltaWxJoystick.h"
#include "DeltaWxObject.h"
#include "DeltaWxPoint.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(joystick, name)
#define WX_FUNC(name) WX_FUNC1(joystick, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getnumberjoysticks)
WX_FUNC_DEF(getbuttonstate)
WX_FUNC_DEF(getmanufacturerid)
WX_FUNC_DEF(getmovementthreshold)
WX_FUNC_DEF(getnumberaxes)
WX_FUNC_DEF(getnumberbuttons)
WX_FUNC_DEF(getpollingmax)
WX_FUNC_DEF(getpollingmin)
WX_FUNC_DEF(getproductid)
WX_FUNC_DEF(getproductname)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(getpovposition)
WX_FUNC_DEF(getpovctsposition)
WX_FUNC_DEF(getruddermax)
WX_FUNC_DEF(getruddermin)
WX_FUNC_DEF(getrudderposition)
WX_FUNC_DEF(getumax)
WX_FUNC_DEF(getumin)
WX_FUNC_DEF(getuposition)
WX_FUNC_DEF(getvmax)
WX_FUNC_DEF(getvmin)
WX_FUNC_DEF(getvposition)
WX_FUNC_DEF(getxmax)
WX_FUNC_DEF(getxmin)
WX_FUNC_DEF(getymax)
WX_FUNC_DEF(getymin)
WX_FUNC_DEF(getzmax)
WX_FUNC_DEF(getzmin)
WX_FUNC_DEF(getzposition)
WX_FUNC_DEF(haspov)
WX_FUNC_DEF(haspov4dir)
WX_FUNC_DEF(haspovcts)
WX_FUNC_DEF(hasrudder)
WX_FUNC_DEF(hasu)
WX_FUNC_DEF(hasv)
WX_FUNC_DEF(hasz)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(releasecapture)
WX_FUNC_DEF(setcapture)
WX_FUNC_DEF(setmovementthreshold)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getnumberjoysticks),
	WX_FUNC(getbuttonstate),
	WX_FUNC(getmanufacturerid),
	WX_FUNC(getmovementthreshold),
	WX_FUNC(getnumberaxes),
	WX_FUNC(getnumberbuttons),
	WX_FUNC(getpollingmax),
	WX_FUNC(getpollingmin),
	WX_FUNC(getproductid),
	WX_FUNC(getproductname),
	WX_FUNC(getposition),
	WX_FUNC(getpovposition),
	WX_FUNC(getpovctsposition),
	WX_FUNC(getruddermax),
	WX_FUNC(getruddermin),
	WX_FUNC(getrudderposition),
	WX_FUNC(getumax),
	WX_FUNC(getumin),
	WX_FUNC(getuposition),
	WX_FUNC(getvmax),
	WX_FUNC(getvmin),
	WX_FUNC(getvposition),
	WX_FUNC(getxmax),
	WX_FUNC(getxmin),
	WX_FUNC(getymax),
	WX_FUNC(getymin),
	WX_FUNC(getzmax),
	WX_FUNC(getzmin),
	WX_FUNC(getzposition),
	WX_FUNC(haspov),
	WX_FUNC(haspov4dir),
	WX_FUNC(haspovcts),
	WX_FUNC(hasrudder),
	WX_FUNC(hasu),
	WX_FUNC(hasv),
	WX_FUNC(hasz),
	WX_FUNC(isok),
	WX_FUNC(releasecapture),
	WX_FUNC(setcapture),
	WX_FUNC(setmovementthreshold)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(2, uarraysize(funcs) - 2, "getbuttonstate", "setmovementthreshold")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Joystick, "joystick", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Joystick, val, joystick) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetButtonState (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetButtonState())
	return true;
}

static bool GetManufacturerId (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetManufacturerId())
	return true;
}

static bool GetMovementThreshold (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetMovementThreshold())
	return true;
}

static bool GetNumberAxes (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetNumberAxes())
	return true;
}

static bool GetNumberButtons (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetNumberButtons())
	return true;
}

static bool GetPollingMax (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetPollingMax())
	return true;
}

static bool GetPollingMin (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetPollingMin())
	return true;
}

static bool GetProductId (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetProductId())
	return true;
}

static bool GetProductName (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETSTRING_EX(*at, joystick->GetProductName())
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Point, new wxPoint(joystick->GetPosition()))
	return true;
}

static bool GetPOVPosition (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetPOVPosition())
	return true;
}

static bool GetPOVCTSPosition (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetPOVCTSPosition())
	return true;
}

static bool GetRudderMax (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetRudderMax())
	return true;
}

static bool GetRudderMin (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetRudderMin())
	return true;
}

static bool GetRudderPosition (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetRudderPosition())
	return true;
}

static bool GetUMax (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetUMax())
	return true;
}

static bool GetUMin (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetUMin())
	return true;
}

static bool GetUPosition (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetUPosition())
	return true;
}

static bool GetVMax (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetVMax())
	return true;
}

static bool GetVMin (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetVMin())
	return true;
}

static bool GetVPosition (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetVPosition())
	return true;
}

static bool GetXMax (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetXMax())
	return true;
}

static bool GetXMin (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetXMin())
	return true;
}

static bool GetYMax (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetYMax())
	return true;
}

static bool GetYMin (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetYMin())
	return true;
}

static bool GetZMax (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetZMax())
	return true;
}

static bool GetZMin (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetZMax())
	return true;
}

static bool GetZPosition (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETNUMBER_EX(*at, joystick->GetZPosition())
	return true;
}

static bool GetHasPOV (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETBOOL_EX(*at, joystick->HasPOV())
	return true;
}

static bool GetHasPOV4Dir (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETBOOL_EX(*at, joystick->HasPOV4Dir())
	return true;
}

static bool GetHasPOVCTS (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETBOOL_EX(*at, joystick->HasPOVCTS())
	return true;
}

static bool GetHasRudder (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETBOOL_EX(*at, joystick->HasRudder())
	return true;
}

static bool GetHasU (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETBOOL_EX(*at, joystick->HasU())
	return true;
}

static bool GetHasV (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETBOOL_EX(*at, joystick->HasV())
	return true;
}

static bool GetHasZ (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETBOOL_EX(*at, joystick->HasZ())
	return true;
}

static bool GetIsOk (void* val, DeltaValue* at) 
{
	wxJoystick *joystick = DLIB_WXTYPECAST_BASE(Joystick, val, joystick);
	WX_SETBOOL_EX(*at, joystick->IsOk())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "buttonState",		&GetButtonState,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "manufacturerId",		&GetManufacturerId,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "movementThreshold",	&GetMovementThreshold,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "numberAxes",			&GetNumberAxes,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "numberButtons",		&GetNumberButtons,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pollingMax",			&GetPollingMax,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pollingMin",			&GetPollingMin,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "productId",			&GetProductId,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "productName",		&GetProductName,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "position",			&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "povPosition",		&GetPOVPosition,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "povctsPosition",		&GetPOVCTSPosition,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rudderMax",			&GetRudderMax,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rudderMin",			&GetRudderMin,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rudderPosition",		&GetRudderPosition,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "uMax",				&GetUMax,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "uMin",				&GetUMin,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "uPosition",			&GetUPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "vMax",				&GetVMax,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "vMin",				&GetVMin,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "vPosition",			&GetVPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "xMax",				&GetXMax,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "xMin",				&GetXMin,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "yMax",				&GetYMax,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "yMin",				&GetYMin,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "zMax",				&GetZMax,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "zMin",				&GetZMin,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "zPosition",			&GetZPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasPOV",				&GetHasPOV,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasPOV4Dir",			&GetHasPOV4Dir,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasPOVCTS",			&GetHasPOVCTS,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasRudder",			&GetHasRudder,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasU",				&GetHasU,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasV",				&GetHasV,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "hasZ",				&GetHasZ,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isOk",				&GetIsOk,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Joystick,joystick)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(joystick_construct, 0, 1, Nil)
	int joystick = wxJOYSTICK1;
	if (n >= 1) { WX_GETDEFINE_DEFINED(joystick) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Joystick, new wxJoystick(joystick))
}

WX_FUNC_START(joystick_getbuttonstate, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetButtonState())
}

WX_FUNC_START(joystick_getmanufacturerid, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetManufacturerId())
}

WX_FUNC_START(joystick_getmovementthreshold, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetMovementThreshold())
}

WX_FUNC_START(joystick_getnumberaxes, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetNumberAxes())
}

WX_FUNC_START(joystick_getnumberbuttons, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetNumberButtons())
}

WX_FUNC_START(joystick_getnumberjoysticks, 0, Nil)
	WX_SETNUMBER(wxJoystick::GetNumberJoysticks())
}

WX_FUNC_START(joystick_getpollingmax, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetPollingMax())
}

WX_FUNC_START(joystick_getpollingmin, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetPollingMin())
}

WX_FUNC_START(joystick_getproductid, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetProductId())
}

WX_FUNC_START(joystick_getproductname, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETSTRING(joystick->GetProductName())
}

WX_FUNC_START(joystick_getposition, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(joystick->GetPosition()))
}

WX_FUNC_START(joystick_getpovposition, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetPOVPosition())
}

WX_FUNC_START(joystick_getpovctsposition, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetPOVCTSPosition())
}

WX_FUNC_START(joystick_getruddermax, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetRudderMax())
}

WX_FUNC_START(joystick_getruddermin, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetRudderMin())
}

WX_FUNC_START(joystick_getrudderposition, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetRudderPosition())
}

WX_FUNC_START(joystick_getumax, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetUMax())
}

WX_FUNC_START(joystick_getumin, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetUMin())
}

WX_FUNC_START(joystick_getuposition, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetUPosition())
}

WX_FUNC_START(joystick_getvmax, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetVMax())
}

WX_FUNC_START(joystick_getvmin, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetVMin())
}

WX_FUNC_START(joystick_getvposition, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetVPosition())
}

WX_FUNC_START(joystick_getxmax, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetXMax())
}

WX_FUNC_START(joystick_getxmin, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetXMin())
}

WX_FUNC_START(joystick_getymax, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetYMax())
}

WX_FUNC_START(joystick_getymin, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetYMin())
}

WX_FUNC_START(joystick_getzmax, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetZMax())
}

WX_FUNC_START(joystick_getzmin, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetZMin())
}

WX_FUNC_START(joystick_getzposition, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETNUMBER(joystick->GetZPosition())
}

WX_FUNC_START(joystick_haspov, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETBOOL(joystick->HasPOV())
}

WX_FUNC_START(joystick_haspov4dir, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETBOOL(joystick->HasPOV4Dir())
}

WX_FUNC_START(joystick_haspovcts, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETBOOL(joystick->HasPOVCTS())
}

WX_FUNC_START(joystick_hasrudder, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETBOOL(joystick->HasRudder())
}

WX_FUNC_START(joystick_hasu, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETBOOL(joystick->HasU())
}

WX_FUNC_START(joystick_hasv, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETBOOL(joystick->HasV())
}

WX_FUNC_START(joystick_hasz, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETBOOL(joystick->HasZ())
}

WX_FUNC_START(joystick_isok, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETBOOL(joystick->IsOk())
}

WX_FUNC_START(joystick_releasecapture, 1, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_SETBOOL(joystick->ReleaseCapture())
}

WX_FUNC_ARGRANGE_START(joystick_setcapture, 2, 3, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	DLIB_WXGET_BASE(window, Window, window)
	int pollingFreq = 0;
	if (n >= 3) { WX_GETNUMBER(pollingFreq) }
	WX_SETBOOL(joystick->SetCapture(window, pollingFreq))
}

WX_FUNC_START(joystick_setmovementthreshold, 2, Nil)
	DLIB_WXGET_BASE(joystick, Joystick, joystick)
	WX_GETNUMBER(threshold)
	joystick->SetMovementThreshold(threshold);
}
