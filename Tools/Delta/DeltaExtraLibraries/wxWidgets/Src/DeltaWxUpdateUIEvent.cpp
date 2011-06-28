#include "DeltaWxUpdateUIEvent.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(updateuievent, name)
#define WX_FUNC(name) WX_FUNC1(updateuievent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(canupdate)
WX_FUNC_DEF(check)
WX_FUNC_DEF(enable)
WX_FUNC_DEF(show)
WX_FUNC_DEF(getchecked)
WX_FUNC_DEF(getenabled)
WX_FUNC_DEF(getshown)
WX_FUNC_DEF(getsetchecked)
WX_FUNC_DEF(getsetenabled)
WX_FUNC_DEF(getsetshown)
WX_FUNC_DEF(getsettext)
WX_FUNC_DEF(gettext)
WX_FUNC_DEF(getmode)
WX_FUNC_DEF(getupdateinterval)
WX_FUNC_DEF(resetupdatetime)
WX_FUNC_DEF(setmode)
WX_FUNC_DEF(settext)
WX_FUNC_DEF(setupdateinterval)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(canupdate),
	WX_FUNC(getmode),
	WX_FUNC(getupdateinterval),
	WX_FUNC(resetupdatetime),
	WX_FUNC(setmode),
	WX_FUNC(setupdateinterval),
	WX_FUNC(destruct),
	WX_FUNC(check),
	WX_FUNC(enable),
	WX_FUNC(show),
	WX_FUNC(getchecked),
	WX_FUNC(getenabled),
	WX_FUNC(getshown),
	WX_FUNC(getsetchecked),
	WX_FUNC(getsetenabled),
	WX_FUNC(getsetshown),
	WX_FUNC(getsettext),
	WX_FUNC(gettext),
	WX_FUNC(settext)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(7, uarraysize(funcs) - 7, "destruct", "settext")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(UpdateUIEvent, "updateuievent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(UpdateUIEvent, val, updateuievent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxCommandEvent *_parent = DLIB_WXTYPECAST_BASE(CommandEvent, val, commandevent);
	DeltaWxCommandEvent *parent = DNEWCLASS(DeltaWxCommandEvent, (_parent));
	WX_SETOBJECT_EX(*at, CommandEvent, parent)
	return true;
}

static bool GetChecked (void* val, DeltaValue* at) 
{
	wxUpdateUIEvent *ev = DLIB_WXTYPECAST_BASE(UpdateUIEvent, val, updateuievent);
	WX_SETBOOL_EX(*at, ev->GetChecked())
	return true;
}

static bool GetEnabled (void* val, DeltaValue* at) 
{
	wxUpdateUIEvent *ev = DLIB_WXTYPECAST_BASE(UpdateUIEvent, val, updateuievent);
	WX_SETBOOL_EX(*at, ev->GetEnabled())
	return true;
}

static bool GetShown (void* val, DeltaValue* at) 
{
	wxUpdateUIEvent *ev = DLIB_WXTYPECAST_BASE(UpdateUIEvent, val, updateuievent);
	WX_SETBOOL_EX(*at, ev->GetShown())
	return true;
}

static bool GetSetEnabled (void* val, DeltaValue* at) 
{
	wxUpdateUIEvent *ev = DLIB_WXTYPECAST_BASE(UpdateUIEvent, val, updateuievent);
	WX_SETBOOL_EX(*at, ev->GetSetEnabled())
	return true;
}

static bool GetSetShown (void* val, DeltaValue* at) 
{
	wxUpdateUIEvent *ev = DLIB_WXTYPECAST_BASE(UpdateUIEvent, val, updateuievent);
	WX_SETBOOL_EX(*at, ev->GetSetShown())
	return true;
}

static bool GetSetText (void* val, DeltaValue* at) 
{
	wxUpdateUIEvent *ev = DLIB_WXTYPECAST_BASE(UpdateUIEvent, val, updateuievent);
	WX_SETBOOL_EX(*at, ev->GetSetText())
	return true;
}

