#include "DeltaWxValidator.h"
#include "DeltaWxEvtHandler.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(validator, name)
#define WX_FUNC(name) WX_FUNC1(validator, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(copy)
WX_FUNC_DEF(getwindow)
WX_FUNC_DEF(setwindow)
WX_FUNC_DEF(issilent)
WX_FUNC_DEF(setbellonerror)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(issilent),
	WX_FUNC(setbellonerror),
	WX_FUNC(destruct),
	WX_FUNC(copy),
	WX_FUNC(getwindow),
	WX_FUNC(setwindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(3, uarraysize(funcs) - 3, "destruct", "setwindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Validator, "validator", EvtHandler)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Validator, val, validator) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, EvtHandler, val)
	return true;
}

static bool GetWindow (void* val, DeltaValue* at) 
{
	wxValidator *validator = DLIB_WXTYPECAST_BASE(Validator, val, validator);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, validator->GetWindow())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "EvtHandler",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "validatorWindow",	&GetWindow,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Validator,validator)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(validator_construct, 0, 1, Nil)
	wxValidator *validator = (wxValidator*) 0;
	if (n == 0)
		validator = new wxValidator();
	else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		std::string val = DPTR(vm)->GetActualArg(_argNo)->ToString();
		if (val == "DefaultValidator" ||
			val == "wxDefaultValidator")
			validator = (wxValidator*)&wxDefaultValidator;
	}
	WX_SETOBJECT(Validator, validator)
}

WX_FUNC_START(validator_destruct, 1, Nil)
	DLIB_WXDELETE(validator, Validator, validator)
}

WX_FUNC_START(validator_copy, 2, Nil)
	DLIB_WXGET_BASE(validator, Validator, validator)
	DLIB_WXGET_BASE(validator, Validator, other)
	WX_SETBOOL(validator->Copy(*other))
}

WX_FUNC_START(validator_getwindow, 1, Nil)
	DLIB_WXGET_BASE(validator, Validator, validator)
	WX_SETOBJECT(Window, validator->GetWindow())
}

WX_FUNC_START(validator_setwindow, 2, Nil)
	DLIB_WXGET_BASE(validator, Validator, validator)
	DLIB_WXGET_BASE(window, Window, window)
	validator->SetWindow(window);
}

WX_FUNC_START(validator_issilent, 0, Nil)
	WX_SETBOOL(wxValidator::IsSilent())
}

WX_FUNC_ARGRANGE_START(validator_setbellonerror, 0, 1, Nil)
	bool doIt = true;
	if (n >= 1) { WX_GETBOOL_DEFINED(doIt) }
	wxValidator::SetBellOnError(doIt);
}
