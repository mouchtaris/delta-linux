#include "DeltaWxBufferedDC.h"
#include "DeltaWxMemoryDC.h"
#include "DeltaWxDC.h"
#include "DeltaWxSize.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(buffereddc, name)
#define WX_FUNC(name) WX_FUNC1(buffereddc, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(init)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(init)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "init")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(BufferedDC, "buffereddc", MemoryDC)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(BufferedDC, val, buffereddc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxMemoryDC *_parent = DLIB_WXTYPECAST_BASE(MemoryDC, val, memorydc);
	DeltaWxMemoryDC *parent = DNEWCLASS(DeltaWxMemoryDC, (_parent));
	WX_SETOBJECT_EX(*at, MemoryDC, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "MemoryDC",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(BufferedDC, buffereddc);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(buffereddc_construct, 0, 3, Nil)
	wxBufferedDC *wxdc = (wxBufferedDC*) 0;
	DeltaWxBufferedDC *dc = (DeltaWxBufferedDC*) 0;
	if (n == 0)
		wxdc = new wxBufferedDC();
	else {
		DLIB_WXGET_BASE(dc, DC, dc)
		if (n == 1) {
			wxdc = new wxBufferedDC(dc);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, data)) {
				wxBitmap *bmp = (wxBitmap*)data->GetCastToNativeInstance();
				int style = wxBUFFER_CLIENT_AREA;
				if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
				wxdc = new wxBufferedDC(dc, *bmp, style);
			} else if (DLIB_WXISBASE(Size, serial_no, size, data)) {
				wxSize* size = (wxSize*)data->GetCastToNativeInstance();
				int style = wxBUFFER_CLIENT_AREA;
				if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
				wxdc = new wxBufferedDC(dc, *size, style);
			}
		}
	}
	if (wxdc) dc = DNEWCLASS(DeltaWxBufferedDC, (wxdc));
	WX_SETOBJECT(BufferedDC, dc)
}

DLIB_FUNC_START(buffereddc_destruct, 1, Nil)
	DLIB_WXDELETE(buffereddc, BufferedDC, dc)
}

WX_FUNC_ARGRANGE_START(buffereddc_init, 2, 4, Nil)
	DLIB_WXGET_BASE(buffereddc, BufferedDC, buffer)
	DLIB_WXGET_BASE(dc, DC, dc)
	if (n == 2) {
		buffer->Init(dc);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Bitmap, serial_no, bitmap, data)) {
			wxBitmap *bmp = (wxBitmap*)data->GetCastToNativeInstance();
			int style = wxBUFFER_CLIENT_AREA;
			if (n >= 4) { WX_GETDEFINE_DEFINED(style) }
			buffer->Init(dc, *bmp, style);
		} else if (DLIB_WXISBASE(Size, serial_no, size, data)) {
			wxSize* size = (wxSize*)data->GetCastToNativeInstance();
			int style = wxBUFFER_CLIENT_AREA;
			if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
			buffer->Init(dc, *size, style);
		}
	}
}
