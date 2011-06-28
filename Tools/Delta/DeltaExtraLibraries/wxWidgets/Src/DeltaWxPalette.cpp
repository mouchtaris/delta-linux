#include "DeltaWxPalette.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(palette, name)
#define WX_FUNC(name) WX_FUNC1(palette, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getcolourscount)
WX_FUNC_DEF(getpixel)
WX_FUNC_DEF(getrgb)
WX_FUNC_DEF(isok)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getcolourscount),
	WX_FUNC(getpixel),
	WX_FUNC(getrgb),
	WX_FUNC(isok)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "isok")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Palette, "palette", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Palette, val, palette) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Palette, palette);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(palette_construct, 0, 4, Nil)
	wxPalette *wxpalette = (wxPalette*) 0;
	DeltaWxPalette *palette = (DeltaWxPalette*) 0;
	if (n == 0) {
		wxpalette = new wxPalette();
	} else if (n == 4) {
		WX_GETNUMBER(num)
		WX_GETTABLE(red_table)
		WX_GETTABLE(green_table)
		WX_GETTABLE(blue_table)
		unsigned char *red = DNEWARR(unsigned char, num);
		unsigned char *green = DNEWARR(unsigned char, num);
		unsigned char *blue = DNEWARR(unsigned char, num);
		for (int i = 0; i < num; ++i) {
			DeltaValue r_value, g_value, b_value;
			red_table->Get(DeltaValue((DeltaNumberValueType)i), &r_value);
			green_table->Get(DeltaValue((DeltaNumberValueType)i), &g_value);
			blue_table->Get(DeltaValue((DeltaNumberValueType)i), &b_value);
			if (r_value.Type() == DeltaValue_Number) { red[i] = (unsigned char)r_value.ToNumber(); }
			if (g_value.Type() == DeltaValue_Number) { green[i] = (unsigned char)g_value.ToNumber(); }
			if (b_value.Type() == DeltaValue_Number) { blue[i] = (unsigned char)b_value.ToNumber(); }
		}
		wxpalette = new wxPalette(num, red, green, blue);
		DDELARR(red);
		DDELARR(green);
		DDELARR(blue);
	}
	if (wxpalette) palette = DNEWCLASS(DeltaWxPalette, (wxpalette));
	WX_SETOBJECT(Palette, palette)
}

DLIB_FUNC_START(palette_destruct, 1, Nil)
	DLIB_WXDELETE(palette, Palette, palette)
}

DLIB_FUNC_START(palette_create, 5, Nil)
	DLIB_WXGET_BASE(palette, Palette, palette)
	WX_GETNUMBER(num)
	WX_GETTABLE(red_table)
	WX_GETTABLE(green_table)
	WX_GETTABLE(blue_table)
	unsigned char *red = DNEWARR(unsigned char, num);
	unsigned char *green = DNEWARR(unsigned char, num);
	unsigned char *blue = DNEWARR(unsigned char, num);
	for (int i = 0; i < num; ++i) {
		DeltaValue r_value, g_value, b_value;
		red_table->Get(DeltaValue((DeltaNumberValueType)i), &r_value);
		green_table->Get(DeltaValue((DeltaNumberValueType)i), &g_value);
		blue_table->Get(DeltaValue((DeltaNumberValueType)i), &b_value);
		if (r_value.Type() == DeltaValue_Number) { red[i] = (unsigned char)r_value.ToNumber(); }
		if (g_value.Type() == DeltaValue_Number) { green[i] = (unsigned char)g_value.ToNumber(); }
		if (b_value.Type() == DeltaValue_Number) { blue[i] = (unsigned char)b_value.ToNumber(); }
	}
	WX_SETBOOL(palette->Create(num, red, green, blue))
	DDELARR(red);
	DDELARR(green);
	DDELARR(blue);
}

DLIB_FUNC_START(palette_getcolourscount, 1, Nil)
	DLIB_WXGET_BASE(palette, Palette, palette)
	WX_SETNUMBER(palette->GetColoursCount())
}

DLIB_FUNC_START(palette_getpixel, 4, Nil)
	DLIB_WXGET_BASE(palette, Palette, palette)
	WX_GETNUMBER(red)
	WX_GETNUMBER(green)
	WX_GETNUMBER(blue)
	WX_SETNUMBER(palette->GetPixel(red, green, blue))
}

DLIB_FUNC_START(palette_getrgb, 5, Nil)
	DLIB_WXGET_BASE(palette, Palette, palette)
	WX_GETNUMBER(pixel)
	WX_GETTABLE(red_table)
	WX_GETTABLE(green_table)
	WX_GETTABLE(blue_table)
	unsigned char red, green, blue;
	WX_SETBOOL(palette->GetRGB(pixel, &red, &green, &blue))
	WX_SETTABLE_RETVAL(red_table, DeltaValue((DeltaNumberValueType)red))
	WX_SETTABLE_RETVAL(green_table, DeltaValue((DeltaNumberValueType)green))
	WX_SETTABLE_RETVAL(blue_table, DeltaValue((DeltaNumberValueType)blue))
}

DLIB_FUNC_START(palette_isok, 1, Nil)
	DLIB_WXGET_BASE(palette, Palette, palette)
	WX_SETBOOL(palette->IsOk())
}
