#include "DeltaWxComboBox.h"
#include "DeltaWxControlWithItems.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxChoice.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(combobox, name)
#define WX_FUNC(name) WX_FUNC1(combobox, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(cancopy)
WX_FUNC_DEF(cancut)
WX_FUNC_DEF(canpaste)
WX_FUNC_DEF(canredo)
WX_FUNC_DEF(canundo)
WX_FUNC_DEF(copy)
WX_FUNC_DEF(cut)
WX_FUNC_DEF(getcurrentselection)
WX_FUNC_DEF(getinsertionpoint)
WX_FUNC_DEF(getlastposition)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(paste)
WX_FUNC_DEF(redo)
WX_FUNC_DEF(replace)
WX_FUNC_DEF(remove)
WX_FUNC_DEF(setinsertionpoint)
WX_FUNC_DEF(setinsertionpointend)
WX_FUNC_DEF(setselection)
WX_FUNC_DEF(setvalue)
WX_FUNC_DEF(undo)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(cancopy),
	WX_FUNC(cancut),
	WX_FUNC(canpaste),
	WX_FUNC(canredo),
	WX_FUNC(canundo),
	WX_FUNC(copy),
	WX_FUNC(cut),
	WX_FUNC(getcurrentselection),
	WX_FUNC(getinsertionpoint),
	WX_FUNC(getlastposition),
	WX_FUNC(getselection),
	WX_FUNC(getvalue),
	WX_FUNC(paste),
	WX_FUNC(redo),
	WX_FUNC(replace),
	WX_FUNC(remove),
	WX_FUNC(setinsertionpoint),
	WX_FUNC(setinsertionpointend),
	WX_FUNC(setselection),
	WX_FUNC(setvalue),
	WX_FUNC(undo)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "undo")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ComboBox, "combobox", ControlWithItems)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ComboBox, val, combobox) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxControlWithItems *_parent = DLIB_WXTYPECAST_BASE(ControlWithItems, val, controlwithitems);
	DeltaWxControlWithItems *parent = DNEWCLASS(DeltaWxControlWithItems, (_parent));
	WX_SETOBJECT_EX(*at, ControlWithItems, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "ControlWithItems",	&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ComboBox, combobox);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(combobox_construct, 0, 9, Nil)
	wxComboBox *wxcombo = (wxComboBox*) 0;
	DeltaWxComboBox *combo = (DeltaWxComboBox*) 0;
	if (n == 0) {
		wxcombo = new wxComboBox();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxString value = wxEmptyString, name = wxComboBoxNameStr;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		int choices_size = 0;
		wxString *choices = NULL;
		long style = 0;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		if (n >= 3) { WX_GETSTRING_DEFINED(value) }
		if (n >= 4) { DLIB_WXGET_BASE(point, Point, pt) pos = *pt; }
		if (n >= 5) { DLIB_WXGET_BASE(size, Size, sz) size = *sz; }
		if (n >= 6) {
			WX_GETTABLE(choices_table)
			choices_size = choices_table->Total();
			choices = DNEWARR(wxString, choices_size);
			for (int i = 0; i < choices_size; ++i) {
				DeltaValue value;
				choices_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
				if (value.Type() == DeltaValue_String) {
					wxString str = wxString(value.ToString().c_str(), wxConvUTF8);
					choices[i] = str;
				}
			}
		}
		if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 9) { WX_GETSTRING_DEFINED(name) }
		wxcombo = new wxComboBox(parent, id, value, pos, size, choices_size, choices, style, *validator, name);
		if (n >= 6) { DDELARR(choices); }
	}
	if (wxcombo) combo = DNEWCLASS(DeltaWxComboBox, (wxcombo));
	WX_SETOBJECT(ComboBox, combo)
}

DLIB_FUNC_START(combobox_destruct, 1, Nil)
	DLIB_WXDELETE(combobox, ComboBox, combo)
}

