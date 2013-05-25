#include "DeltaWxBitmap.h"
#include "DeltaWxObject.h"
#include "DeltaWxDC.h"
#include "DeltaWxPalette.h"
#include "DeltaWxRect.h"
#include "DeltaWxIcon.h"
#include "DeltaWxMask.h"
#include "DeltaWxImage.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(bitmap, name)
#define WX_FUNC(name) WX_FUNC1(bitmap, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(converttoimage)
WX_FUNC_DEF(copyfromicon)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getdepth)
WX_FUNC_DEF(getheight)
WX_FUNC_DEF(getpalette)
WX_FUNC_DEF(getmask)
WX_FUNC_DEF(getwidth)
WX_FUNC_DEF(getsubbitmap)
WX_FUNC_DEF(loadfile)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(savefile)
WX_FUNC_DEF(setdepth)
WX_FUNC_DEF(setheight)
WX_FUNC_DEF(setmask)
WX_FUNC_DEF(setpalette)
WX_FUNC_DEF(setwidth)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(converttoimage),
	WX_FUNC(copyfromicon),
	WX_FUNC(create),
	WX_FUNC(getdepth),
	WX_FUNC(getheight),
	WX_FUNC(getpalette),
	WX_FUNC(getmask),
	WX_FUNC(getwidth),
	WX_FUNC(getsubbitmap),
	WX_FUNC(loadfile),
	WX_FUNC(isok),
	WX_FUNC(savefile),
	WX_FUNC(setdepth),
	WX_FUNC(setheight),
	WX_FUNC(setmask),
	WX_FUNC(setpalette),
	WX_FUNC(setwidth)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "converttoimage", "setwidth")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Bitmap, "bitmap", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Bitmap, val, bitmap) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Bitmap,bitmap)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(bitmap_construct, 0, 3, Nil)
	wxBitmap *bitmap = (wxBitmap*) 0;
	if (n == 0) {
		bitmap = new wxBitmap();
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETSTRING(name)
#if defined (__WXMSW__)
			long type = wxBITMAP_TYPE_BMP_RESOURCE;
			if (n >= 2) { WX_GETDEFINE_DEFINED(type) }
#else if defined (__WXGTK__) || defined (__WXGET20__)
			wxBitmapType type = wxBITMAP_TYPE_XPM;
			if (n >= 2) { WX_GETDEFINE(def) type = (wxBitmapType) def; }
#endif
			bitmap = new wxBitmap(name, (wxBitmapType) type);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Image, serial_no, image, image)) {
				int depth = -1;
				if (n >= 2) { WX_GETNUMBER_DEFINED(depth) }
				bitmap = new wxBitmap(*image, depth);
			} else
			if (DLIB_WXISBASE(Icon, serial_no, icon, icon)) {
				bitmap = new wxBitmap(*icon);
			}
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(width)
			WX_GETNUMBER(height)
			if (n >= 3 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
#if defined(__WXMSW__)
				DLIB_WXGET_BASE(dc, DC, dc)
				bitmap = new wxBitmap(width, height, *dc);
#else
				DLIB_ERROR_CHECK(
					true,
					"This function overload is only available on MS Windows implementation of wxWidgets."
				);
#endif //__WXMSW__
			} else {
				int depth = -1;
				if (n >= 3 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
					WX_GETNUMBER_DEFINED(depth)
				}
				bitmap = new wxBitmap(width, height, depth);
			}
		}
	}
	WX_SETOBJECT(Bitmap, bitmap)
}

WX_FUNC_START(bitmap_converttoimage, 1, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Image, new wxImage(bitmap->ConvertToImage()))
}

WX_FUNC_START(bitmap_copyfromicon, 2, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_SETBOOL(bitmap->CopyFromIcon(*icon));
}

WX_FUNC_ARGRANGE_START(bitmap_create, 3, 4, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_GETNUMBER(width)
	WX_GETNUMBER(height)
	if (n >= 4 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
#if defined(__WXMSW__)
		DLIB_WXGET_BASE(dc, DC, dc)
		WX_SETBOOL(bitmap->Create(width, height, *dc))
#else
		DLIB_ERROR_CHECK(
			true,
			"This function overload is only available on MS Windows implementation of wxWidgets."
		);
#endif //__WXMSW__
	} else {
		int depth = -1;
		if (n >= 4 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER_DEFINED(depth)
		}
		WX_SETBOOL(bitmap->Create(width, height, depth))
	}
}

WX_FUNC_START(bitmap_getdepth, 1, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_SETNUMBER(bitmap->GetDepth())
}

WX_FUNC_START(bitmap_getheight, 1, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_SETNUMBER(bitmap->GetHeight())
}

WX_FUNC_START(bitmap_getpalette, 1, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	wxPalette* retval	= bitmap->GetPalette();
	WX_SETOBJECT(Palette, retval)
}

WX_FUNC_START(bitmap_getmask, 1, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_SETOBJECT(Mask, bitmap->GetMask())
}

WX_FUNC_START(bitmap_getwidth, 1, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_SETNUMBER(bitmap->GetWidth())
}

WX_FUNC_START(bitmap_getsubbitmap, 2, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETOBJECT(Bitmap, new wxBitmap(bitmap->GetSubBitmap(*rect)))
}

WX_FUNC_ARGRANGE_START(bitmap_loadfile, 2, 3, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_GETSTRING(name)
#if defined (__WXMSW__)
	int type = wxBITMAP_TYPE_BMP_RESOURCE;
	if (n >= 3) { WX_GETDEFINE_DEFINED(type) }
#else if defined (__WXGTK__) || defined (__WXGET20__)
	wxBitmapType type = wxBITMAP_TYPE_XPM;
	if (n >= 3) { WX_GETDEFINE(def) type = (wxBitmapType) def; }
#endif
	WX_SETBOOL(bitmap->LoadFile(name, (wxBitmapType) type))
}

WX_FUNC_START(bitmap_isok, 1, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_SETBOOL(bitmap->IsOk())
}

WX_FUNC_ARGRANGE_START(bitmap_savefile, 3, 4, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_GETSTRING(name)
	WX_GETDEFINE(_type)
	wxBitmapType type = (wxBitmapType) _type;
	wxPalette *cmap = NULL;
	if (n >= 4) { DLIB_WXGET_BASE(palette, Palette, palette) cmap = palette; }
	WX_SETBOOL(bitmap->SaveFile(name, type, cmap))
}

WX_FUNC_START(bitmap_setdepth, 2, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_GETNUMBER(depth)
	bitmap->SetDepth(depth);
}

WX_FUNC_START(bitmap_setheight, 2, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_GETNUMBER(height)
	bitmap->SetHeight(height);
}

WX_FUNC_START(bitmap_setmask, 2, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	DLIB_WXGET_BASE(mask, Mask, mask)
	bitmap->SetMask(mask);
}

WX_FUNC_START(bitmap_setpalette, 2, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	DLIB_WXGET_BASE(palette, Palette, palette)
	bitmap->SetPalette(*palette);
}

WX_FUNC_START(bitmap_setwidth, 2, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_GETNUMBER(width)
	bitmap->SetWidth(width);
}
