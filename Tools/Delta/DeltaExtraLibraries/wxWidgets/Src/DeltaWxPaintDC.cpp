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

WX_FUNCS_START
	WX_FUNC(construct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

//DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(PaintDC, "paintdc", ClientDC)
VCLASSID_IMPL(DeltaWxPaintDCClassId, "wx::paintdc")
DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(PaintDC, "paintdc")
void PaintDCUtils::InstallAll(DeltaTable *methods)
{
	DPTR(methods)->DelegateInternal(ClientDCUtils::GetMethods());
}

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(PaintDC, val, paintdc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, ClientDC, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "ClientDC",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(PaintDC,paintdc)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(paintdc_construct, 0, 1, Nil)
	wxPaintDC *dc = (wxPaintDC*) 0;
	if (n == 0) {
#if wxCHECK_VERSION(2, 9, 0)
		DPTR(vm)->Error("in wxWidgets 2.9+ paintdc_construct should necessarily take a window argument");
		DLIB_RESET_RETURN;
#else
		dc = new wxPaintDC();
#endif
	}
	else {
		DLIB_WXGET_BASE(window, Window, win)
		dc = new wxPaintDC(win);
	}
	WX_SETOBJECT(PaintDC, dc)
}
