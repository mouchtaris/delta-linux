#include "DeltaWxDateEvent.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxWindow.h"
#include "DeltaWxDateTime.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(dateevent, name)
#define WX_FUNC(name) WX_FUNC1(dateevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getdate)
WX_FUNC_DEF(setdate)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getdate),
	WX_FUNC(setdate)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setdate")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DateEvent, "dateevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DateEvent, val, dateevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxCommandEvent *_parent = DLIB_WXTYPECAST_BASE(CommandEvent, val, commandevent);
	DeltaWxCommandEvent *parent = DNEWCLASS(DeltaWxCommandEvent, (_parent));
	WX_SETOBJECT_EX(*at, CommandEvent, parent)
	return true;
}

static bool GetDate (void* val, DeltaValue* at) 
{
	wxDateEvent *ev = DLIB_WXTYPECAST_BASE(DateEvent, val, dateevent);
	DeltaWxDateTime *retval = DNEWCLASS(DeltaWxDateTime, (new wxDateTime(ev->GetDate())));
	WX_SETOBJECT_EX(*at, DateTime, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "date",				&GetDate,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DateEvent, dateevent);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(dateevent_construct, 0, 3, Nil)
	wxDateEvent *wxevt = (wxDateEvent*) 0;
	DeltaWxDateEvent *evt = (DeltaWxDateEvent*) 0;
	if (n == 0) {
		wxevt = new wxDateEvent();
	} else if (n == 3) {
		DLIB_WXGET_BASE(window, Window, win)
		DLIB_WXGET_BASE(datetime, DateTime, dt)
		WX_GETDEFINE(type)
		wxevt = new wxDateEvent(win, *dt, type);
	}
	if (wxevt) evt = DNEWCLASS(DeltaWxDateEvent, (wxevt));
	WX_SETOBJECT(DateEvent, evt)
}

DLIB_FUNC_START(dateevent_destruct, 1, Nil)
	DLIB_WXDELETE(dateevent, DateEvent, evt)
}

DLIB_FUNC_START(dateevent_getdate, 1, Nil)
	DLIB_WXGET_BASE(dateevent, DateEvent, evt)
	DeltaWxDateTime *retval = DNEWCLASS(DeltaWxDateTime, (new wxDateTime(evt->GetDate())));
	WX_SETOBJECT(DateTime, retval)
}

DLIB_FUNC_START(dateevent_setdate, 2, Nil)
	DLIB_WXGET_BASE(dateevent, DateEvent, evt)
	DLIB_WXGET_BASE(datetime, DateTime, date)
	evt->SetDate(*date);
}
