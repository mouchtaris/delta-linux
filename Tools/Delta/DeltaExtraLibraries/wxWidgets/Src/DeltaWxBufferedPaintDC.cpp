#include "DeltaWxBufferedPaintDC.h"
#include "DeltaWxBufferedDC.h"
#include "DeltaWxWindow.h"
#include "DeltaWxBitmap.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(bufferedpaintdc, name)
#define WX_FUNC(name) WX_FUNC1(bufferedpaintdc, name)

WX_FUNC_DEF(construct)

WX_FUNCS_START
	WX_FUNC(construct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

//DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(BufferedPaintDC, , BufferedDC)
VCLASSID_IMPL(DeltaWxBufferedPaintDCClassId, "wx::bufferedpaintdc")
DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(BufferedPaintDC, "bufferedpaintdc")
void BufferedPaintDCUtils::InstallAll(DeltaTable *methods)
{
	DPTR(methods)->DelegateInternal(BufferedDCUtils::GetMethods());
}

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(BufferedPaintDC, val, bufferedpaintdc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, BufferedDC, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "BufferedDC",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(BufferedPaintDC,bufferedpaintdc)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(bufferedpaintdc_construct, 1, 3, Nil)
	wxBufferedPaintDC *dc = (wxBufferedPaintDC*) 0;
	DLIB_WXGET_BASE(window, Window, window)
	if (n >= 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(bitmap, Bitmap, buffer)
			int style = wxBUFFER_CLIENT_AREA;
			if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
			dc = new wxBufferedPaintDC(window, *buffer, style);
		} else if (	DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String ||
					DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETDEFINE(style)
			dc = new wxBufferedPaintDC(window, style);
		}
	} else {
		dc = new wxBufferedPaintDC(window);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(BufferedPaintDC, dc)
}
