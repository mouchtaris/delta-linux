#include "DeltaWxToolBar.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxControl.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxToolBarToolBase.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(toolbar, name)
#define WX_FUNC(name) WX_FUNC1(toolbar, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(addcontrol)
WX_FUNC_DEF(addseparator)
WX_FUNC_DEF(addtool)
WX_FUNC_DEF(addchecktool)
WX_FUNC_DEF(addradiotool)
WX_FUNC_DEF(cleartools)
WX_FUNC_DEF(deletetool)
WX_FUNC_DEF(deletetoolbypos)
WX_FUNC_DEF(enabletool)
WX_FUNC_DEF(findbyid)
WX_FUNC_DEF(findcontrol)
WX_FUNC_DEF(findtoolforposition)
WX_FUNC_DEF(gettoolscount)
WX_FUNC_DEF(gettoolsize)
WX_FUNC_DEF(gettoolbitmapsize)
WX_FUNC_DEF(getmargins)
WX_FUNC_DEF(gettoolenabled)
WX_FUNC_DEF(gettoollonghelp)
WX_FUNC_DEF(gettoolpacking)
WX_FUNC_DEF(gettoolpos)
WX_FUNC_DEF(gettoolseparation)
WX_FUNC_DEF(gettoolshorthelp)
WX_FUNC_DEF(gettoolstate)
WX_FUNC_DEF(insertcontrol)
WX_FUNC_DEF(insertseparator)
WX_FUNC_DEF(inserttool)
WX_FUNC_DEF(realize)
WX_FUNC_DEF(removetool)
WX_FUNC_DEF(setmargins)
WX_FUNC_DEF(settoolbitmapsize)
WX_FUNC_DEF(settooldisabledbitmap)
WX_FUNC_DEF(settoollonghelp)
WX_FUNC_DEF(settoolpacking)
WX_FUNC_DEF(settoolshorthelp)
WX_FUNC_DEF(settoolnormalbitmap)
WX_FUNC_DEF(settoolseparation)
WX_FUNC_DEF(toggletool)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(addcontrol),
	WX_FUNC(addseparator),
	WX_FUNC(addtool),
	WX_FUNC(addchecktool),
	WX_FUNC(addradiotool),
	WX_FUNC(cleartools),
	WX_FUNC(deletetool),
	WX_FUNC(deletetoolbypos),
	WX_FUNC(enabletool),
	WX_FUNC(findbyid),
	WX_FUNC(findcontrol),
	WX_FUNC(findtoolforposition),
	WX_FUNC(gettoolscount),
	WX_FUNC(gettoolsize),
	WX_FUNC(gettoolbitmapsize),
	WX_FUNC(getmargins),
	WX_FUNC(gettoolenabled),
	WX_FUNC(gettoollonghelp),
	WX_FUNC(gettoolpacking),
	WX_FUNC(gettoolpos),
	WX_FUNC(gettoolseparation),
	WX_FUNC(gettoolshorthelp),
	WX_FUNC(gettoolstate),
	WX_FUNC(insertcontrol),
	WX_FUNC(insertseparator),
	WX_FUNC(inserttool),
	WX_FUNC(realize),
	WX_FUNC(removetool),
	WX_FUNC(setmargins),
	WX_FUNC(settoolbitmapsize),
	WX_FUNC(settooldisabledbitmap),
	WX_FUNC(settoollonghelp),
	WX_FUNC(settoolpacking),
	WX_FUNC(settoolshorthelp),
	WX_FUNC(settoolnormalbitmap),
	WX_FUNC(settoolseparation),
	WX_FUNC(toggletool)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "addcontrol", "toggletool")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ToolBar, "toolbar", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ToolBar, val, toolbar) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetXMargin (void* val, DeltaValue* at) 
{
	wxToolBar *toolbar = DLIB_WXTYPECAST_BASE(ToolBar, val, toolbar);
	WX_SETNUMBER_EX(*at, toolbar->GetToolMargins().x)
	return true;
}

