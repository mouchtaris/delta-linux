#include "DeltaWxImage.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxObject.h"
#include "DeltaWxPalette.h"
#include "DeltaWxRect.h"
#include "DeltaWxSize.h"
#include "DeltaWxPoint.h"
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
typedef int wxImageResizeQuality;
#endif

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(image, name)
#define WX_FUNC(name) WX_FUNC1(image, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(blur)
WX_FUNC_DEF(blurhorizontal)
WX_FUNC_DEF(blurvertical)
WX_FUNC_DEF(convertalphatomask)
WX_FUNC_DEF(converttogreyscale)
WX_FUNC_DEF(converttomono)
WX_FUNC_DEF(copy)
WX_FUNC_DEF(create)
WX_FUNC_DEF(destroy)
WX_FUNC_DEF(findfirstunusedcolour)
WX_FUNC_DEF(getalpha)
WX_FUNC_DEF(getblue)
WX_FUNC_DEF(getgreen)
WX_FUNC_DEF(getheight)
WX_FUNC_DEF(getmaskblue)
WX_FUNC_DEF(getmaskgreen)
WX_FUNC_DEF(getmaskred)
WX_FUNC_DEF(getorfindmaskcolour)
WX_FUNC_DEF(getpalette)
WX_FUNC_DEF(getred)
WX_FUNC_DEF(getsubimage)
WX_FUNC_DEF(getwidth)
WX_FUNC_DEF(hsvtorgb)
WX_FUNC_DEF(hasalpha)
WX_FUNC_DEF(hasmask)
WX_FUNC_DEF(getoption)
WX_FUNC_DEF(getoptionint)
WX_FUNC_DEF(hasoption)
WX_FUNC_DEF(initalpha)
WX_FUNC_DEF(istransparent)
WX_FUNC_DEF(loadfile)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(rgbtohsv)
WX_FUNC_DEF(mirror)
WX_FUNC_DEF(replace)
WX_FUNC_DEF(rescale)
WX_FUNC_DEF(resize)
WX_FUNC_DEF(rotate)
WX_FUNC_DEF(rotatehue)
WX_FUNC_DEF(rotate90)
WX_FUNC_DEF(savefile)
WX_FUNC_DEF(scale)
WX_FUNC_DEF(size)
WX_FUNC_DEF(setalpha)
WX_FUNC_DEF(setmask)
WX_FUNC_DEF(setmaskcolour)
WX_FUNC_DEF(setmaskfromimage)
WX_FUNC_DEF(setoption)
WX_FUNC_DEF(setpalette)
WX_FUNC_DEF(setrgb)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(hsvtorgb),
	WX_FUNC(rgbtohsv),
	WX_FUNC(blur),
	WX_FUNC(blurhorizontal),
	WX_FUNC(blurvertical),
	WX_FUNC(convertalphatomask),
	WX_FUNC(converttogreyscale),
	WX_FUNC(converttomono),
	WX_FUNC(copy),
	WX_FUNC(create),
	WX_FUNC(findfirstunusedcolour),
	WX_FUNC(getalpha),
	WX_FUNC(getblue),
	WX_FUNC(getgreen),
	WX_FUNC(getheight),
	WX_FUNC(getmaskblue),
	WX_FUNC(getmaskgreen),
	WX_FUNC(getmaskred),
	WX_FUNC(getorfindmaskcolour),
	WX_FUNC(getpalette),
	WX_FUNC(getred),
	WX_FUNC(getsubimage),
	WX_FUNC(getwidth),
	WX_FUNC(hasalpha),
	WX_FUNC(hasmask),
	WX_FUNC(getoption),
	WX_FUNC(getoptionint),
	WX_FUNC(hasoption),
	WX_FUNC(initalpha),
	WX_FUNC(istransparent),
	WX_FUNC(loadfile),
	WX_FUNC(isok),
	WX_FUNC(mirror),
	WX_FUNC(replace),
	WX_FUNC(rescale),
	WX_FUNC(resize),
	WX_FUNC(rotate),
	WX_FUNC(rotatehue),
	WX_FUNC(rotate90),
	WX_FUNC(savefile),
	WX_FUNC(scale),
	WX_FUNC(size),
	WX_FUNC(setalpha),
	WX_FUNC(setmask),
	WX_FUNC(setmaskcolour),
	WX_FUNC(setmaskfromimage),
	WX_FUNC(setoption),
	WX_FUNC(setpalette),
	WX_FUNC(setrgb)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(3, uarraysize(funcs) - 3, "blur", "setrgb")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Image, "image", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Image, val, image) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetHeight (void* val, DeltaValue* at) 
{
	wxImage *image = DLIB_WXTYPECAST_BASE(Image, val, image);
	WX_SETNUMBER_EX(*at, image->GetHeight())
	return true;
}

