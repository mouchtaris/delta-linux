#include "DeltaWxNotebookEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(notebookevent, name)
#define WX_FUNC(name) WX_FUNC1(notebookevent, name)

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

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(NotebookEvent, "notebookevent", NotifyEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(NotebookEvent, val, notebookevent) ?
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
	wxNotebookEvent *ev = DLIB_WXTYPECAST_BASE(NotebookEvent, val, notebookevent);
	WX_SETNUMBER_EX(*at, ev->GetSelection())
	return true;
}

static bool GetOldSelection (void* val, DeltaValue* at) 
{
	wxNotebookEvent *ev = DLIB_WXTYPECAST_BASE(NotebookEvent, val, notebookevent);
	WX_SETNUMBER_EX(*at, ev->GetOldSelection())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "NotifyEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "selection",			&GetSelection,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "oldSelection",		&GetOldSelection,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(NotebookEvent, notebookevent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(notebookevent_construct, 0, 4, Nil)
	int commandType = wxEVT_NULL;
	int winid = 0;
	int nSel = -1;
	int nOldSel = -1;
	if (n >= 1) { WX_GETDEFINE_DEFINED(commandType) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
	if (n >= 3) { WX_GETNUMBER_DEFINED(nSel) }
	if (n >= 4) { WX_GETNUMBER_DEFINED(nOldSel) }
	DeltaWxNotebookEvent *evt = DNEWCLASS(DeltaWxNotebookEvent,
		(new wxNotebookEvent(commandType, winid, nSel, nOldSel)));
	WX_SETOBJECT(NotebookEvent, evt)
}

DLIB_FUNC_START(notebookevent_destruct, 1, Nil)
	DLIB_WXDELETE(notebookevent, NotebookEvent, evt)
}

DLIB_FUNC_START(notebookevent_getoldselection, 1, Nil)
	DLIB_WXGET_BASE(notebookevent, NotebookEvent, evt)
	WX_SETNUMBER(evt->GetOldSelection())
}

DLIB_FUNC_START(notebookevent_getselection, 1, Nil)
	DLIB_WXGET_BASE(notebookevent, NotebookEvent, evt)
	WX_SETNUMBER(evt->GetSelection())
}

DLIB_FUNC_START(notebookevent_setoldselection, 2, Nil)
	DLIB_WXGET_BASE(notebookevent, NotebookEvent, evt)
	WX_GETNUMBER(nOldSel)
	evt->SetOldSelection(nOldSel);
}

DLIB_FUNC_START(notebookevent_setselection, 2, Nil)
	DLIB_WXGET_BASE(notebookevent, NotebookEvent, evt)
	WX_GETNUMBER(nSel)
	evt->SetSelection(nSel);
}
