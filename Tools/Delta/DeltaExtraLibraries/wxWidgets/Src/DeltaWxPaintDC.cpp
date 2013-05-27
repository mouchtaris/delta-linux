#include "DeltaWxPaintDC.h"
#include "DeltaWxClientDC.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(paintdc, name)
#define WX_FUNC(name) WX_FUNC1(paintdc, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "destruct")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PaintDC, "paintdc", ClientDC)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PaintDC, val, paintdc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxClientDC *_parent = DLIB_WXTYPECAST_BASE(ClientDC, val, clientdc);
	DeltaWxClientDC *parent = DNEWCLASS(DeltaWxClientDC, (_parent));
	WX_SETOBJECT_EX(*at, ClientDC, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "ClientDC",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PaintDC,paintdc)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(paintdc_construct, 0, 1, Nil)
	wxPaintDC *wxdc = (wxPaintDC*) 0;
	DeltaWxPaintDC *dc = (DeltaWxPaintDC*) 0;
	if (n == 0) {
#if wxCHECK_VERSION(2, 9, 0)
		DPTR(vm)->Error("in wxWidgets 2.9+ paintdc_construct should necessarily take a window argument");
		DLIB_RESET_RETURN;
#else
		wxdc = new wxPaintDC();
#endif
	}
	else {
		DLIB_WXGET_BASE(window, Window, win)
		wxdc = new wxPaintDC(win);
	}
	if (wxdc) dc = DNEWCLASS(DeltaWxPaintDC, (wxdc));
	WX_SETOBJECT(PaintDC, dc)
}

DLIB_FUNC_START(paintdc_destruct, 1, Nil)
	DLIB_WXDELETE(paintdc, PaintDC, dc)
}
