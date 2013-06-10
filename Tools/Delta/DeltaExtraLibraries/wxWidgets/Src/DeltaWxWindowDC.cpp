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

WX_FUNCS_START
	WX_FUNC(construct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

//DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(WindowDC, "windowdc", DC)
VCLASSID_IMPL(DeltaWxWindowDCClassId, "wx::windowdc")
DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(WindowDC, "windowdc")
void WindowDCUtils::InstallAll(DeltaTable *methods)
{
	DPTR(methods)->DelegateInternal(DCUtils::GetMethods());
}

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(WindowDC, val, windowdc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, DC, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "DC",					&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(WindowDC,windowdc)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(windowdc_construct, 0, 1, Nil)
	wxWindowDC *dc = (wxWindowDC*) 0;
	if (n == 0) {
#if wxCHECK_VERSION(2, 9, 0)
		DPTR(vm)->Error("in wxWidgets 2.9+ windowdc_construct should necessarily take a window argument");
		DLIB_RESET_RETURN;
#else
		dc = new wxWindowDC();
#endif
	}
	else {
		DLIB_WXGET_BASE(window, Window, win)
		dc = new wxWindowDC(win);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(WindowDC, dc)
}
