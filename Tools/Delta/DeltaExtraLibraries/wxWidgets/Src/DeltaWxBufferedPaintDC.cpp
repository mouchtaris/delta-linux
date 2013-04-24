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
WX_FUNC_DEF(destruct)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "destruct")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(BufferedPaintDC, "bufferedpaintdc", BufferedDC)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(BufferedPaintDC, val, bufferedpaintdc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxBufferedDC *_parent = DLIB_WXTYPECAST_BASE(BufferedDC, val, buffereddc);
	DeltaWxBufferedDC *parent = DNEWCLASS(DeltaWxBufferedDC, (_parent));
	WX_SETOBJECT_EX(*at, BufferedDC, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "BufferedDC",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(BufferedPaintDC,bufferedpaintdc)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(bufferedpaintdc_construct, 1, 3, Nil)
	wxBufferedPaintDC *wxdc = (wxBufferedPaintDC*) 0;
	DeltaWxBufferedPaintDC *dc = (DeltaWxBufferedPaintDC*) 0;
	DLIB_WXGET_BASE(window, Window, window)
	if (n >= 2) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(bitmap, Bitmap, buffer)
			int style = wxBUFFER_CLIENT_AREA;
			if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
			wxdc = new wxBufferedPaintDC(window, *buffer, style);
		} else if (	DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String ||
					DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETDEFINE(style)
			wxdc = new wxBufferedPaintDC(window, style);
		}
	} else {
		wxdc = new wxBufferedPaintDC(window);
	}
	if (wxdc) dc = DNEWCLASS(DeltaWxBufferedPaintDC, (wxdc));
	WX_SETOBJECT(BufferedPaintDC, dc)
}

DLIB_FUNC_START(bufferedpaintdc_destruct, 1, Nil)
	DLIB_WXDELETE(bufferedpaintdc, BufferedPaintDC, dc)
}
