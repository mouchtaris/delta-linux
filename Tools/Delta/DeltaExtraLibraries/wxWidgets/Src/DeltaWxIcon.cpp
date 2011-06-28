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
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(copyfrombitmap)
WX_FUNC_DEF(getdepth)
WX_FUNC_DEF(getheight)
WX_FUNC_DEF(getwidth)
WX_FUNC_DEF(isok)
#if defined (__WXMSW__)
WX_FUNC_DEF(loadfile)
#endif //__WXMSW__
WX_FUNC_DEF(setdepth)
WX_FUNC_DEF(setheight)
WX_FUNC_DEF(setwidth)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(copyfrombitmap),
	WX_FUNC(getdepth),
	WX_FUNC(getheight),
	WX_FUNC(getwidth),
	WX_FUNC(isok),
#if defined (__WXMSW__)
	WX_FUNC(loadfile),
#endif //__WXMSW__
	WX_FUNC(setdepth),
	WX_FUNC(setheight),
	WX_FUNC(setwidth)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setwidth")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Icon, "icon", Bitmap)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Icon, val, icon) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxBitmap *_parent = DLIB_WXTYPECAST_BASE(Bitmap, val, bitmap);
	DeltaWxBitmap *parent = DNEWCLASS(DeltaWxBitmap, (_parent));
	WX_SETOBJECT_EX(*at, Bitmap, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Bitmap",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Icon, icon);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(icon_construct, 0, 4, Nil)
	wxIcon *wxicon = (wxIcon*) 0;
	DeltaWxIcon *icon = (DeltaWxIcon*) 0;
	if (n == 0) {
		wxicon = new wxIcon();
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETSTRING(name)
			long type = wxBITMAP_TYPE_ICO_RESOURCE;
			int desiredWidth = -1, desiredHeight = -1;
			if (n >= 2) { WX_GETDEFINE_DEFINED(type) }
			if (n >= 3) { WX_GETDEFINE_DEFINED(desiredWidth) }
			if (n >= 4) { WX_GETDEFINE_DEFINED(desiredHeight) }
			wxicon = new wxIcon(name, (wxBitmapType)type, desiredWidth, desiredHeight);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(iconlocation, IconLocation, loc)
			wxicon = new wxIcon(*loc);
		}
	}
	if (wxicon) icon = DNEWCLASS(DeltaWxIcon, (wxicon));
	WX_SETOBJECT(Icon, icon)
}

DLIB_FUNC_START(icon_destruct, 1, Nil)
	DLIB_WXDELETE(icon, Icon, icon)
}

DLIB_FUNC_START(icon_copyfrombitmap, 2, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
	icon->CopyFromBitmap(*bmp);
}

DLIB_FUNC_START(icon_getdepth, 1, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_SETNUMBER(icon->GetDepth())
}

DLIB_FUNC_START(icon_getheight, 1, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_SETNUMBER(icon->GetHeight())
}

DLIB_FUNC_START(icon_getwidth, 1, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_SETNUMBER(icon->GetWidth())
}

DLIB_FUNC_START(icon_isok, 1, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_SETBOOL(icon->IsOk())
}
#if defined (__WXMSW__)
WX_FUNC_ARGRANGE_START(icon_loadfile, 2, 5, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_GETSTRING(name)
	long type = wxBITMAP_TYPE_ICO_RESOURCE;
	int desiredWidth = -1, desiredHeight = -1;
	if (n >= 3) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 4) { WX_GETDEFINE_DEFINED(desiredWidth) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(desiredHeight) }
	WX_SETBOOL(icon->LoadFile(name, type, desiredWidth, desiredHeight))
}
#endif //__WXMSW__
DLIB_FUNC_START(icon_setdepth, 2, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_GETNUMBER(depth)
	icon->SetDepth(depth);
}

DLIB_FUNC_START(icon_setheight, 2, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_GETNUMBER(height)
	icon->SetHeight(height);
}

DLIB_FUNC_START(icon_setwidth, 2, Nil)
	DLIB_WXGET_BASE(icon, Icon, icon)
	WX_GETNUMBER(width)
	icon->SetWidth(width);
}