static bool GetYMargin (void* val, DeltaValue* at) 
{
	wxToolBar *toolbar = DLIB_WXTYPECAST_BASE(ToolBar, val, toolbar);
	WX_SETNUMBER_EX(*at, toolbar->GetToolMargins().y)
	return true;
}

static bool GetMaxRows (void* val, DeltaValue* at) 
{
	wxToolBar *toolbar = DLIB_WXTYPECAST_BASE(ToolBar, val, toolbar);
	WX_SETNUMBER_EX(*at, toolbar->GetMaxRows())
	return true;
}

static bool GetMaxCols (void* val, DeltaValue* at) 
{
	wxToolBar *toolbar = DLIB_WXTYPECAST_BASE(ToolBar, val, toolbar);
	WX_SETNUMBER_EX(*at, toolbar->GetMaxCols())
	return true;
}

static bool GetToolPacking (void* val, DeltaValue* at) 
{
	wxToolBar *toolbar = DLIB_WXTYPECAST_BASE(ToolBar, val, toolbar);
	WX_SETNUMBER_EX(*at, toolbar->GetToolPacking())
	return true;
}

static bool GetToolSeparation (void* val, DeltaValue* at) 
{
	wxToolBar *toolbar = DLIB_WXTYPECAST_BASE(ToolBar, val, toolbar);
	WX_SETNUMBER_EX(*at, toolbar->GetToolSeparation())
	return true;
}

static bool GetDefaultWidth (void* val, DeltaValue* at) 
{
	wxToolBar *toolbar = DLIB_WXTYPECAST_BASE(ToolBar, val, toolbar);
	WX_SETNUMBER_EX(*at, toolbar->GetToolBitmapSize().x)
	return true;
}

