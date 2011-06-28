#include "DeltaWxGenericValidator.h"
#include "DeltaWxValidator.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(genericvalidator, name)
#define WX_FUNC(name) WX_FUNC1(genericvalidator, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(copy)
WX_FUNC_DEF(transferfromwindow)
WX_FUNC_DEF(transfertowindow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(copy),
	WX_FUNC(transferfromwindow),
	WX_FUNC(transfertowindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "transfertowindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(GenericValidator, "genericvalidator", Validator)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(GenericValidator, val, genericvalidator) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxValidator *_parent = DLIB_WXTYPECAST_BASE(Validator, val, validator);
	DeltaWxValidator *parent = DNEWCLASS(DeltaWxValidator, (_parent));
	WX_SETOBJECT_EX(*at, Validator, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Validator",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(GenericValidator, genericvalidator);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(genericvalidator_construct, 1, Nil)
	DeltaWxGenericValidator *validator = (DeltaWxGenericValidator*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Bool) {
		WX_GETBOOL(val)
		validator = DNEWCLASS(DeltaWxGenericValidator, (&val));
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(val)
		validator = DNEWCLASS(DeltaWxGenericValidator, (&val));
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(double_val)
		int val = (int)double_val;
		validator = DNEWCLASS(DeltaWxGenericValidator, (&val));
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Table) {
		WX_GETTABLE(table)
		wxArrayInt val;
		for (int i = 0, num = table->Total(); i < num; ++i) {
			DeltaValue index((DeltaNumberValueType)i), content;
			table->Get(index, &content);
			if (content.Type() == DeltaValue_Number) {
				val.Add(content.ToNumber());
			}
		}
		validator = DNEWCLASS(DeltaWxGenericValidator, (&val));
	}
	WX_SETOBJECT(GenericValidator, validator)
}

DLIB_FUNC_START(genericvalidator_destruct, 1, Nil)
	DLIB_WXDELETE(genericvalidator, GenericValidator, validator)
}

DLIB_FUNC_START(genericvalidator_copy, 2, Nil)
	DLIB_WXGET_BASE(genericvalidator, GenericValidator, validator)
	DLIB_WXGET_BASE(genericvalidator, GenericValidator, other)
	WX_SETBOOL(validator->Copy(*other))
}

DLIB_FUNC_START(genericvalidator_transferfromwindow, 1, Nil)
	DLIB_WXGET_BASE(genericvalidator, GenericValidator, validator)
	WX_SETBOOL(validator->TransferFromWindow())
}

DLIB_FUNC_START(genericvalidator_transfertowindow, 1, Nil)
	DLIB_WXGET_BASE(genericvalidator, GenericValidator, validator)
	WX_SETBOOL(validator->TransferToWindow())
}
