#include "DeltaWxNotebook.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxColour.h"
#include "DeltaWxNotebookEvent.h"
#include "DeltaWxImageList.h"
#include "DeltaWxSizer.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(notebook, name)
#define WX_FUNC(name) WX_FUNC1(notebook, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(addpage)
WX_FUNC_DEF(advanceselection)
WX_FUNC_DEF(assignimagelist)
WX_FUNC_DEF(changeselection)
WX_FUNC_DEF(create)
WX_FUNC_DEF(deleteallpages)
WX_FUNC_DEF(deletepage)
WX_FUNC_DEF(getcurrentpage)
WX_FUNC_DEF(getimagelist)
WX_FUNC_DEF(getpage)
WX_FUNC_DEF(getpagecount)
WX_FUNC_DEF(getpageimage)
WX_FUNC_DEF(getpagetext)
WX_FUNC_DEF(getrowcount)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(getthemebackgroundcolour)
WX_FUNC_DEF(hittest)
WX_FUNC_DEF(insertpage)
WX_FUNC_DEF(onselchange)
WX_FUNC_DEF(removepage)
WX_FUNC_DEF(setimagelist)
WX_FUNC_DEF(setpadding)
WX_FUNC_DEF(setpagesize)
WX_FUNC_DEF(setpageimage)
WX_FUNC_DEF(setpagetext)
WX_FUNC_DEF(setselection)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(addpage),
	WX_FUNC(advanceselection),
	WX_FUNC(assignimagelist),
	WX_FUNC(changeselection),
	WX_FUNC(create),
	WX_FUNC(deleteallpages),
	WX_FUNC(deletepage),
	WX_FUNC(getcurrentpage),
	WX_FUNC(getimagelist),
	WX_FUNC(getpage),
	WX_FUNC(getpagecount),
	WX_FUNC(getpageimage),
	WX_FUNC(getpagetext),
	WX_FUNC(getrowcount),
	WX_FUNC(getselection),
	WX_FUNC(getthemebackgroundcolour),
	WX_FUNC(hittest),
	WX_FUNC(insertpage),
	WX_FUNC(onselchange),
	WX_FUNC(removepage),
	WX_FUNC(setimagelist),
	WX_FUNC(setpadding),
	WX_FUNC(setpagesize),
	WX_FUNC(setpageimage),
	WX_FUNC(setpagetext),
	WX_FUNC(setselection)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setselection")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Notebook, "notebook", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Notebook, val, notebook) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxControl *_parent = DLIB_WXTYPECAST_BASE(Control, val, control);
	DeltaWxControl *parent = DNEWCLASS(DeltaWxControl, (_parent));
	WX_SETOBJECT_EX(*at, Control, parent)
	return true;
}

static bool GetPages (void* val, DeltaValue* at) 
{
	wxNotebook *book = DLIB_WXTYPECAST_BASE(Notebook, val, notebook);
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, pageSize = (int)book->GetPageCount(); i < pageSize; ++i) {
		DeltaValue value;
		wxWindow *win = book->GetPage(i);
		DeltaWxWindow *retval = win ? DNEWCLASS(DeltaWxWindow, (win)) : (DeltaWxWindow*) 0;
		WX_SETOBJECT_EX(value, Window, retval)
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static bool GetImageList (void* val, DeltaValue* at) 
{
	wxNotebook *book = DLIB_WXTYPECAST_BASE(Notebook, val, notebook);
	wxImageList *imagelist = book->GetImageList();
	DeltaWxImageList *retval = imagelist ?
		DNEWCLASS(DeltaWxImageList, (imagelist)) :
		(DeltaWxImageList*) 0;
	WX_SETOBJECT_EX(*at, ImageList, retval)
	return true;
}

static bool GetFitToCurrentPage (void* val, DeltaValue* at) 
{
	wxNotebook *book = DLIB_WXTYPECAST_BASE(Notebook, val, notebook);
	at->FromBool(book->GetFitToCurrentPage());
	return true;
}

static bool GetControlSizer (void* val, DeltaValue* at) 
{
	wxNotebook *book = DLIB_WXTYPECAST_BASE(Notebook, val, notebook);
	wxSizer *sizer = book->GetControlSizer();
	DeltaWxSizer *retval = sizer ? DNEWCLASS(DeltaWxSizer, (sizer)) : (DeltaWxSizer*) 0;
	return true;
}

static bool GetControlMargin (void* val, DeltaValue* at) 
{
	wxNotebook *book = DLIB_WXTYPECAST_BASE(Notebook, val, notebook);
	at->FromNumber(book->GetControlMargin());
	return true;
}

static bool GetInternalBorder (void* val, DeltaValue* at) 
{
	wxNotebook *book = DLIB_WXTYPECAST_BASE(Notebook, val, notebook);
	at->FromNumber(book->GetInternalBorder());
	return true;
}

static bool GetSelection (void* val, DeltaValue* at) 
{
	wxNotebook *book = DLIB_WXTYPECAST_BASE(Notebook, val, notebook);
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Notebook,notebook)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(notebook_construct, 0, 6, Nil)
	wxNotebook *wxnotebk = (wxNotebook*) 0;
	DeltaWxNotebook *notebk = (DeltaWxNotebook*) 0;
	if (n == 0)
		wxnotebk = new wxNotebook();
	else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(winid)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxString name = wxNotebookNameStr;
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { WX_GETSTRING_DEFINED(name) }
		wxnotebk = new wxNotebook(parent, winid, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		return;
	}
	if (wxnotebk) notebk = DNEWCLASS(DeltaWxNotebook, (wxnotebk));
	WX_SETOBJECT(Notebook, notebk)
}