static bool GetMaskBlue (void* val, DeltaValue* at) 
{
	wxImage *image = DLIB_WXTYPECAST_BASE(Image, val, image);
	WX_SETNUMBER_EX(*at, image->GetMaskBlue())
	return true;
}

static bool GetMaskGreen (void* val, DeltaValue* at) 
{
	wxImage *image = DLIB_WXTYPECAST_BASE(Image, val, image);
	WX_SETNUMBER_EX(*at, image->GetMaskGreen())
	return true;
}

static bool GetMaskRed (void* val, DeltaValue* at) 
{
	wxImage *image = DLIB_WXTYPECAST_BASE(Image, val, image);
	WX_SETNUMBER_EX(*at, image->GetMaskRed())
	return true;
}

static bool GetPalette (void* val, DeltaValue* at) 
{
	wxImage *image = DLIB_WXTYPECAST_BASE(Image, val, image);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Palette, new wxPalette(image->GetPalette()))
	return true;
}

static bool GetWidth (void* val, DeltaValue* at) 
{
	wxImage *image = DLIB_WXTYPECAST_BASE(Image, val, image);
	WX_SETNUMBER_EX(*at, image->GetWidth())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "height",				&GetHeight,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maskBlue",			&GetMaskBlue,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maskGreen",			&GetMaskGreen,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maskRed",			&GetMaskRed,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "palette",			&GetPalette,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "width",				&GetWidth,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Image,image)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(image_construct, 0, 3, Nil)
	wxImage *image = (wxImage*) 0;
	if (n == 0) {
		image = new wxImage();
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETSTRING(name)
			if (n == 1)
				image = new wxImage(name);
			else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
				WX_GETDEFINE(type)
				int index = -1;
				if (n >= 3) { WX_GETNUMBER_DEFINED(index) }
				image = new wxImage(name, type, index);
			} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
				bool found;
				std::string arg = DPTR(vm)->GetActualArg(_argNo++)->ToString();
				int type = wxWidgets::DefineMapGet().Search(arg, &found);
				int index = -1;
				if (n >= 3) { WX_GETNUMBER_DEFINED(index) }
				if (found)
					image = new wxImage(name, type, index);
				else
					image = new wxImage(name, wxString(arg.c_str(), wxConvUTF8), index);
			}
		} else if (n >= 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(width)
			WX_GETNUMBER(height)
			bool clear = true;
			if (n >= 3) { WX_GETBOOL_DEFINED(clear) }
			image = new wxImage(width, height, clear);
		}
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, image)
}

WX_FUNC_START(image_blur, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(blurRadius)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->Blur(blurRadius)))
}

WX_FUNC_START(image_blurhorizontal, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(blurRadius)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->BlurHorizontal(blurRadius)))
}

WX_FUNC_START(image_blurvertical, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(blurRadius)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->BlurVertical(blurRadius)))
}

WX_FUNC_ARGRANGE_START(image_convertalphatomask, 1, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	int threshold = wxIMAGE_ALPHA_THRESHOLD;
	if (n >= 2) { WX_GETDEFINE_DEFINED(threshold) }
	WX_SETBOOL(image->ConvertAlphaToMask(threshold))
}

WX_FUNC_ARGRANGE_START(image_converttogreyscale, 1, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	double lr = 0.299, lg = 0.587, lb = 0.114;
	if (n >= 2) { WX_GETNUMBER_DEFINED(lr) }
	if (n >= 3) { WX_GETNUMBER_DEFINED(lg) }
	if (n >= 4) { WX_GETNUMBER_DEFINED(lb) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->ConvertToGreyscale(lr, lg, lb)))
}

WX_FUNC_START(image_converttomono, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(red)
	WX_GETNUMBER(green)
	WX_GETNUMBER(blue)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->ConvertToMono(red, green, blue)))
}

WX_FUNC_START(image_copy, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->Copy()))
}

WX_FUNC_ARGRANGE_START(image_create, 3, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(width)
	WX_GETNUMBER(height)
	bool clear = true;
	if (n >= 4) { WX_GETBOOL_DEFINED(clear) }
	WX_SETBOOL(image->Create(width, height, clear))
}

WX_FUNC_START(image_destroy, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	image->Destroy();
}

