#include "DeltaWxGBPosition.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(gbposition, name)
#define WX_FUNC(name) WX_FUNC1(gbposition, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getcol)
WX_FUNC_DEF(getrow)
WX_FUNC_DEF(setcol)
WX_FUNC_DEF(setrow)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getcol),
	WX_FUNC(getrow),
	WX_FUNC(setcol),
	WX_FUNC(setrow),
	WX_FUNC(equal),
	WX_FUNC(notequal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getcol", "notequal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(GBPosition, "gbposition")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(GBPosition, val, gbposition) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetRow (void* val, DeltaValue* at) 
{
	wxGBPosition *gbposition = DLIB_WXTYPECAST_BASE(GBPosition, val, gbposition);
	WX_SETNUMBER_EX(*at, gbposition->GetRow())
	return true;
}

static bool GetCol (void* val, DeltaValue* at) 
{
	wxGBPosition *gbposition = DLIB_WXTYPECAST_BASE(GBPosition, val, gbposition);
	WX_SETNUMBER_EX(*at, gbposition->GetCol())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "row",				&GetRow,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "column",				&GetCol,				DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxGBPositionInitFunc()
{
	GBPositionUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&gbposition_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&gbposition_notequal_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(GBPosition, gbposition,
								   DeltaWxGBPositionInitFunc();,
								   );

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(gbposition_construct, 0, 2, Nil)
	wxGBPosition *gbposition = (wxGBPosition*) 0;
	if (n == 0)
		gbposition = new wxGBPosition();
	else if (n == 2) {
		WX_GETNUMBER(row)
		WX_GETNUMBER(col)
		gbposition = new wxGBPosition(row, col);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			DPTR(vm)->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(GBPosition, gbposition)
}

WX_FUNC_START(gbposition_getcol, 1, Nil)
	DLIB_WXGET_BASE(gbposition, GBPosition, wxgbposition)
	WX_SETNUMBER(wxgbposition->GetCol());
}

WX_FUNC_START(gbposition_getrow, 1, Nil)
	DLIB_WXGET_BASE(gbposition, GBPosition, wxgbposition)
	WX_SETNUMBER(wxgbposition->GetRow());
}

WX_FUNC_START(gbposition_setcol, 2, Nil)
	DLIB_WXGET_BASE(gbposition, GBPosition, wxgbposition)
	WX_GETNUMBER(col)
	wxgbposition->SetCol(col);
}

WX_FUNC_START(gbposition_setrow, 2, Nil)
	DLIB_WXGET_BASE(gbposition, GBPosition, wxgbposition)
	WX_GETNUMBER(row)
	wxgbposition->SetRow(row);
}

WX_FUNC_START(gbposition_equal, 2, Nil)
	DLIB_WXGET_BASE(gbposition, GBPosition, wxgbposition)
	DLIB_WXGET_BASE(gbposition, GBPosition, wxgbposition2)
	WX_SETBOOL(wxgbposition->operator==(*wxgbposition2))
}

WX_FUNC_START(gbposition_notequal, 2, Nil)
	DLIB_WXGET_BASE(gbposition, GBPosition, wxgbposition)
	DLIB_WXGET_BASE(gbposition, GBPosition, wxgbposition2)
	WX_SETBOOL(wxgbposition->operator!=(*wxgbposition2))
}
