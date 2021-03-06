#include "DeltaWxTreeEvent.h"
#include "DeltaWxNotifyEvent.h"
#include "DeltaWxTreeCtrl.h"
#include "DeltaWxTreeItemId.h"
#include "DeltaWxKeyEvent.h"
#include "DeltaWxPoint.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(treeevent, name)
#define WX_FUNC(name) WX_FUNC1(treeevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getkeycode)
WX_FUNC_DEF(getitem)
WX_FUNC_DEF(getkeyevent)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(getolditem)
WX_FUNC_DEF(getpoint)
WX_FUNC_DEF(iseditcancelled)
WX_FUNC_DEF(settooltip)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getkeycode),
	WX_FUNC(getitem),
	WX_FUNC(getkeyevent),
	WX_FUNC(getlabel),
	WX_FUNC(getolditem),
	WX_FUNC(getpoint),
	WX_FUNC(iseditcancelled),
	WX_FUNC(settooltip)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getkeycode", "settooltip")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(TreeEvent, "treeevent", NotifyEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TreeEvent, val, treeevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, NotifyEvent, val)
	return true;
}

static bool GetKeyEvent (void* val, DeltaValue* at) 
{
	wxTreeEvent *ev = DLIB_WXTYPECAST_BASE(TreeEvent, val, treeevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, KeyEvent, new wxKeyEvent(ev->GetKeyEvent()))
	return true;
}

static bool GetItem (void* val, DeltaValue* at) 
{
	wxTreeEvent *ev = DLIB_WXTYPECAST_BASE(TreeEvent, val, treeevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, TreeItemId, new wxTreeItemId(ev->GetItem()))
	return true;
}

static bool GetOldItem (void* val, DeltaValue* at) 
{
	wxTreeEvent *ev = DLIB_WXTYPECAST_BASE(TreeEvent, val, treeevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, TreeItemId, new wxTreeItemId(ev->GetOldItem()))
	return true;
}

static bool GetPointDrag (void* val, DeltaValue* at) 
{
	wxTreeEvent *ev = DLIB_WXTYPECAST_BASE(TreeEvent, val, treeevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Point, new wxPoint(ev->GetPoint()))
	return true;
}

static bool GetLabel (void* val, DeltaValue* at) 
{
	wxTreeEvent *ev = DLIB_WXTYPECAST_BASE(TreeEvent, val, treeevent);
	WX_SETSTRING_EX(*at, ev->GetLabel())
	return true;
}

static bool GetIsEditCancelled (void* val, DeltaValue* at) 
{
	wxTreeEvent *ev = DLIB_WXTYPECAST_BASE(TreeEvent, val, treeevent);
	WX_SETBOOL_EX(*at, ev->IsEditCancelled())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "NotifyEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "eventKey",			&GetKeyEvent,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "item",				&GetItem,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "oldItem",			&GetOldItem,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pointDrag",			&GetPointDrag,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "label",				&GetLabel,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "editCancelled",		&GetIsEditCancelled,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TreeEvent,treeevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(treeevent_construct, 0, 3, Nil)
	wxTreeEvent *evt = (wxTreeEvent*) 0;
	int commandType = wxEVT_NULL;
	if (n >= 1) { WX_GETDEFINE_DEFINED(commandType) }
	if (n >= 2 && DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(treectrl, TreeCtrl, tree)
		wxTreeItemId item = wxTreeItemId();
		if (n >= 3) { DLIB_WXGET_BASE(treeitemid, TreeItemId, _item) item = *_item; }
		evt = new wxTreeEvent(commandType, tree, item);
	} else {
		int id = 0;
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		evt = new wxTreeEvent(commandType, id);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeEvent, evt)
}

WX_FUNC_START(treeevent_getkeycode, 1, Nil)
	DLIB_WXGET_BASE(treeevent, TreeEvent, evt)
	WX_SETNUMBER(evt->GetKeyCode())
}

WX_FUNC_START(treeevent_getitem, 1, Nil)
	DLIB_WXGET_BASE(treeevent, TreeEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(evt->GetItem()))
}

WX_FUNC_START(treeevent_getkeyevent, 1, Nil)
	DLIB_WXGET_BASE(treeevent, TreeEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(KeyEvent, new wxKeyEvent(evt->GetKeyEvent()))
}

WX_FUNC_START(treeevent_getlabel, 1, Nil)
	DLIB_WXGET_BASE(treeevent, TreeEvent, evt)
	WX_SETSTRING(evt->GetLabel())
}

WX_FUNC_START(treeevent_getolditem, 1, Nil)
	DLIB_WXGET_BASE(treeevent, TreeEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(evt->GetOldItem()))
}

WX_FUNC_START(treeevent_getpoint, 1, Nil)
	DLIB_WXGET_BASE(treeevent, TreeEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(evt->GetPoint()))
}

WX_FUNC_START(treeevent_iseditcancelled, 1, Nil)
	DLIB_WXGET_BASE(treeevent, TreeEvent, evt)
	WX_SETBOOL(evt->IsEditCancelled())
}

WX_FUNC_START(treeevent_settooltip, 2, Nil)
	DLIB_WXGET_BASE(treeevent, TreeEvent, evt)
	WX_GETSTRING(tooltip)
	evt->SetToolTip(tooltip);
}