WX_FUNC_ARGRANGE_START(combobox_create, 3, 10, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxString value = wxEmptyString, name = wxComboBoxNameStr;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	int choices_size = 0;
	wxString *choices = NULL;
	long style = 0;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	if (n >= 4) { WX_GETSTRING_DEFINED(value) }
	if (n >= 5) { DLIB_WXGET_BASE(point, Point, pt) pos = *pt; }
	if (n >= 6) { DLIB_WXGET_BASE(size, Size, sz) size = *sz; }
	if (n >= 7) {
		WX_GETTABLE(choices_table)
		choices_size = choices_table->Total();
		choices = DNEWARR(wxString, choices_size);
		for (int i = 0; i < choices_size; ++i) {
			DeltaValue value;
			choices_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
			if (value.Type() == DeltaValue_String) {
				wxString str = wxString(value.ToString().c_str(), wxConvUTF8);
				choices[i] = str;
			}
		}
	}
	if (n >= 8) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 9) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 10) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(combo->Create(parent, id, value, pos, size, choices_size, choices, style, *validator, name))
	if (n >= 7) { DDELARR(choices); }
}

DLIB_FUNC_START(combobox_cancopy, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_SETBOOL(combo->CanCopy());
}

DLIB_FUNC_START(combobox_cancut, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_SETBOOL(combo->CanCut());
}

DLIB_FUNC_START(combobox_canpaste, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_SETBOOL(combo->CanPaste());
}

DLIB_FUNC_START(combobox_canredo, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_SETBOOL(combo->CanRedo());
}

DLIB_FUNC_START(combobox_canundo, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_SETBOOL(combo->CanUndo());
}

DLIB_FUNC_START(combobox_copy, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	combo->Copy();
}

DLIB_FUNC_START(combobox_cut, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	combo->Cut();
}

DLIB_FUNC_START(combobox_getcurrentselection, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_SETNUMBER(combo->GetCurrentSelection());
}

DLIB_FUNC_START(combobox_getinsertionpoint, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_SETNUMBER(combo->GetInsertionPoint());
}

DLIB_FUNC_START(combobox_getlastposition, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_SETNUMBER(combo->GetLastPosition());
}

DLIB_FUNC_START(combobox_getselection, 3, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	long from, to;
	combo->GetSelection(&from, &to);
	WX_GETTABLE(_from)
	WX_GETTABLE(_to)
	WX_SETTABLE_RETVAL(_from, DeltaValue(DeltaNumberValueType(from)))
	WX_SETTABLE_RETVAL(_to, DeltaValue(DeltaNumberValueType(to)))
}

DLIB_FUNC_START(combobox_getvalue, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_SETSTRING(combo->GetValue());
}

DLIB_FUNC_START(combobox_paste, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	combo->Paste();
}

DLIB_FUNC_START(combobox_redo, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	combo->Redo();
}

DLIB_FUNC_START(combobox_replace, 4, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_GETNUMBER(from)
	WX_GETNUMBER(to)
	WX_GETSTRING(text)
	combo->Replace(from, to, text);
}

DLIB_FUNC_START(combobox_remove, 3, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_GETNUMBER(from)
	WX_GETNUMBER(to)
	combo->Remove(from, to);
}

DLIB_FUNC_START(combobox_setinsertionpoint, 2, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_GETNUMBER(pos)
	combo->SetInsertionPoint(pos);
}

DLIB_FUNC_START(combobox_setinsertionpointend, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	combo->SetInsertionPointEnd();
}

DLIB_FUNC_START(combobox_setselection, 3, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_GETNUMBER(from)
	WX_GETNUMBER(to)
	combo->SetSelection(from, to);
}

DLIB_FUNC_START(combobox_setvalue, 2, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	WX_GETSTRING(text)
	combo->SetValue(text);
}

DLIB_FUNC_START(combobox_undo, 1, Nil)
	DLIB_WXGET_BASE(combobox, ComboBox, combo)
	combo->Undo();
}
