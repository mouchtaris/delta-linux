#include "DeltaWxTextCtrl.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxKeyEvent.h"
#include "DeltaWxTextAttr.h"
#include "DeltaWxDropFilesEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(textctrl, name)
#define WX_FUNC(name) WX_FUNC1(textctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(appendtext)
WX_FUNC_DEF(cancopy)
WX_FUNC_DEF(cancut)
WX_FUNC_DEF(canpaste)
WX_FUNC_DEF(canredo)
WX_FUNC_DEF(canundo)
WX_FUNC_DEF(clear)
WX_FUNC_DEF(copy)
WX_FUNC_DEF(create)
WX_FUNC_DEF(cut)
WX_FUNC_DEF(discardedits)
WX_FUNC_DEF(emulatekeypress)
WX_FUNC_DEF(getdefaultstyle)
WX_FUNC_DEF(getinsertionpoint)
WX_FUNC_DEF(getlastposition)
WX_FUNC_DEF(getlinelength)
WX_FUNC_DEF(getlinetext)
WX_FUNC_DEF(getnumberoflines)
WX_FUNC_DEF(getrange)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(getstringselection)
WX_FUNC_DEF(getstyle)
WX_FUNC_DEF(getvalue)
WX_FUNC_DEF(hittest)
WX_FUNC_DEF(iseditable)
WX_FUNC_DEF(isempty)
WX_FUNC_DEF(ismodified)
WX_FUNC_DEF(ismultiline)
WX_FUNC_DEF(issingleline)
WX_FUNC_DEF(loadfile)
WX_FUNC_DEF(markdirty)
WX_FUNC_DEF(ondropfiles)
WX_FUNC_DEF(paste)
WX_FUNC_DEF(positiontoxy)
WX_FUNC_DEF(redo)
WX_FUNC_DEF(remove)
WX_FUNC_DEF(replace)
WX_FUNC_DEF(savefile)
WX_FUNC_DEF(setdefaultstyle)
WX_FUNC_DEF(seteditable)
WX_FUNC_DEF(setinsertionpoint)
WX_FUNC_DEF(setinsertionpointend)
WX_FUNC_DEF(setmaxlength)
WX_FUNC_DEF(setmodified)
WX_FUNC_DEF(setselection)
WX_FUNC_DEF(setstyle)
WX_FUNC_DEF(setvalue)
WX_FUNC_DEF(changevalue)
WX_FUNC_DEF(showposition)
WX_FUNC_DEF(undo)
WX_FUNC_DEF(writetext)
WX_FUNC_DEF(xytoposition)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(appendtext),
	WX_FUNC(cancopy),
	WX_FUNC(cancut),
	WX_FUNC(canpaste),
	WX_FUNC(canredo),
	WX_FUNC(canundo),
	WX_FUNC(clear),
	WX_FUNC(copy),
	WX_FUNC(create),
	WX_FUNC(cut),
	WX_FUNC(discardedits),
	WX_FUNC(emulatekeypress),
	WX_FUNC(getdefaultstyle),
	WX_FUNC(getinsertionpoint),
	WX_FUNC(getlastposition),
	WX_FUNC(getlinelength),
	WX_FUNC(getlinetext),
	WX_FUNC(getnumberoflines),
	WX_FUNC(getrange),
	WX_FUNC(getselection),
	WX_FUNC(getstringselection),
	WX_FUNC(getstyle),
	WX_FUNC(getvalue),
	WX_FUNC(hittest),
	WX_FUNC(iseditable),
	WX_FUNC(isempty),
	WX_FUNC(ismodified),
	WX_FUNC(ismultiline),
	WX_FUNC(issingleline),
	WX_FUNC(loadfile),
	WX_FUNC(markdirty),
	WX_FUNC(ondropfiles),
	WX_FUNC(paste),
	WX_FUNC(positiontoxy),
	WX_FUNC(redo),
	WX_FUNC(remove),
	WX_FUNC(replace),
	WX_FUNC(savefile),
	WX_FUNC(setdefaultstyle),
	WX_FUNC(seteditable),
	WX_FUNC(setinsertionpoint),
	WX_FUNC(setinsertionpointend),
	WX_FUNC(setmaxlength),
	WX_FUNC(setmodified),
	WX_FUNC(setselection),
	WX_FUNC(setstyle),
	WX_FUNC(setvalue),
	WX_FUNC(changevalue),
	WX_FUNC(showposition),
	WX_FUNC(undo),
	WX_FUNC(writetext),
	WX_FUNC(xytoposition)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "xytoposition")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(TextCtrl, "textctrl", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TextCtrl, val, textctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxControl *_parent = DLIB_WXTYPECAST_BASE(Control, val, control);
	DeltaWxControl *parent = DNEWCLASS(DeltaWxControl, (_parent));
	WX_SETOBJECT_EX(*at, Control, parent)
	return true;
}

