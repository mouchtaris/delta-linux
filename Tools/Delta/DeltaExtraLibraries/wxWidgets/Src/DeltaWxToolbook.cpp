#include "DeltaWxToolbook.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxColour.h"
#include "DeltaWxNotebookEvent.h"
#include "DeltaWxImageList.h"
#include "DeltaWxSizer.h"
#include "DeltaWxToolBar.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(toolbook, name)
#define WX_FUNC(name) WX_FUNC1(toolbook, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(addpage)
WX_FUNC_DEF(advanceselection)
WX_FUNC_DEF(assignimagelist)
WX_FUNC_DEF(changeselection)
WX_FUNC_DEF(create)
WX_FUNC_DEF(deleteallpages)
WX_FUNC_DEF(deletepage)
WX_FUNC_DEF(getcontrolsizer)
WX_FUNC_DEF(getcurrentpage)
WX_FUNC_DEF(getimagelist)
WX_FUNC_DEF(getpage)
WX_FUNC_DEF(getpagecount)
WX_FUNC_DEF(getpageimage)
WX_FUNC_DEF(getpagetext)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(gettoolbar)
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
	WX_FUNC(changeselection),
	WX_FUNC(create),
	WX_FUNC(deleteallpages),
	WX_FUNC(deletepage),
	WX_FUNC(getcontrolsizer),
	WX_FUNC(getcurrentpage),
	WX_FUNC(getimagelist),
	WX_FUNC(getpage),
	WX_FUNC(getpagecount),
	WX_FUNC(getpageimage),
	WX_FUNC(getpagetext),
	WX_FUNC(getselection),
	WX_FUNC(gettoolbar),
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

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Toolbook, "toolbook", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Toolbook, val, toolbook) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetPages (void* val, DeltaValue* at) 
{
	wxToolbook *book = DLIB_WXTYPECAST_BASE(Toolbook, val, toolbook);
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
	wxToolbook *book = DLIB_WXTYPECAST_BASE(Toolbook, val, toolbook);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ImageList, book->GetImageList())
	return true;
}

static bool GetFitToCurrentPage (void* val, DeltaValue* at) 
{
	wxToolbook *book = DLIB_WXTYPECAST_BASE(Toolbook, val, toolbook);
	at->FromBool(book->GetFitToCurrentPage());
	return true;
}

static bool GetControlSizer (void* val, DeltaValue* at) 
{
	wxToolbook *book = DLIB_WXTYPECAST_BASE(Toolbook, val, toolbook);
	wxSizer *sizer = book->GetControlSizer();
	DeltaWxSizer *retval = sizer ? DNEWCLASS(DeltaWxSizer, (sizer)) : (DeltaWxSizer*) 0;
	return true;
}

static bool GetControlMargin (void* val, DeltaValue* at) 
{
	wxToolbook *book = DLIB_WXTYPECAST_BASE(Toolbook, val, toolbook);
	at->FromNumber(book->GetControlMargin());
	return true;
}

static bool GetInternalBorder (void* val, DeltaValue* at) 
{
	wxToolbook *book = DLIB_WXTYPECAST_BASE(Toolbook, val, toolbook);
	at->FromNumber(book->GetInternalBorder());
	return true;
}

static bool GetSelection (void* val, DeltaValue* at) 
{
	wxToolbook *book = DLIB_WXTYPECAST_BASE(Toolbook, val, toolbook);
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Toolbook,toolbook)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(toolbook_construct, 0, 6, Nil)
	wxToolbook *toolbk = (wxToolbook*) 0;
	if (n == 0)
		toolbk = new wxToolbook();
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
		toolbk = new wxToolbook(parent, id, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		return;
	}
	WX_SET_WINDOW_OBJECT(Toolbook, toolbk)
}

WX_FUNC_ARGRANGE_START(toolbook_addpage, 3, 5, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool bSelect = false;
	int imageId = -1;
	if (n >= 4) { WX_GETBOOL_DEFINED(bSelect) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(toolbk->AddPage(page, text, bSelect, imageId))
}

WX_FUNC_ARGRANGE_START(toolbook_advanceselection, 1, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	bool forward = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(forward) }
	toolbk->AdvanceSelection(forward);
}

