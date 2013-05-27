#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
//

std::map<std::string, wxPoint> defaultPointMap;

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(point, name)
#define WX_FUNC(name) WX_FUNC1(point, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getx)
WX_FUNC_DEF(gety)
WX_FUNC_DEF(setx)
WX_FUNC_DEF(sety)
WX_FUNC_DEF(plus)
WX_FUNC_DEF(minus)
WX_FUNC_DEF(add)
WX_FUNC_DEF(subtract)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)
WX_FUNC_DEF(assign)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getx),
	WX_FUNC(gety),
	WX_FUNC(setx),
	WX_FUNC(sety),
	WX_FUNC(plus),
	WX_FUNC(minus),
	WX_FUNC(equal),
	WX_FUNC(notequal),
	WX_FUNC(assign)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "assign")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(Point, "point")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Point, val, point) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetX (void* val, DeltaValue* at) 
{
	wxPoint *point = DLIB_WXTYPECAST_BASE(Point, val, point);
	WX_SETNUMBER_EX(*at, point->x)
	return true;
}

static bool GetY (void* val, DeltaValue* at) 
{
	wxPoint *point = DLIB_WXTYPECAST_BASE(Point, val, point);
	WX_SETNUMBER_EX(*at, point->y)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "x",					&GetX,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "y",					&GetY,					DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxPointInitFunc()
{
	defaultPointMap.insert(std::pair<std::string, wxPoint> ("DefaultPosition", wxDefaultPosition));
	PointUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("+"), DeltaValue(&point_plus_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("-"), DeltaValue(&point_minus_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&point_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&point_notequal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("="), DeltaValue(&point_assign_LibFunc, binder));
}

void DeltaWxPointCleanUpFunc()
{
	defaultPointMap.clear();
}

bool DeltaWxPointSearch(std::string str, wxPoint *pt)
{
	if (str.find("wx") == 0 && str.length() > 0)
		str = str.substr(2);
	std::map<std::string, wxPoint>::iterator it;
	it = defaultPointMap.find(str);
	if (it == defaultPointMap.end())
		return false;
	*pt = it->second;
	return true;
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(Point, point,
								   DeltaWxPointInitFunc();,
								   DeltaWxPointCleanUpFunc(););

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(point_construct, 0, 2, Nil)
	wxPoint *wxpoint = (wxPoint*) 0;
	DeltaWxPoint *point = (DeltaWxPoint*) 0;
	if (n == 0)
		wxpoint = new wxPoint();
	else if (n == 2) {
		WX_GETDEFINE(x)
		WX_GETDEFINE(y)
		wxpoint = new wxPoint((int)x, (int)y);
	} else {
		DLIB_WXGETPOINT_BASE(pt)
		wxpoint = new wxPoint(*pt);
	}
	if (wxpoint) point = DNEWCLASS(DeltaWxPoint, (wxpoint));
	WX_SETOBJECT(Point, point)
}

DLIB_FUNC_START(point_destruct, 1, Nil)
	DLIB_WXDELETE(point, Point, point)
}

DLIB_FUNC_START(point_getx, 1, Nil)
	DLIB_WXGET_BASE(point, Point, wxpoint)
	WX_SETNUMBER(wxpoint->x);
}

DLIB_FUNC_START(point_gety, 1, Nil)
	DLIB_WXGET_BASE(point, Point, wxpoint)
	WX_SETNUMBER(wxpoint->y);
}

DLIB_FUNC_START(point_setx, 2, Nil)
	DLIB_WXGET_BASE(point, Point, wxpoint)
	WX_GETNUMBER(_x)
	wxpoint->x = _x;
}

DLIB_FUNC_START(point_sety, 2, Nil)
	DLIB_WXGET_BASE(point, Point, wxpoint)
	WX_GETNUMBER(_y)
	wxpoint->y = _y;
}

DLIB_FUNC_START(point_plus, 2, Nil)
	DLIB_WXGET_BASE(point, Point, point)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Point, serial_no, point, point_wr)) {
			wxPoint *point2 = (wxPoint*) point_wr->GetCastToNativeInstance();
			WX_SETOBJECT(Point, DNEWCLASS(DeltaWxPoint, (new wxPoint(*point + *point2))))
		} else
		if (DLIB_WXISBASE(Size, serial_no, size, size_wr)) {
			wxSize *size = (wxSize*) size_wr->GetCastToNativeInstance();
			WX_SETOBJECT(Point, DNEWCLASS(DeltaWxPoint, (new wxPoint(*point + *size))))
		}
	}
}

DLIB_FUNC_START(point_minus, 2, Nil)
	DLIB_WXGET_BASE(point, Point, point)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Point, serial_no, point, point_wr)) {
			wxPoint *point2 = (wxPoint*) point_wr->GetCastToNativeInstance();
			WX_SETOBJECT(Point, DNEWCLASS(DeltaWxPoint, (new wxPoint(*point - *point2))))
		} else
		if (DLIB_WXISBASE(Size, serial_no, size, size_wr)) {
			wxSize *size = (wxSize*) size_wr->GetCastToNativeInstance();
			WX_SETOBJECT(Point, DNEWCLASS(DeltaWxPoint, (new wxPoint(*point - *size))))
		}
	}
}

DLIB_FUNC_START(point_equal, 2, Nil)
	DLIB_WXGET_BASE(point, Point, point)
	DLIB_WXGETPOINT_BASE(point2)
	WX_SETBOOL(*point == *point2)
}

DLIB_FUNC_START(point_notequal, 2, Nil)
	DLIB_WXGET_BASE(point, Point, point)
	DLIB_WXGETPOINT_BASE(point2)
	WX_SETBOOL(*point != *point2)
}

DLIB_FUNC_START(point_assign, 2, Nil)
	DLIB_WXGET_BASE(point, Point, point)
	DLIB_WXGETPOINT_BASE(point2)
	point->operator=(*point2);
}
