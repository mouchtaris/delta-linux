#include "DeltaWxMask.h"
#include "DeltaWxColour.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(mask, name)
#define WX_FUNC(name) WX_FUNC1(mask, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "create")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Mask, "mask", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Mask, val, mask) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Mask, mask);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(mask_construct, 0, 2, Nil)
	wxMask *wxmask = (wxMask*) 0;
	DeltaWxMask *mask = (DeltaWxMask*) 0;
	if (n == 0) {
		wxmask = new wxMask();
	} else {
		DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
		if (n == 1)
			wxmask = new wxMask(*bitmap);
		else if (n == 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(index)
			wxmask = new wxMask(*bitmap, index);
		} else if (n == 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(colour, Colour, colour)
			wxmask = new wxMask(*bitmap, *colour);
		}
	}
	if (wxmask) mask = DNEWCLASS(DeltaWxMask, (wxmask));
	WX_SETOBJECT(Mask, mask)
}

DLIB_FUNC_START(mask_destruct, 1, Nil)
	DLIB_WXDELETE(mask, Mask, mask)
}

WX_FUNC_ARGRANGE_START(mask_create, 1, 3, Nil)
	DLIB_WXGET_BASE(mask, Mask, mask)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	if (n == 2)
		WX_SETBOOL(mask->Create(*bitmap))
	else if (n == 3 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		WX_SETBOOL(mask->Create(*bitmap, index))
	} else if (n == 3 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(colour, Colour, colour)
		WX_SETBOOL(mask->Create(*bitmap, *colour))
	}
}
