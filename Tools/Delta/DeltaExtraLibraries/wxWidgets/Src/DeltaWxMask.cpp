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
WX_FUNC_DEF(create)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "create")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Mask, "mask", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Mask, val, mask) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Mask,mask)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(mask_construct, 0, 2, Nil)
	wxMask *mask = (wxMask*) 0;
	if (n == 0) {
		mask = new wxMask();
	} else {
		DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
		if (n == 1)
			mask = new wxMask(*bitmap);
		else if (n == 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(index)
			mask = new wxMask(*bitmap, index);
		} else if (n == 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(colour, Colour, colour)
			mask = new wxMask(*bitmap, *colour);
		}
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Mask, mask)
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
