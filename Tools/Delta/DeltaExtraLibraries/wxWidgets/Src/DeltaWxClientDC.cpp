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

WX_FUNCS_START
	WX_FUNC(construct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

//DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ClientDC, "clientdc", WindowDC)
VCLASSID_IMPL(DeltaWxClientDCClassId, "wx::clientdc")
DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(ClientDC, "clientdc")
void ClientDCUtils::InstallAll(DeltaTable *methods)
{
	DPTR(methods)->DelegateInternal(WindowDCUtils::GetMethods());
}

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ClientDC, val, clientdc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, WindowDC, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "WindowDC",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ClientDC,clientdc)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(clientdc_construct, 0, 1, Nil)
	wxClientDC *dc = (wxClientDC*) 0;
	if (n == 0){
#if wxCHECK_VERSION(2, 9, 0)
		DPTR(vm)->Error("in wxWidgets 2.9+ clientdc_construct should necessarily take a window argument");
		DLIB_RESET_RETURN;
#else
		dc = new wxClientDC();
#endif
	}
	else {
		DLIB_WXGET_BASE(window, Window, win)
		dc = new wxClientDC(win);
	}
	WX_SETOBJECT(ClientDC, dc)
}