static bool GetDefaultHeight (void* val, DeltaValue* at) 
{
	wxToolBar *toolbar = DLIB_WXTYPECAST_BASE(ToolBar, val, toolbar);
	WX_SETNUMBER_EX(*at, toolbar->GetToolBitmapSize().y)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "xMargin",			&GetXMargin,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "yMargin",			&GetYMargin,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maxRows",			&GetMaxRows,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maxCols",			&GetMaxCols,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "toolPacking",		&GetToolPacking,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "toolSeparation",		&GetToolSeparation,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "defaultWidth",		&GetDefaultWidth,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "defaultHeight",		&GetDefaultHeight,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ToolBar,toolbar)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(toolbar_construct, 0, 6, Nil)
	wxToolBar *toolbar = (wxToolBar*) 0;
	if (n == 0) {
		toolbar = new wxToolBar();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxNO_BORDER | wxTB_HORIZONTAL;
		wxString name = wxToolBarNameStr;
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { WX_GETSTRING_DEFINED(name) }
		toolbar = new wxToolBar(parent, id, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(ToolBar, toolbar)
}

WX_FUNC_START(toolbar_addcontrol, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	DLIB_WXGET_BASE(control, Control, control)
	wxToolBarToolBase* retval	= toolbar->AddControl(control);
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_START(toolbar_addseparator, 1, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	wxToolBarToolBase* retval	= toolbar->AddSeparator();
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_ARGRANGE_START(toolbar_addtool, 2, 8, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	wxToolBarToolBase *retval = (wxToolBarToolBase*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
		retval	= toolbar->AddTool(tool);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number ||
				DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
		WX_GETDEFINE(toolid)
		if (n >= 4) {
			WX_GETSTRING(label)
			DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
			if (n == 4) {
				retval	= toolbar->AddTool(toolid, label, *bitmap);
			} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
				WX_GETSTRING(shortHelp)
				int kind = wxITEM_NORMAL;
				if (n >= 6) { WX_GETDEFINE_DEFINED(kind) }
				retval	= toolbar->AddTool(toolid, label, *bitmap,
					shortHelp, (wxItemKind) kind);
			} else {
				wxBitmap bmpDisabled = wxNullBitmap;
				int kind = wxITEM_NORMAL;
				wxString shortHelp = wxEmptyString, longHelp = wxEmptyString;
				if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId)
					{ DLIB_WXGET_BASE(bitmap, Bitmap, bmp) bmpDisabled = *bmp; }
				else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Nil)
					{ ++_argNo; }
				if (n >= 6) { WX_GETDEFINE_DEFINED(kind) }
				if (n >= 7) { WX_GETSTRING_DEFINED(shortHelp) }
				if (n >= 8) { WX_GETSTRING_DEFINED(longHelp) }
				retval	= toolbar->AddTool(toolid, label, *bitmap,
					bmpDisabled, (wxItemKind) kind, shortHelp, longHelp);
			}
		}
	}
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_ARGRANGE_START(toolbar_addchecktool, 4, 7, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	WX_GETSTRING(label)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	wxBitmap bmpDisabled = wxNullBitmap;
	wxString shortHelp = wxEmptyString, longHelp = wxEmptyString;
	if (n >= 5) { DLIB_WXGET_BASE(bitmap, Bitmap, bmp) bmpDisabled = *bmp; }
	if (n >= 6) { WX_GETSTRING_DEFINED(shortHelp) }
	if (n >= 7) { WX_GETSTRING_DEFINED(longHelp) }
	wxToolBarToolBase* retval	= toolbar->AddCheckTool(toolid, label, *bitmap, bmpDisabled,
		shortHelp, longHelp);
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_ARGRANGE_START(toolbar_addradiotool, 4, 7, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	WX_GETSTRING(label)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	wxBitmap bmpDisabled = wxNullBitmap;
	wxString shortHelp = wxEmptyString, longHelp = wxEmptyString;
	if (n >= 5) { DLIB_WXGET_BASE(bitmap, Bitmap, bmp) bmpDisabled = *bmp; }
	if (n >= 6) { WX_GETSTRING_DEFINED(shortHelp) }
	if (n >= 7) { WX_GETSTRING_DEFINED(longHelp) }
	wxToolBarToolBase* retval	= toolbar->AddRadioTool(toolid, label, *bitmap, bmpDisabled,
		shortHelp, longHelp);
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_START(toolbar_cleartools, 1, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	toolbar->ClearTools();
}

WX_FUNC_START(toolbar_deletetool, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	WX_SETBOOL(toolbar->DeleteTool(toolid))
}

WX_FUNC_START(toolbar_deletetoolbypos, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETNUMBER(pos)
	WX_SETBOOL(toolbar->DeleteToolByPos(pos))
}

WX_FUNC_START(toolbar_enabletool, 3, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	WX_GETBOOL(enable)
	toolbar->EnableTool(toolid, enable);
}

WX_FUNC_START(toolbar_findbyid, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	wxToolBarToolBase* retval	= toolbar->FindById(toolid);
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_START(toolbar_findcontrol, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	wxControl* retval	= toolbar->FindControl(toolid);
	WX_SETOBJECT(Control, retval)
}

WX_FUNC_START(toolbar_findtoolforposition, 3, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	wxToolBarToolBase* retval	= toolbar->FindToolForPosition(x, y);
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_START(toolbar_gettoolscount, 1, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_SETNUMBER(toolbar->GetToolsCount())
}

WX_FUNC_START(toolbar_gettoolsize, 1, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(toolbar->GetToolSize()))
}

WX_FUNC_START(toolbar_gettoolbitmapsize, 1, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(toolbar->GetToolBitmapSize()))
}

WX_FUNC_START(toolbar_getmargins, 1, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(toolbar->GetMargins()))
}

WX_FUNC_START(toolbar_gettoolenabled, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	WX_SETBOOL(toolbar->GetToolEnabled(toolid))
}

WX_FUNC_START(toolbar_gettoollonghelp, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	WX_SETSTRING(toolbar->GetToolLongHelp(toolid))
}

WX_FUNC_START(toolbar_gettoolpacking, 1, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_SETNUMBER(toolbar->GetToolPacking())
}

WX_FUNC_START(toolbar_gettoolpos, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	WX_SETNUMBER(toolbar->GetToolPos(toolid))
}

WX_FUNC_START(toolbar_gettoolseparation, 1, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_SETNUMBER(toolbar->GetToolSeparation())
}

WX_FUNC_START(toolbar_gettoolshorthelp, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	WX_SETSTRING(toolbar->GetToolShortHelp(toolid))
}

WX_FUNC_START(toolbar_gettoolstate, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(toolid)
	WX_SETBOOL(toolbar->GetToolState(toolid))
}

WX_FUNC_START(toolbar_insertcontrol, 3, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETNUMBER(pos)
	DLIB_WXGET_BASE(control, Control, control)
	wxToolBarToolBase* retval	= toolbar->InsertControl(pos, control);
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_START(toolbar_insertseparator, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETNUMBER(pos)
	wxToolBarToolBase* retval	= toolbar->InsertSeparator(pos);
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_ARGRANGE_START(toolbar_inserttool, 3, 9, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETNUMBER(pos)
	wxToolBarToolBase *retval = (wxToolBarToolBase*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
		retval	= toolbar->InsertTool(pos, tool);
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number ||
				DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {

		if (n >= 5) {
			WX_GETDEFINE(toolid)
			WX_GETSTRING(label)
			DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
			wxBitmap bmpDisabled = wxNullBitmap;
			int kind = wxITEM_NORMAL;
			wxString shortHelp = wxEmptyString, longHelp = wxEmptyString;
			if (n >= 6) { DLIB_WXGET_BASE(bitmap, Bitmap, bmp) bmpDisabled = *bmp; }
			if (n >= 7) { WX_GETDEFINE_DEFINED(kind) }
			if (n >= 8) { WX_GETSTRING_DEFINED(shortHelp) }
			if (n >= 9) { WX_GETSTRING_DEFINED(longHelp) }
			retval	= toolbar->InsertTool(pos,
				toolid, label, *bitmap, bmpDisabled, (wxItemKind) kind, shortHelp, longHelp);
		}	
	}
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_START(toolbar_realize, 1, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_SETBOOL(toolbar->Realize())
}

WX_FUNC_START(toolbar_removetool, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(id)
	wxToolBarToolBase* retval	= toolbar->RemoveTool(id);
	WX_SETOBJECT(ToolBarToolBase, retval)
}

WX_FUNC_ARGRANGE_START(toolbar_setmargins, 2, 3, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	if (n == 2) {
		DLIB_WXGETSIZE_BASE(size)
		toolbar->wxToolBarBase::SetMargins(*size);
	} else {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		toolbar->SetMargins(x, y);
	}
}

WX_FUNC_START(toolbar_settoolbitmapsize, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	DLIB_WXGETSIZE_BASE(size)
	toolbar->SetToolBitmapSize(*size);
}

WX_FUNC_START(toolbar_settooldisabledbitmap, 3, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(id)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	toolbar->SetToolDisabledBitmap(id, *bitmap);
}

WX_FUNC_START(toolbar_settoollonghelp, 3, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(id)
	WX_GETSTRING(help)
	toolbar->SetToolLongHelp(id, help);
}

WX_FUNC_START(toolbar_settoolpacking, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETNUMBER(packing)
	toolbar->SetToolPacking(packing);
}

WX_FUNC_START(toolbar_settoolshorthelp, 3, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(id)
	WX_GETSTRING(help)
	toolbar->SetToolShortHelp(id, help);
}

WX_FUNC_START(toolbar_settoolnormalbitmap, 3, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(id)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	toolbar->SetToolNormalBitmap(id, *bitmap);
}

WX_FUNC_START(toolbar_settoolseparation, 2, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETNUMBER(separation)
	toolbar->SetToolSeparation(separation);
}

WX_FUNC_START(toolbar_toggletool, 3, Nil)
	DLIB_WXGET_BASE(toolbar, ToolBar, toolbar)
	WX_GETDEFINE(id)
	WX_GETBOOL(toggle)
	toolbar->ToggleTool(id, toggle);
}
