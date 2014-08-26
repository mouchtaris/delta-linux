#include "DeltaWxChoice.h"
#include "DeltaWxControlWithItems.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxValidator.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(choice, name)
#define WX_FUNC(name) WX_FUNC1(choice, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getcolumns)
WX_FUNC_DEF(getcurrentselection)
WX_FUNC_DEF(setcolumns)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getcolumns),
	WX_FUNC(getcurrentselection),
	WX_FUNC(setcolumns)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "setcolumns")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Choice, "choice", ControlWithItems)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Choice, val, choice) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, ControlWithItems, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "ControlWithItems",	&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Choice,choice)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(choice_construct, 0, 8, Nil)
	wxChoice *choice = (wxChoice*) 0;
	if (n == 0) {
		choice = new wxChoice();
	} else if (n >= 4) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		DLIB_WXGETPOINT_BASE(pos)
		DLIB_WXGETSIZE_BASE(size)
		wxArrayString choices;
		long style = 0;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxChoiceNameStr;
		if (n >= 5) {
			WX_GETTABLE(choices_table)
			int num = choices_table->Total();
			for (int i = 0; i < num; ++i) {
				DeltaValue value;
				choices_table->Get(DeltaValue(DeltaNumberValueType(i)), &value);
				if (value.Type() == DeltaValue_String) {
					choices.Add(wxString(value.ToString().c_str(), wxConvUTF8));
				}
			}
		}
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		choice = new wxChoice(parent, id, *pos, *size, choices, style, *validator, name);
	}
	WX_SET_WINDOW_OBJECT(Choice, choice)
}

WX_FUNC_ARGRANGE_START(choice_create, 3, 9, Nil)
	DLIB_WXGET_BASE(choice, Choice, choice)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	wxArrayString choices;
	long style = 0;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxChoiceNameStr;
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) {
		WX_GETTABLE(choices_table)
		int num = choices_table->Total();
		for (int i = 0; i < num; ++i) {
			DeltaValue value;
			choices_table->Get(DeltaValue(DeltaNumberValueType(i)), &value);
			if (value.Type() == DeltaValue_String) {
				choices.Add(wxString(value.ToString().c_str(), wxConvUTF8));
			}
		}
	}
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(choice->Create(parent, id, pos, size, choices, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(choice);
}

WX_FUNC_START(choice_getcolumns, 1, Nil)
	DLIB_WXGET_BASE(choice, Choice, choice)
	WX_SETNUMBER(choice->GetColumns())
}

WX_FUNC_START(choice_getcurrentselection, 1, Nil)
	DLIB_WXGET_BASE(choice, Choice, choice)
	WX_SETNUMBER(choice->GetCurrentSelection())
}

WX_FUNC_ARGRANGE_START(choice_setcolumns, 1, 2, Nil)
	DLIB_WXGET_BASE(choice, Choice, choice)
	int num = 1;
	if (n >= 2) { WX_GETNUMBER_DEFINED(num) }
	choice->SetColumns(num);
}
