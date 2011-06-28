#include "DeltaWxTreebookEvent.h"
#include "DeltaWxNotifyEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(treebookevent, name)
#define WX_FUNC(name) WX_FUNC1(treebookevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getoldselection)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(setoldselection)
WX_FUNC_DEF(setselection)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getoldselection),
	WX_FUNC(getselection),
	WX_FUNC(setoldselection),
	WX_FUNC(setselection)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setselection")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(TreebookEvent, "treebookevent", NotifyEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TreebookEvent, val, treebookevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxNotifyEvent *_parent = DLIB_WXTYPECAST_BASE(NotifyEvent, val, notifyevent);
	DeltaWxNotifyEvent *parent = DNEWCLASS(DeltaWxNotifyEvent, (_parent));
	WX_SETOBJECT_EX(*at, NotifyEvent, parent)
	return true;
}

static bool GetSelection (void* val, DeltaValue* at) 
{
	wxTreebookEvent *ev = DLIB_WXTYPECAST_BASE(TreebookEvent, val, treebookevent);
	at->FromNumber(ev->GetSelection());
	return true;
}

static bool GetOldSelection (void* val, DeltaValue* at) 
{
	wxTreebookEvent *ev = DLIB_WXTYPECAST_BASE(TreebookEvent, val, treebookevent);
	at->FromNumber(ev->GetOldSelection());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "NotifyEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "selection",			&GetSelection,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "oldSelection",		&GetOldSelection,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TreebookEvent, treebookevent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(treebookevent_construct, 0, 4, Nil)
	int commandType = wxEVT_NULL;
	int id = 0;
	int nSel = wxNOT_FOUND;
	int nOldSel = wxNOT_FOUND;
	if (n >= 1) { WX_GETDEFINE_DEFINED(commandType) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 3) { WX_GETNUMBER_DEFINED(nSel) }
	if (n >= 4) { WX_GETNUMBER_DEFINED(nOldSel) }
	DeltaWxTreebookEvent *evt = DNEWCLASS(DeltaWxTreebookEvent,
		(new wxTreebookEvent(commandType, id, nSel, nOldSel)));
	WX_SETOBJECT(TreebookEvent, evt)
}

DLIB_FUNC_START(treebookevent_destruct, 1, Nil)
	DLIB_WXDELETE(treebookevent, TreebookEvent, evt)
}

DLIB_FUNC_START(treebookevent_getoldselection, 1, Nil)
	DLIB_WXGET_BASE(treebookevent, TreebookEvent, evt)
	WX_SETNUMBER(evt->GetOldSelection())
}

DLIB_FUNC_START(treebookevent_getselection, 1, Nil)
	DLIB_WXGET_BASE(treebookevent, TreebookEvent, evt)
	WX_SETNUMBER(evt->GetSelection())
}

DLIB_FUNC_START(treebookevent_setoldselection, 2, Nil)
	DLIB_WXGET_BASE(treebookevent, TreebookEvent, evt)
	WX_GETNUMBER(nOldSel)
	evt->SetOldSelection(nOldSel);
}

DLIB_FUNC_START(treebookevent_setselection, 2, Nil)
	DLIB_WXGET_BASE(treebookevent, TreebookEvent, evt)
	WX_GETNUMBER(nSel)
	evt->SetSelection(nSel);
}
