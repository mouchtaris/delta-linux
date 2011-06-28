#include "DeltaWxStaticBox.h"
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
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(staticbox, name)
#define WX_FUNC(name) WX_FUNC1(staticbox, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "create")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(StaticBox, "staticbox", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(StaticBox, val, staticbox) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxControl *_parent = DLIB_WXTYPECAST_BASE(Control, val, control);
	DeltaWxControl *parent = DNEWCLASS(DeltaWxControl, (_parent));
	WX_SETOBJECT_EX(*at, Control, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(StaticBox, staticbox);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(staticbox_construct, 0, 7, Nil)
	wxStaticBox *wxstatbox = (wxStaticBox*) 0;
	DeltaWxStaticBox *statbox = (DeltaWxStaticBox*) 0;
	if (n == 0) {
		wxstatbox = new wxStaticBox();
	} else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		WX_GETSTRING(label)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxString name = wxStaticBoxNameStr;
		if (n >= 4) { DLIB_WXGET_BASE(point, Point, _pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGET_BASE(size, Size, _size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { WX_GETSTRING_DEFINED(name) }
		wxstatbox = new wxStaticBox(parent, id, label, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxstatbox) statbox = DNEWCLASS(DeltaWxStaticBox, (wxstatbox));
	WX_SETOBJECT(StaticBox, statbox)
}

DLIB_FUNC_START(staticbox_destruct, 1, Nil)
	DLIB_WXDELETE(staticbox, StaticBox, statbox)
}

WX_FUNC_ARGRANGE_START(staticbox_create, 4, 8, Nil)
	DLIB_WXGET_BASE(staticbox, StaticBox, statbox)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	WX_GETSTRING(label)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = 0;
	wxString name = wxStaticBoxNameStr;
	if (n >= 5) { DLIB_WXGET_BASE(point, Point, _pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGET_BASE(size, Size, _size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(statbox->Create(parent, id, label, pos, size, style, name))
}