static bool GetDefaultStyle (void* val, DeltaValue* at) 
{
	wxTextCtrl *textctrl = DLIB_WXTYPECAST_BASE(TextCtrl, val, textctrl);
	DeltaWxTextAttr *retval = DNEWCLASS(DeltaWxTextAttr, (new wxTextAttr(textctrl->GetDefaultStyle())));
	WX_SETOBJECT_EX(*at, TextAttr, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "defaultStyle",		&GetDefaultStyle,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TextCtrl, textctrl);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(textctrl_construct, 0, 8, Nil)
	wxTextCtrl *wxtextctrl = (wxTextCtrl*) 0;
	DeltaWxTextCtrl *textctrl = (DeltaWxTextCtrl*) 0;
	if (n == 0) {
		wxtextctrl = new wxTextCtrl();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxString value = wxEmptyString, name = wxTextCtrlNameStr;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		if (n >= 3) { WX_GETSTRING_DEFINED(value) }
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		wxtextctrl = new wxTextCtrl(parent, id, value, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxtextctrl) textctrl = DNEWCLASS(DeltaWxTextCtrl, (wxtextctrl));
	WX_SETOBJECT(TextCtrl, textctrl)
}

DLIB_FUNC_START(textctrl_destruct, 1, Nil)
	DLIB_WXDELETE(textctrl, TextCtrl, textctrl)
}

DLIB_FUNC_START(textctrl_appendtext, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETSTRING(text)
	textctrl->AppendText(text);
}

DLIB_FUNC_START(textctrl_cancopy, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->CanCopy())
}

DLIB_FUNC_START(textctrl_cancut, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->CanCut())
}

DLIB_FUNC_START(textctrl_canpaste, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->CanPaste())
}

DLIB_FUNC_START(textctrl_canredo, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->CanRedo())
}

DLIB_FUNC_START(textctrl_canundo, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->CanUndo())
}

DLIB_FUNC_START(textctrl_clear, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	textctrl->Clear();
}

DLIB_FUNC_START(textctrl_copy, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	textctrl->Copy();
}

WX_FUNC_ARGRANGE_START(textctrl_create, 3, 9, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxString value = wxEmptyString, name = wxTextCtrlNameStr;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = 0;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	if (n >= 4) { WX_GETSTRING_DEFINED(value) }
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(textctrl->Create(parent, id, value, pos, size, style, *validator, name))
}

DLIB_FUNC_START(textctrl_cut, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	textctrl->Cut();
}

DLIB_FUNC_START(textctrl_discardedits, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	textctrl->DiscardEdits();
}

DLIB_FUNC_START(textctrl_emulatekeypress, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	DLIB_WXGET_BASE(keyevent, KeyEvent, evt)
	WX_SETBOOL(textctrl->EmulateKeyPress(*evt))
}

DLIB_FUNC_START(textctrl_getdefaultstyle, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	DeltaWxTextAttr *retval = DNEWCLASS(DeltaWxTextAttr, (new wxTextAttr(textctrl->GetDefaultStyle())));
	WX_SETOBJECT(TextAttr, retval)
}

DLIB_FUNC_START(textctrl_getinsertionpoint, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETNUMBER(textctrl->GetInsertionPoint())
}

DLIB_FUNC_START(textctrl_getlastposition, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETNUMBER(textctrl->GetLastPosition())
}

DLIB_FUNC_START(textctrl_getlinelength, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(lineNo)
	WX_SETNUMBER(textctrl->GetLineLength(lineNo))
}

DLIB_FUNC_START(textctrl_getlinetext, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(lineNo)
	WX_SETSTRING(textctrl->GetLineText(lineNo))
}

DLIB_FUNC_START(textctrl_getnumberoflines, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETNUMBER(textctrl->GetNumberOfLines())
}

DLIB_FUNC_START(textctrl_getrange, 3, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(from)
	WX_GETNUMBER(to)
	WX_SETSTRING(textctrl->GetRange(from, to))
}

DLIB_FUNC_START(textctrl_getselection, 3, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETTABLE(from_table)
	WX_GETTABLE(to_table)
	long from, to;
	textctrl->GetSelection(&from, &to);
	WX_SETTABLE_RETVAL(from_table, DeltaValue((DeltaNumberValueType)from))
	WX_SETTABLE_RETVAL(to_table, DeltaValue((DeltaNumberValueType)to))
}

DLIB_FUNC_START(textctrl_getstringselection, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETSTRING(textctrl->GetStringSelection())
}

DLIB_FUNC_START(textctrl_getstyle, 3, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(position)
	WX_GETTABLE(style_table)
	wxTextAttr style;
	WX_SETBOOL(textctrl->GetStyle(position, style))
	DeltaWxTextAttr *retval = DNEWCLASS(DeltaWxTextAttr, (new wxTextAttr(style)));
	DeltaValue value;
	WX_SETOBJECT_EX(value, TextAttr, retval)
	WX_SETTABLE_RETVAL(style_table, value)
}

DLIB_FUNC_START(textctrl_getvalue, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETSTRING(textctrl->GetValue())
}

