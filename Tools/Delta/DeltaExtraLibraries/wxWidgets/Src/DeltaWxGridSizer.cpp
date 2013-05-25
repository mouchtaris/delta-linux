#include "DeltaWxGridSizer.h"
#include "DeltaWxSizer.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(gridsizer, name)
#define WX_FUNC(name) WX_FUNC1(gridsizer, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getcols)
WX_FUNC_DEF(gethgap)
WX_FUNC_DEF(getrows)
WX_FUNC_DEF(getvgap)
WX_FUNC_DEF(setcols)
WX_FUNC_DEF(sethgap)
WX_FUNC_DEF(setrows)
WX_FUNC_DEF(setvgap)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getcols),
	WX_FUNC(gethgap),
	WX_FUNC(getrows),
	WX_FUNC(getvgap),
	WX_FUNC(setcols),
	WX_FUNC(sethgap),
	WX_FUNC(setrows),
	WX_FUNC(setvgap)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setvgap")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(GridSizer, "gridsizer", Sizer)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(GridSizer, val, gridsizer) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Sizer, val)
	return true;
}

static bool GetRows (void* val, DeltaValue* at)
{
	wxGridSizer *sizer = DLIB_WXTYPECAST_BASE(GridSizer, val, gridsizer);
	WX_SETNUMBER_EX(*at, sizer->GetRows())
	return true;
}

static bool GetCols (void* val, DeltaValue* at)
{
	wxGridSizer *sizer = DLIB_WXTYPECAST_BASE(GridSizer, val, gridsizer);
	WX_SETNUMBER_EX(*at, sizer->GetCols())
	return true;
}

static bool GetVGap (void* val, DeltaValue* at)
{
	wxGridSizer *sizer = DLIB_WXTYPECAST_BASE(GridSizer, val, gridsizer);
	WX_SETNUMBER_EX(*at, sizer->GetVGap())
	return true;
}

static bool GetHGap (void* val, DeltaValue* at)
{
	wxGridSizer *sizer = DLIB_WXTYPECAST_BASE(GridSizer, val, gridsizer);
	WX_SETNUMBER_EX(*at, sizer->GetHGap())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Sizer",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rows",				&GetRows,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "cols",				&GetCols,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "verticalGap",		&GetVGap,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "horizontalGap",		&GetHGap,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(GridSizer,gridsizer)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(gridsizer_construct, 1, 4, Nil)
	wxGridSizer *sizer = (wxGridSizer*) 0;
	if (n == 4) {
		WX_GETNUMBER(rows)
		WX_GETNUMBER(cols)
		WX_GETNUMBER(vgap)
		WX_GETNUMBER(hgap)
		sizer = new wxGridSizer(rows, cols, vgap, hgap);
	} else {
		WX_GETNUMBER(cols)
		int vgap = 0, hgap = 0;
		if (n >= 2) { WX_GETNUMBER_DEFINED(vgap) }
		if (n >= 3) { WX_GETNUMBER_DEFINED(hgap) }
		sizer = new wxGridSizer(cols, vgap, hgap);
	}
	WX_SETOBJECT(GridSizer, sizer)
}

WX_FUNC_START(gridsizer_destruct, 1, Nil)
	DLIB_WXDELETE(gridsizer, GridSizer, sizer)
}

WX_FUNC_START(gridsizer_getcols, 1, Nil)
	DLIB_WXGET_BASE(gridsizer, GridSizer, sizer)
	WX_SETNUMBER(sizer->GetCols())
}

WX_FUNC_START(gridsizer_gethgap, 1, Nil)
	DLIB_WXGET_BASE(gridsizer, GridSizer, sizer)
	WX_SETNUMBER(sizer->GetHGap())
}

WX_FUNC_START(gridsizer_getrows, 1, Nil)
	DLIB_WXGET_BASE(gridsizer, GridSizer, sizer)
	WX_SETNUMBER(sizer->GetRows())
}

WX_FUNC_START(gridsizer_getvgap, 1, Nil)
	DLIB_WXGET_BASE(gridsizer, GridSizer, sizer)
	WX_SETNUMBER(sizer->GetVGap())
}

WX_FUNC_START(gridsizer_setcols, 2, Nil)
	DLIB_WXGET_BASE(gridsizer, GridSizer, sizer)
	WX_GETNUMBER(cols)
	sizer->SetCols(cols);
}

WX_FUNC_START(gridsizer_sethgap, 2, Nil)
	DLIB_WXGET_BASE(gridsizer, GridSizer, sizer)
	WX_GETNUMBER(gap)
	sizer->SetHGap(gap);
}

WX_FUNC_START(gridsizer_setrows, 2, Nil)
	DLIB_WXGET_BASE(gridsizer, GridSizer, sizer)
	WX_GETNUMBER(rows)
	sizer->SetRows(rows);
}

WX_FUNC_START(gridsizer_setvgap, 2, Nil)
	DLIB_WXGET_BASE(gridsizer, GridSizer, sizer)
	WX_GETNUMBER(gap)
	sizer->SetVGap(gap);
}
