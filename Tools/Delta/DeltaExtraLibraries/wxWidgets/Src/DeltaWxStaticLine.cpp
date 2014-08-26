#include "DeltaWxStaticLine.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(staticline, name)
#define WX_FUNC(name) WX_FUNC1(staticline, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(isvertical)
WX_FUNC_DEF(getdefaultsize)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(isvertical),
	WX_FUNC(getdefaultsize)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "getdefaultsize")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(StaticLine, "staticline", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(StaticLine, val, staticline) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(StaticLine,staticline)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(staticline_construct, 0, 6, Nil)
	wxStaticLine *statline = (wxStaticLine*) 0;
	if (n == 0) {
		statline = new wxStaticLine();
	} else {
		DLIB_WXGET_BASE(window, Window, parent)
		int id = wxID_ANY;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxLI_HORIZONTAL;
		wxString name = wxStaticLineNameStr;
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { WX_GETSTRING_DEFINED(name) }
		statline = new wxStaticLine(parent, id, pos, size, style, name);
	}
	WX_SET_WINDOW_OBJECT(StaticLine, statline)
}

WX_FUNC_ARGRANGE_START(staticline_create, 2, 7, Nil)
	DLIB_WXGET_BASE(staticline, StaticLine, statline)
	DLIB_WXGET_BASE(window, Window, parent)
	int id = wxID_ANY;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxLI_HORIZONTAL;
	wxString name = wxStaticLineNameStr;
	if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 6) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(statline->Create(parent, id, pos, size, style, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(statline);
}

WX_FUNC_START(staticline_isvertical, 1, Nil)
	DLIB_WXGET_BASE(staticline, StaticLine, statline)
	WX_SETBOOL(statline->IsVertical())
}

WX_FUNC_START(staticline_getdefaultsize, 1, Nil)
	DLIB_WXGET_BASE(staticline, StaticLine, statline)
	WX_SETNUMBER(statline->GetDefaultSize())
}
