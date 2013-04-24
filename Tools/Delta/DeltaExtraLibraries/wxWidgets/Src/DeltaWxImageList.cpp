#include "DeltaWxImageList.h"
#include "DeltaWxObject.h"
#include "DeltaWxDC.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxIcon.h"
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

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(imagelist, name)
#define WX_FUNC(name) WX_FUNC1(imagelist, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(add)
WX_FUNC_DEF(create)
WX_FUNC_DEF(draw)
WX_FUNC_DEF(getbitmap)
WX_FUNC_DEF(geticon)
WX_FUNC_DEF(getimagecount)
WX_FUNC_DEF(getsize)
WX_FUNC_DEF(remove)
WX_FUNC_DEF(removeall)
WX_FUNC_DEF(replace)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(add),
	WX_FUNC(create),
	WX_FUNC(draw),
	WX_FUNC(getbitmap),
	WX_FUNC(geticon),
	WX_FUNC(getimagecount),
	WX_FUNC(getsize),
	WX_FUNC(remove),
	WX_FUNC(removeall),
	WX_FUNC(replace)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "replace")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ImageList, "imagelist", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ImageList, val, imagelist) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(ImageList,imagelist)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(imagelist_construct, 0, 4, Nil)
	wxImageList *wximglist = (wxImageList*) 0;
	DeltaWxImageList *imglist = (DeltaWxImageList*) 0;
	if (n == 0) {
		wximglist = new wxImageList();
	} else if (n >= 2) {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		bool mask = true;
		int initialCount = 1;
		if (n >= 3) { WX_GETBOOL_DEFINED(mask) }
		if (n >= 4) { WX_GETNUMBER_DEFINED(initialCount) }
		wximglist = new wxImageList(width, height, mask, initialCount);
	}
	if (wximglist) imglist = DNEWCLASS(DeltaWxImageList, (wximglist));
	WX_SETOBJECT(ImageList, imglist)
}

DLIB_FUNC_START(imagelist_destruct, 1, Nil)
	DLIB_WXDELETE(imagelist, ImageList, imglist)
}

WX_FUNC_ARGRANGE_START(imagelist_add, 2, 3, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, bitmap_wr)) {
			wxBitmap *bitmap = (wxBitmap*) bitmap_wr->GetCastToNativeInstance();
			if (n == 2) {
				WX_SETNUMBER(imglist->Add(*bitmap))
			} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
				util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
				if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, mask_wr)) {
					wxBitmap *mask = (wxBitmap*) mask_wr->GetCastToNativeInstance();
					WX_SETNUMBER(imglist->Add(*bitmap, *mask))
				} else if (DLIB_WXISBASE(Colour, serial_no, colour, colour)) {
					wxColour *maskColour = (wxColour*) colour->GetCastToNativeInstance();
					WX_SETNUMBER(imglist->Add(*bitmap, *maskColour))
				}
			}
		} else if (DLIB_WXISBASE(Icon, serial_no, icon, icon_wr)) {
			wxIcon *icon = (wxIcon*) icon_wr->GetCastToNativeInstance();
			WX_SETNUMBER(imglist->Add(*icon))
		}
	}
}

WX_FUNC_ARGRANGE_START(imagelist_create, 3, 5, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	WX_GETNUMBER(width)
	WX_GETNUMBER(height)
	bool mask = true;
	int initialCount = 1;
	if (n >= 4) { WX_GETBOOL_DEFINED(mask) }
	if (n >= 5) { WX_GETNUMBER_DEFINED(initialCount) }
	WX_SETBOOL(imglist->Create(width, height, mask, initialCount))
}

WX_FUNC_ARGRANGE_START(imagelist_draw, 5, 7, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	WX_GETNUMBER(index)
	DLIB_WXGET_BASE(dc, DC, dc)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	int flags = wxIMAGELIST_DRAW_NORMAL;
	bool solidBackground = false;
	if (n >= 6) { WX_GETDEFINE_DEFINED(flags) }
	if (n >= 7) { WX_GETBOOL_DEFINED(solidBackground) }
	WX_SETBOOL(imglist->Draw(index, *dc, x, y, flags, solidBackground))
}

DLIB_FUNC_START(imagelist_getbitmap, 2, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	WX_GETNUMBER(index)
	DeltaWxBitmap *retval = DNEWCLASS(DeltaWxBitmap, (new wxBitmap(imglist->GetBitmap(index))));
	WX_SETOBJECT(Bitmap, retval)
}

DLIB_FUNC_START(imagelist_geticon, 2, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	WX_GETNUMBER(index)
	DeltaWxIcon *retval = DNEWCLASS(DeltaWxIcon, (new wxIcon(imglist->GetIcon(index))));
	WX_SETOBJECT(Icon, retval)
}

DLIB_FUNC_START(imagelist_getimagecount, 1, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	WX_SETNUMBER(imglist->GetImageCount())
}

DLIB_FUNC_START(imagelist_getsize, 4, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	WX_GETNUMBER(index)
	int width, height;
	WX_SETBOOL(imglist->GetSize(index, width, height))
	WX_GETTABLE(width_table)
	WX_GETTABLE(height_table)
	WX_SETTABLE_RETVAL(width_table, DeltaValue(DeltaNumberValueType(width)))
	WX_SETTABLE_RETVAL(height_table, DeltaValue(DeltaNumberValueType(height)))
}

DLIB_FUNC_START(imagelist_remove, 2, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	WX_GETNUMBER(index)
	WX_SETBOOL(imglist->Remove(index))
}

DLIB_FUNC_START(imagelist_removeall, 1, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	WX_SETBOOL(imglist->RemoveAll())
}

WX_FUNC_ARGRANGE_START(imagelist_replace, 3, 4, Nil)
	DLIB_WXGET_BASE(imagelist, ImageList, imglist)
	WX_GETNUMBER(index)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, bitmap_wr)) {
			wxBitmap *bitmap = (wxBitmap*) bitmap_wr->GetCastToNativeInstance();
			if (n == 3) {
				WX_SETBOOL(imglist->Replace(index, *bitmap))
			} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
				DLIB_WXGET_BASE(bitmap, Bitmap, mask)
				WX_SETBOOL(imglist->Replace(index, *bitmap, *mask))
			}
		} else if (DLIB_WXISBASE(Icon, serial_no, icon, icon_wr)) {
			wxIcon *icon = (wxIcon*) icon_wr->GetCastToNativeInstance();
			WX_SETBOOL(imglist->Replace(index, *icon))
		}
	}
}
