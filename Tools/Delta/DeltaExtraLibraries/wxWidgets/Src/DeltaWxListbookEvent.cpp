#include "DeltaWxListbookEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(listbookevent, name)
#define WX_FUNC(name) WX_FUNC1(listbookevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getoldselection)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(setoldselection)
WX_FUNC_DEF(setselection)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getoldselection),
	WX_FUNC(getselection),
	WX_FUNC(setoldselection),
	WX_FUNC(setselection)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getoldselection", "setselection")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ListbookEvent, "listbookevent", NotifyEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ListbookEvent, val, listbookevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, NotifyEvent, val)
	return true;
}

static bool GetSelection (void* val, DeltaValue* at) 
{
	wxListbookEvent *ev = DLIB_WXTYPECAST_BASE(ListbookEvent, val, listbookevent);
	WX_SETNUMBER_EX(*at, ev->GetSelection())
	return true;
}

static bool GetOldSelection (void* val, DeltaValue* at) 
{
	wxListbookEvent *ev = DLIB_WXTYPECAST_BASE(ListbookEvent, val, listbookevent);
	WX_SETNUMBER_EX(*at, ev->GetOldSelection())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "NotifyEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "selection",			&GetSelection,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "oldSelection",		&GetOldSelection,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ListbookEvent,listbookevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(listbookevent_construct, 0, 4, Nil)
	int commandType = wxEVT_NULL;
	int id = 0;
	int nSel = wxNOT_FOUND;
	int nOldSel = wxNOT_FOUND;
	if (n >= 1) { WX_GETDEFINE_DEFINED(commandType) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 3) { WX_GETNUMBER_DEFINED(nSel) }
	if (n >= 4) { WX_GETNUMBER_DEFINED(nOldSel) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ListbookEvent, new wxListbookEvent(commandType, id, nSel, nOldSel))
}

WX_FUNC_START(listbookevent_getoldselection, 1, Nil)
	DLIB_WXGET_BASE(listbookevent, ListbookEvent, evt)
	WX_SETNUMBER(evt->GetOldSelection())
}

WX_FUNC_START(listbookevent_getselection, 1, Nil)
	DLIB_WXGET_BASE(listbookevent, ListbookEvent, evt)
	WX_SETNUMBER(evt->GetSelection())
}

WX_FUNC_START(listbookevent_setoldselection, 2, Nil)
	DLIB_WXGET_BASE(listbookevent, ListbookEvent, evt)
	WX_GETNUMBER(nOldSel)
	evt->SetOldSelection(nOldSel);
}

WX_FUNC_START(listbookevent_setselection, 2, Nil)
	DLIB_WXGET_BASE(listbookevent, ListbookEvent, evt)
	WX_GETNUMBER(nSel)
	evt->SetSelection(nSel);
}
