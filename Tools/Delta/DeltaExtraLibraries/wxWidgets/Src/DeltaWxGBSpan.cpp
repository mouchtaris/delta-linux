#include "DeltaWxGBSpan.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(gbspan, name)
#define WX_FUNC(name) WX_FUNC1(gbspan, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getcolspan)
WX_FUNC_DEF(getrowspan)
WX_FUNC_DEF(setcolspan)
WX_FUNC_DEF(setrowspan)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getcolspan),
	WX_FUNC(getrowspan),
	WX_FUNC(setcolspan),
	WX_FUNC(setrowspan),
	WX_FUNC(equal),
	WX_FUNC(notequal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getcolspan", "notequal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(GBSpan, "gbspan")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(GBSpan, val, gbspan) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetRowSpan (void* val, DeltaValue* at) 
{
	wxGBSpan *gbspan = DLIB_WXTYPECAST_BASE(GBSpan, val, gbspan);
	WX_SETNUMBER_EX(*at, gbspan->GetRowspan())
	return true;
}

static bool GetColSpan (void* val, DeltaValue* at) 
{
	wxGBSpan *gbspan = DLIB_WXTYPECAST_BASE(GBSpan, val, gbspan);
	WX_SETNUMBER_EX(*at, gbspan->GetColspan())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rowSpan",			&GetRowSpan,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "columnSpan",			&GetColSpan,			DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxGBSpanInitFunc()
{
	GBSpanUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&gbspan_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&gbspan_notequal_LibFunc, binder));
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(GBSpan, gbspan,
								   DeltaWxGBSpanInitFunc();,
								   )

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(gbspan_construct, 0, 2, Nil)
	wxGBSpan *gbspan = (wxGBSpan*) 0;
	if (n == 0)
		gbspan = new wxGBSpan();
	else if (n == 2) {
		WX_GETNUMBER(rowSpan)
		WX_GETNUMBER(colSpan)
		gbspan = new wxGBSpan(rowSpan, colSpan);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			DPTR(vm)->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(GBSpan, gbspan)
}

WX_FUNC_START(gbspan_getcolspan, 1, Nil)
	DLIB_WXGET_BASE(gbspan, GBSpan, wxgbspan)
	WX_SETNUMBER(wxgbspan->GetColspan());
}

WX_FUNC_START(gbspan_getrowspan, 1, Nil)
	DLIB_WXGET_BASE(gbspan, GBSpan, wxgbspan)
	WX_SETNUMBER(wxgbspan->GetRowspan());
}

WX_FUNC_START(gbspan_setcolspan, 2, Nil)
	DLIB_WXGET_BASE(gbspan, GBSpan, wxgbspan)
	WX_GETNUMBER(colSpan)
	wxgbspan->SetColspan(colSpan);
}

WX_FUNC_START(gbspan_setrowspan, 2, Nil)
	DLIB_WXGET_BASE(gbspan, GBSpan, wxgbspan)
	WX_GETNUMBER(rowSpan)
	wxgbspan->SetRowspan(rowSpan);
}

WX_FUNC_START(gbspan_equal, 2, Nil)
	DLIB_WXGET_BASE(gbspan, GBSpan, wxgbspan)
	DLIB_WXGET_BASE(gbspan, GBSpan, wxgbspan2)
	WX_SETBOOL(wxgbspan->operator==(*wxgbspan2))
}

WX_FUNC_START(gbspan_notequal, 2, Nil)
	DLIB_WXGET_BASE(gbspan, GBSpan, wxgbspan)
	DLIB_WXGET_BASE(gbspan, GBSpan, wxgbspan2)
	WX_SETBOOL(wxgbspan->operator!=(*wxgbspan2))
}
