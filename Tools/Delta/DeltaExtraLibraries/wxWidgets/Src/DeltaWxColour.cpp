#include "DeltaWxColour.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(colour, name)
#define WX_FUNC(name) WX_FUNC1(colour, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(alpha)
WX_FUNC_DEF(blue)
WX_FUNC_DEF(getasstring)
WX_FUNC_DEF(getpixel)
WX_FUNC_DEF(green)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(red)
WX_FUNC_DEF(set)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(alpha),
	WX_FUNC(blue),
	WX_FUNC(getasstring),
	WX_FUNC(getpixel),
	WX_FUNC(green),
	WX_FUNC(isok),
	WX_FUNC(red),
	WX_FUNC(set),
	WX_FUNC(equal),
	WX_FUNC(notequal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "notequal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Colour, "colour", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Colour, val, colour) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetIsInit (void* val, DeltaValue* at) 
{
	wxColour *col = DLIB_WXTYPECAST_BASE(Colour, val, colour);
	at->FromBool(col->IsOk());
	return true;
}

static bool GetRed (void* val, DeltaValue* at) 
{
	wxColour *col = DLIB_WXTYPECAST_BASE(Colour, val, colour);
	at->FromNumber(col->Red());
	return true;
}

static bool GetGreen (void* val, DeltaValue* at) 
{
	wxColour *col = DLIB_WXTYPECAST_BASE(Colour, val, colour);
	at->FromNumber(col->Green());
	return true;
}

static bool GetBlue (void* val, DeltaValue* at) 
{
	wxColour *col = DLIB_WXTYPECAST_BASE(Colour, val, colour);
	at->FromNumber(col->Blue());
	return true;
}

static bool GetAlpha (void* val, DeltaValue* at) 
{
	wxColour *col = DLIB_WXTYPECAST_BASE(Colour, val, colour);
	at->FromNumber(col->Alpha());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isInit",				&GetIsInit,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "red",				&GetRed,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "green",				&GetGreen,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "blue",				&GetBlue,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "alpha",				&GetAlpha,				DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxColourInitFunc()
{
	ColourUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&colour_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&colour_notequal_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(Colour, colour, DeltaWxColourInitFunc();, );

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(colour_construct, 0, 4, Nil)
	wxColour *wxcolour = (wxColour*) 0;
	DeltaWxColour *colour = (DeltaWxColour*) 0;
	if (n == 0)
		wxcolour = new wxColour();
	else if (n == 1) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Colour, serial_no, colour, colour_wr)) {
				wxColour *colour = (wxColour*) colour_wr->GetCastToNativeInstance();
				wxcolour = new wxColour(*colour);
			}
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETSTRING(colourNname)
			wxcolour = new wxColour(colourNname);
		}
	} else if (n >= 3) {
		WX_GETNUMBER(red)
		WX_GETNUMBER(green)
		WX_GETNUMBER(blue)
		if (n == 3)
			wxcolour = new wxColour(red, green, blue);
		else {
			WX_GETDEFINE(alpha)
			wxcolour = new wxColour(red, green, blue, alpha);
		}
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxcolour) colour = DNEWCLASS(DeltaWxColour, (wxcolour));
	WX_SETOBJECT(Colour, colour)
}

DLIB_FUNC_START(colour_destruct, 1, Nil)
	DLIB_WXDELETE(colour, Colour, colour)
}

DLIB_FUNC_START(colour_alpha, 1, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	WX_SETNUMBER(colour->Alpha());
}

DLIB_FUNC_START(colour_blue, 1, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	WX_SETNUMBER(colour->Blue());
}

DLIB_FUNC_START(colour_getasstring, 2, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	WX_GETDEFINE(flags)
	WX_SETSTRING(colour->GetAsString(flags))
}

DLIB_FUNC_START(colour_getpixel, 1, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	WX_SETNUMBER(colour->GetPixel());
}

DLIB_FUNC_START(colour_green, 1, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	WX_SETNUMBER(colour->Green());
}

DLIB_FUNC_START(colour_isok, 1, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	WX_SETBOOL(colour->IsOk());
}

DLIB_FUNC_START(colour_red, 1, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	WX_SETNUMBER(colour->Red());
}

WX_FUNC_ARGRANGE_START(colour_set, 2, 5, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	if (n == 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(RGB)
			colour->Set(RGB);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETSTRING(str)
			WX_SETBOOL(colour->Set(str));
		}
	} else if (n >= 4) {
		WX_GETNUMBER(red)
		WX_GETNUMBER(green)
		WX_GETNUMBER(blue)
		if (n == 4) {
			colour->Set(red, green, blue);
		} else {
			WX_GETDEFINE(alpha)
			colour->Set(red, green, blue, alpha);
		}
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
}

DLIB_FUNC_START(colour_equal, 2, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	DLIB_WXGET_BASE(colour, Colour, colour2)
	WX_SETBOOL(colour->operator==(*colour2));
}

DLIB_FUNC_START(colour_notequal, 2, Nil)
	DLIB_WXGET_BASE(colour, Colour, colour)
	DLIB_WXGET_BASE(colour, Colour, colour2)
	WX_SETBOOL(colour->operator!=(*colour2));
}