WX_FUNC_ARGRANGE_START(image_findfirstunusedcolour, 4, 7, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETTABLE(r_table)
	WX_GETTABLE(g_table)
	WX_GETTABLE(b_table)
	int startR = 1, startG = 0, startB = 0;
	unsigned char r = 0, g = 0, b = 0;
	if (n >= 5) { WX_GETNUMBER_DEFINED(startR) }
	if (n >= 6) { WX_GETNUMBER_DEFINED(startG) }
	if (n >= 7) { WX_GETNUMBER_DEFINED(startB) }
	WX_SETBOOL(image->FindFirstUnusedColour(&r, &g, &b, startR, startG, startB))
	WX_SETTABLE_RETVAL(r_table, DeltaValue((DeltaNumberValueType)r))
	WX_SETTABLE_RETVAL(g_table, DeltaValue((DeltaNumberValueType)g))
	WX_SETTABLE_RETVAL(b_table, DeltaValue((DeltaNumberValueType)b))
}

WX_FUNC_START(image_getalpha, 3, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	WX_SETNUMBER(image->GetAlpha(x, y))
}

WX_FUNC_START(image_getblue, 3, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	WX_SETNUMBER(image->GetBlue(x, y))
}

WX_FUNC_START(image_getgreen, 3, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	WX_SETNUMBER(image->GetGreen(x, y))
}

WX_FUNC_START(image_getheight, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETNUMBER(image->GetHeight())
}

WX_FUNC_START(image_getmaskblue, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETNUMBER(image->GetMaskBlue())
}

WX_FUNC_START(image_getmaskgreen, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETNUMBER(image->GetMaskGreen())
}

WX_FUNC_START(image_getmaskred, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETNUMBER(image->GetMaskRed())
}

WX_FUNC_START(image_getorfindmaskcolour, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETTABLE(r_table)
	WX_GETTABLE(g_table)
	WX_GETTABLE(b_table)
	unsigned char r = 0, g = 0, b = 0;
	WX_SETBOOL(image->GetOrFindMaskColour(&r, &g, &b))
	WX_SETTABLE_RETVAL(r_table, DeltaValue((DeltaNumberValueType)r))
	WX_SETTABLE_RETVAL(g_table, DeltaValue((DeltaNumberValueType)g))
	WX_SETTABLE_RETVAL(b_table, DeltaValue((DeltaNumberValueType)b))
}

WX_FUNC_START(image_getpalette, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Palette, new wxPalette(image->GetPalette()))
}

WX_FUNC_START(image_getred, 3, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	WX_SETNUMBER(image->GetRed(x, y))
}

WX_FUNC_START(image_getsubimage, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->GetSubImage(*rect)))
}

WX_FUNC_START(image_getwidth, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETNUMBER(image->GetWidth())
}

WX_FUNC_ARGRANGE_START(image_hsvtorgb, 1, 3, Nil)
	DLIB_RETVAL_REF.FromTable(DNEW(DELTA_OBJECT));
	double h = 0, s = 0, v = 0;
	if (n == 1 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Table) {
		WX_GETTABLE(hsv_table)
		DeltaValue h_value, s_value, v_value;
		hsv_table->Get(DeltaValue((DeltaNumberValueType)0), &h_value);
		hsv_table->Get(DeltaValue((DeltaNumberValueType)1), &s_value);
		hsv_table->Get(DeltaValue((DeltaNumberValueType)2), &v_value);
		if (h_value.Type() == DeltaValue_Number) h = h_value.ToNumber();
		if (s_value.Type() == DeltaValue_Number) s = s_value.ToNumber();
		if (v_value.Type() == DeltaValue_Number) v = v_value.ToNumber();
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		if (n >= 1) { WX_GETNUMBER_DEFINED(h) }
		if (n >= 2) { WX_GETNUMBER_DEFINED(s) }
		if (n >= 3) { WX_GETNUMBER_DEFINED(v) }
	}
	wxImage::RGBValue rgb = wxImage::HSVtoRGB(wxImage::HSVValue(h, s, v));
	DLIB_RETVAL_REF.ToTable()->Set(DeltaValue((DeltaNumberValueType)0), DeltaValue((DeltaNumberValueType)rgb.red));
	DLIB_RETVAL_REF.ToTable()->Set(DeltaValue((DeltaNumberValueType)1), DeltaValue((DeltaNumberValueType)rgb.green));
	DLIB_RETVAL_REF.ToTable()->Set(DeltaValue((DeltaNumberValueType)2), DeltaValue((DeltaNumberValueType)rgb.blue));
}

WX_FUNC_START(image_hasalpha, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETBOOL(image->HasAlpha())
}

WX_FUNC_START(image_hasmask, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETBOOL(image->HasMask())
}

WX_FUNC_START(image_getoption, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETSTRING(name)
	WX_SETSTRING(image->GetOption(name))
}

