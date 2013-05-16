#include "DeltaWxClientDC.h"
#include "DeltaWxWindowDC.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(clientdc, name)
#define WX_FUNC(name) WX_FUNC1(clientdc, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "destruct")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ClientDC, "clientdc", WindowDC)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ClientDC, val, clientdc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxWindowDC *_parent = DLIB_WXTYPECAST_BASE(WindowDC, val, windowdc);
	DeltaWxWindowDC *parent = DNEWCLASS(DeltaWxWindowDC, (_parent));
	WX_SETOBJECT_EX(*at, WindowDC, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "WindowDC",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ClientDC,clientdc)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(clientdc_construct, 0, 1, Nil)
	wxClientDC *wxdc = (wxClientDC*) 0;
	DeltaWxClientDC *dc = (DeltaWxClientDC*) 0;
	if (n == 0){
#if wxCHECK_VERSION(2, 9, 0)
		DPTR(vm)->Error("in wxWidgets 2.9+ clientdc_construct should necessarily take a window argument");
		DLIB_RESET_RETURN;
#else
		wxdc = new wxClientDC();
#endif
	}
	else {
		DLIB_WXGET_BASE(window, Window, win)
		wxdc = new wxClientDC(win);
	}
	if (wxdc) dc = DNEWCLASS(DeltaWxClientDC, (wxdc));
	WX_SETOBJECT(ClientDC, dc)
}

DLIB_FUNC_START(clientdc_destruct, 1, Nil)
	DLIB_WXDELETE(clientdc, ClientDC, dc)
}
