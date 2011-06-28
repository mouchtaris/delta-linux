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
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(isok)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(isok)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "isok")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Cursor, "cursor", Bitmap)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Cursor, val, cursor) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Cursor, cursor);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(cursor_construct, 0, 4, Nil)
	wxCursor *wxcursor = (wxCursor*) 0;
	DeltaWxCursor *cursor = (DeltaWxCursor*) 0;
	if (n == 0) {
		wxcursor = new wxCursor();
	} else if (n == 1) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(cursorId)
			wxcursor = new wxCursor(cursorId);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETDEFINE(cursorId)
			wxcursor = new wxCursor(cursorId);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Image, serial_no, image, image_wr)) {
				wxImage *image = (wxImage*) image_wr->GetCastToNativeInstance();
				wxcursor = new wxCursor(*image);
			}
		}
	} else {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			WX_GETSTRING(cursorName)
			WX_GETDEFINE(type)
			int hotSpotX = 0, hotSpotY = 0;
			if (n >= 3) { WX_GETNUMBER_DEFINED(hotSpotX) }
			if (n >= 4) { WX_GETNUMBER_DEFINED(hotSpotY) }
			wxcursor = new wxCursor(cursorName, type, hotSpotX, hotSpotY);
		}
	}
	if (wxcursor) cursor = DNEWCLASS(DeltaWxCursor, (wxcursor));
	WX_SETOBJECT(Cursor, cursor)
}

DLIB_FUNC_START(cursor_destruct, 1, Nil)
	DLIB_WXDELETE(cursor, Cursor, cursor)
}

DLIB_FUNC_START(cursor_isok, 1, Nil)
	DLIB_WXGET_BASE(cursor, Cursor, cursor)
	WX_SETBOOL(cursor->IsOk())
}
