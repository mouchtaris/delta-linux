#include "DeltaWxWindowDC.h"
#include "DeltaWxDC.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(windowdc, name)
#define WX_FUNC(name) WX_FUNC1(windowdc, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "destruct")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(WindowDC, "windowdc", DC)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(WindowDC, val, windowdc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDC *_parent = DLIB_WXTYPECAST_BASE(DC, val, dc);
	DeltaWxDC *parent = DNEWCLASS(DeltaWxDC, (_parent));
	WX_SETOBJECT_EX(*at, DC, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "DC",					&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(WindowDC,windowdc)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(windowdc_construct, 0, 1, Nil)
	wxWindowDC *wxdc = (wxWindowDC*) 0;
	DeltaWxWindowDC *dc = (DeltaWxWindowDC*) 0;
	if (n == 0)
		wxdc = new wxWindowDC();
	else {
		DLIB_WXGET_BASE(window, Window, win)
		wxdc = new wxWindowDC(win);
	}
	if (wxdc) dc = DNEWCLASS(DeltaWxWindowDC, (wxdc));
	WX_SETOBJECT(WindowDC, dc)
}

DLIB_FUNC_START(windowdc_destruct, 1, Nil)
	DLIB_WXDELETE(windowdc, WindowDC, dc)
}
