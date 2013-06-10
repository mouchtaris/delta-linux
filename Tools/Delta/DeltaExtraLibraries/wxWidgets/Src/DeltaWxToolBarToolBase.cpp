#include "DeltaWxToolBarToolBase.h"
#include "DeltaWxObject.h"
#include "DeltaWxToolBar.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxControl.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(toolbartoolbase, name)
#define WX_FUNC(name) WX_FUNC1(toolbartoolbase, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(attach)
WX_FUNC_DEF(canbetoggled)
WX_FUNC_DEF(detach)
WX_FUNC_DEF(enable)
WX_FUNC_DEF(isbutton)
WX_FUNC_DEF(iscontrol)
WX_FUNC_DEF(isenabled)
WX_FUNC_DEF(isseparator)
WX_FUNC_DEF(istoggled)
WX_FUNC_DEF(getbitmap)
WX_FUNC_DEF(getcontrol)
WX_FUNC_DEF(getdisabledbitmap)
WX_FUNC_DEF(getid)
WX_FUNC_DEF(getkind)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(getlonghelp)
WX_FUNC_DEF(getnormalbitmap)
WX_FUNC_DEF(getshorthelp)
WX_FUNC_DEF(getstyle)
WX_FUNC_DEF(gettoolbar)
WX_FUNC_DEF(setdisabledbitmap)
WX_FUNC_DEF(setlabel)
WX_FUNC_DEF(setlonghelp)
WX_FUNC_DEF(setnormalbitmap)
WX_FUNC_DEF(setshorthelp)
WX_FUNC_DEF(settoggle)
WX_FUNC_DEF(toggle)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(attach),
	WX_FUNC(canbetoggled),
	WX_FUNC(detach),
	WX_FUNC(enable),
	WX_FUNC(isbutton),
	WX_FUNC(iscontrol),
	WX_FUNC(isenabled),
	WX_FUNC(isseparator),
	WX_FUNC(istoggled),
	WX_FUNC(getbitmap),
	WX_FUNC(getcontrol),
	WX_FUNC(getdisabledbitmap),
	WX_FUNC(getid),
	WX_FUNC(getkind),
	WX_FUNC(getlabel),
	WX_FUNC(getlonghelp),
	WX_FUNC(getnormalbitmap),
	WX_FUNC(getshorthelp),
	WX_FUNC(getstyle),
	WX_FUNC(gettoolbar),
	WX_FUNC(setdisabledbitmap),
	WX_FUNC(setlabel),
	WX_FUNC(setlonghelp),
	WX_FUNC(setnormalbitmap),
	WX_FUNC(setshorthelp),
	WX_FUNC(settoggle),
	WX_FUNC(toggle)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "toggle")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ToolBarToolBase, "toolbartoolbase", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetToolBar (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ToolBar, (wxToolBar*)tool->GetToolBar())
	return true;
}

static bool GetStyle (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETNUMBER_EX(*at, tool->GetStyle())
	return true;
}

static bool GetId (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETNUMBER_EX(*at, tool->GetId())
	return true;
}

static bool GetKind (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETNUMBER_EX(*at, tool->GetKind())
	return true;
}

static bool GetIsToggled (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETBOOL_EX(*at, tool->IsToggled())
	return true;
}

static bool GetIsEnabled (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETBOOL_EX(*at, tool->IsEnabled())
	return true;
}

static bool GetNormalBitmap (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Bitmap, new wxBitmap(tool->GetNormalBitmap()))
	return true;
}

static bool GetDisabledBitmap (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Bitmap, new wxBitmap(tool->GetDisabledBitmap()))
	return true;
}

static bool GetLabel (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETSTRING_EX(*at, tool->GetLabel())
	return true;
}

static bool GetShortHelp (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETSTRING_EX(*at, tool->GetShortHelp())
	return true;
}

static bool GetLongHelp (void* val, DeltaValue* at) 
{
	wxToolBarToolBase *tool = DLIB_WXTYPECAST_BASE(ToolBarToolBase, val, toolbartoolbase);
	WX_SETSTRING_EX(*at, tool->GetLongHelp())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "toolbar",			&GetToolBar,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "toolStyle",			&GetStyle,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "id",					&GetId,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "kind",				&GetKind,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "toggled",			&GetIsToggled,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "enabled",			&GetIsEnabled,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "normalBitmap",		&GetNormalBitmap,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "disabledBitmap",		&GetDisabledBitmap,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "label",				&GetLabel,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "shortHelpString",	&GetShortHelp,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "longHelpString",		&GetLongHelp,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ToolBarToolBase,toolbartoolbase)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(toolbartoolbase_construct, 0, 8, Nil)
	wxToolBarToolBase *tool = (wxToolBarToolBase*) 0;
	if (n == 0) {
		tool = new wxToolBarToolBase();
	} else {
		DLIB_WXGET_BASE(toolbar, ToolBar, tbar)
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number ||
			DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {

			int toolid = wxID_SEPARATOR, kind = wxITEM_NORMAL;
			wxString label = wxEmptyString, shortHelpString = wxEmptyString, longHelpString = wxEmptyString;
			wxBitmap bmpNormal = wxNullBitmap, bmpDisabled = wxNullBitmap;
			if (n >= 2) { WX_GETDEFINE_DEFINED(toolid) }
			if (n >= 3) { WX_GETSTRING_DEFINED(label) }
			if (n >= 4) { DLIB_WXGET_BASE(bitmap, Bitmap, bmp) bmpNormal = *bmp; }
			if (n >= 5) { DLIB_WXGET_BASE(bitmap, Bitmap, bmp) bmpDisabled = *bmp; }
			if (n >= 6) { WX_GETDEFINE_DEFINED(kind) }
			if (n >= 7) { WX_GETSTRING_DEFINED(shortHelpString) }
			if (n >= 8) { WX_GETSTRING_DEFINED(longHelpString) }
			tool = new wxToolBarToolBase(tbar, toolid, label, bmpNormal, bmpDisabled,
				(wxItemKind)kind, NULL, shortHelpString, longHelpString);
		} else if (n >= 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(control, Control, control)
			tool = new wxToolBarToolBase(tbar, control
#if wxCHECK_VERSION(2, 9, 0)
				, wxEmptyString
#endif
			);
		}
	}
	WX_SETOBJECT(ToolBarToolBase, tool)
}

