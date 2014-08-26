#include "DeltaWxTreebook.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxColour.h"
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
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME SetWrapperChild
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(treebook, name)
#define WX_FUNC(name) WX_FUNC1(treebook, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(addpage)
WX_FUNC_DEF(addsubpage)
WX_FUNC_DEF(assignimagelist)
WX_FUNC_DEF(changeselection)
WX_FUNC_DEF(collapsenode)
WX_FUNC_DEF(create)
WX_FUNC_DEF(deleteallpages)
WX_FUNC_DEF(deletepage)
WX_FUNC_DEF(expandnode)
WX_FUNC_DEF(getpageimage)
WX_FUNC_DEF(getpageparent)
WX_FUNC_DEF(getpagetext)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(insertpage)
WX_FUNC_DEF(insertsubpage)
WX_FUNC_DEF(isnodeexpanded)
WX_FUNC_DEF(setimagelist)
WX_FUNC_DEF(setpageimage)
WX_FUNC_DEF(setpagetext)
WX_FUNC_DEF(setselection)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(addpage),
	WX_FUNC(addsubpage),
	WX_FUNC(assignimagelist),
	WX_FUNC(changeselection),
	WX_FUNC(collapsenode),
	WX_FUNC(create),
	WX_FUNC(deleteallpages),
	WX_FUNC(deletepage),
	WX_FUNC(expandnode),
	WX_FUNC(getpageimage),
	WX_FUNC(getpageparent),
	WX_FUNC(getpagetext),
	WX_FUNC(getselection),
	WX_FUNC(insertpage),
	WX_FUNC(insertsubpage),
	WX_FUNC(isnodeexpanded),
	WX_FUNC(setimagelist),
	WX_FUNC(setpageimage),
	WX_FUNC(setpagetext),
	WX_FUNC(setselection)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "addpage", "setselection")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Treebook, "treebook", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Treebook, val, treebook) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetPages (void* val, DeltaValue* at) 
{
	wxTreebook *book = DLIB_WXTYPECAST_BASE(Treebook, val, treebook);
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
	wxTreebook *book = DLIB_WXTYPECAST_BASE(Treebook, val, treebook);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ImageList, book->GetImageList())
	return true;
}

static bool GetFitToCurrentPage (void* val, DeltaValue* at) 
{
	wxTreebook *book = DLIB_WXTYPECAST_BASE(Treebook, val, treebook);
	at->FromBool(book->GetFitToCurrentPage());
	return true;
}

static bool GetControlSizer (void* val, DeltaValue* at) 
{
	wxTreebook *book = DLIB_WXTYPECAST_BASE(Treebook, val, treebook);
	wxSizer *sizer = book->GetControlSizer();
	DeltaWxSizer *retval = sizer ? DNEWCLASS(DeltaWxSizer, (sizer)) : (DeltaWxSizer*) 0;
	return true;
}

static bool GetControlMargin (void* val, DeltaValue* at) 
{
	wxTreebook *book = DLIB_WXTYPECAST_BASE(Treebook, val, treebook);
	at->FromNumber(book->GetControlMargin());
	return true;
}

static bool GetInternalBorder (void* val, DeltaValue* at) 
{
	wxTreebook *book = DLIB_WXTYPECAST_BASE(Treebook, val, treebook);
	at->FromNumber(book->GetInternalBorder());
	return true;
}

static bool GetSelection (void* val, DeltaValue* at) 
{
	wxTreebook *book = DLIB_WXTYPECAST_BASE(Treebook, val, treebook);
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

WX_LIBRARY_FUNCS_IMPLEMENTATION(Treebook,treebook)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(treebook_construct, 0, 6, Nil)
	wxTreebook *treebk = (wxTreebook*) 0;
	if (n == 0)
		treebk = new wxTreebook();
	else if (n >= 2) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxBK_DEFAULT;
		wxString name = wxEmptyString;
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { WX_GETSTRING_DEFINED(name) }
		treebk = new wxTreebook(parent, id, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		return;
	}
	WX_SET_WINDOW_OBJECT(Treebook, treebk)
}

