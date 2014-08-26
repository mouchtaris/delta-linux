#include "DeltaWxRegion.h"
#include "DeltaWxObject.h"
#include "DeltaWxRect.h"
#include "DeltaWxPoint.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxColour.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
//

#if !wxCHECK_VERSION(2, 9, 0)
typedef int wxPolygonFillMode;
#endif

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(region, name)
#define WX_FUNC(name) WX_FUNC1(region, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(clear)
WX_FUNC_DEF(contains)
WX_FUNC_DEF(converttobitmap)
WX_FUNC_DEF(getbox)
WX_FUNC_DEF(intersect)
WX_FUNC_DEF(isempty)
WX_FUNC_DEF(isequal)
WX_FUNC_DEF(subtract)
WX_FUNC_DEF(offset)
WX_FUNC_DEF(union)
WX_FUNC_DEF(xor)
WX_FUNC_DEF(notequal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(clear),
	WX_FUNC(contains),
	WX_FUNC(converttobitmap),
	WX_FUNC(getbox),
	WX_FUNC(intersect),
	WX_FUNC(isempty),
	WX_FUNC(isequal),
	WX_FUNC(subtract),
	WX_FUNC(offset),
	WX_FUNC(union),
	WX_FUNC(xor),
	WX_FUNC(notequal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "clear", "notequal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Region, "region", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Region, val, region) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxRegionInitFunc()
{
	RegionUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&region_isequal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&region_notequal_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(Region, region, DeltaWxRegionInitFunc();, UEMPTY)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(region_construct, 0, 4, Nil)
	wxRegion *region = (wxRegion*) 0;
	if (n == 0)
		region = new wxRegion();
	else if (n == 4) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		region = new wxRegion(x, y, width, height);
	} else if (n == 1) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Rect, serial_no, rect, rect)) {
				region = new wxRegion(*rect);
			} else if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, bmp)) {
				region = new wxRegion(*bmp);
			}
		}
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(size_d)
			WX_GETTABLE(points)
			const size_t size = size_d;
			wxPoint *pts = DNEWARR(wxPoint, size);
			for (unsigned int i = 0; i < size; ++i) {
				DeltaValue value;
				points->Get(DeltaValue((DeltaNumberValueType)i), &value);
				if (value.Type() == DeltaValue_ExternId) {
					util_ui32 serial_no = (util_ui32)value.ToExternId();
					if (DLIB_WXISBASE(Point, serial_no, point, point)) {
						pts[i] = *point;
					}
				}
			}
			int fillStyle = wxODDEVEN_RULE;
			if (n >= 3) { WX_GETDEFINE_DEFINED(fillStyle) }
			region = new wxRegion(size, pts, (wxPolygonFillMode) fillStyle);
			DDELARR(pts);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Point, serial_no, point, topLeft)) {
				if (n == 2) {
					DLIB_WXGET_BASE(point, Point, bottomRight)
					region = new wxRegion(*topLeft, *bottomRight);
				}
			} else if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, bmp)) {
				DLIB_WXGET_BASE(colour, Colour, transColour)
				int tolerance = 0;
				if (n >= 3) { WX_GETNUMBER_DEFINED(tolerance) }
				region = new wxRegion(*bmp, *transColour, tolerance);
			}
		}
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Region, region)
}

WX_FUNC_START(region_clear, 1, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	region->Clear();
}

WX_FUNC_ARGRANGE_START(region_contains, 2, 5, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Point, serial_no, point, pt)) {
				WX_SETNUMBER(region->Contains(*pt))
			} else if (DLIB_WXISBASE(Rect, serial_no, rect, rect)) {
				WX_SETNUMBER(region->Contains(*rect))
			}
		}
	} else if (n == 3) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_SETNUMBER(region->Contains(x, y))
	} else if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		WX_SETNUMBER(region->Contains(x, y, width, height))
	}
}

WX_FUNC_START(region_converttobitmap, 1, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	WX_SETOBJECT(Bitmap, new wxBitmap(region->ConvertToBitmap()))
}

WX_FUNC_ARGRANGE_START(region_getbox, 1, 5, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	if (n == 1) {
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Rect, new wxRect(region->GetBox()))
	} else if (n == 5) {
		int x, y, width, height;
		region->GetBox(x, y, width, height);
		WX_GETTABLE(x_table)
		WX_SETTABLE_RETVAL(x_table, DeltaValue(DeltaNumberValueType(x)))
		WX_GETTABLE(y_table)
		WX_SETTABLE_RETVAL(y_table, DeltaValue(DeltaNumberValueType(y)))
		WX_GETTABLE(width_table)
		WX_SETTABLE_RETVAL(width_table, DeltaValue(DeltaNumberValueType(width)))
		WX_GETTABLE(height_table)
		WX_SETTABLE_RETVAL(height_table, DeltaValue(DeltaNumberValueType(height)))
	}
}

WX_FUNC_ARGRANGE_START(region_intersect, 2, 5, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Region, serial_no, region, reg)) {
				WX_SETBOOL(region->Intersect(*reg))
			} else if (DLIB_WXISBASE(Rect, serial_no, rect, rect)) {
				WX_SETBOOL(region->Intersect(*rect))
			}
		}
	} else if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		WX_SETBOOL(region->Contains(x, y, width, height))
	}
}

WX_FUNC_START(region_isempty, 1, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	WX_SETBOOL(region->IsEmpty())
}

WX_FUNC_START(region_isequal, 2, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	DLIB_WXGET_BASE(region, Region, reg)
	WX_SETBOOL(region->IsEqual(*reg))
}

WX_FUNC_START(region_subtract, 2, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Region, serial_no, region, reg)) {
			WX_SETBOOL(region->Subtract(*reg))
		} else if (DLIB_WXISBASE(Rect, serial_no, rect, rect)) {
			WX_SETBOOL(region->Subtract(*rect))
		}
	}
}

WX_FUNC_ARGRANGE_START(region_offset, 2, 3, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	if (n == 2) {
		DLIB_WXGETPOINT_BASE(pt)
		WX_SETBOOL(region->Offset(*pt))
	} else {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_SETBOOL(region->Offset(x, y))
	}
}

WX_FUNC_ARGRANGE_START(region_union, 2, 5, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		WX_SETBOOL(region->Union(x, y, width, height))
	} else if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Region, serial_no, region, reg)) {
				WX_SETBOOL(region->Union(*reg))
			} else if (DLIB_WXISBASE(Rect, serial_no, rect, rect)) {
				WX_SETBOOL(region->Union(*rect))
			} else if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, bmp)) {
				WX_SETBOOL(region->Union(*bmp))
			}
		}
	} else {
		DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
		DLIB_WXGET_BASE(colour, Colour, transColour)
		int tolerance = 0;
		if (n >= 4) { WX_GETNUMBER_DEFINED(tolerance) }
		WX_SETBOOL(region->Union(*bmp, *transColour, tolerance))
	}
}

WX_FUNC_ARGRANGE_START(region_xor, 2, 5, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	if (n == 5) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		WX_SETBOOL(region->Xor(x, y, width, height))
	} else if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Region, serial_no, region, reg)) {
				WX_SETBOOL(region->Xor(*reg))
			} else if (DLIB_WXISBASE(Rect, serial_no, rect, rect)) {
				WX_SETBOOL(region->Xor(*rect))
			}
		}
	}
}

WX_FUNC_START(region_notequal, 2, Nil)
	DLIB_WXGET_BASE(region, Region, region)
	DLIB_WXGET_BASE(region, Region, region2)
	WX_SETBOOL(region->operator!=(*region2))
}
