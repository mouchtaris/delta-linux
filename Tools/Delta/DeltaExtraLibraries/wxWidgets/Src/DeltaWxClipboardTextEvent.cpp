#include "DeltaWxClipboardTextEvent.h"
#include "DeltaWxCommandEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(clipboardtextevent, name)
#define WX_FUNC(name) WX_FUNC1(clipboardtextevent, name)

WX_FUNC_DEF(construct)

WX_FUNCS_START
	WX_FUNC(construct)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

//DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ClipboardTextEvent, "clipboardtextevent", CommandEvent)
VCLASSID_IMPL(DeltaWxClipboardTextEventClassId, "wx::clipboardtextevent")
DLIB_WXMAKE_GETTER_CHECKER_METHODS_TABLE(ClipboardTextEvent, "clipboardtextevent")
void ClipboardTextEventUtils::InstallAll(DeltaTable *methods)
{
	DPTR(methods)->DelegateInternal(CommandEventUtils::GetMethods());
}

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ClipboardTextEvent, val, clipboardtextevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ClipboardTextEvent,clipboardtextevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(clipboardtextevent_construct, 0, 2, Nil)
	int type = wxEVT_NULL;
	int winid = 0;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ClipboardTextEvent, new wxClipboardTextEvent(type, winid))
}
