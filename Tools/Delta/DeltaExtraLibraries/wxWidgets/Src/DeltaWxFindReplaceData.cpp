#include "DeltaWxFindReplaceData.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(findreplacedata, name)
#define WX_FUNC(name) WX_FUNC1(findreplacedata, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getfindstring)
WX_FUNC_DEF(getreplacestring)
WX_FUNC_DEF(getflags)
WX_FUNC_DEF(setflags)
WX_FUNC_DEF(setfindstring)
WX_FUNC_DEF(setreplacestring)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getfindstring),
	WX_FUNC(getreplacestring),
	WX_FUNC(getflags),
	WX_FUNC(setflags),
	WX_FUNC(setfindstring),
	WX_FUNC(setreplacestring)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getfindstring", "setreplacestring")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FindReplaceData, "findreplacedata", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FindReplaceData, val, findreplacedata) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetFlags (void* val, DeltaValue* at) 
{
	wxFindReplaceData *data = DLIB_WXTYPECAST_BASE(FindReplaceData, val, findreplacedata);
	WX_SETNUMBER_EX(*at, data->GetFlags())
	return true;
}

static bool GetFindString (void* val, DeltaValue* at) 
{
	wxFindReplaceData *data = DLIB_WXTYPECAST_BASE(FindReplaceData, val, findreplacedata);
	WX_SETSTRING_EX(*at, data->GetFindString())
	return true;
}

static bool GetReplaceString (void* val, DeltaValue* at) 
{
	wxFindReplaceData *data = DLIB_WXTYPECAST_BASE(FindReplaceData, val, findreplacedata);
	WX_SETSTRING_EX(*at, data->GetReplaceString())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "flags",				&GetFlags,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "findString",			&GetFindString,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "replaceString",		&GetReplaceString,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FindReplaceData,findreplacedata)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(findreplacedata_construct, 0, 1, Nil)
	wxFindReplaceData *data = (wxFindReplaceData*) 0;
	if (n == 0) {
		data = new wxFindReplaceData();
	} else {
		WX_GETDEFINE(flags)
		data = new wxFindReplaceData(flags);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(FindReplaceData, data)
}

WX_FUNC_START(findreplacedata_getfindstring, 1, Nil)
	DLIB_WXGET_BASE(findreplacedata, FindReplaceData, data)
	WX_SETSTRING(data->GetFindString())
}

WX_FUNC_START(findreplacedata_getreplacestring, 1, Nil)
	DLIB_WXGET_BASE(findreplacedata, FindReplaceData, data)
	WX_SETSTRING(data->GetReplaceString())
}

WX_FUNC_START(findreplacedata_getflags, 1, Nil)
	DLIB_WXGET_BASE(findreplacedata, FindReplaceData, data)
	WX_SETNUMBER(data->GetFlags())
}

WX_FUNC_START(findreplacedata_setflags, 2, Nil)
	DLIB_WXGET_BASE(findreplacedata, FindReplaceData, data)
	WX_GETDEFINE(flags)
	data->SetFlags(flags);
}

WX_FUNC_START(findreplacedata_setfindstring, 2, Nil)
	DLIB_WXGET_BASE(findreplacedata, FindReplaceData, data)
	WX_GETSTRING(str)
	data->SetFindString(str);
}

WX_FUNC_START(findreplacedata_setreplacestring, 2, Nil)
	DLIB_WXGET_BASE(findreplacedata, FindReplaceData, data)
	WX_GETSTRING(str)
	data->SetReplaceString(str);
}
