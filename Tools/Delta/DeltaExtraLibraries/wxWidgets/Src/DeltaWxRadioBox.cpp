#include "DeltaWxRadioBox.h"
#include "DeltaWxControlWithItems.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxToolTip.h"
#include "DeltaWxStaticBox.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(radiobox, name)
#define WX_FUNC(name) WX_FUNC1(radiobox, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(enable)
WX_FUNC_DEF(findstring)
WX_FUNC_DEF(getcolumncount)
WX_FUNC_DEF(getitemhelptext)
WX_FUNC_DEF(getitemtooltip)
WX_FUNC_DEF(getitemfrompoint)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(getrowcount)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(getstringselection)
WX_FUNC_DEF(getstring)
WX_FUNC_DEF(isitemenabled)
WX_FUNC_DEF(isitemshown)
WX_FUNC_DEF(setitemhelptext)
WX_FUNC_DEF(setlabel)
WX_FUNC_DEF(setselection)
WX_FUNC_DEF(setstringselection)
WX_FUNC_DEF(setitemtooltip)
WX_FUNC_DEF(show)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(enable),
	WX_FUNC(findstring),
	WX_FUNC(getcolumncount),
	WX_FUNC(getitemhelptext),
	WX_FUNC(getitemtooltip),
	WX_FUNC(getitemfrompoint),
	WX_FUNC(getlabel),
	WX_FUNC(getrowcount),
	WX_FUNC(getselection),
	WX_FUNC(getstringselection),
	WX_FUNC(getstring),
	WX_FUNC(isitemenabled),
	WX_FUNC(isitemshown),
	WX_FUNC(setitemhelptext),
	WX_FUNC(setlabel),
	WX_FUNC(setselection),
	WX_FUNC(setstringselection),
	WX_FUNC(setitemtooltip),
	WX_FUNC(show)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "show")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(RadioBox, "radiobox", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(RadioBox, val, radiobox) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetColumnCount (void* val, DeltaValue* at) 
{
	wxRadioBox *radiobox = DLIB_WXTYPECAST_BASE(RadioBox, val, radiobox);
	WX_SETNUMBER_EX(*at, radiobox->GetColumnCount())
	return true;
}

static bool GetRowCount (void* val, DeltaValue* at) 
{
	wxRadioBox *radiobox = DLIB_WXTYPECAST_BASE(RadioBox, val, radiobox);
	WX_SETNUMBER_EX(*at, radiobox->GetRowCount())
	return true;
}

static bool GetItemTooltips (void* val, DeltaValue* at) 
{
	wxRadioBox *radiobox = DLIB_WXTYPECAST_BASE(RadioBox, val, radiobox);
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, n = radiobox->GetCount(); i < n; ++i) {
		DeltaValue value;
		WX_SETOBJECT_NO_CONTEXT_EX(value, ToolTip, radiobox->GetItemToolTip(i))
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static bool GetItemHelpTexts (void* val, DeltaValue* at) 
{
	wxRadioBox *radiobox = DLIB_WXTYPECAST_BASE(RadioBox, val, radiobox);
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, n = radiobox->GetCount(); i < n; ++i) {
		DeltaValue value;
		wxString string = radiobox->GetItemHelpText(i);
		if (!string.empty()) {
			WX_SETSTRING_EX(value, string)
			at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
		}
	}
	return true;
}

static bool GetSelectedButton (void* val, DeltaValue* at) 
{
	wxRadioBox *radiobox = DLIB_WXTYPECAST_BASE(RadioBox, val, radiobox);
	WX_SETNUMBER_EX(*at, radiobox->GetSelection())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "StaticBox",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "columnCount",		&GetColumnCount,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rowCount",			&GetRowCount,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "itemTooltips",		&GetItemTooltips,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "itemHelpTexts",		&GetItemHelpTexts,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "selectedButton",		&GetSelectedButton,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(RadioBox,radiobox)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(radiobox_construct, 0, 10, Nil)
	wxRadioBox *box = (wxRadioBox*) 0;
	if (n == 0) {
		box = new wxRadioBox;
	} else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		WX_GETSTRING(label)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		int choices_size = 0, majorDim = 0;
		wxString *choices = NULL;
		long style = wxRA_HORIZONTAL;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxRadioBoxNameStr;
		if (n >= 4) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(sz) size = *sz; }
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
		if (n >= 7) { WX_GETNUMBER_DEFINED(majorDim) }
		if (n >= 8) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 9) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 10) { WX_GETSTRING_DEFINED(name) }
		box = new wxRadioBox(parent, id, label, pos, size, choices_size, choices, majorDim, style, *validator, name);
		if (n >= 6) { DDELARR(choices); }
	}
	WX_SET_WINDOW_OBJECT(RadioBox, box)
}

