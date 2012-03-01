#include "DeltaWxIconLocation.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(iconlocation, name)
#define WX_FUNC(name) WX_FUNC1(iconlocation, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getfilename)
WX_FUNC_DEF(getindex)
WX_FUNC_DEF(setindex)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(setfilename)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getfilename),
	WX_FUNC(getindex),
	WX_FUNC(setindex),
	WX_FUNC(isok),
	WX_FUNC(setfilename)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setfilename")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(IconLocation, "iconlocation")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(IconLocation, val, iconlocation) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetFilename (void* val, DeltaValue* at) 
{
	wxIconLocation *iconloc = DLIB_WXTYPECAST_BASE(IconLocation, val, iconlocation);
	WX_SETSTRING_EX(*at, iconloc->GetFileName())
	return true;
}
#if defined (__WXMSW__)
static bool GetIndex (void* val, DeltaValue* at) 
{
	wxIconLocation *iconloc = DLIB_WXTYPECAST_BASE(IconLocation, val, iconlocation);
	WX_SETNUMBER_EX(*at, iconloc->GetIndex())
	return true;
}
#endif //__WXMSW__
static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "filename",			&GetFilename,			DELTA_GETBYSTRING_NO_PRECOND	}
#if defined (__WXMSW__)
	,{ "index",				&GetIndex,				DELTA_GETBYSTRING_NO_PRECOND	}
#endif //__WXMSW__
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(IconLocation, iconlocation);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(iconlocation_construct, 0, 2, Nil)
	wxString file = wxEmptyString;
	int num = 0;
	if (n >= 1) { WX_GETSTRING_DEFINED(file) }
#if defined (__WXMSW__)
	if (n >= 2) { WX_GETNUMBER_DEFINED(num) }
	DeltaWxIconLocation *iconloc = DNEWCLASS(DeltaWxIconLocation, (new wxIconLocation(file, num)));
#else
	DeltaWxIconLocation *iconloc = DNEWCLASS(DeltaWxIconLocation, (new wxIconLocation(file)));
#endif
	WX_SETOBJECT(IconLocation, iconloc)
}

DLIB_FUNC_START(iconlocation_destruct, 1, Nil)
	DLIB_WXDELETE(iconlocation, IconLocation, iconloc)
}

DLIB_FUNC_START(iconlocation_getfilename, 1, Nil)
	DLIB_WXGET_BASE(iconlocation, IconLocation, iconloc)
	WX_SETSTRING(iconloc->GetFileName())
}

DLIB_FUNC_START(iconlocation_getindex, 1, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(iconlocation, IconLocation, iconloc)
	WX_SETNUMBER(iconloc->GetIndex())
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

DLIB_FUNC_START(iconlocation_setindex, 2, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(iconlocation, IconLocation, iconloc)
	WX_GETNUMBER(index)
	iconloc->SetIndex(index);
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

DLIB_FUNC_START(iconlocation_isok, 1, Nil)
	DLIB_WXGET_BASE(iconlocation, IconLocation, iconloc)
	WX_SETBOOL(iconloc->IsOk())
}

DLIB_FUNC_START(iconlocation_setfilename, 2, Nil)
	DLIB_WXGET_BASE(iconlocation, IconLocation, iconloc)
	WX_GETSTRING(filename)
	iconloc->SetFileName(filename);
}