WX_FUNC_ARGRANGE_START(treebook_addpage, 3, 5, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool bSelect = false;
	int imageId = wxNOT_FOUND;
	if (n >= 4) { WX_GETBOOL_DEFINED(bSelect) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(treebk->AddPage(page, text, bSelect, imageId))
}

WX_FUNC_ARGRANGE_START(treebook_addsubpage, 3, 5, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool bSelect = false;
	int imageId = wxNOT_FOUND;
	if (n >= 4) { WX_GETBOOL_DEFINED(bSelect) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(treebk->AddSubPage(page, text, bSelect, imageId))
}

WX_FUNC_START(treebook_assignimagelist, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	treebk->AssignImageList(imageList);
}

WX_FUNC_START(treebook_changeselection, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(size)
	WX_SETNUMBER(treebk->ChangeSelection(size))
}

WX_FUNC_START(treebook_collapsenode, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(pageId)
	WX_SETBOOL(treebk->CollapseNode(pageId))
}

WX_FUNC_ARGRANGE_START(treebook_create, 3, 7, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxBK_DEFAULT;
	wxString name = wxEmptyString;
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(treebk->Create(parent, id, pos, size, style, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(treebk);
}

WX_FUNC_START(treebook_deleteallpages, 1, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_SETBOOL(treebk->DeleteAllPages())
}

WX_FUNC_START(treebook_deletepage, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(page)
	WX_SETBOOL(treebk->DeletePage(page))
}

WX_FUNC_ARGRANGE_START(treebook_expandnode, 2, 3, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(page)
	bool expand = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(expand) }
	WX_SETBOOL(treebk->ExpandNode(page, expand))
}

WX_FUNC_START(treebook_getpageimage, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(nPage)
	WX_SETNUMBER(treebk->GetPageImage(nPage))
}

WX_FUNC_START(treebook_getpageparent, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(nPage)
	WX_SETNUMBER(treebk->GetPageParent(nPage))
}

WX_FUNC_START(treebook_getpagetext, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(page)
	WX_SETSTRING(treebk->GetPageText(page))
}

WX_FUNC_START(treebook_getselection, 1, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_SETNUMBER(treebk->GetSelection())
}

WX_FUNC_ARGRANGE_START(treebook_insertpage, 4, 6, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(index)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool select = false;
	int imageId = wxNOT_FOUND;
	if (n >= 5) { WX_GETBOOL_DEFINED(select) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(treebk->InsertPage(index, page, text, select, imageId))
}

WX_FUNC_ARGRANGE_START(treebook_insertsubpage, 4, 6, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(index)
	DLIB_WXGET_BASE(window, Window, page)
	WX_GETSTRING(text)
	bool select = false;
	int imageId = wxNOT_FOUND;
	if (n >= 5) { WX_GETBOOL_DEFINED(select) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(imageId) }
	WX_SETBOOL(treebk->InsertSubPage(index, page, text, select, imageId))
}

WX_FUNC_START(treebook_isnodeexpanded, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(page)
	WX_SETBOOL(treebk->IsNodeExpanded(page))
}

WX_FUNC_START(treebook_setimagelist, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	treebk->SetImageList(imageList);
}

WX_FUNC_START(treebook_setpageimage, 3, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(page)
	WX_GETNUMBER(image)
	WX_SETBOOL(treebk->SetPageImage(page, image))
}

WX_FUNC_START(treebook_setpagetext, 3, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(page)
	WX_GETSTRING(text)
	WX_SETBOOL(treebk->SetPageText(page, text))
}

WX_FUNC_START(treebook_setselection, 2, Nil)
	DLIB_WXGET_BASE(treebook, Treebook, treebk)
	WX_GETNUMBER(page)
	WX_SETNUMBER(treebk->SetSelection(page))
}
