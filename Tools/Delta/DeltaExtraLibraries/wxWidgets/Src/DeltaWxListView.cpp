#include "DeltaWxListView.h"
#include "DeltaWxListCtrl.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(listview, name)
#define WX_FUNC(name) WX_FUNC1(listview, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(clearcolumnimage)
WX_FUNC_DEF(focus)
WX_FUNC_DEF(getfirstselected)
WX_FUNC_DEF(getfocuseditem)
WX_FUNC_DEF(getnextselected)
WX_FUNC_DEF(isselected)
WX_FUNC_DEF(select)
WX_FUNC_DEF(setcolumnimage)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(clearcolumnimage),
	WX_FUNC(focus),
	WX_FUNC(getfirstselected),
	WX_FUNC(getfocuseditem),
	WX_FUNC(getnextselected),
	WX_FUNC(isselected),
	WX_FUNC(select),
	WX_FUNC(setcolumnimage)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setcolumnimage")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ListView, "listview", ListCtrl)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ListView, val, listview) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxListCtrl *_parent = DLIB_WXTYPECAST_BASE(ListCtrl, val, listctrl);
	DeltaWxListCtrl *parent = DNEWCLASS(DeltaWxListCtrl, (_parent));
	WX_SETOBJECT_EX(*at, ListCtrl, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "ListCtrl",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ListView,listview)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(listview_construct, 0, 7, Nil)
	wxListView *wxlistview = (wxListView*) 0;
	DeltaWxListView *listview = (DeltaWxListView*) 0;
	if (n == 0) {
		wxlistview = new wxListView();
	} else {
		DLIB_WXGET_BASE(window, Window, parent)
		int winid = wxID_ANY;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxLC_REPORT;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxListCtrlNameStr;
		if (n >= 2) { WX_GETDEFINE_DEFINED(winid) }
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 7) { WX_GETSTRING_DEFINED(name) }
		wxlistview = new wxListView(parent, winid, pos, size, style, *validator, name);
	}
	if (wxlistview) listview = DNEWCLASS(DeltaWxListView, (wxlistview));
	WX_SETOBJECT(ListView, listview)
}

DLIB_FUNC_START(listview_destruct, 1, Nil)
	DLIB_WXDELETE(listview, ListView, listview)
}

DLIB_FUNC_START(listview_clearcolumnimage, 2, Nil)
	DLIB_WXGET_BASE(listview, ListView, listview)
	WX_GETNUMBER(col)
	listview->ClearColumnImage(col);
}

DLIB_FUNC_START(listview_focus, 2, Nil)
	DLIB_WXGET_BASE(listview, ListView, listview)
	WX_GETNUMBER(index)
	listview->Focus(index);
}

DLIB_FUNC_START(listview_getfirstselected, 1, Nil)
	DLIB_WXGET_BASE(listview, ListView, listview)
	WX_SETNUMBER(listview->GetFirstSelected())
}

DLIB_FUNC_START(listview_getfocuseditem, 1, Nil)
	DLIB_WXGET_BASE(listview, ListView, listview)
	WX_SETNUMBER(listview->GetFocusedItem())
}

DLIB_FUNC_START(listview_getnextselected, 2, Nil)
	DLIB_WXGET_BASE(listview, ListView, listview)
	WX_GETNUMBER(item)
	WX_SETNUMBER(listview->GetNextSelected(item))
}

DLIB_FUNC_START(listview_isselected, 2, Nil)
	DLIB_WXGET_BASE(listview, ListView, listview)
	WX_GETNUMBER(index)
	WX_SETBOOL(listview->IsSelected(index))
}

WX_FUNC_ARGRANGE_START(listview_select, 2, 3, Nil)
	DLIB_WXGET_BASE(listview, ListView, listview)
	WX_GETNUMBER(num)
	bool on = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(on) }
	listview->Select(num, on);
}

DLIB_FUNC_START(listview_setcolumnimage, 3, Nil)
	DLIB_WXGET_BASE(listview, ListView, listview)
	WX_GETNUMBER(col)
	WX_GETNUMBER(image)
	listview->SetColumnImage(col, image);
}
