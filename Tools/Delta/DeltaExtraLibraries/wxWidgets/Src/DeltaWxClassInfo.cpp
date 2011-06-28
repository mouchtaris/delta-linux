#include "DeltaWxClassInfo.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(classinfo, name)
#define WX_FUNC(name) WX_FUNC1(classinfo, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(createobject)
WX_FUNC_DEF(findclass)
WX_FUNC_DEF(getbaseclassname1)
WX_FUNC_DEF(getbaseclassname2)
WX_FUNC_DEF(getclassname)
WX_FUNC_DEF(getsize)
WX_FUNC_DEF(isdynamic)
WX_FUNC_DEF(iskindof)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(findclass),
	WX_FUNC(destruct),
	WX_FUNC(createobject),
	WX_FUNC(getbaseclassname1),
	WX_FUNC(getbaseclassname2),
	WX_FUNC(getclassname),
	WX_FUNC(getsize),
	WX_FUNC(isdynamic),
	WX_FUNC(iskindof)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(2, uarraysize(funcs) - 2, "destruct", "iskindof")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(ClassInfo, "classinfo")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ClassInfo, val, classinfo) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetClassName (void* val, DeltaValue* at) 
{
	wxClassInfo *info = DLIB_WXTYPECAST_BASE(ClassInfo, val, classinfo);
	wxString str(info->GetClassName());
	WX_SETSTRING_EX(*at, str)
	return true;
}

static bool GetSize (void* val, DeltaValue* at) 
{
	wxClassInfo *info = DLIB_WXTYPECAST_BASE(ClassInfo, val, classinfo);
	WX_SETNUMBER_EX(*at, info->GetSize())
	return true;
}

static bool GetBaseClass1 (void* val, DeltaValue* at) 
{
	wxClassInfo *info = DLIB_WXTYPECAST_BASE(ClassInfo, val, classinfo);
	wxClassInfo *classinfo = (wxClassInfo*)info->GetBaseClass1();
	DeltaWxClassInfo *retval = classinfo ? DNEWCLASS(DeltaWxClassInfo, (classinfo)) : (DeltaWxClassInfo*) 0;
	WX_SETOBJECT_EX(*at, ClassInfo, retval)
	return true;
}

static bool GetBaseClass2 (void* val, DeltaValue* at) 
{
	wxClassInfo *info = DLIB_WXTYPECAST_BASE(ClassInfo, val, classinfo);
	wxClassInfo *classinfo = (wxClassInfo*)info->GetBaseClass2();
	DeltaWxClassInfo *retval = classinfo ? DNEWCLASS(DeltaWxClassInfo, (classinfo)) : (DeltaWxClassInfo*) 0;
	WX_SETOBJECT_EX(*at, ClassInfo, retval)
	return true;
}

static bool GetNext (void* val, DeltaValue* at) 
{
	wxClassInfo *info = DLIB_WXTYPECAST_BASE(ClassInfo, val, classinfo);
	wxClassInfo *classinfo = (wxClassInfo*)info->GetNext();
	DeltaWxClassInfo *retval = classinfo ? DNEWCLASS(DeltaWxClassInfo, (classinfo)) : (DeltaWxClassInfo*) 0;
	WX_SETOBJECT_EX(*at, ClassInfo, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "className",			&GetClassName,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "objectSize",			&GetSize,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "baseInfo1",			&GetBaseClass1,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "baseInfo2",			&GetBaseClass2,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "next",				&GetNext,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ClassInfo, classinfo);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(classinfo_construct, 4, Nil)
	WX_GETSTRING(className)
	DLIB_WXGET_BASE(classinfo, ClassInfo, baseClass1)
	DLIB_WXGET_BASE(classinfo, ClassInfo, baseClass2)
	WX_GETNUMBER(size)
	DeltaWxClassInfo *wxinfo = DNEWCLASS(DeltaWxClassInfo,
		(new wxClassInfo(className.GetData(), baseClass1, baseClass2, size, NULL)));
	WX_SETOBJECT(ClassInfo, wxinfo)
}

DLIB_FUNC_START(classinfo_destruct, 1, Nil)
	DLIB_WXDELETE(classinfo, ClassInfo, info)
}

DLIB_FUNC_START(classinfo_createobject, 1, Nil)
	DLIB_WXGET_BASE(classinfo, ClassInfo, info)
	WXNEWCLASS(DeltaWxObject, retval, wxObject, info->CreateObject());
	WX_SETOBJECT(Object, retval)
}

DLIB_FUNC_START(classinfo_findclass, 1, Nil)
	WX_GETSTRING(name)
	WXNEWCLASS(DeltaWxClassInfo, retval, wxClassInfo, wxClassInfo::FindClass(name));
	WX_SETOBJECT(ClassInfo, retval)
}

DLIB_FUNC_START(classinfo_getbaseclassname1, 1, Nil)
	DLIB_WXGET_BASE(classinfo, ClassInfo, info)
	WX_SETSTRING(wxString(info->GetBaseClassName1()))
}

DLIB_FUNC_START(classinfo_getbaseclassname2, 1, Nil)
	DLIB_WXGET_BASE(classinfo, ClassInfo, info)
	WX_SETSTRING(wxString(info->GetBaseClassName2()))
}

DLIB_FUNC_START(classinfo_getclassname, 1, Nil)
	DLIB_WXGET_BASE(classinfo, ClassInfo, info)
	WX_SETSTRING(wxString(info->GetClassName()))
}

DLIB_FUNC_START(classinfo_getsize, 1, Nil)
	DLIB_WXGET_BASE(classinfo, ClassInfo, info)
	WX_SETNUMBER(info->GetSize())
}

DLIB_FUNC_START(classinfo_isdynamic, 1, Nil)
	DLIB_WXGET_BASE(classinfo, ClassInfo, info)
	WX_SETBOOL(info->IsDynamic())
}

DLIB_FUNC_START(classinfo_iskindof, 2, Nil)
	DLIB_WXGET_BASE(classinfo, ClassInfo, info)
	DLIB_WXGET_BASE(classinfo, ClassInfo, other)
	WX_SETBOOL(info->IsKindOf(other))
}