WX_FUNC_START(image_getoptionint, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETSTRING(name)
	WX_SETNUMBER(image->GetOptionInt(name))
}

WX_FUNC_START(image_hasoption, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETSTRING(name)
	WX_SETBOOL(image->HasOption(name))
}

WX_FUNC_START(image_initalpha, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	image->InitAlpha();
}

WX_FUNC_ARGRANGE_START(image_istransparent, 3, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	unsigned char threshold = 128;
	if (n >= 4) { WX_GETNUMBER(num) threshold = num; }
	WX_SETBOOL(image->IsTransparent(x, y, threshold))
}

WX_FUNC_ARGRANGE_START(image_loadfile, 2, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETSTRING(name)
	if (n == 2)
		WX_SETBOOL(image->LoadFile(name))
	else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETDEFINE(type)
		int index = -1;
		if (n >= 4) { WX_GETNUMBER_DEFINED(index) }
		WX_SETBOOL(image->LoadFile(name, type, index))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		bool found;
		std::string arg = DPTR(vm)->GetActualArg(_argNo++)->ToString();
		int type = wxWidgets::DefineMapGet().Search(arg, &found);
		int index = -1;
		if (n >= 4) { WX_GETNUMBER_DEFINED(index) }
		if (found)
			WX_SETBOOL(image->LoadFile(name, type, index))
		else
			WX_SETBOOL(image->LoadFile(name, wxString(arg.c_str(), wxConvUTF8), index))
	}
}

WX_FUNC_START(image_isok, 1, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_SETBOOL(image->IsOk())
}

WX_FUNC_ARGRANGE_START(image_rgbtohsv, 1, 3, Nil)
	DLIB_RETVAL_REF.FromTable(DNEW(DELTA_OBJECT));
	unsigned char r = 0, g = 0, b = 0;
	if (n == 1 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Table) {
		WX_GETTABLE(rgb_table)
		DeltaValue r_value, g_value, b_value;
		rgb_table->Get(DeltaValue((DeltaNumberValueType)0), &r_value);
		rgb_table->Get(DeltaValue((DeltaNumberValueType)1), &g_value);
		rgb_table->Get(DeltaValue((DeltaNumberValueType)2), &b_value);
		if (r_value.Type() == DeltaValue_Number) r = r_value.ToNumber();
		if (g_value.Type() == DeltaValue_Number) g = g_value.ToNumber();
		if (b_value.Type() == DeltaValue_Number) b = b_value.ToNumber();
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		if (n >= 1) { WX_GETNUMBER_DEFINED(r) }
		if (n >= 2) { WX_GETNUMBER_DEFINED(g) }
		if (n >= 3) { WX_GETNUMBER_DEFINED(b) }
	}
	wxImage::HSVValue hsv = wxImage::RGBtoHSV(wxImage::RGBValue(r, g, b));
	DLIB_RETVAL_REF.ToTable()->Set(DeltaValue((DeltaNumberValueType)0), DeltaValue((DeltaNumberValueType)hsv.hue));
	DLIB_RETVAL_REF.ToTable()->Set(DeltaValue((DeltaNumberValueType)1), DeltaValue((DeltaNumberValueType)hsv.saturation));
	DLIB_RETVAL_REF.ToTable()->Set(DeltaValue((DeltaNumberValueType)2), DeltaValue((DeltaNumberValueType)hsv.value));
}

WX_FUNC_ARGRANGE_START(image_mirror, 1, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	bool horizontally = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(horizontally) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->Mirror(horizontally)))
}

WX_FUNC_START(image_replace, 7, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(r1)
	WX_GETNUMBER(g1)
	WX_GETNUMBER(b1)
	WX_GETNUMBER(r2)
	WX_GETNUMBER(g2)
	WX_GETNUMBER(b2)
	image->Replace(r1, g1, b1, r2, g2, b2);
}

WX_FUNC_ARGRANGE_START(image_rescale, 3, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(width)
	WX_GETNUMBER(height)
	int quality = wxIMAGE_QUALITY_NORMAL;
	if (n >= 4) { WX_GETDEFINE_DEFINED(quality) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->Rescale(width, height, (wxImageResizeQuality) quality)))
}

WX_FUNC_ARGRANGE_START(image_resize, 3, 6, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	DLIB_WXGETSIZE_BASE(size)
	DLIB_WXGETPOINT_BASE(pos)
	int r = -1, g = -1, b = -1;
	if (n >= 4) { WX_GETNUMBER_DEFINED(r) }
	if (n >= 5) { WX_GETNUMBER_DEFINED(g) }
	if (n >= 6) { WX_GETNUMBER_DEFINED(b) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->Resize(*size, *pos, r, g, b)))
}

