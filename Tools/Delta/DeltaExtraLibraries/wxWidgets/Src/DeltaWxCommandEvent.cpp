#include "DeltaWxCommandEvent.h"
#include "DeltaWxEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(commandevent, name)
#define WX_FUNC(name) WX_FUNC1(commandevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getextralong)
WX_FUNC_DEF(getint)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(getstring)
WX_FUNC_DEF(ischecked)
WX_FUNC_DEF(isselection)
WX_FUNC_DEF(setextralong)
WX_FUNC_DEF(setint)
WX_FUNC_DEF(setstring)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getextralong),
	WX_FUNC(getint),
	WX_FUNC(getselection),
	WX_FUNC(getstring),
	WX_FUNC(ischecked),
	WX_FUNC(isselection),
	WX_FUNC(setextralong),
	WX_FUNC(setint),
	WX_FUNC(setstring)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getextralong", "setstring")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CommandEvent, "commandevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CommandEvent, val, commandevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetString (void* val, DeltaValue* at) 
{
	wxCommandEvent *ev = DLIB_WXTYPECAST_BASE(CommandEvent, val, commandevent);
	WX_SETSTRING_EX(*at, ev->GetString())
	return true;
}

static bool GetSelection (void* val, DeltaValue* at) 
{
	wxCommandEvent *ev = DLIB_WXTYPECAST_BASE(CommandEvent, val, commandevent);
	WX_SETNUMBER_EX(*at, ev->GetSelection())
	return true;
}

static bool GetExtraLong (void* val, DeltaValue* at) 
{
	wxCommandEvent *ev = DLIB_WXTYPECAST_BASE(CommandEvent, val, commandevent);
	WX_SETNUMBER_EX(*at, ev->GetExtraLong())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "cmdString",			&GetString,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "commandInt",			&GetSelection,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "extraLong",			&GetExtraLong,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(CommandEvent,commandevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(commandevent_construct, 0, 2, Nil)
	wxCommandEvent *commevt = (wxCommandEvent*) 0;
	if (n == 0)
		commevt = new wxCommandEvent();
	else if (n == 1) {
		WX_GETDEFINE(commandType)
		commevt = new wxCommandEvent(commandType);
	} else {
		WX_GETDEFINE(commandType)
		WX_GETDEFINE(winid)
		commevt = new wxCommandEvent(commandType, winid);
	}
	WX_SETOBJECT(CommandEvent, commevt)
}

WX_FUNC_START(commandevent_getextralong, 1, Nil)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	WX_SETNUMBER(evt->GetExtraLong())
}

WX_FUNC_START(commandevent_getint, 1, Nil)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	WX_SETNUMBER(evt->GetInt())
}

WX_FUNC_START(commandevent_getselection, 1, Nil)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	WX_SETNUMBER(evt->GetSelection())
}

WX_FUNC_START(commandevent_getstring, 1, Nil)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	WX_SETSTRING(evt->GetString())
}

WX_FUNC_START(commandevent_ischecked, 1, Nil)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	WX_SETBOOL(evt->IsChecked())
}

WX_FUNC_START(commandevent_isselection, 1, Nil)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	WX_SETBOOL(evt->IsSelection())
}

WX_FUNC_START(commandevent_setextralong, 2, Nil)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	WX_GETNUMBER(extraLong)
	evt->SetExtraLong(extraLong);
}

WX_FUNC_START(commandevent_setint, 2, Nil)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	WX_GETNUMBER(intCommand)
	evt->SetInt(intCommand);
}

WX_FUNC_START(commandevent_setstring, 2, Nil)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	WX_GETSTRING(string)
	evt->SetString(string);
}
