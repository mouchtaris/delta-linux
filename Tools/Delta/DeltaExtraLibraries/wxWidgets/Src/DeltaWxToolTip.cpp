#include "DeltaWxToolTip.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(tooltip, name)
#define WX_FUNC(name) WX_FUNC1(tooltip, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(enable)
WX_FUNC_DEF(setdelay)
WX_FUNC_DEF(settip)
WX_FUNC_DEF(gettip)
WX_FUNC_DEF(getwindow)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(enable),
	WX_FUNC(setdelay),
	WX_FUNC(destruct),
	WX_FUNC(settip),
	WX_FUNC(gettip),
	WX_FUNC(getwindow)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(3, uarraysize(funcs) - 3, "destruct", "getwindow")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ToolTip, "tooltip", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ToolTip, val, tooltip) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetTip (void* val, DeltaValue* at) 
{
	wxToolTip *tooltip = DLIB_WXTYPECAST_BASE(ToolTip, val, tooltip);
	WX_SETSTRING_EX(*at, tooltip->GetTip())
	return true;
}

static bool GetWindow (void* val, DeltaValue* at) 
{
	wxToolTip *tooltip = DLIB_WXTYPECAST_BASE(ToolTip, val, tooltip);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, tooltip->GetWindow())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "text",				&GetTip,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "window",				&GetWindow,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ToolTip,tooltip)

////////////////////////////////////////////////////////////////

WX_FUNC_START(tooltip_construct, 1, Nil)
	WX_GETSTRING(tip)
	WX_SETOBJECT(ToolTip, new wxToolTip(tip))
}

WX_FUNC_START(tooltip_destruct, 1, Nil)
	DLIB_WXDELETE(tooltip, ToolTip, tooltip)
}

WX_FUNC_START(tooltip_enable, 1, Nil)
	WX_GETBOOL(flag)
	wxToolTip::Enable(flag);
}

WX_FUNC_START(tooltip_setdelay, 1, Nil)
	WX_GETNUMBER(delay)
	wxToolTip::SetDelay(delay);
}

WX_FUNC_START(tooltip_settip, 2, Nil)
	DLIB_WXGET_BASE(tooltip, ToolTip, tooltip)
	WX_GETSTRING(tip)
	tooltip->SetTip(tip);
}

WX_FUNC_START(tooltip_gettip, 1, Nil)
	DLIB_WXGET_BASE(tooltip, ToolTip, tooltip)
	WX_SETSTRING(tooltip->GetTip());
}

WX_FUNC_START(tooltip_getwindow, 1, Nil)
	DLIB_WXGET_BASE(tooltip, ToolTip, tooltip)
	wxWindow* retval	= tooltip->GetWindow();;
	WX_SETOBJECT(Window, retval)
}
