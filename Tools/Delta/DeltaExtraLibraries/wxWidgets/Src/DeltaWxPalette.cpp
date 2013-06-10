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
WX_FUNC_DEF(create)
WX_FUNC_DEF(getcolourscount)
WX_FUNC_DEF(getpixel)
WX_FUNC_DEF(getrgb)
WX_FUNC_DEF(isok)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getcolourscount),
	WX_FUNC(getpixel),
	WX_FUNC(getrgb),
	WX_FUNC(isok)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "isok")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Palette, "palette", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Palette, val, palette) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Palette,palette)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(palette_construct, 0, 4, Nil)
	wxPalette *palette = (wxPalette*) 0;
	if (n == 0) {
		palette = new wxPalette();
	} else if (n == 4) {
		WX_GETNUMBER(num_d)
		WX_GETTABLE(red_table)
		WX_GETTABLE(green_table)
		WX_GETTABLE(blue_table)
		const size_t num = num_d;
		unsigned char *red = DNEWARR(unsigned char, num);
		unsigned char *green = DNEWARR(unsigned char, num);
		unsigned char *blue = DNEWARR(unsigned char, num);
		for (unsigned int i = 0; i < num; ++i) {
			DeltaValue r_value, g_value, b_value;
			red_table->Get(DeltaValue((DeltaNumberValueType)i), &r_value);
			green_table->Get(DeltaValue((DeltaNumberValueType)i), &g_value);
			blue_table->Get(DeltaValue((DeltaNumberValueType)i), &b_value);
			if (r_value.Type() == DeltaValue_Number) { red[i] = (unsigned char)r_value.ToNumber(); }
			if (g_value.Type() == DeltaValue_Number) { green[i] = (unsigned char)g_value.ToNumber(); }
			if (b_value.Type() == DeltaValue_Number) { blue[i] = (unsigned char)b_value.ToNumber(); }
		}
		palette = new wxPalette(num, red, green, blue);
		DDELARR(red);
		DDELARR(green);
		DDELARR(blue);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Palette, palette)
}

WX_FUNC_START(palette_create, 5, Nil)
	DLIB_WXGET_BASE(palette, Palette, palette)
	WX_GETNUMBER(num_d)
	WX_GETTABLE(red_table)
	WX_GETTABLE(green_table)
	WX_GETTABLE(blue_table)
	const size_t num = num_d;
	unsigned char *red = DNEWARR(unsigned char, num);
	unsigned char *green = DNEWARR(unsigned char, num);
	unsigned char *blue = DNEWARR(unsigned char, num);
	for (unsigned int i = 0; i < num; ++i) {
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

WX_FUNC_START(palette_getcolourscount, 1, Nil)
	DLIB_WXGET_BASE(palette, Palette, palette)
	WX_SETNUMBER(palette->GetColoursCount())
}

WX_FUNC_START(palette_getpixel, 4, Nil)
	DLIB_WXGET_BASE(palette, Palette, palette)
	WX_GETNUMBER(red)
	WX_GETNUMBER(green)
	WX_GETNUMBER(blue)
	WX_SETNUMBER(palette->GetPixel(red, green, blue))
}

WX_FUNC_START(palette_getrgb, 5, Nil)
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

WX_FUNC_START(palette_isok, 1, Nil)
	DLIB_WXGET_BASE(palette, Palette, palette)
	WX_SETBOOL(palette->IsOk())
}
