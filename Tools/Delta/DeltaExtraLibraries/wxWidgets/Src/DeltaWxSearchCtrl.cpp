#include "DeltaWxSearchCtrl.h"
#include "DeltaWxTextCtrl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxValidator.h"
#include "DeltaWxMenu.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(searchctrl, name)
#define WX_FUNC(name) WX_FUNC1(searchctrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(setmenu)
WX_FUNC_DEF(getmenu)
WX_FUNC_DEF(showsearchbutton)
WX_FUNC_DEF(issearchbuttonvisible)
WX_FUNC_DEF(showcancelbutton)
WX_FUNC_DEF(iscancelbuttonvisible)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(setmenu),
	WX_FUNC(getmenu),
	WX_FUNC(showsearchbutton),
	WX_FUNC(issearchbuttonvisible),
	WX_FUNC(showcancelbutton),
	WX_FUNC(iscancelbuttonvisible)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "iscancelbuttonvisible")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SearchCtrl, "searchctrl", TextCtrl)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SearchCtrl, val, searchctrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxTextCtrl *_parent = DLIB_WXTYPECAST_BASE(TextCtrl, val, textctrl);
	DeltaWxTextCtrl *parent = DNEWCLASS(DeltaWxTextCtrl, (_parent));
	WX_SETOBJECT_EX(*at, TextCtrl, parent)
	return true;
}

static bool GetMenu (void* val, DeltaValue* at) 
{
	wxSearchCtrl *ctrl = DLIB_WXTYPECAST_BASE(SearchCtrl, val, searchctrl);
	wxMenu *menu = ctrl->GetMenu();
	DeltaWxMenu *retval = menu ? DNEWCLASS(DeltaWxMenu, (menu)) : (DeltaWxMenu*) 0;
	WX_SETOBJECT_EX(*at, Menu, retval)
	return true;
}

static bool GetIsSearchButtonVisible (void* val, DeltaValue* at) 
{
	wxSearchCtrl *ctrl = DLIB_WXTYPECAST_BASE(SearchCtrl, val, searchctrl);
	WX_SETBOOL_EX(*at, ctrl->IsSearchButtonVisible())
	return true;
}

static bool GetIsCancelButtonVisible (void* val, DeltaValue* at) 
{
	wxSearchCtrl *ctrl = DLIB_WXTYPECAST_BASE(SearchCtrl, val, searchctrl);
	WX_SETBOOL_EX(*at, ctrl->IsCancelButtonVisible())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",					&GetKeys,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "TextCtrl",				&GetBaseClass,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "menu",					&GetMenu,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isSearchButtonVisible",	&GetIsSearchButtonVisible,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isCancelButtonVisible",	&GetIsCancelButtonVisible,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SearchCtrl, searchctrl);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(searchctrl_construct, 0, 8, Nil)
	wxSearchCtrl *wxctrl = (wxSearchCtrl*) 0;
	DeltaWxSearchCtrl *ctrl = (DeltaWxSearchCtrl*) 0;
	if (n == 0) {
		wxctrl = new wxSearchCtrl();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxString value = wxEmptyString;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxSearchCtrlNameStr;
		if (n >= 3) { WX_GETSTRING_DEFINED(value) }
		if (n >= 4) { DLIB_WXGET_BASE(point, Point, _pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGET_BASE(size, Size, _size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		wxctrl = new wxSearchCtrl(parent, id, value, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxctrl) ctrl = DNEWCLASS(DeltaWxSearchCtrl, (wxctrl));
	WX_SETOBJECT(SearchCtrl, ctrl)
}

DLIB_FUNC_START(searchctrl_destruct, 1, Nil)
	DLIB_WXDELETE(searchctrl, SearchCtrl, ctrl)
}

DLIB_FUNC_START(searchctrl_setmenu, 2, Nil)
	DLIB_WXGET_BASE(searchctrl, SearchCtrl, ctrl)
	DLIB_WXGET_BASE(menu, Menu, menu)
	ctrl->SetMenu(menu);
}

DLIB_FUNC_START(searchctrl_getmenu, 1, Nil)
	DLIB_WXGET_BASE(searchctrl, SearchCtrl, ctrl)
	DeltaWxMenu *retval = DNEWCLASS(DeltaWxMenu, (ctrl->GetMenu()));
	WX_SETOBJECT(Menu, retval)
}

DLIB_FUNC_START(searchctrl_showsearchbutton, 2, Nil)
	DLIB_WXGET_BASE(searchctrl, SearchCtrl, ctrl)
	WX_GETBOOL(show)
	ctrl->ShowSearchButton(show);
}

DLIB_FUNC_START(searchctrl_issearchbuttonvisible, 1, Nil)
	DLIB_WXGET_BASE(searchctrl, SearchCtrl, ctrl)
	WX_SETBOOL(ctrl->IsSearchButtonVisible())
}

DLIB_FUNC_START(searchctrl_showcancelbutton, 2, Nil)
	DLIB_WXGET_BASE(searchctrl, SearchCtrl, ctrl)
	WX_GETBOOL(show)
	ctrl->ShowCancelButton(show);
}

DLIB_FUNC_START(searchctrl_iscancelbuttonvisible, 1, Nil)
	DLIB_WXGET_BASE(searchctrl, SearchCtrl, ctrl)
	WX_SETBOOL(ctrl->IsCancelButtonVisible())
}
