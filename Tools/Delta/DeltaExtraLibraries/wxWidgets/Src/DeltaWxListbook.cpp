#include "DeltaWxListbook.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxImageList.h"
#include "DeltaWxSizer.h"
#include "DeltaWxListView.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(listbook, name)
#define WX_FUNC(name) WX_FUNC1(listbook, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(addpage)
WX_FUNC_DEF(advanceselection)
WX_FUNC_DEF(assignimagelist)
WX_FUNC_DEF(calcsizefrompage)
WX_FUNC_DEF(changeselection)
WX_FUNC_DEF(create)
WX_FUNC_DEF(deleteallpages)
WX_FUNC_DEF(deletepage)
WX_FUNC_DEF(getcurrentpage)
WX_FUNC_DEF(getimagelist)
WX_FUNC_DEF(getlistview)
WX_FUNC_DEF(getpage)
WX_FUNC_DEF(getpagecount)
WX_FUNC_DEF(getpageimage)
WX_FUNC_DEF(getpagetext)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(hittest)
WX_FUNC_DEF(insertpage)
WX_FUNC_DEF(removepage)
WX_FUNC_DEF(setimagelist)
WX_FUNC_DEF(setpagesize)
WX_FUNC_DEF(setpageimage)
WX_FUNC_DEF(setpagetext)
WX_FUNC_DEF(setselection)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(addpage),
	WX_FUNC(advanceselection),
	WX_FUNC(assignimagelist),
	WX_FUNC(calcsizefrompage),
	WX_FUNC(changeselection),
	WX_FUNC(create),
	WX_FUNC(deleteallpages),
	WX_FUNC(deletepage),
	WX_FUNC(getcurrentpage),
	WX_FUNC(getimagelist),
	WX_FUNC(getlistview),
	WX_FUNC(getpage),
	WX_FUNC(getpagecount),
	WX_FUNC(getpageimage),
	WX_FUNC(getpagetext),
	WX_FUNC(getselection),
	WX_FUNC(hittest),
	WX_FUNC(insertpage),
	WX_FUNC(removepage),
	WX_FUNC(setimagelist),
	WX_FUNC(setpagesize),
	WX_FUNC(setpageimage),
	WX_FUNC(setpagetext),
	WX_FUNC(setselection)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "addpage", "setselection")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Listbook, "listbook", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Listbook, val, listbook) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetPages (void* val, DeltaValue* at) 
{
	wxListbook *book = DLIB_WXTYPECAST_BASE(Listbook, val, listbook);
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, pageSize = (int)book->GetPageCount(); i < pageSize; ++i) {
		DeltaValue value;
		WX_SETOBJECT_NO_CONTEXT_EX(value, Window, book->GetPage(i))
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static bool GetImageList (void* val, DeltaValue* at) 
{
	wxListbook *book = DLIB_WXTYPECAST_BASE(Listbook, val, listbook);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ImageList, book->GetImageList())
	return true;
}

static bool GetFitToCurrentPage (void* val, DeltaValue* at) 
{
	wxListbook *book = DLIB_WXTYPECAST_BASE(Listbook, val, listbook);
	at->FromBool(book->GetFitToCurrentPage());
	return true;
}

static bool GetControlSizer (void* val, DeltaValue* at) 
{
	wxListbook *book = DLIB_WXTYPECAST_BASE(Listbook, val, listbook);
	wxSizer *sizer = book->GetControlSizer();
	DeltaWxSizer *retval = sizer ? DNEWCLASS(DeltaWxSizer, (sizer)) : (DeltaWxSizer*) 0;
	return true;
}

static bool GetControlMargin (void* val, DeltaValue* at) 
{
	wxListbook *book = DLIB_WXTYPECAST_BASE(Listbook, val, listbook);
	at->FromNumber(book->GetControlMargin());
	return true;
}

static bool GetInternalBorder (void* val, DeltaValue* at) 
{
	wxListbook *book = DLIB_WXTYPECAST_BASE(Listbook, val, listbook);
	at->FromNumber(book->GetInternalBorder());
	return true;
}

