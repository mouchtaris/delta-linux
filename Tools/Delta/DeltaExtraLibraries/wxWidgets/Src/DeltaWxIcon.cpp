#include "DeltaWxIcon.h"
#include "DeltaWxIconLocation.h"
#include "DeltaWxBitmap.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(icon, name)
#define WX_FUNC(name) WX_FUNC1(icon, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(copyfrombitmap)
WX_FUNC_DEF(getdepth)
WX_FUNC_DEF(getheight)
WX_FUNC_DEF(getwidth)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(loadfile)
WX_FUNC_DEF(setdepth)
WX_FUNC_DEF(setheight)
WX_FUNC_DEF(setwidth)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(copyfrombitmap),
	WX_FUNC(getdepth),
	WX_FUNC(getheight),
	WX_FUNC(getwidth),
	WX_FUNC(isok),
	WX_FUNC(loadfile),
	WX_FUNC(setdepth),
	WX_FUNC(setheight),
	WX_FUNC(setwidth)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "copyfrombitmap", "setwidth")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Icon, "icon", Bitmap)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Icon, val, icon) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Bitmap, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Bitmap",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Icon,icon)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(icon_construct, 0, 4, Nil)
	wxIcon *icon = (wxIcon*) 0;
	if (n == 0) {
		icon = new wxIcon();
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETSTRING(name)
			long type = wxBITMAP_TYPE_ICO_RESOURCE;
			int desiredWidth = -1, desiredHeight = -1;
			if (n >= 2) { WX_GETDEFINE_DEFINED(type) }
			if (n >= 3) { WX_GETDEFINE_DEFINED(desiredWidth) }
			if (n >= 4) { WX_GETDEFINE_DEFINED(desiredHeight) }
			icon = new wxIcon(name, (wxBitmapType)type, desiredWidth, desiredHeight);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(iconlocation, IconLocation, loc)
			icon = new wxIcon(*loc);
		}
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Icon, icon)
}

WX_FUNC_START(icon_copyfrombitmap, 2, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
	icon->CopyFromBitmap(*bmp);
}

WX_FUNC_START(icon_getdepth, 1, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_SETNUMBER(icon->GetDepth())
}

WX_FUNC_START(icon_getheight, 1, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_SETNUMBER(icon->GetHeight())
}

WX_FUNC_START(icon_getwidth, 1, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_SETNUMBER(icon->GetWidth())
}

WX_FUNC_START(icon_isok, 1, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_SETBOOL(icon->IsOk())
}

WX_FUNC_ARGRANGE_START(icon_loadfile, 2, 5, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_GETSTRING(name)
	int type = wxBITMAP_TYPE_ICO_RESOURCE;
	int desiredWidth = -1, desiredHeight = -1;
	if (n >= 3) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 4) { WX_GETDEFINE_DEFINED(desiredWidth) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(desiredHeight) }
	WX_SETBOOL(icon->LoadFile(name, (wxBitmapType) type, desiredWidth, desiredHeight))
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

WX_FUNC_START(icon_setdepth, 2, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_GETNUMBER(depth)
	icon->SetDepth(depth);
}

WX_FUNC_START(icon_setheight, 2, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_GETNUMBER(height)
	icon->SetHeight(height);
}

WX_FUNC_START(icon_setwidth, 2, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_GETNUMBER(width)
	icon->SetWidth(width);
}