WX_FUNC_ARGRANGE_START(image_rotate, 3, 5, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(angle)
	DLIB_WXGETPOINT_BASE(rotationCentre)
	bool interpolating = true;
	wxPoint offsetAfterRotation;
	if (n >= 4) { WX_GETBOOL_DEFINED(interpolating) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->Rotate(angle, *rotationCentre, interpolating, &offsetAfterRotation)))
	if (n >= 5) {
		DeltaValue offset;
		WX_GETTABLE(offset_table)
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE_EX(offset, Point, new wxPoint(offsetAfterRotation))
		WX_SETTABLE_RETVAL(offset_table, offset)
	}
}

WX_FUNC_START(image_rotatehue, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(angle)
	image->RotateHue(angle);
}

WX_FUNC_ARGRANGE_START(image_rotate90, 1, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	bool clockwise = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(clockwise) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->Rotate90(clockwise)))
}

WX_FUNC_ARGRANGE_START(image_savefile, 2, 3, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETSTRING(name)
	if (n == 2)
		WX_SETBOOL(image->SaveFile(name))
	else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETDEFINE(type)
		WX_SETBOOL(image->SaveFile(name, type))
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		bool found;
		std::string arg = DPTR(vm)->GetActualArg(_argNo++)->ToString();
		int type = wxWidgets::DefineMapGet().Search(arg, &found);
		if (found)
			WX_SETBOOL(image->SaveFile(name, type))
		else
			WX_SETBOOL(image->SaveFile(name, wxString(arg.c_str(), wxConvUTF8)))
	}
}

WX_FUNC_ARGRANGE_START(image_scale, 3, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(width)
	WX_GETNUMBER(height)
	int quality = wxIMAGE_QUALITY_NORMAL;
	if (n >= 4) { WX_GETDEFINE_DEFINED(quality) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->Scale(width, height, (wxImageResizeQuality) quality)))
}

WX_FUNC_ARGRANGE_START(image_size, 3, 6, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	DLIB_WXGETSIZE_BASE(size)
	DLIB_WXGETPOINT_BASE(pos)
	int r = -1, g = -1, b = -1;
	if (n >= 4) { WX_GETNUMBER_DEFINED(r) }
	if (n >= 5) { WX_GETNUMBER_DEFINED(g) }
	if (n >= 6) { WX_GETNUMBER_DEFINED(b) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(image->Size(*size, *pos, r, g, b)))
}

WX_FUNC_START(image_setalpha, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	WX_GETNUMBER(alpha)
	image->SetAlpha(x, y, alpha);
}

WX_FUNC_ARGRANGE_START(image_setmask, 1, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	bool hasMask = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(hasMask) }
	image->SetMask(hasMask);
}

WX_FUNC_START(image_setmaskcolour, 4, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETNUMBER(red)
	WX_GETNUMBER(green)
	WX_GETNUMBER(blue)
	image->SetMaskColour(red, green, blue);
}

WX_FUNC_START(image_setmaskfromimage, 5, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	DLIB_WXGET_BASE(image, Image, mask)
	WX_GETNUMBER(mr)
	WX_GETNUMBER(mg)
	WX_GETNUMBER(mb)
	WX_SETBOOL(image->SetMaskFromImage(*mask, mr, mg, mb))
}

WX_FUNC_START(image_setoption, 3, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	WX_GETSTRING(name)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETDEFINE(value)
		image->SetOption(name, value);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		bool found;
		std::string arg = DPTR(vm)->GetActualArg(_argNo++)->ToString();
		int type = wxWidgets::DefineMapGet().Search(arg, &found);
		if (found)
			image->SetOption(name, type);
		else
			image->SetOption(name, wxString(arg.c_str(), wxConvUTF8));
	}
}

WX_FUNC_START(image_setpalette, 2, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	DLIB_WXGET_BASE(palette, Palette, palette)
	image->SetPalette(*palette);
}

WX_FUNC_ARGRANGE_START(image_setrgb, 5, 6, Nil)
	DLIB_WXGET_BASE(image, Image, image)
	if (n == 5) {
		DLIB_WXGET_BASE(rect, Rect, rect)
		WX_GETNUMBER(red)
		WX_GETNUMBER(green)
		WX_GETNUMBER(blue)
		image->SetRGB(*rect, red, green, blue);
	} else if (n == 6) {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		WX_GETNUMBER(red)
		WX_GETNUMBER(green)
		WX_GETNUMBER(blue)
		image->SetRGB(x, y, red, green, blue);
	}
}
