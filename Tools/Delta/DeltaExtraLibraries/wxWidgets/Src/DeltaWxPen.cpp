#include "DeltaWxPen.h"
#include "DeltaWxObject.h"
#include "DeltaWxColour.h"
#include "DeltaWxBitmap.h"
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
typedef int wxPenCap;
typedef int wxPenJoin;
#endif

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(pen, name)
#define WX_FUNC(name) WX_FUNC1(pen, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getcap)
WX_FUNC_DEF(getcolour)
WX_FUNC_DEF(getdashes)
WX_FUNC_DEF(getjoin)
WX_FUNC_DEF(getstipple)
WX_FUNC_DEF(getstyle)
WX_FUNC_DEF(getwidth)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(setcap)
WX_FUNC_DEF(setcolour)
WX_FUNC_DEF(setdashes)
WX_FUNC_DEF(setjoin)
WX_FUNC_DEF(setstipple)
WX_FUNC_DEF(setstyle)
WX_FUNC_DEF(setwidth)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getcap),
	WX_FUNC(getcolour),
	WX_FUNC(getdashes),
	WX_FUNC(getjoin),
	WX_FUNC(getstipple),
	WX_FUNC(getstyle),
	WX_FUNC(getwidth),
	WX_FUNC(isok),
	WX_FUNC(setcap),
	WX_FUNC(setcolour),
	WX_FUNC(setdashes),
	WX_FUNC(setjoin),
	WX_FUNC(setstipple),
	WX_FUNC(setstyle),
	WX_FUNC(setwidth),
	WX_FUNC(equal),
	WX_FUNC(notequal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getcap", "notequal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Pen, "pen", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Pen, val, pen) ?
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

void DeltaWxPenInitFunc()
{
	PenUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&pen_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&pen_notequal_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(Pen, pen, DeltaWxPenInitFunc();, )

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(pen_construct, 0, 3, Nil)
	wxPen *pen = (wxPen*) 0;
	if (n == 0) {
		pen = new wxPen();
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Colour, serial_no, colour, colour)) {
				int width = 1, style = wxSOLID;
				if (n >= 2) { WX_GETNUMBER_DEFINED(width) }
				if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
				pen = new wxPen(*colour, width, style);
			} else if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, bitmap)) {
#if defined (__WXMSW__)
				if (n >= 2) {
					WX_GETNUMBER(width)
					pen = new wxPen(*bitmap, width);
				}
#else
				DLIB_ERROR_CHECK(
					true,
					"This function overload is only available on MS Windows implementation of wxWidgets."
				);
#endif //__WXMSW__
			}
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String && n == 3) {
			WX_GETSTRING(colourName)
			WX_GETDEFINE(width)
			WX_GETDEFINE(style)
			pen = new wxPen(colourName, width, style);
		}
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Pen, pen)
}

WX_FUNC_START(pen_getcap, 1, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_SETNUMBER(pen->GetCap())
}

WX_FUNC_START(pen_getcolour, 1, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(pen->GetColour()))
}

WX_FUNC_START(pen_getdashes, 2, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_GETTABLE(dashes_table)
	wxDash *dashes;
	WX_SETNUMBER(pen->GetDashes(&dashes))
	for(int i = 0, num = pen->GetDashCount(); i < num; ++i) {
		dashes_table->Set(DeltaValue(DeltaNumberValueType(i)), DeltaValue(DeltaNumberValueType((double)dashes[i])));
	}
}

WX_FUNC_START(pen_getjoin, 1, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_SETNUMBER(pen->GetJoin())
}

WX_FUNC_START(pen_getstipple, 1, Nil)
#if defined(__WXMSW__)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_SETOBJECT(Bitmap, pen->GetStipple())
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(pen_getstyle, 1, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_SETNUMBER(pen->GetStyle())
}

WX_FUNC_START(pen_getwidth, 1, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_SETNUMBER(pen->GetWidth())
}

WX_FUNC_START(pen_isok, 1, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_SETBOOL(pen->IsOk())
}

WX_FUNC_START(pen_setcap, 2, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_GETDEFINE(capStyle)
	pen->SetCap( (wxPenCap) capStyle);
}

WX_FUNC_ARGRANGE_START(pen_setcolour, 2, 4, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	if (n == 2) {
		DLIB_WXGET_BASE(colour, Colour, colour)
		pen->SetColour(*colour);
	} else if (n == 4) {
		WX_GETNUMBER(red)
		WX_GETNUMBER(green)
		WX_GETNUMBER(blue)
		pen->SetColour(red, green, blue);
	}
}

WX_FUNC_START(pen_setdashes, 2, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_GETTABLE(dashes_table)
	int num = dashes_table->Total();
	wxDash *dashes = (wxDash*) malloc(num*sizeof(wxDash));
	for (int i = 0; i < num; ++i) {
		DeltaValue value;
		dashes_table->Get(DeltaValue(DeltaNumberValueType(i)), &value);
		if (value.Type() == DeltaValue_Number) {
			dashes[i] = (wxDash)value.ToNumber();
		}
	}
	pen->SetDashes(num, dashes);
}

WX_FUNC_START(pen_setjoin, 2, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_GETDEFINE(join_style)
	pen->SetJoin((wxPenJoin) join_style);
}

WX_FUNC_START(pen_setstipple, 2, Nil)
#if defined(__WXMSW__)
	DLIB_WXGET_BASE(pen, Pen, pen)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	pen->SetStipple(*bitmap);
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(pen_setstyle, 2, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_GETDEFINE(style)
	pen->SetStyle(style);
}

WX_FUNC_START(pen_setwidth, 2, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	WX_GETNUMBER(width)
	pen->SetWidth(width);
}

WX_FUNC_START(pen_equal, 2, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	DLIB_WXGET_BASE(pen, Pen, pen2)
	WX_SETBOOL(pen->operator==(*pen2))
}

WX_FUNC_START(pen_notequal, 2, Nil)
	DLIB_WXGET_BASE(pen, Pen, pen)
	DLIB_WXGET_BASE(pen, Pen, pen2)
	WX_SETBOOL(pen->operator!=(*pen2))
}