DLIB_FUNC_START(notebook_destruct, 1, Nil)
	DLIB_WXDELETE(notebook, Notebook, notebk)
}

WX_FUNC_ARGRANGE_START(notebook_addpage, 3, 5, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool bSelect = false;
	int imageId = -1;
	if (n >= 4) { WX_GETBOOL_DEFINED(bSelect) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(notebk->AddPage(page, text, bSelect, imageId))
}

WX_FUNC_ARGRANGE_START(notebook_advanceselection, 1, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	bool forward = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(forward) }
	notebk->AdvanceSelection(forward);
}

DLIB_FUNC_START(notebook_assignimagelist, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	notebk->AssignImageList(imageList);
}

DLIB_FUNC_START(notebook_changeselection, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(size)
	WX_SETNUMBER(notebk->ChangeSelection(size))
}

WX_FUNC_ARGRANGE_START(notebook_create, 3, 7, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(winid)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = 0;
	wxString name = wxNotebookNameStr;
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(notebk->Create(parent, winid, pos, size, style, name))
}

DLIB_FUNC_START(notebook_deleteallpages, 1, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_SETBOOL(notebk->DeleteAllPages())
}

DLIB_FUNC_START(notebook_deletepage, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(page)
	WX_SETBOOL(notebk->DeletePage(page))
}

DLIB_FUNC_START(notebook_getcurrentpage, 1, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WXNEWCLASS(DeltaWxWindow, retval, wxWindow, notebk->GetCurrentPage())
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(notebook_getimagelist, 1, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WXNEWCLASS(DeltaWxImageList, retval, wxImageList, notebk->GetImageList())
	WX_SETOBJECT(ImageList, retval)
}

DLIB_FUNC_START(notebook_getpage, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(page)
	WXNEWCLASS(DeltaWxWindow, retval, wxWindow, notebk->GetPage(page))
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(notebook_getpagecount, 1, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_SETNUMBER(notebk->GetPageCount())
}

DLIB_FUNC_START(notebook_getpageimage, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(nPage)
	WX_SETNUMBER(notebk->GetPageImage(nPage))
}

DLIB_FUNC_START(notebook_getpagetext, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(page)
	WX_SETSTRING(notebk->GetPageText(page))
}

DLIB_FUNC_START(notebook_getrowcount, 1, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_SETNUMBER(notebk->GetRowCount())
}

DLIB_FUNC_START(notebook_getselection, 1, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_SETNUMBER(notebk->GetSelection())
}

DLIB_FUNC_START(notebook_getthemebackgroundcolour, 1, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(notebk->GetThemeBackgroundColour())));
	WX_SETOBJECT(Colour, retval)
}

WX_FUNC_ARGRANGE_START(notebook_hittest, 2, 3, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	DLIB_WXGETPOINT_BASE(pt)
	long style;
	WX_SETNUMBER(notebk->HitTest(*pt, &style))
	if (n >= 3) {
		WX_GETTABLE(style_table)
		WX_SETTABLE_RETVAL(style_table, DeltaValue((DeltaNumberValueType)style))
	}
}

WX_FUNC_ARGRANGE_START(notebook_insertpage, 4, 6, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(index)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool select = false;
	int imageId = -1;
	if (n >= 5) { WX_GETBOOL_DEFINED(select) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(notebk->InsertPage(index, page, text, select, imageId))
}

DLIB_FUNC_START(notebook_onselchange, 2, Nil)
#if defined (__WXMSW__)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	DLIB_WXGET_BASE(notebookevent, NotebookEvent, ev)
	notebk->OnSelChange(*ev);
#else
	DLIB_ERROR_CHECK(
		true,
		"This function is only available on MS Windows implementation of wxWidgets."
	);
#endif //__WXMSW__
}

DLIB_FUNC_START(notebook_removepage, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(page)
	WX_SETBOOL(notebk->RemovePage(page))
}

DLIB_FUNC_START(notebook_setimagelist, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	notebk->SetImageList(imageList);
}

DLIB_FUNC_START(notebook_setpadding, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	DLIB_WXGETSIZE_BASE(padding)
	notebk->SetPadding(*padding);
}

DLIB_FUNC_START(notebook_setpagesize, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	DLIB_WXGETSIZE_BASE(size)
	notebk->SetPageSize(*size);
}

DLIB_FUNC_START(notebook_setpageimage, 3, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(page)
	WX_GETNUMBER(image)
	WX_SETBOOL(notebk->SetPageImage(page, image))
}

DLIB_FUNC_START(notebook_setpagetext, 3, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(page)
	WX_GETSTRING(text)
	WX_SETBOOL(notebk->SetPageText(page, text))
}

DLIB_FUNC_START(notebook_setselection, 2, Nil)
	DLIB_WXGET_BASE(notebook, Notebook, notebk)
	WX_GETNUMBER(page)
	WX_SETNUMBER(notebk->SetSelection(page))
}
