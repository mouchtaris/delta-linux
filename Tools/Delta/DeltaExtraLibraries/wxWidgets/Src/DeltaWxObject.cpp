#include "DeltaWxObject.h"
#include "DeltaWxObjectRefData.h"
#include "DeltaWxClassInfo.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(object, name)
#define WX_FUNC(name) WX_FUNC1(object, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getclassinfo)
WX_FUNC_DEF(getrefdata)
WX_FUNC_DEF(iskindof)
WX_FUNC_DEF(issameas)
WX_FUNC_DEF(ref)
WX_FUNC_DEF(setrefdata)
WX_FUNC_DEF(unref)
WX_FUNC_DEF(unshare)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getclassinfo),
	WX_FUNC(getrefdata),
	WX_FUNC(iskindof),
	WX_FUNC(issameas),
	WX_FUNC(ref),
	WX_FUNC(setrefdata),
	WX_FUNC(unref),
	WX_FUNC(unshare)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getclassinfo", "unshare")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(Object, "object")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Object, val, object) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetRefData (void* val, DeltaValue* at) 
{
	wxObject *obj = DLIB_WXTYPECAST_BASE(Object, val, object);
	wxObjectRefData *refdata = obj->GetRefData();
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ObjectRefData, refdata)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "refdata",			&GetRefData,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Object,object)

////////////////////////////////////////////////////////////////

WX_FUNC_START(object_construct, 0, Nil)
	WX_SETOBJECT(Object, new wxObject())
}

WX_FUNC_START(object_getclassinfo, 1, Nil)
	DLIB_WXGET_BASE(object, Object, object)
	wxClassInfo* retval	= object->GetClassInfo();;
	WX_SETOBJECT(ClassInfo, retval)
}

WX_FUNC_START(object_getrefdata, 1, Nil)
	DLIB_WXGET_BASE(object, Object, object)
	wxObjectRefData* retval	= object->GetRefData();;
	WX_SETOBJECT(ObjectRefData, retval)
}

WX_FUNC_START(object_iskindof, 2, Nil)
	DLIB_WXGET_BASE(object, Object, object)
	DLIB_WXGET_BASE(classinfo, ClassInfo, info)
	WX_SETBOOL(object->IsKindOf(info))
}

WX_FUNC_START(object_issameas, 2, Nil)
	DLIB_WXGET_BASE(object, Object, object)
	DLIB_WXGET_BASE(object, Object, obj)
	WX_SETBOOL(object->IsSameAs(*obj))
}

WX_FUNC_START(object_ref, 2, Nil)
	DLIB_WXGET_BASE(object, Object, object)
	DLIB_WXGET_BASE(object, Object, clone)
	object->Ref(*clone);
}

WX_FUNC_START(object_setrefdata, 2, Nil)
	DLIB_WXGET_BASE(object, Object, object)
	DLIB_WXGET_BASE(objectrefdata, ObjectRefData, data)
	object->SetRefData(data);
}

WX_FUNC_START(object_unref, 1, Nil)
	DLIB_WXGET_BASE(object, Object, object)
	object->UnRef();
}

WX_FUNC_START(object_unshare, 1, Nil)
	DLIB_WXGET_BASE(object, Object, object)
	object->UnShare();
}