DLIB_FUNC_START(textctrl_hittest, 4, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	DLIB_WXGETPOINT_BASE(pt)
	WX_GETTABLE(col_table)
	WX_GETTABLE(row_table)
	long col, row;
	WX_SETNUMBER(textctrl->HitTest(*pt, &col, &row))
	WX_SETTABLE_RETVAL(col_table, DeltaValue((DeltaNumberValueType)col))
	WX_SETTABLE_RETVAL(row_table, DeltaValue((DeltaNumberValueType)row))
}

DLIB_FUNC_START(textctrl_iseditable, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->IsEditable())
}

DLIB_FUNC_START(textctrl_isempty, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->IsEmpty())
}

DLIB_FUNC_START(textctrl_ismodified, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->IsModified())
}

DLIB_FUNC_START(textctrl_issingleline, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->IsSingleLine())
}

DLIB_FUNC_START(textctrl_ismultiline, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_SETBOOL(textctrl->IsMultiLine())
}

WX_FUNC_ARGRANGE_START(textctrl_loadfile, 2, 3, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETSTRING(filename)
	int fileType = wxTEXT_TYPE_ANY;
	if (n >= 3) { WX_GETDEFINE_DEFINED(fileType) }
	WX_SETBOOL(textctrl->LoadFile(filename, fileType))
}

DLIB_FUNC_START(textctrl_markdirty, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	textctrl->MarkDirty();
}

DLIB_FUNC_START(textctrl_ondropfiles, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	DLIB_WXGET_BASE(dropfilesevent, DropFilesEvent, evt)
	textctrl->OnDropFiles(*evt);
}

DLIB_FUNC_START(textctrl_paste, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	textctrl->Paste();
}

DLIB_FUNC_START(textctrl_positiontoxy, 4, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(pos)
	WX_GETTABLE(x_table)
	WX_GETTABLE(y_table)
	long x, y;
	WX_SETBOOL(textctrl->PositionToXY(pos, &x, &y))
	WX_SETTABLE_RETVAL(x_table, DeltaValue((DeltaNumberValueType)x))
	WX_SETTABLE_RETVAL(y_table, DeltaValue((DeltaNumberValueType)y))
}

DLIB_FUNC_START(textctrl_redo, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	textctrl->Redo();
}

DLIB_FUNC_START(textctrl_remove, 3, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(from)
	WX_GETNUMBER(to)
	textctrl->Remove(from, to);
}

DLIB_FUNC_START(textctrl_replace, 4, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(from)
	WX_GETNUMBER(to)
	WX_GETSTRING(value)
	textctrl->Replace(from, to, value);
}

WX_FUNC_ARGRANGE_START(textctrl_savefile, 2, 3, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETSTRING(filename)
	int fileType = wxTEXT_TYPE_ANY;
	if (n >= 3) { WX_GETDEFINE_DEFINED(fileType) }
	WX_SETBOOL(textctrl->SaveFile(filename, fileType))
}

DLIB_FUNC_START(textctrl_setdefaultstyle, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	DLIB_WXGET_BASE(textattr, TextAttr, style)
	WX_SETBOOL(textctrl->SetDefaultStyle(*style))
}

DLIB_FUNC_START(textctrl_seteditable, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETBOOL(editable)
	textctrl->SetEditable(editable);
}

DLIB_FUNC_START(textctrl_setinsertionpoint, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(pos)
	textctrl->SetInsertionPoint(pos);
}

DLIB_FUNC_START(textctrl_setinsertionpointend, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	textctrl->SetInsertionPointEnd();
}

DLIB_FUNC_START(textctrl_setmaxlength, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(len)
	textctrl->SetMaxLength(len);
}

DLIB_FUNC_START(textctrl_setmodified, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETBOOL(modified)
	textctrl->SetModified(modified);
}

DLIB_FUNC_START(textctrl_setselection, 3, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(from)
	WX_GETNUMBER(to)
	textctrl->SetSelection(from, to);
}

DLIB_FUNC_START(textctrl_setstyle, 4, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(start)
	WX_GETNUMBER(end)
	DLIB_WXGET_BASE(textattr, TextAttr, style)
	WX_SETBOOL(textctrl->SetStyle(start, end, *style))
}

DLIB_FUNC_START(textctrl_setvalue, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETSTRING(value)
	textctrl->SetValue(value);
}

DLIB_FUNC_START(textctrl_changevalue, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETSTRING(value)
	textctrl->ChangeValue(value);
}

DLIB_FUNC_START(textctrl_showposition, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(pos)
	textctrl->ShowPosition(pos);
}

DLIB_FUNC_START(textctrl_undo, 1, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	textctrl->Undo();
}

DLIB_FUNC_START(textctrl_writetext, 2, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETSTRING(text)
	textctrl->WriteText(text);
}

DLIB_FUNC_START(textctrl_xytoposition, 3, Nil)
	DLIB_WXGET_BASE(textctrl, TextCtrl, textctrl)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	WX_SETNUMBER(textctrl->XYToPosition(x, y))
}