WX_FUNC_ARGRANGE_START(radiobox_create, 4, 11, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	WX_GETSTRING(label)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	int choices_size = 0, majorDim = 0;
	wxString *choices = NULL;
	long style = wxRA_HORIZONTAL;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxRadioBoxNameStr;
	if (n >= 5) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(sz) size = *sz; }
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
	if (n >= 8) { WX_GETNUMBER_DEFINED(majorDim) }
	if (n >= 9) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 10) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 11) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(rdbox->Create(parent, id, label, pos, size, choices_size, choices, majorDim, style, *validator, name));
	if (n >= 7) { DDELARR(choices); }
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(rdbox);
}

WX_FUNC_ARGRANGE_START(radiobox_enable, 1, 3, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	if (n == 1) { WX_SETBOOL(rdbox->Enable()) }
	else if (n == 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Bool) {
		WX_GETBOOL(enable)
		WX_SETBOOL(rdbox->Enable(enable))
	} else {
		WX_GETNUMBER(num)
		bool enable = true;
		if (n >= 3) { WX_GETBOOL_DEFINED(enable) }
		WX_SETBOOL(rdbox->Enable(num, enable))
	}
}

WX_FUNC_START(radiobox_findstring, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETSTRING(str)
	WX_SETNUMBER(rdbox->FindString(str))
}

WX_FUNC_START(radiobox_getcolumncount, 1, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_SETNUMBER(rdbox->GetColumnCount())
}

WX_FUNC_START(radiobox_getitemhelptext, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETNUMBER(item)
	WX_SETSTRING(rdbox->GetItemHelpText(item))
}

WX_FUNC_START(radiobox_getitemtooltip, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETNUMBER(item)
	wxToolTip* retval	= rdbox->GetItemToolTip(item);
	WX_SETOBJECT(ToolTip, retval)
}

WX_FUNC_START(radiobox_getitemfrompoint, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	DLIB_WXGETPOINT_BASE(pt)
	WX_SETNUMBER(rdbox->GetItemFromPoint(*pt))
}

WX_FUNC_START(radiobox_getlabel, 1, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_SETSTRING(rdbox->GetLabel())
}

WX_FUNC_START(radiobox_getrowcount, 1, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_SETNUMBER(rdbox->GetRowCount())
}

WX_FUNC_START(radiobox_getselection, 1, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_SETNUMBER(rdbox->GetSelection())
}

WX_FUNC_START(radiobox_getstringselection, 1, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_SETSTRING(rdbox->GetStringSelection())
}

WX_FUNC_START(radiobox_getstring, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETNUMBER(num)
	WX_SETSTRING(rdbox->GetString(num))
}

WX_FUNC_START(radiobox_isitemenabled, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETNUMBER(num)
	WX_SETBOOL(rdbox->IsItemEnabled(num))
}

WX_FUNC_START(radiobox_isitemshown, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETNUMBER(num)
	WX_SETBOOL(rdbox->IsItemShown(num))
}

WX_FUNC_START(radiobox_setitemhelptext, 3, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETNUMBER(item)
	WX_GETSTRING(helptext)
	rdbox->SetItemHelpText(item, helptext);
}

WX_FUNC_START(radiobox_setlabel, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETSTRING(label)
	rdbox->SetLabel(label);
}

WX_FUNC_START(radiobox_setselection, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETNUMBER(sel)
	rdbox->SetSelection(sel);
}

WX_FUNC_START(radiobox_setstringselection, 2, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETSTRING(sel)
	rdbox->SetStringSelection(sel);
}

WX_FUNC_START(radiobox_setitemtooltip, 3, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	WX_GETNUMBER(item)
	WX_GETSTRING(text)
	rdbox->SetItemToolTip(item, text);
}

WX_FUNC_ARGRANGE_START(radiobox_show, 2, 3, Nil)
	DLIB_WXGET_BASE(radiobox, RadioBox, rdbox)
	if (n == 1) { WX_SETBOOL(rdbox->Show()) }
	else if (n == 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Bool) {
		WX_GETBOOL(show)
		WX_SETBOOL(rdbox->Show(show))
	} else {
		WX_GETNUMBER(num)
		bool show = true;
		if (n >= 3) { WX_GETBOOL_DEFINED(show) }
		WX_SETBOOL(rdbox->Show(num, show))
	}
}