static bool GetSelection (void* val, DeltaValue* at) 
{
	wxListbook *book = DLIB_WXTYPECAST_BASE(Listbook, val, listbook);
	at->FromNumber(book->GetSelection());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pages",				&GetPages,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "imagelist",			&GetImageList,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "fitToCurrentPage",	&GetFitToCurrentPage,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "controlSizer",		&GetControlSizer,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "controlMargin",		&GetControlMargin,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "internalBorder",		&GetInternalBorder,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "selection",			&GetSelection,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Listbook,listbook)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(listbook_construct, 0, 6, Nil)
	wxListbook *listbk = (wxListbook*) 0;
	if (n == 0)
		listbk = new wxListbook();
	else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxString name = wxEmptyString;
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { WX_GETSTRING_DEFINED(name) }
		listbk = new wxListbook(parent, id, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		return;
	}
	WX_SET_WINDOW_OBJECT(Listbook, listbk)
}

WX_FUNC_ARGRANGE_START(listbook_addpage, 3, 5, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool bSelect = false;
	int imageId = -1;
	if (n >= 4) { WX_GETBOOL_DEFINED(bSelect) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(listbk->AddPage(page, text, bSelect, imageId))
}

WX_FUNC_ARGRANGE_START(listbook_advanceselection, 1, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	bool forward = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(forward) }
	listbk->AdvanceSelection(forward);
}

WX_FUNC_START(listbook_assignimagelist, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	listbk->AssignImageList(imageList);
}

WX_FUNC_START(listbook_calcsizefrompage, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	DLIB_WXGETSIZE_BASE(sizePage)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(listbk->CalcSizeFromPage(*sizePage)))
}

WX_FUNC_START(listbook_changeselection, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(size)
	WX_SETNUMBER(listbk->ChangeSelection(size))
}

WX_FUNC_ARGRANGE_START(listbook_create, 3, 7, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = 0;
	wxString name = wxEmptyString;
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(listbk->Create(parent, id, pos, size, style, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(listbk);
}

WX_FUNC_START(listbook_deleteallpages, 1, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_SETBOOL(listbk->DeleteAllPages())
}

WX_FUNC_START(listbook_deletepage, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(page)
	WX_SETBOOL(listbk->DeletePage(page))
}

WX_FUNC_START(listbook_getcurrentpage, 1, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	wxWindow* retval	= listbk->GetCurrentPage();
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_START(listbook_getimagelist, 1, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	wxImageList* retval	= listbk->GetImageList();
	WX_SETOBJECT(ImageList, retval)
}

WX_FUNC_START(listbook_getlistview, 1, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_SETOBJECT(ListView, listbk->GetListView())
}

WX_FUNC_START(listbook_getpage, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(page)
	wxWindow* retval	= listbk->GetPage(page);
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_START(listbook_getpagecount, 1, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_SETNUMBER(listbk->GetPageCount())
}

WX_FUNC_START(listbook_getpageimage, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(nPage)
	WX_SETNUMBER(listbk->GetPageImage(nPage))
}

WX_FUNC_START(listbook_getpagetext, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(page)
	WX_SETSTRING(listbk->GetPageText(page))
}

WX_FUNC_START(listbook_getselection, 1, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_SETNUMBER(listbk->GetSelection())
}

WX_FUNC_ARGRANGE_START(listbook_hittest, 2, 3, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	DLIB_WXGETPOINT_BASE(pt)
	long style;
	WX_SETNUMBER(listbk->HitTest(*pt, &style))
	if (n >= 3) {
		WX_GETTABLE(style_table)
		WX_SETTABLE_RETVAL(style_table, DeltaValue((DeltaNumberValueType)style))
	}
}

WX_FUNC_ARGRANGE_START(listbook_insertpage, 4, 6, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(index)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool select = false;
	int imageId = -1;
	if (n >= 5) { WX_GETBOOL_DEFINED(select) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(listbk->InsertPage(index, page, text, select, imageId))
}

WX_FUNC_START(listbook_removepage, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(page)
	WX_SETBOOL(listbk->RemovePage(page))
}

WX_FUNC_START(listbook_setimagelist, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	listbk->SetImageList(imageList);
}

WX_FUNC_START(listbook_setpagesize, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	DLIB_WXGETSIZE_BASE(size)
	listbk->SetPageSize(*size);
}

WX_FUNC_START(listbook_setpageimage, 3, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(page)
	WX_GETNUMBER(image)
	WX_SETBOOL(listbk->SetPageImage(page, image))
}

WX_FUNC_START(listbook_setpagetext, 3, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(page)
	WX_GETSTRING(text)
	WX_SETBOOL(listbk->SetPageText(page, text))
}

WX_FUNC_START(listbook_setselection, 2, Nil)
	DLIB_WXGET_BASE(listbook, Listbook, listbk)
	WX_GETNUMBER(page)
	WX_SETNUMBER(listbk->SetSelection(page))
}
