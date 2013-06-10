#include "DeltaWxCursor.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(cursor, name)
#define WX_FUNC(name) WX_FUNC1(cursor, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(isok)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(isok)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "isok", "isok")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Cursor, "cursor", Bitmap)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Cursor, val, cursor) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Cursor,cursor)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(cursor_construct, 0, 4, Nil)
	wxCursor *cursor = (wxCursor*) 0;
	if (n == 0) {
		cursor = new wxCursor();
	} else if (n == 1) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(cursorId)
			cursor = new wxCursor(cursorId);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETDEFINE(cursorId)
			cursor = new wxCursor(cursorId);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Image, serial_no, image, image)) {
				cursor = new wxCursor(*image);
			}
		}
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
#if defined (__WXMSW__)
			WX_GETSTRING(cursorName)
			WX_GETDEFINE(type)
			int hotSpotX = 0, hotSpotY = 0;
			if (n >= 3) { WX_GETNUMBER_DEFINED(hotSpotX) }
			if (n >= 4) { WX_GETNUMBER_DEFINED(hotSpotY) }
			cursor = new wxCursor(cursorName, (wxBitmapType) type, hotSpotX, hotSpotY);
#else
			DLIB_ERROR_CHECK(
				true,
				"This function overload is only available on MS Windows implementation of wxWidgets."
			);
#endif //__WXMSW__
		}
	}
	WX_SETOBJECT(Cursor, cursor)
}

WX_FUNC_START(cursor_isok, 1, Nil)
	DLIB_WXGET_BASE(cursor, Cursor, cursor)
	WX_SETBOOL(cursor->IsOk())
}
