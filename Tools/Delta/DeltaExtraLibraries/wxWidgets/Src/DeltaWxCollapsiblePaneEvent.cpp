#include "DeltaWxCollapsiblePaneEvent.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(collapsiblepaneevent, name)
#define WX_FUNC(name) WX_FUNC1(collapsiblepaneevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getcollapsed)
WX_FUNC_DEF(setcollapsed)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getcollapsed),
	WX_FUNC(setcollapsed)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getcollapsed", "setcollapsed")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(CollapsiblePaneEvent, "collapsiblepaneevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(CollapsiblePaneEvent, val, collapsiblepaneevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetCollapsed (void* val, DeltaValue* at) 
{
	wxCollapsiblePaneEvent *ev = DLIB_WXTYPECAST_BASE(CollapsiblePaneEvent, val, collapsiblepaneevent);
	WX_SETBOOL_EX(*at, ev->GetCollapsed())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "collapsed",			&GetCollapsed,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(CollapsiblePaneEvent,collapsiblepaneevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(collapsiblepaneevent_construct, 0, 3, Nil)
	wxCollapsiblePaneEvent *evt = (wxCollapsiblePaneEvent*) 0;
	if (n == 0)
		evt = new wxCollapsiblePaneEvent();
	else if (n == 3) {
		DLIB_WXGET_BASE(object, Object, generator)
		WX_GETDEFINE(id)
		WX_GETBOOL(collapsed)
		evt = new wxCollapsiblePaneEvent(generator, id, collapsed);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(CollapsiblePaneEvent, evt)
}

WX_FUNC_START(collapsiblepaneevent_getcollapsed, 1, Nil)
	DLIB_WXGET_BASE(collapsiblepaneevent, CollapsiblePaneEvent, evt)
	WX_SETBOOL(evt->GetCollapsed())
}

WX_FUNC_START(collapsiblepaneevent_setcollapsed, 2, Nil)
	DLIB_WXGET_BASE(collapsiblepaneevent, CollapsiblePaneEvent, evt)
	WX_GETBOOL(collapsed)
	evt->SetCollapsed(collapsed);
}
