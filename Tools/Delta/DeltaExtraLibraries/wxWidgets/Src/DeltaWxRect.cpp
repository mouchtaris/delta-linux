#include "DeltaWxRect.h"
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

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(rect, name)
#define WX_FUNC(name) WX_FUNC1(rect, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(centrein)
WX_FUNC_DEF(contains)
WX_FUNC_DEF(deflate)
WX_FUNC_DEF(getbottom)
WX_FUNC_DEF(getheight)
WX_FUNC_DEF(getleft)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(gettopleft)
WX_FUNC_DEF(gettopright)
WX_FUNC_DEF(getbottomleft)
WX_FUNC_DEF(getbottomright)
WX_FUNC_DEF(getright)
WX_FUNC_DEF(getsize)
WX_FUNC_DEF(gettop)
WX_FUNC_DEF(getwidth)
WX_FUNC_DEF(getx)
WX_FUNC_DEF(gety)
WX_FUNC_DEF(inflate)
WX_FUNC_DEF(intersect)
WX_FUNC_DEF(intersects)
WX_FUNC_DEF(isempty)
WX_FUNC_DEF(offset)
WX_FUNC_DEF(setheight)
WX_FUNC_DEF(setposition)
WX_FUNC_DEF(setsize)
WX_FUNC_DEF(setwidth)
WX_FUNC_DEF(setx)
WX_FUNC_DEF(sety)
WX_FUNC_DEF(union)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(centrein),
	WX_FUNC(contains),
	WX_FUNC(deflate),
	WX_FUNC(getbottom),
	WX_FUNC(getheight),
	WX_FUNC(getleft),
	WX_FUNC(getposition),
	WX_FUNC(gettopleft),
	WX_FUNC(gettopright),
	WX_FUNC(getbottomleft),
	WX_FUNC(getbottomright),
	WX_FUNC(getright),
	WX_FUNC(getsize),
	WX_FUNC(gettop),
	WX_FUNC(getwidth),
	WX_FUNC(getx),
	WX_FUNC(gety),
	WX_FUNC(inflate),
	WX_FUNC(intersect),
	WX_FUNC(intersects),
	WX_FUNC(isempty),
	WX_FUNC(offset),
	WX_FUNC(setheight),
	WX_FUNC(setposition),
	WX_FUNC(setsize),
	WX_FUNC(setwidth),
	WX_FUNC(setx),
	WX_FUNC(sety),
	WX_FUNC(union),
	WX_FUNC(equal),
	WX_FUNC(notequal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "notequal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(Rect, "rect")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Rect, val, rect) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetX (void* val, DeltaValue* at) 
{
	wxRect *rect = DLIB_WXTYPECAST_BASE(Rect, val, rect);
	WX_SETNUMBER_EX(*at, rect->x)
	return true;
}

static bool GetY (void* val, DeltaValue* at) 
{
	wxRect *rect = DLIB_WXTYPECAST_BASE(Rect, val, rect);
	WX_SETNUMBER_EX(*at, rect->y)
	return true;
}

static bool GetWidth (void* val, DeltaValue* at) 
{
	wxRect *rect = DLIB_WXTYPECAST_BASE(Rect, val, rect);
	WX_SETNUMBER_EX(*at, rect->width)
	return true;
}

static bool GetHeight (void* val, DeltaValue* at) 
{
	wxRect *rect = DLIB_WXTYPECAST_BASE(Rect, val, rect);
	WX_SETNUMBER_EX(*at, rect->height)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "x",					&GetX,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "y",					&GetY,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "width",				&GetWidth,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "height",				&GetHeight,				DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxRectInitFunc()
{
	RectUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&rect_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&rect_notequal_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(Rect, rect, DeltaWxRectInitFunc();, );

////////////////////////////////////////////////////////////////

#define WXRECT_AVOID_UNNECESSARY_OBJECTS_NO_FUNC(rect, rectRef)					\
	if (rectRef == rect) {														\
		DLIB_RETVAL_REF = DPTR(vm)->GetActualArg(0);							\
	} else {																	\
		DeltaWxRect *retval = DNEWCLASS(DeltaWxRect, (new wxRect(*rectRef)));	\
		WX_SETOBJECT(Rect, retval)												\
	}

#define WXRECT_AVOID_UNNECESSARY_OBJECTS(rect, func)							\
	wxRect *rect##Ref = &(rect->func);											\
	WXRECT_AVOID_UNNECESSARY_OBJECTS_NO_FUNC(rect, rect##Ref)

WX_FUNC_ARGRANGE_START(rect_construct, 0, 4, Nil)
	wxRect *wxrect = (wxRect*) 0;
	DeltaWxRect *rect = (DeltaWxRect*) 0;
	if (n == 0) {
		wxrect = new wxRect();
	} else if (n == 1) {
		DLIB_WXGET_BASE(size, Size, size)
		wxrect = new wxRect(*size);
	} else if (n == 4) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		wxrect = new wxRect(x, y, width, height);
	} else if (n == 2) {
		DLIB_WXGET_BASE(point, Point, pt)
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Point, serial_no, point, point)) {
			wxPoint *pt2 = (wxPoint*) point->GetCastToNativeInstance();
			wxrect = new wxRect(*pt, *pt2);
		} else
		if (DLIB_WXISBASE(Size, serial_no, size, size)) {
			wxSize *sz = (wxSize*) size->GetCastToNativeInstance();
			wxrect = new wxRect(*pt, *sz);
		}
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		return;
	}
	if (wxrect) rect = DNEWCLASS(DeltaWxRect, (wxrect));
	WX_SETOBJECT(Rect, rect)
}

