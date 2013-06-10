#include "DeltaWxTextValidator.h"
#include "DeltaWxValidator.h"
#include "DeltaWxWindow.h"
#include "DeltaWxKeyEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(textvalidator, name)
#define WX_FUNC(name) WX_FUNC1(textvalidator, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(validate)
WX_FUNC_DEF(transfertowindow)
WX_FUNC_DEF(transferfromwindow)
WX_FUNC_DEF(getstyle)
WX_FUNC_DEF(setstyle)
WX_FUNC_DEF(getincludes)
WX_FUNC_DEF(setincludes)
WX_FUNC_DEF(getexcludes)
WX_FUNC_DEF(setexcludes)
WX_FUNC_DEF(isincharincludes)
WX_FUNC_DEF(isnotincharexcludes)
WX_FUNC_DEF(onchar)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(validate),
	WX_FUNC(transfertowindow),
	WX_FUNC(transferfromwindow),
	WX_FUNC(getstyle),
	WX_FUNC(setstyle),
	WX_FUNC(getincludes),
	WX_FUNC(setincludes),
	WX_FUNC(getexcludes),
	WX_FUNC(setexcludes),
	WX_FUNC(isincharincludes),
	WX_FUNC(isnotincharexcludes),
	WX_FUNC(onchar)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "onchar")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(TextValidator, "textvalidator", Validator)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TextValidator, val, textvalidator) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Validator, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Validator",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TextValidator,textvalidator)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(textvalidator_construct, 0, 2, Nil)
	long style = wxFILTER_NONE;
	wxString *val = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 2) { WX_GETSTRING(str) val = &str; }
	WX_SETOBJECT(TextValidator, new wxTextValidator(style, val))
}

WX_FUNC_START(textvalidator_destruct, 1, Nil)
	DLIB_WXDELETE(textvalidator, TextValidator, validator)
}

WX_FUNC_START(textvalidator_copy, 2, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	DLIB_WXGET_BASE(textvalidator, TextValidator, other)
	WX_SETBOOL(validator->Copy(*other))
}

WX_FUNC_START(textvalidator_validate, 2, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	DLIB_WXGET_BASE(window, Window, win)
	WX_SETBOOL(validator->Validate(win))
}

WX_FUNC_START(textvalidator_transfertowindow, 1, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	WX_SETBOOL(validator->TransferToWindow())
}

WX_FUNC_START(textvalidator_transferfromwindow, 1, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	WX_SETBOOL(validator->TransferFromWindow())
}

WX_FUNC_START(textvalidator_getstyle, 1, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	WX_SETNUMBER(validator->GetStyle())
}

WX_FUNC_START(textvalidator_setstyle, 2, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	WX_GETDEFINE(style)
	validator->SetStyle(style);
}

WX_FUNC_START(textvalidator_getincludes, 1, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	wxArrayString includes = validator->GetIncludes();
	DLIB_RETVAL_REF.FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, num = (int)includes.Count(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		WX_SETSTRING_EX(content, includes[i])
		DLIB_RETVAL_REF.ToTable()->Set(index, content);
	}
}

WX_FUNC_START(textvalidator_setincludes, 2, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	WX_GETTABLE(table)
	wxArrayString includes;
	for (int i = 0, num = (int)table->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		table->Get(index, &content);
		if (content.Type() == DeltaValue_String) {
			includes.Add(wxString(content.ToString().c_str(), wxConvUTF8));
		}
	}
	validator->SetIncludes(includes);
}

WX_FUNC_START(textvalidator_getexcludes, 1, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	wxArrayString excludes = validator->GetExcludes();
	DLIB_RETVAL_REF.FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, num = (int)excludes.Count(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		WX_SETSTRING_EX(content, excludes[i])
		DLIB_RETVAL_REF.ToTable()->Set(index, content);
	}
}

WX_FUNC_START(textvalidator_setexcludes, 2, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	WX_GETTABLE(table)
	wxArrayString excludes;
	for (int i = 0, num = (int)table->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		table->Get(index, &content);
		if (content.Type() == DeltaValue_String) {
			excludes.Add(wxString(content.ToString().c_str(), wxConvUTF8));
		}
	}
	validator->SetExcludes(excludes);
}

WX_FUNC_START(textvalidator_isincharincludes, 2, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	WX_GETSTRING(val)
	bool retval;
#if wxCHECK_VERSION(2, 9, 0)
	retval = false;
	wxArrayString& array = validator->GetIncludes();
	for (unsigned i = 0; i < array.size(); ++i)
		if (array.Item(i) == val) {
			retval = true;
			break;
		}
#else
	retval = validator->IsInCharIncludes(val);
#endif
	WX_SETBOOL(retval)
}

WX_FUNC_START(textvalidator_isnotincharexcludes, 2, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	WX_GETSTRING(val)
	bool retval;
#if wxCHECK_VERSION(2, 9, 0)
	retval = true;
	wxArrayString& array = validator->GetExcludes();
	for (unsigned i = 0; i < array.size(); ++i)
		if (array.Item(i) == val) {
			retval = false;
			break;
		}
#else
	retval = validator->IsNotInCharExcludes(val);
#endif
	WX_SETBOOL(retval)
}

WX_FUNC_START(textvalidator_onchar, 2, Nil)
	DLIB_WXGET_BASE(textvalidator, TextValidator, validator)
	DLIB_WXGET_BASE(keyevent, KeyEvent, ev)
	validator->OnChar(*ev);
}
