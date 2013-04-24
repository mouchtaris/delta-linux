#include "DeltaWxFlexGridSizer.h"
#include "DeltaWxGridSizer.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(flexgridsizer, name)
#define WX_FUNC(name) WX_FUNC1(flexgridsizer, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(addgrowablecol)
WX_FUNC_DEF(addgrowablerow)
WX_FUNC_DEF(getflexibledirection)
WX_FUNC_DEF(getnonflexiblegrowmode)
WX_FUNC_DEF(removegrowablecol)
WX_FUNC_DEF(removegrowablerow)
WX_FUNC_DEF(setflexibledirection)
WX_FUNC_DEF(setnonflexiblegrowmode)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(addgrowablecol),
	WX_FUNC(addgrowablerow),
	WX_FUNC(getflexibledirection),
	WX_FUNC(getnonflexiblegrowmode),
	WX_FUNC(removegrowablecol),
	WX_FUNC(removegrowablerow),
	WX_FUNC(setflexibledirection),
	WX_FUNC(setnonflexiblegrowmode)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setnonflexiblegrowmode")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FlexGridSizer, "flexgridsizer", GridSizer)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FlexGridSizer, val, flexgridsizer) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxGridSizer *_parent = DLIB_WXTYPECAST_BASE(GridSizer, val, gridsizer);
	DeltaWxGridSizer *parent = DNEWCLASS(DeltaWxGridSizer, (_parent));
	WX_SETOBJECT_EX(*at, GridSizer, parent)
	return true;
}

static bool GetFlexibleDirection (void* val, DeltaValue* at) 
{
	wxFlexGridSizer *sizer = DLIB_WXTYPECAST_BASE(FlexGridSizer, val, flexgridsizer);
	WX_SETNUMBER_EX(*at, sizer->GetFlexibleDirection())
	return true;
}

static bool GetNonFlexibleGrowMode (void* val, DeltaValue* at) 
{
	wxFlexGridSizer *sizer = DLIB_WXTYPECAST_BASE(FlexGridSizer, val, flexgridsizer);
	WX_SETNUMBER_EX(*at, sizer->GetNonFlexibleGrowMode())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "GridSizer",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "flexibleDirection",	&GetFlexibleDirection,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "nonflexibleGrowMode",&GetNonFlexibleGrowMode,DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FlexGridSizer,flexgridsizer)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(flexgridsizer_construct, 1, 4, Nil)
	wxFlexGridSizer *wxsizer = (wxFlexGridSizer*) 0;
	DeltaWxFlexGridSizer *sizer = (DeltaWxFlexGridSizer*) 0;
	if (n == 4) {
		WX_GETNUMBER(rows)
		WX_GETNUMBER(cols)
		WX_GETNUMBER(vgap)
		WX_GETNUMBER(hgap)
		wxsizer = new wxFlexGridSizer(rows, cols, vgap, hgap);
	} else {
		WX_GETNUMBER(cols)
		int vgap = 0, hgap = 0;
		if (n >= 2) { WX_GETNUMBER_DEFINED(vgap) }
		if (n >= 3) { WX_GETNUMBER_DEFINED(hgap) }
		wxsizer = new wxFlexGridSizer(cols, vgap, hgap);
	}
	if (wxsizer) sizer = DNEWCLASS(DeltaWxFlexGridSizer, (wxsizer));
	WX_SETOBJECT(FlexGridSizer, sizer)
}

DLIB_FUNC_START(flexgridsizer_destruct, 1, Nil)
	DLIB_WXDELETE(flexgridsizer, FlexGridSizer, sizer)
}

WX_FUNC_ARGRANGE_START(flexgridsizer_addgrowablecol, 2, 3, Nil)
	DLIB_WXGET_BASE(flexgridsizer, FlexGridSizer, sizer)
	WX_GETNUMBER(idx)
	int proportion = 0;
	if (n >= 3) { WX_GETNUMBER_DEFINED(proportion) }
	sizer->AddGrowableCol(idx, proportion);
}

WX_FUNC_ARGRANGE_START(flexgridsizer_addgrowablerow, 2, 3, Nil)
	DLIB_WXGET_BASE(flexgridsizer, FlexGridSizer, sizer)
	WX_GETNUMBER(idx)
	int proportion = 0;
	if (n >= 3) { WX_GETNUMBER_DEFINED(proportion) }
	sizer->AddGrowableRow(idx, proportion);
}

DLIB_FUNC_START(flexgridsizer_getflexibledirection, 1, Nil)
	DLIB_WXGET_BASE(flexgridsizer, FlexGridSizer, sizer)
	WX_SETNUMBER(sizer->GetFlexibleDirection())
}

DLIB_FUNC_START(flexgridsizer_getnonflexiblegrowmode, 1, Nil)
	DLIB_WXGET_BASE(flexgridsizer, FlexGridSizer, sizer)
	WX_SETNUMBER(sizer->GetNonFlexibleGrowMode())
}

DLIB_FUNC_START(flexgridsizer_removegrowablecol, 2, Nil)
	DLIB_WXGET_BASE(flexgridsizer, FlexGridSizer, sizer)
	WX_GETNUMBER(idx)
	sizer->RemoveGrowableCol(idx);
}

DLIB_FUNC_START(flexgridsizer_removegrowablerow, 2, Nil)
	DLIB_WXGET_BASE(flexgridsizer, FlexGridSizer, sizer)
	WX_GETNUMBER(idx)
	sizer->RemoveGrowableRow(idx);
}

DLIB_FUNC_START(flexgridsizer_setflexibledirection, 2, Nil)
	DLIB_WXGET_BASE(flexgridsizer, FlexGridSizer, sizer)
	WX_GETDEFINE(direction)
	sizer->SetFlexibleDirection(direction);
}

DLIB_FUNC_START(flexgridsizer_setnonflexiblegrowmode, 2, Nil)
	DLIB_WXGET_BASE(flexgridsizer, FlexGridSizer, sizer)
	WX_GETDEFINE(mode)
	sizer->SetNonFlexibleGrowMode((wxFlexSizerGrowMode)mode);
}
