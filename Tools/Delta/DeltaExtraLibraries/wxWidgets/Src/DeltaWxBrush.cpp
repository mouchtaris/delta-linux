#include "DeltaWxBrush.h"
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

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(brush, name)
#define WX_FUNC(name) WX_FUNC1(brush, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getcolour)
WX_FUNC_DEF(getstipple)
WX_FUNC_DEF(getstyle)
WX_FUNC_DEF(ishatch)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(setcolour)
WX_FUNC_DEF(setstipple)
WX_FUNC_DEF(setstyle)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getcolour),
	WX_FUNC(getstipple),
	WX_FUNC(getstyle),
	WX_FUNC(ishatch),
	WX_FUNC(isok),
	WX_FUNC(setcolour),
	WX_FUNC(setstipple),
	WX_FUNC(setstyle),
	WX_FUNC(equal),
	WX_FUNC(notequal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getcolour", "notequal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Brush, "brush", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Brush, val, brush) ?
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

void DeltaWxBrushInitFunc()
{
	BrushUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&brush_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&brush_notequal_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(Brush, brush, DeltaWxBrushInitFunc();, )

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(brush_construct, 0, 2, Nil)
	wxBrush *brush = (wxBrush*) 0;
	if (n == 0) {
		brush = new wxBrush();
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Colour, serial_no, colour, colour)) {
				int style = wxSOLID;
				if (n >= 2) { WX_GETDEFINE_DEFINED(style) }
				brush = new wxBrush(*colour, style);
			} else
			if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, stippleBitmap)) {
				brush = new wxBrush(*stippleBitmap);
			}
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String && n == 2) {
			WX_GETSTRING(colourName)
			WX_GETDEFINE(style)
			brush = new wxBrush(colourName, style);
		}
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Brush, brush)
}

WX_FUNC_START(brush_getcolour, 1, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(brush->GetColour()))
}

WX_FUNC_START(brush_getstipple, 1, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	WX_SETOBJECT(Bitmap, brush->GetStipple())
}

WX_FUNC_START(brush_getstyle, 1, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	WX_SETNUMBER(brush->GetStyle())
}

WX_FUNC_START(brush_ishatch, 1, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	WX_SETBOOL(brush->IsHatch())
}

WX_FUNC_START(brush_isok, 1, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	WX_SETBOOL(brush->IsOk())
}

WX_FUNC_ARGRANGE_START(brush_setcolour, 2, 4, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	if (n == 2) {
		DLIB_WXGET_BASE(colour, Colour, colour)
		brush->SetColour(*colour);
	} else if (n == 4) {
		WX_GETNUMBER(red)
		WX_GETNUMBER(green)
		WX_GETNUMBER(blue)
		brush->SetColour((unsigned char)red, (unsigned char)green, (unsigned char)blue);
	}
}

WX_FUNC_START(brush_setstipple, 2, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	brush->SetStipple(*bitmap);
}

WX_FUNC_START(brush_setstyle, 2, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	WX_GETDEFINE(style)
	brush->SetStyle(style);
}

WX_FUNC_START(brush_equal, 2, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	DLIB_WXGET_BASE(brush, Brush, brush2)
	WX_SETBOOL(brush->operator==(*brush2))
}

WX_FUNC_START(brush_notequal, 2, Nil)
	DLIB_WXGET_BASE(brush, Brush, brush)
	DLIB_WXGET_BASE(brush, Brush, brush2)
	WX_SETBOOL(brush->operator!=(*brush2))
}