WX_FUNC_START(toolbook_assignimagelist, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	toolbk->AssignImageList(imageList);
}

WX_FUNC_START(toolbook_changeselection, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(size)
	WX_SETNUMBER(toolbk->ChangeSelection(size))
}

WX_FUNC_ARGRANGE_START(toolbook_create, 3, 7, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
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
	WX_SETBOOL(toolbk->Create(parent, winid, pos, size, style, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(toolbk);
}

WX_FUNC_START(toolbook_deleteallpages, 1, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_SETBOOL(toolbk->DeleteAllPages())
}

WX_FUNC_START(toolbook_deletepage, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(page)
	WX_SETBOOL(toolbk->DeletePage(page))
}

WX_FUNC_START(toolbook_getcontrolsizer, 1, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_SETOBJECT(Sizer, toolbk->GetControlSizer())
}

WX_FUNC_START(toolbook_getcurrentpage, 1, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	wxWindow* retval	= toolbk->GetCurrentPage();
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_START(toolbook_getimagelist, 1, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	wxImageList* retval	= toolbk->GetImageList();
	WX_SETOBJECT(ImageList, retval)
}

WX_FUNC_START(toolbook_getpage, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(page)
	wxWindow* retval	= toolbk->GetPage(page);
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_START(toolbook_getpagecount, 1, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_SETNUMBER(toolbk->GetPageCount())
}

WX_FUNC_START(toolbook_getpageimage, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(nPage)
	WX_SETNUMBER(toolbk->GetPageImage(nPage))
}

WX_FUNC_START(toolbook_getpagetext, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(page)
	WX_SETSTRING(toolbk->GetPageText(page))
}

WX_FUNC_START(toolbook_getselection, 1, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_SETNUMBER(toolbk->GetSelection())
}

WX_FUNC_START(toolbook_gettoolbar, 1, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	context.UpdateContext(_argNo, _sig1, _sig2);
	if (SetValueFromNativeInstance
		<DeltaWxToolBarClassId, DeltaWxToolBar, wxToolBarBase>
		(toolbk->GetToolBar(), DLIB_RETVAL_PTR, &LetWrapperLive,
		&ToolBarUtils::GetGetter, &ToolBarUtils::GetMethods, &context)) {
		context.UpdateLocals(NULL, NULL, &_argNo, &_sig1, &_sig2);
	} else
		return;
}

WX_FUNC_ARGRANGE_START(toolbook_hittest, 2, 3, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	DLIB_WXGETPOINT_BASE(pt)
	long style;
	WX_SETNUMBER(toolbk->HitTest(*pt, &style))
	if (n >= 3) {
		WX_GETTABLE(style_table)
		WX_SETTABLE_RETVAL(style_table, DeltaValue((DeltaNumberValueType)style))
	}
}

WX_FUNC_ARGRANGE_START(toolbook_insertpage, 4, 6, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(index)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool select = false;
	int imageId = -1;
	if (n >= 5) { WX_GETBOOL_DEFINED(select) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(toolbk->InsertPage(index, page, text, select, imageId))
}

WX_FUNC_START(toolbook_removepage, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(page)
	WX_SETBOOL(toolbk->RemovePage(page))
}

WX_FUNC_START(toolbook_setimagelist, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	toolbk->SetImageList(imageList);
}

WX_FUNC_START(toolbook_setpagesize, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	DLIB_WXGETSIZE_BASE(size)
	toolbk->SetPageSize(*size);
}

WX_FUNC_START(toolbook_setpageimage, 3, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(page)
	WX_GETNUMBER(image)
	WX_SETBOOL(toolbk->SetPageImage(page, image))
}

WX_FUNC_START(toolbook_setpagetext, 3, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(page)
	WX_GETSTRING(text)
	WX_SETBOOL(toolbk->SetPageText(page, text))
}

WX_FUNC_START(toolbook_setselection, 2, Nil)
	DLIB_WXGET_BASE(toolbook, Toolbook, toolbk)
	WX_GETNUMBER(page)
	WX_SETNUMBER(toolbk->SetSelection(page))
}
