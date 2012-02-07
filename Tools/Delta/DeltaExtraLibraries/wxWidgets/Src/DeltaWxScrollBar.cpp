#include "DeltaWxScrollBar.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxValidator.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(scrollbar, name)
#define WX_FUNC(name) WX_FUNC1(scrollbar, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getrange)
WX_FUNC_DEF(getpagesize)
WX_FUNC_DEF(getthumbposition)
WX_FUNC_DEF(getthumbsize)
WX_FUNC_DEF(setthumbposition)
WX_FUNC_DEF(setscrollbar)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getrange),
	WX_FUNC(getpagesize),
	WX_FUNC(getthumbposition),
	WX_FUNC(getthumbsize),
	WX_FUNC(setthumbposition),
	WX_FUNC(setscrollbar)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setscrollbar")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ScrollBar, "scrollbar", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ScrollBar, val, scrollbar) ?
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(ScrollBar, scrollbar);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(scrollbar_construct, 0, 7, Nil)
	wxScrollBar *wxscrollbar = (wxScrollBar*) 0;
	DeltaWxScrollBar *scrollbar = (DeltaWxScrollBar*) 0;
	if (n == 0) {
		wxscrollbar = new wxScrollBar();
	} else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxSB_HORIZONTAL;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxScrollBarNameStr;
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 7) { WX_GETSTRING_DEFINED(name) }
		wxscrollbar = new wxScrollBar(parent, id, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxscrollbar) scrollbar = DNEWCLASS(DeltaWxScrollBar, (wxscrollbar));
	WX_SETOBJECT(ScrollBar, scrollbar)
}

DLIB_FUNC_START(scrollbar_destruct, 1, Nil)
	DLIB_WXDELETE(scrollbar, ScrollBar, scrollbar)
}

WX_FUNC_ARGRANGE_START(scrollbar_create, 3, 8, Nil)
	DLIB_WXGET_BASE(scrollbar, ScrollBar, scrollbar)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxSB_HORIZONTAL;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxScrollBarNameStr;
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 8) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(scrollbar->Create(parent, id, pos, size, style, *validator, name))
}

DLIB_FUNC_START(scrollbar_getrange, 1, Nil)
	DLIB_WXGET_BASE(scrollbar, ScrollBar, scrollbar)
	WX_SETNUMBER(scrollbar->GetRange())
}

DLIB_FUNC_START(scrollbar_getpagesize, 1, Nil)
	DLIB_WXGET_BASE(scrollbar, ScrollBar, scrollbar)
	WX_SETNUMBER(scrollbar->GetPageSize())
}

DLIB_FUNC_START(scrollbar_getthumbposition, 1, Nil)
	DLIB_WXGET_BASE(scrollbar, ScrollBar, scrollbar)
	WX_SETNUMBER(scrollbar->GetThumbPosition())
}

DLIB_FUNC_START(scrollbar_getthumbsize, 1, Nil)
	DLIB_WXGET_BASE(scrollbar, ScrollBar, scrollbar)
	WX_SETNUMBER(scrollbar->GetThumbSize())
}

DLIB_FUNC_START(scrollbar_setthumbposition, 2, Nil)
	DLIB_WXGET_BASE(scrollbar, ScrollBar, scrollbar)
	WX_GETNUMBER(viewStart)
	scrollbar->SetThumbPosition(viewStart);
}

WX_FUNC_ARGRANGE_START(scrollbar_setscrollbar, 5, 6, Nil)
	DLIB_WXGET_BASE(scrollbar, ScrollBar, scrollbar)
	WX_GETNUMBER(position)
	WX_GETNUMBER(thumbSize)
	WX_GETNUMBER(range)
	WX_GETNUMBER(pageSize)
	bool refresh = true;
	if (n >= 6) { WX_GETBOOL_DEFINED(refresh) }
	scrollbar->SetScrollbar(position, thumbSize, range, pageSize, refresh);
}
