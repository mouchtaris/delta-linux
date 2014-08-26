#include "DeltaWxListBox.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(listbox, name)
#define WX_FUNC(name) WX_FUNC1(listbox, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(deselect)
WX_FUNC_DEF(getselections)
WX_FUNC_DEF(insertitems)
WX_FUNC_DEF(hittest)
WX_FUNC_DEF(isselected)
WX_FUNC_DEF(set)
WX_FUNC_DEF(setfirstitem)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(deselect),
	WX_FUNC(getselections),
	WX_FUNC(insertitems),
	WX_FUNC(hittest),
	WX_FUNC(isselected),
	WX_FUNC(set),
	WX_FUNC(setfirstitem)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "setfirstitem")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ListBox, "listbox", ControlWithItems)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ListBox, val, listbox) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(ListBox, listbox)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(listbox_construct, 0, 8, Nil)
	wxListBox *listbox = (wxListBox*) 0;
	if (n == 0) {
		listbox = new wxListBox();
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
		listbox = new wxListBox(parent, id, pos, size, choices, style, *validator, name);
	}
	WX_SET_WINDOW_OBJECT(ListBox, listbox)
}

WX_FUNC_ARGRANGE_START(listbox_create, 3, 9, Nil)
	DLIB_WXGET_BASE(listbox, ListBox, listbox)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	wxArrayString choices;
	long style = 0;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxListBoxNameStr;
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
	WX_SETBOOL(listbox->Create(parent, id, pos, size, choices, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(listbox);
}

WX_FUNC_START(listbox_deselect, 2, Nil)
	DLIB_WXGET_BASE(listbox, ListBox, listbox)
	WX_GETNUMBER(num)
	listbox->Deselect(num);
}

WX_FUNC_START(listbox_getselections, 2, Nil)
	DLIB_WXGET_BASE(listbox, ListBox, listbox)
	WX_GETTABLE(selections_table)
	wxArrayInt selections;
	WX_SETNUMBER(listbox->GetSelections(selections))
	for (int i = 0, num = (int)selections.capacity(); i < num; ++i) {
		DeltaValue value;
		value.FromNumber(selections[i]);
		selections_table->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
}

WX_FUNC_START(listbox_insertitems, 3, Nil)
	DLIB_WXGET_BASE(listbox, ListBox, listbox)
	WX_GETTABLE(nItems_table)
	wxArrayString nItems;
	WX_GETNUMBER(pos)
	for (int i = 0, num = (int)nItems_table->Total(); i < num; ++i) {
		DeltaValue value;
		nItems_table->Get(DeltaValue(DeltaNumberValueType(i)), &value);
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String)
			nItems.Add(wxString(value.ToString().c_str(), wxConvUTF8));
	}
	listbox->InsertItems(nItems, pos);
}

WX_FUNC_START(listbox_hittest, 2, Nil)
	DLIB_WXGET_BASE(listbox, ListBox, listbox)
	DLIB_WXGETPOINT_BASE(point)
	WX_SETNUMBER(listbox->HitTest(*point))
}

WX_FUNC_START(listbox_isselected, 2, Nil)
	DLIB_WXGET_BASE(listbox, ListBox, listbox)
	WX_GETNUMBER(num)
	WX_SETBOOL(listbox->IsSelected(num))
}

WX_FUNC_START(listbox_set, 2, Nil)
	DLIB_WXGET_BASE(listbox, ListBox, listbox)
	WX_GETTABLE(choices_table)
	wxArrayString choices;
	for (int i = 0, num = choices_table->Total(); i < num; ++i) {
		DeltaValue value;
		choices_table->Get(DeltaValue(DeltaNumberValueType(i)), &value);
		if (value.Type() == DeltaValue_String) {
			choices.Add(wxString(value.ToString().c_str(), wxConvUTF8));
		}
	}
	listbox->Set(choices);
}

WX_FUNC_START(listbox_setfirstitem, 2, Nil)
	DLIB_WXGET_BASE(listbox, ListBox, listbox)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETSTRING(string)
		listbox->SetFirstItem(string);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(num)
		listbox->SetFirstItem(num);
	}
}
