#include "DeltaWxObjectRefData.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(objectrefdata, name)
#define WX_FUNC(name) WX_FUNC1(objectrefdata, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getrefcount)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getrefcount)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "getrefcount")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ObjectRefData, "objectrefdata", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ObjectRefData, val, objectrefdata) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetCount (void* val, DeltaValue* at) 
{
	wxObjectRefData *refdata = DLIB_WXTYPECAST_BASE(ObjectRefData, val, objectrefdata);
	WX_SETNUMBER_EX(*at, refdata->GetRefCount())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "count",				&GetCount,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ObjectRefData,objectrefdata)

////////////////////////////////////////////////////////////////

WX_FUNC_START(objectrefdata_construct, 0, Nil)
	WX_SETOBJECT(ObjectRefData, new wxObjectRefData())
}

WX_FUNC_START(objectrefdata_destruct, 1, Nil)
#if wxCHECK_VERSION(2, 9, 0)
	DLIB_WXGET(objectrefdata, ObjectRefData, objrefdata)
	if (wxObjectRefData* nativeInst = (wxObjectRefData*) objrefdata->GetNativeInstance())
		while(nativeInst->GetRefCount() > 0)	//loop equivalent to delete nativeInst; but used because of private dtor
			nativeInst->DecRef();
	DDELETE(objrefdata);
#else
	DLIB_WXDELETE(objectrefdata, ObjectRefData, objrefdata)
#endif
}

WX_FUNC_START(objectrefdata_getrefcount, 1, Nil)
	DLIB_WXGET_BASE(objectrefdata, ObjectRefData, objrefdata)
	WX_SETNUMBER(objrefdata->GetRefCount())
}
