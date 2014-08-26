#include "DeltaWxDirFilterListCtrl.h"
#include "DeltaWxChoice.h"
#include "DeltaWxGenericDirCtrl.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxCommandEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(dirfilterlistctrl, name)
#define WX_FUNC(name) WX_FUNC1(dirfilterlistctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(fillfilterlist)
WX_FUNC_DEF(init)
WX_FUNC_DEF(onselfilter)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(fillfilterlist),
	WX_FUNC(init),
	WX_FUNC(onselfilter)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "onselfilter")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DirFilterListCtrl, "dirfilterlistctrl", Choice)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DirFilterListCtrl, val, dirfilterlistctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Choice, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Choice",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DirFilterListCtrl,dirfilterlistctrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(dirfilterlistctrl_construct, 0, 5, Nil)
	wxDirFilterListCtrl *dirctrl = (wxDirFilterListCtrl*) 0;
	if (n == 0) {
		dirctrl = new wxDirFilterListCtrl();
	} else {
		DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, parent)
		int id = wxID_ANY;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		dirctrl = new wxDirFilterListCtrl(parent, id, pos, size, style);
	}
	WX_SET_WINDOW_OBJECT(DirFilterListCtrl, dirctrl)
}

WX_FUNC_ARGRANGE_START(dirfilterlistctrl_create, 2, 6, Nil)
	DLIB_WXGET_BASE(dirfilterlistctrl, DirFilterListCtrl, dirctrl)
	DLIB_WXGET_BASE(genericdirctrl, GenericDirCtrl, parent)
	int id = wxID_ANY;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = 0;
	if (n >= 3) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	WX_SETBOOL(dirctrl->Create(parent, id, pos, size, style))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(dirctrl);
}

WX_FUNC_START(dirfilterlistctrl_fillfilterlist, 3, Nil)
	DLIB_WXGET_BASE(dirfilterlistctrl, DirFilterListCtrl, dirctrl)
	WX_GETSTRING(filter)
	WX_GETNUMBER(defaultFilter)
	dirctrl->FillFilterList(filter, defaultFilter);
}

WX_FUNC_START(dirfilterlistctrl_init, 1, Nil)
	DLIB_WXGET_BASE(dirfilterlistctrl, DirFilterListCtrl, dirctrl)
	dirctrl->Init();
}

WX_FUNC_START(dirfilterlistctrl_onselfilter, 2, Nil)
	DLIB_WXGET_BASE(dirfilterlistctrl, DirFilterListCtrl, dirctrl)
	DLIB_WXGET_BASE(commandevent, CommandEvent, evt)
	dirctrl->OnSelFilter(*evt);
}