static bool GetSetChecked (void* val, DeltaValue* at) 
{
	wxUpdateUIEvent *ev = DLIB_WXTYPECAST_BASE(UpdateUIEvent, val, updateuievent);
	WX_SETBOOL_EX(*at, ev->GetSetChecked())
	return true;
}

static bool GetText (void* val, DeltaValue* at) 
{
	wxUpdateUIEvent *ev = DLIB_WXTYPECAST_BASE(UpdateUIEvent, val, updateuievent);
	WX_SETSTRING_EX(*at, ev->GetText())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "checked",			&GetChecked,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "enabled",			&GetEnabled,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "shown",				&GetShown,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "setEnabled",			&GetSetEnabled,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "setShown",			&GetSetShown,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "setText",			&GetSetText,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "setChecked",			&GetSetChecked,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "text",				&GetText,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(UpdateUIEvent, updateuievent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(updateuievent_construct, 0, 1, Nil)
	int commandId = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(commandId) }
	DeltaWxUpdateUIEvent *evt = DNEWCLASS(DeltaWxUpdateUIEvent,
		(new wxUpdateUIEvent(commandId)));
	WX_SETOBJECT(UpdateUIEvent, evt)
}

DLIB_FUNC_START(updateuievent_destruct, 1, Nil)
	DLIB_WXDELETE(updateuievent, UpdateUIEvent, evt)
}

DLIB_FUNC_START(updateuievent_canupdate, 1, Nil)
	DLIB_WXGET_BASE(window, Window, window)
	WX_SETBOOL(wxUpdateUIEvent::CanUpdate(window))
}

DLIB_FUNC_START(updateuievent_check, 2, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_GETBOOL(check)
	evt->Check(check);
}

DLIB_FUNC_START(updateuievent_enable, 2, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_GETBOOL(enable)
	evt->Enable(enable);
}

DLIB_FUNC_START(updateuievent_show, 2, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_GETBOOL(show)
	evt->Show(show);
}

DLIB_FUNC_START(updateuievent_getchecked, 1, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_SETBOOL(evt->GetChecked())
}

DLIB_FUNC_START(updateuievent_getenabled, 1, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_SETBOOL(evt->GetEnabled())
}

DLIB_FUNC_START(updateuievent_getshown, 1, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_SETBOOL(evt->GetShown())
}

DLIB_FUNC_START(updateuievent_getsetchecked, 1, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_SETBOOL(evt->GetSetChecked())
}

DLIB_FUNC_START(updateuievent_getsetenabled, 1, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_SETBOOL(evt->GetSetEnabled())
}

DLIB_FUNC_START(updateuievent_getsetshown, 1, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_SETBOOL(evt->GetSetShown())
}

DLIB_FUNC_START(updateuievent_getsettext, 1, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_SETBOOL(evt->GetSetText())
}

DLIB_FUNC_START(updateuievent_gettext, 1, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_SETSTRING(evt->GetText())
}

DLIB_FUNC_START(updateuievent_getmode, 0, Nil)
	WX_SETNUMBER(wxUpdateUIEvent::GetMode())
}

DLIB_FUNC_START(updateuievent_getupdateinterval, 0, Nil)
	WX_SETNUMBER(wxUpdateUIEvent::GetUpdateInterval())
}

DLIB_FUNC_START(updateuievent_resetupdatetime, 0, Nil)
	wxUpdateUIEvent::ResetUpdateTime();
}

DLIB_FUNC_START(updateuievent_setmode, 1, Nil)
	WX_GETDEFINE(mode)
	wxUpdateUIEvent::SetMode((wxUpdateUIMode)mode);
}

DLIB_FUNC_START(updateuievent_settext, 2, Nil)
	DLIB_WXGET_BASE(updateuievent, UpdateUIEvent, evt)
	WX_GETSTRING(text)
	evt->SetText(text);
}

DLIB_FUNC_START(updateuievent_setupdateinterval, 1, Nil)
	WX_GETNUMBER(interval)
	wxUpdateUIEvent::SetUpdateInterval(interval);
}
