#include "DeltaWxGenericDirCtrl.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxDirFilterListCtrl.h"
#include "DeltaWxTreeItemId.h"
#include "DeltaWxTreeCtrl.h"
#include "DeltaWxDirFilterListCtrl.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(genericdirctrl, name)
#define WX_FUNC(name) WX_FUNC1(genericdirctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(init)
WX_FUNC_DEF(collapsetree)
WX_FUNC_DEF(expandpath)
WX_FUNC_DEF(collapsepath)
WX_FUNC_DEF(getdefaultpath)
WX_FUNC_DEF(getpath)
WX_FUNC_DEF(getfilepath)
WX_FUNC_DEF(getfilter)
WX_FUNC_DEF(getfilterindex)
WX_FUNC_DEF(getfilterlistctrl)
WX_FUNC_DEF(getrootid)
WX_FUNC_DEF(gettreectrl)
WX_FUNC_DEF(recreatetree)
WX_FUNC_DEF(setdefaultpath)
WX_FUNC_DEF(setfilter)
WX_FUNC_DEF(setfilterindex)
WX_FUNC_DEF(setpath)
WX_FUNC_DEF(showhidden)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(init),
	WX_FUNC(collapsetree),
	WX_FUNC(expandpath),
	WX_FUNC(collapsepath),
	WX_FUNC(getdefaultpath),
	WX_FUNC(getpath),
	WX_FUNC(getfilepath),
	WX_FUNC(getfilter),
	WX_FUNC(getfilterindex),
	WX_FUNC(getfilterlistctrl),
	WX_FUNC(getrootid),
	WX_FUNC(gettreectrl),
	WX_FUNC(recreatetree),
	WX_FUNC(setdefaultpath),
	WX_FUNC(setfilter),
	WX_FUNC(setfilterindex),
	WX_FUNC(setpath),
	WX_FUNC(showhidden)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "showhidden")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(GenericDirCtrl, "genericdirctrl", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(GenericDirCtrl, val, genericdirctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetShowHidden (void* val, DeltaValue* at) 
{
	wxGenericDirCtrl *ctrl = DLIB_WXTYPECAST_BASE(GenericDirCtrl, val, genericdirctrl);
	WX_SETBOOL_EX(*at, ctrl->GetShowHidden())
	return true;
}

static bool GetRootId (void* val, DeltaValue* at) 
{
	wxGenericDirCtrl *ctrl = DLIB_WXTYPECAST_BASE(GenericDirCtrl, val, genericdirctrl);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, TreeItemId, new wxTreeItemId(ctrl->GetRootId()))
	return true;
}

static bool GetDefaultPath (void* val, DeltaValue* at) 
{
	wxGenericDirCtrl *ctrl = DLIB_WXTYPECAST_BASE(GenericDirCtrl, val, genericdirctrl);
	WX_SETSTRING_EX(*at, ctrl->GetDefaultPath())
	return true;
}

static bool GetFilter (void* val, DeltaValue* at) 
{
	wxGenericDirCtrl *ctrl = DLIB_WXTYPECAST_BASE(GenericDirCtrl, val, genericdirctrl);
	WX_SETSTRING_EX(*at, ctrl->GetFilter())
	return true;
}

static bool GetFilterIndex (void* val, DeltaValue* at) 
{
	wxGenericDirCtrl *ctrl = DLIB_WXTYPECAST_BASE(GenericDirCtrl, val, genericdirctrl);
	WX_SETNUMBER_EX(*at, ctrl->GetFilterIndex())
	return true;
}

static bool GetTreeCtrl (void* val, DeltaValue* at) 
{
	wxGenericDirCtrl *ctrl = DLIB_WXTYPECAST_BASE(GenericDirCtrl, val, genericdirctrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, TreeCtrl, ctrl->GetTreeCtrl())
	return true;
}

static bool GetFilterListCtrl (void* val, DeltaValue* at) 
{
	wxGenericDirCtrl *ctrl = DLIB_WXTYPECAST_BASE(GenericDirCtrl, val, genericdirctrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, DirFilterListCtrl, ctrl->GetFilterListCtrl())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "showHidden",			&GetShowHidden,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rootId",				&GetRootId,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "defaultPath",		&GetDefaultPath,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "filter",				&GetFilter,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "currentFilter",		&GetFilterIndex,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "treeCtrl",			&GetTreeCtrl,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "filterListCtrl",		&GetFilterListCtrl,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(GenericDirCtrl,genericdirctrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(genericdirctrl_construct, 0, 9, Nil)
	wxGenericDirCtrl *dirctrl = (wxGenericDirCtrl*) 0;
	if (n == 0) {
		dirctrl = new wxGenericDirCtrl();
	} else {
		DLIB_WXGET_BASE(window, Window, parent)
		int id = wxID_ANY, defaultFilter = 0;
		wxString dir = wxDirDialogDefaultFolderStr, filter = wxEmptyString, name = wxTreeCtrlNameStr;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER;
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { WX_GETSTRING_DEFINED(dir) }
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { WX_GETSTRING_DEFINED(filter) }
		if (n >= 8) { WX_GETNUMBER_DEFINED(defaultFilter) }
		if (n >= 9) { WX_GETSTRING_DEFINED(name) }
		dirctrl = new wxGenericDirCtrl(parent, id, dir, pos, size, style, filter, defaultFilter, name);
	}
	WX_SET_WINDOW_OBJECT(GenericDirCtrl, dirctrl)
}

WX_FUNC_ARGRANGE_START(genericdirctrl_create, 2, 10, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	int id = wxID_ANY, defaultFilter = 0;
	wxString dir = wxDirDialogDefaultFolderStr, filter = wxEmptyString, name = wxTreeCtrlNameStr;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER;
	if (n >= 3) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 4) { WX_GETSTRING_DEFINED(dir) }
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { WX_GETSTRING_DEFINED(filter) }
	if (n >= 9) { WX_GETNUMBER_DEFINED(defaultFilter) }
	if (n >= 10) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(dirctrl->Create(parent, id, dir, pos, size, style, filter, defaultFilter, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(dirctrl);
}

WX_FUNC_START(genericdirctrl_init, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	dirctrl->Init();
}

WX_FUNC_START(genericdirctrl_collapsetree, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	dirctrl->CollapseTree();
}

WX_FUNC_START(genericdirctrl_expandpath, 2, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_GETSTRING(path)
	WX_SETBOOL(dirctrl->ExpandPath(path))
}

WX_FUNC_START(genericdirctrl_collapsepath, 2, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_GETSTRING(path)
	WX_SETBOOL(dirctrl->CollapsePath(path))
}

WX_FUNC_START(genericdirctrl_getdefaultpath, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_SETSTRING(dirctrl->GetDefaultPath())
}

WX_FUNC_START(genericdirctrl_getpath, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_SETSTRING(dirctrl->GetPath())
}

WX_FUNC_START(genericdirctrl_getfilepath, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_SETSTRING(dirctrl->GetFilePath())
}

WX_FUNC_START(genericdirctrl_getfilter, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_SETSTRING(dirctrl->GetFilter())
}

WX_FUNC_START(genericdirctrl_getfilterindex, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_SETNUMBER(dirctrl->GetFilterIndex())
}

WX_FUNC_START(genericdirctrl_getfilterlistctrl, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_SETOBJECT(DirFilterListCtrl, dirctrl->GetFilterListCtrl())
}

WX_FUNC_START(genericdirctrl_getrootid, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(dirctrl->GetRootId()))
}

WX_FUNC_START(genericdirctrl_gettreectrl, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_SETOBJECT(TreeCtrl, dirctrl->GetTreeCtrl())
}

WX_FUNC_START(genericdirctrl_recreatetree, 1, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	dirctrl->ReCreateTree();
}

WX_FUNC_START(genericdirctrl_setdefaultpath, 2, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_GETSTRING(path)
	dirctrl->SetDefaultPath(path);
}

WX_FUNC_START(genericdirctrl_setfilter, 2, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_GETSTRING(filter)
	dirctrl->SetFilter(filter);
}

WX_FUNC_START(genericdirctrl_setfilterindex, 2, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_GETNUMBER(num)
	dirctrl->SetFilterIndex(num);
}

WX_FUNC_START(genericdirctrl_setpath, 2, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_GETSTRING(path)
	dirctrl->SetPath(path);
}

WX_FUNC_START(genericdirctrl_showhidden, 2, Nil)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, dirctrl)
	WX_GETBOOL(show)
	dirctrl->ShowHidden(show);
}