WX_FUNC_START(toolbartoolbase_destruct, 1, Nil)
	DLIB_WXDELETE(toolbartoolbase, ToolBarToolBase, tool)
}

WX_FUNC_START(toolbartoolbase_attach, 2, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	DLIB_WXGET_BASE(toolbar, ToolBar, tbar)
	tool->Attach((wxToolBar*) tbar);
}

WX_FUNC_START(toolbartoolbase_canbetoggled, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETBOOL(tool->CanBeToggled())
}

WX_FUNC_START(toolbartoolbase_detach, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	tool->Detach();
}

WX_FUNC_START(toolbartoolbase_enable, 2, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_GETBOOL(enable)
	WX_SETBOOL(tool->Enable(enable))
}

WX_FUNC_START(toolbartoolbase_isbutton, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETBOOL(tool->IsButton())
}

WX_FUNC_START(toolbartoolbase_iscontrol, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETBOOL(tool->IsControl())
}

WX_FUNC_START(toolbartoolbase_isenabled, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETBOOL(tool->IsEnabled())
}

WX_FUNC_START(toolbartoolbase_isseparator, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETBOOL(tool->IsSeparator())
}

WX_FUNC_START(toolbartoolbase_istoggled, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETBOOL(tool->IsToggled())
}

WX_FUNC_START(toolbartoolbase_getbitmap, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETOBJECT(Bitmap, new wxBitmap(tool->GetBitmap()))
}

WX_FUNC_START(toolbartoolbase_getcontrol, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	wxControl* retval	= tool->GetControl();
	WX_SETOBJECT(Control, retval)
}

WX_FUNC_START(toolbartoolbase_getdisabledbitmap, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETOBJECT(Bitmap, new wxBitmap(tool->GetDisabledBitmap()))
}

WX_FUNC_START(toolbartoolbase_getid, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETNUMBER(tool->GetId())
}

WX_FUNC_START(toolbartoolbase_getkind, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETNUMBER(tool->GetKind())
}

WX_FUNC_START(toolbartoolbase_getlabel, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETSTRING(tool->GetLabel())
}

WX_FUNC_START(toolbartoolbase_getlonghelp, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETSTRING(tool->GetLongHelp())
}

WX_FUNC_START(toolbartoolbase_getnormalbitmap, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETOBJECT(Bitmap, new wxBitmap(tool->GetNormalBitmap()))
}

WX_FUNC_START(toolbartoolbase_getshorthelp, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETSTRING(tool->GetShortHelp())
}

WX_FUNC_START(toolbartoolbase_getstyle, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_SETNUMBER(tool->GetStyle())
}

WX_FUNC_START(toolbartoolbase_gettoolbar, 1, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	wxToolBar* retval	= (wxToolBar*)tool->GetToolBar();
	WX_SETOBJECT(ToolBar, retval)
}

WX_FUNC_START(toolbartoolbase_setdisabledbitmap, 2, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
	tool->SetDisabledBitmap(*bmp);
}

WX_FUNC_START(toolbartoolbase_setlabel, 2, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_GETSTRING(label)
	tool->SetLabel(label);
}

WX_FUNC_START(toolbartoolbase_setlonghelp, 2, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_GETSTRING(help)
	WX_SETBOOL(tool->SetLongHelp(help))
}

WX_FUNC_START(toolbartoolbase_setnormalbitmap, 2, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
	tool->SetNormalBitmap(*bmp);
}

WX_FUNC_START(toolbartoolbase_setshorthelp, 2, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_GETSTRING(help)
	WX_SETBOOL(tool->SetShortHelp(help))
}

WX_FUNC_START(toolbartoolbase_settoggle, 2, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	WX_GETBOOL(toggle)
	WX_SETBOOL(tool->SetToggle(toggle))
}

WX_FUNC_ARGRANGE_START(toolbartoolbase_toggle, 1, 2, Nil)
	DLIB_WXGET_BASE(toolbartoolbase, ToolBarToolBase, tool)
	if (n == 2) {
		WX_GETBOOL(toggle)
		WX_SETBOOL(tool->Toggle(toggle))
	} else {
		tool->Toggle();
	}
}
