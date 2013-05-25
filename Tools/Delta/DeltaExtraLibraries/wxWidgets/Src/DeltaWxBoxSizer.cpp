#include "DeltaWxBoxSizer.h"
#include "DeltaWxSizer.h"
#include "DeltaWxSize.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(boxsizer, name)
#define WX_FUNC(name) WX_FUNC1(boxsizer, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(recalcsizes)
WX_FUNC_DEF(calcmin)
WX_FUNC_DEF(getorientation)
WX_FUNC_DEF(setorientation)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(recalcsizes),
	WX_FUNC(calcmin),
	WX_FUNC(getorientation),
	WX_FUNC(setorientation)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setorientation")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(BoxSizer, "boxsizer", Sizer)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(BoxSizer, val, boxsizer) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Sizer, val)
	return true;
}

static bool GetOrientation (void* val, DeltaValue* at) 
{
	wxBoxSizer *sizer = DLIB_WXTYPECAST_BASE(BoxSizer, val, boxsizer);
	at->FromNumber(sizer->GetOrientation());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Sizer",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "orient",				&GetOrientation,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(BoxSizer,boxsizer)

////////////////////////////////////////////////////////////////

WX_FUNC_START(boxsizer_construct, 1, Nil)
	WX_GETDEFINE(orient)
	WX_SETOBJECT(BoxSizer, new wxBoxSizer(orient))
}

WX_FUNC_START(boxsizer_destruct, 1, Nil)
	DLIB_WXDELETE(boxsizer, BoxSizer, sizer)
}

WX_FUNC_START(boxsizer_recalcsizes, 1, Nil)
	DLIB_WXGET_BASE(boxsizer, BoxSizer, sizer)
	sizer->RecalcSizes();
}

WX_FUNC_START(boxsizer_calcmin, 1, Nil)
	DLIB_WXGET_BASE(boxsizer, BoxSizer, sizer)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(sizer->CalcMin()))
}

WX_FUNC_START(boxsizer_getorientation, 1, Nil)
	DLIB_WXGET_BASE(boxsizer, BoxSizer, sizer)
	WX_SETNUMBER(sizer->GetOrientation())
}

WX_FUNC_START(boxsizer_setorientation, 2, Nil)
	DLIB_WXGET_BASE(boxsizer, BoxSizer, sizer)
	WX_GETDEFINE(orientation)
	sizer->SetOrientation(orientation);
}
