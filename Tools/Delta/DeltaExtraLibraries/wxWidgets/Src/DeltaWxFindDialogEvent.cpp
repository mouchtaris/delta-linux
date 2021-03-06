#include "DeltaWxFindDialogEvent.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxFindReplaceDialog.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(finddialogevent, name)
#define WX_FUNC(name) WX_FUNC1(finddialogevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getflags)
WX_FUNC_DEF(getfindstring)
WX_FUNC_DEF(getreplacestring)
WX_FUNC_DEF(getdialog)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getflags),
	WX_FUNC(getfindstring),
	WX_FUNC(getreplacestring),
	WX_FUNC(getdialog)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getflags", "getdialog")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FindDialogEvent, "finddialogevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FindDialogEvent, val, finddialogevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetReplaceString (void* val, DeltaValue* at) 
{
	wxFindDialogEvent *ev = DLIB_WXTYPECAST_BASE(FindDialogEvent, val, finddialogevent);
	WX_SETSTRING_EX(*at, ev->GetReplaceString())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "replaceString",		&GetReplaceString,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FindDialogEvent,finddialogevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(finddialogevent_construct, 0, 2, Nil)
	int commandType = wxEVT_NULL, id = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(commandType) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(FindDialogEvent, new wxFindDialogEvent(commandType, id))
}

WX_FUNC_START(finddialogevent_getflags, 1, Nil)
	DLIB_WXGET_BASE(finddialogevent, FindDialogEvent, evt)
	WX_SETNUMBER(evt->GetFlags())
}

WX_FUNC_START(finddialogevent_getfindstring, 1, Nil)
	DLIB_WXGET_BASE(finddialogevent, FindDialogEvent, evt)
	WX_SETSTRING(evt->GetFindString())
}

WX_FUNC_START(finddialogevent_getreplacestring, 1, Nil)
	DLIB_WXGET_BASE(finddialogevent, FindDialogEvent, evt)
	WX_SETSTRING(evt->GetReplaceString())
}

WX_FUNC_START(finddialogevent_getdialog, 1, Nil)
	DLIB_WXGET_BASE(finddialogevent, FindDialogEvent, evt)
	WX_SETOBJECT(FindReplaceDialog, evt->GetDialog())
}