DLIB_FUNC_START(rect_destruct, 1, Nil)
	DLIB_WXDELETE(rect, Rect, rect)
}

WX_FUNC_ARGRANGE_START(rect_centrein, 2, 3, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(rect, Rect, r)
	int dir = wxBOTH;
	if (n >= 3) { WX_GETDEFINE_DEFINED(dir) }
	WXRECT_AVOID_UNNECESSARY_OBJECTS(rect, CenterIn(*r, dir))
}

WX_FUNC_ARGRANGE_START(rect_contains, 2, 3, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	if (n == 3) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_SETBOOL(rect->Contains(x, y))
	} else {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Point, serial_no, point, point)) {
			wxPoint *pt = (wxPoint*) point->GetCastToNativeInstance();
			WX_SETBOOL(rect->Contains(*pt))
		}
		else
		if (DLIB_WXISBASE(Rect, serial_no, rect, rect_wr)) {
			wxRect *r = (wxRect*) rect_wr->GetCastToNativeInstance();
			WX_SETBOOL(rect->Contains(*r))
		}
	}
}

WX_FUNC_ARGRANGE_START(rect_deflate, 2, 3, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	wxRect *rectRef = (wxRect*) NULL;
	if (n == 3) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		rectRef = &(rect->Deflate((wxCoord)x, (wxCoord)y));
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(diff)
			rectRef = &(rect->Deflate((wxCoord)diff));
		} else
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(size, Size, diff)
			rectRef = &(rect->Deflate(*diff));
		}
	}
	WXRECT_AVOID_UNNECESSARY_OBJECTS_NO_FUNC(rect, rectRef);
}

DLIB_FUNC_START(rect_getbottom, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETNUMBER(rect->GetBottom())
}

DLIB_FUNC_START(rect_getheight, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETNUMBER(rect->GetHeight())
}

DLIB_FUNC_START(rect_getleft, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETNUMBER(rect->GetLeft())
}

DLIB_FUNC_START(rect_getposition, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(rect->GetPosition())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(rect_gettopleft, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(rect->GetTopLeft())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(rect_gettopright, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(rect->GetTopRight())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(rect_getbottomleft, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(rect->GetBottomLeft())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(rect_getbottomright, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DeltaWxPoint *retval = DNEWCLASS(DeltaWxPoint, (new wxPoint(rect->GetBottomRight())));
	WX_SETOBJECT(Point, retval)
}

DLIB_FUNC_START(rect_getright, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETNUMBER(rect->GetRight())
}

DLIB_FUNC_START(rect_getsize, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DeltaWxSize *retval = DNEWCLASS(DeltaWxSize, (new wxSize(rect->GetSize())));
	WX_SETOBJECT(Size, retval)
}

DLIB_FUNC_START(rect_gettop, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETNUMBER(rect->GetTop())
}

DLIB_FUNC_START(rect_getwidth, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETNUMBER(rect->GetWidth())
}

DLIB_FUNC_START(rect_getx, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETNUMBER(rect->GetX())
}

DLIB_FUNC_START(rect_gety, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETNUMBER(rect->GetY())
}

WX_FUNC_ARGRANGE_START(rect_inflate, 2, 3, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	wxRect *rectRef = (wxRect*) NULL;
	if (n == 3) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		rectRef = &(rect->Inflate((wxCoord)x, (wxCoord)y));
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(diff)
			rectRef = &(rect->Inflate((wxCoord)diff));
		} else
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(size, Size, diff)
			rectRef = &(rect->Inflate(*diff));
		}
	}
	WXRECT_AVOID_UNNECESSARY_OBJECTS_NO_FUNC(rect, rectRef);
}

DLIB_FUNC_START(rect_intersect, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(rect, Rect, r)
	WXRECT_AVOID_UNNECESSARY_OBJECTS(rect, Intersect(*r))
}

DLIB_FUNC_START(rect_intersects, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(rect, Rect, r)
	WX_SETBOOL(rect->Intersects(*r))
}

DLIB_FUNC_START(rect_isempty, 1, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETBOOL(rect->IsEmpty())
}

WX_FUNC_ARGRANGE_START(rect_offset, 2, 3, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	if (n == 2) {
		DLIB_WXGET_BASE(point, Point, pt)
		rect->Offset(*pt);
	} else {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		rect->Offset(x, y);
	}
}

DLIB_FUNC_START(rect_setheight, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_GETNUMBER(height)
	rect->SetHeight(height);
}

DLIB_FUNC_START(rect_setposition, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(point, Point, point)
	rect->SetPosition(*point);
}

DLIB_FUNC_START(rect_setsize, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(size, Size, s)
	rect->SetSize(*s);
}

DLIB_FUNC_START(rect_setwidth, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_GETNUMBER(width)
	rect->SetWidth(width);
}

DLIB_FUNC_START(rect_setx, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_GETNUMBER(x)
	rect->SetX(x);
}

DLIB_FUNC_START(rect_sety, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_GETNUMBER(y)
	rect->SetY(y);
}

DLIB_FUNC_START(rect_union, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(rect, Rect, r)
	WXRECT_AVOID_UNNECESSARY_OBJECTS(rect, Union(*r))
}

DLIB_FUNC_START(rect_equal, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(rect, Rect, rect2)
	WX_SETBOOL(rect->operator==(*rect2))
}

DLIB_FUNC_START(rect_notequal, 2, Nil)
	DLIB_WXGET_BASE(rect, Rect, rect)
	DLIB_WXGET_BASE(rect, Rect, rect2)
	WX_SETBOOL(rect->operator!=(*rect2))
}
