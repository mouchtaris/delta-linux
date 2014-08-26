#include "DeltaWxCheckListBox.h"
#include "DeltaWxListBox.h"
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
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(checklistbox, name)
#define WX_FUNC(name) WX_FUNC1(checklistbox, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(check)
WX_FUNC_DEF(ischecked)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(check),
	WX_FUNC(ischecked)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "check", "ischecked")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CheckListBox, "checklistbox", ListBox)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CheckListBox, val, checklistbox) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, ListBox, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "ListBox",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(CheckListBox,checklistbox)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(checklistbox_construct, 0, 8, Nil)
	wxCheckListBox *listbox = (wxCheckListBox*) 0;
	if (n == 0) {
		listbox = new wxCheckListBox();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		wxArrayString choices;
		long style = 0;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxListBoxNameStr;
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
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
		listbox = new wxCheckListBox(parent, id, pos, size, choices, style, *validator, name);
	}
	WX_SET_WINDOW_OBJECT(CheckListBox, listbox)
}

WX_FUNC_ARGRANGE_START(checklistbox_check, 2, 3, Nil)
	DLIB_WXGET_BASE(checklistbox, CheckListBox, listbox)
	WX_GETNUMBER(item)
	bool bCheck = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(bCheck) }
	listbox->Check(item, bCheck);
}

WX_FUNC_START(checklistbox_ischecked, 2, Nil)
	DLIB_WXGET_BASE(checklistbox, CheckListBox, listbox)
	WX_GETNUMBER(item)
	WX_SETBOOL(listbox->IsChecked(item))
}
