#include "DeltaWxTreeCtrl.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxTreeItemId.h"
#include "DeltaWxTreeItemData.h"
#include "DeltaWxImageList.h"
#include "DeltaWxRect.h"
#include "DeltaWxTextCtrl.h"
#include "DeltaWxColour.h"
#include "DeltaWxFont.h"
#include "DeltaWxValidator.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(treectrl, name)
#define WX_FUNC(name) WX_FUNC1(treectrl, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(addroot)
WX_FUNC_DEF(appenditem)
WX_FUNC_DEF(assignimagelist)
WX_FUNC_DEF(assignstateimagelist)
WX_FUNC_DEF(collapse)
WX_FUNC_DEF(collapseall)
WX_FUNC_DEF(collapseallchildren)
WX_FUNC_DEF(collapseandreset)
WX_FUNC_DEF(create)
WX_FUNC_DEF(delete)
WX_FUNC_DEF(deleteallitems)
WX_FUNC_DEF(deletechildren)
WX_FUNC_DEF(editlabel)
WX_FUNC_DEF(endeditlabel)
WX_FUNC_DEF(ensurevisible)
WX_FUNC_DEF(expand)
WX_FUNC_DEF(expandall)
WX_FUNC_DEF(expandallchildren)
WX_FUNC_DEF(getboundingrect)
WX_FUNC_DEF(getchildrencount)
WX_FUNC_DEF(getcount)
WX_FUNC_DEF(geteditcontrol)
WX_FUNC_DEF(getfirstchild)
WX_FUNC_DEF(getfirstvisibleitem)
WX_FUNC_DEF(getimagelist)
WX_FUNC_DEF(getindent)
WX_FUNC_DEF(getitembackgroundcolour)
WX_FUNC_DEF(getitemdata)
WX_FUNC_DEF(getitemfont)
WX_FUNC_DEF(getitemparent)
WX_FUNC_DEF(getitemimage)
WX_FUNC_DEF(getitemtext)
WX_FUNC_DEF(getitemtextcolour)
WX_FUNC_DEF(getlastchild)
WX_FUNC_DEF(getnextchild)
WX_FUNC_DEF(getnextsibling)
WX_FUNC_DEF(getnextvisible)
WX_FUNC_DEF(getitemparent)
WX_FUNC_DEF(getprevsibling)
WX_FUNC_DEF(getprevvisible)
WX_FUNC_DEF(getquickbestsize)
WX_FUNC_DEF(getrootitem)
WX_FUNC_DEF(getselection)
WX_FUNC_DEF(getselections)
WX_FUNC_DEF(getstateimagelist)
WX_FUNC_DEF(hittest)
WX_FUNC_DEF(insertitem)
WX_FUNC_DEF(isbold)
WX_FUNC_DEF(isempty)
WX_FUNC_DEF(isexpanded)
WX_FUNC_DEF(isselected)
WX_FUNC_DEF(isvisible)
WX_FUNC_DEF(itemhaschildren)
WX_FUNC_DEF(prependitem)
WX_FUNC_DEF(scrollto)
WX_FUNC_DEF(selectitem)
WX_FUNC_DEF(setindent)
WX_FUNC_DEF(setimagelist)
WX_FUNC_DEF(setitembackgroundcolour)
WX_FUNC_DEF(setitembold)
WX_FUNC_DEF(setitemdata)
WX_FUNC_DEF(setitemdrophighlight)
WX_FUNC_DEF(setitemfont)
WX_FUNC_DEF(setitemhaschildren)
WX_FUNC_DEF(setitemimage)
WX_FUNC_DEF(setitemtext)
WX_FUNC_DEF(setitemtextcolour)
WX_FUNC_DEF(setquickbestsize)
WX_FUNC_DEF(setstateimagelist)
WX_FUNC_DEF(setwindowstyle)
WX_FUNC_DEF(sortchildren)
WX_FUNC_DEF(toggle)
WX_FUNC_DEF(toggleitemselection)
WX_FUNC_DEF(unselect)
WX_FUNC_DEF(unselectall)
WX_FUNC_DEF(unselectitem)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(addroot),
	WX_FUNC(appenditem),
	WX_FUNC(assignimagelist),
	WX_FUNC(assignstateimagelist),
	WX_FUNC(collapse),
	WX_FUNC(collapseall),
	WX_FUNC(collapseallchildren),
	WX_FUNC(collapseandreset),
	WX_FUNC(create),
	WX_FUNC(delete),
	WX_FUNC(deleteallitems),
	WX_FUNC(deletechildren),
	WX_FUNC(editlabel),
	WX_FUNC(endeditlabel),
	WX_FUNC(ensurevisible),
	WX_FUNC(expand),
	WX_FUNC(expandall),
	WX_FUNC(expandallchildren),
	WX_FUNC(getboundingrect),
	WX_FUNC(getchildrencount),
	WX_FUNC(getcount),
	WX_FUNC(geteditcontrol),
	WX_FUNC(getfirstchild),
	WX_FUNC(getfirstvisibleitem),
	WX_FUNC(getimagelist),
	WX_FUNC(getindent),
	WX_FUNC(getitembackgroundcolour),
	WX_FUNC(getitemdata),
	WX_FUNC(getitemfont),
	WX_FUNC(getitemparent),
	WX_FUNC(getitemimage),
	WX_FUNC(getitemtext),
	WX_FUNC(getitemtextcolour),
	WX_FUNC(getlastchild),
	WX_FUNC(getnextchild),
	WX_FUNC(getnextsibling),
	WX_FUNC(getquickbestsize),
	WX_FUNC(getrootitem),
	WX_FUNC(getselection),
	WX_FUNC(getselections),
	WX_FUNC(getstateimagelist),
	WX_FUNC(hittest),
	WX_FUNC(insertitem),
	WX_FUNC(isbold),
	WX_FUNC(isempty),
	WX_FUNC(isexpanded),
	WX_FUNC(isselected),
	WX_FUNC(isvisible),
	WX_FUNC(scrollto),
	WX_FUNC(selectitem),
	WX_FUNC(setindent),
	WX_FUNC(setimagelist),
	WX_FUNC(setitembackgroundcolour),
	WX_FUNC(setitembold),
	WX_FUNC(setitemdata),
	WX_FUNC(setitemdrophighlight),
	WX_FUNC(setitemfont),
	WX_FUNC(setitemhaschildren),
	WX_FUNC(setitemimage),
	WX_FUNC(setitemtext),
	WX_FUNC(setitemtextcolour),
	WX_FUNC(setquickbestsize),
	WX_FUNC(setstateimagelist),
	WX_FUNC(setwindowstyle),
	WX_FUNC(sortchildren),
	WX_FUNC(toggle),
	WX_FUNC(toggleitemselection),
	WX_FUNC(unselect),
	WX_FUNC(unselectall),
	WX_FUNC(unselectitem)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "addroot", "unselectitem")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(TreeCtrl, "treectrl", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TreeCtrl, val, treectrl) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static bool GetNormalImageList (void* val, DeltaValue* at) 
{
	wxTreeCtrl *ctrl = DLIB_WXTYPECAST_BASE(TreeCtrl, val, treectrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ImageList, ctrl->GetImageList())
	return true;
}

static bool GetStateImageList (void* val, DeltaValue* at) 
{
	wxTreeCtrl *ctrl = DLIB_WXTYPECAST_BASE(TreeCtrl, val, treectrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ImageList, ctrl->GetStateImageList())
	return true;
}

static bool GetSpacing (void* val, DeltaValue* at) 
{
	wxTreeCtrl *ctrl = DLIB_WXTYPECAST_BASE(TreeCtrl, val, treectrl);
	WX_SETNUMBER_EX(*at, ctrl->GetSpacing())
	return true;
}

static bool GetQuickBestSize (void* val, DeltaValue* at) 
{
	wxTreeCtrl *ctrl = DLIB_WXTYPECAST_BASE(TreeCtrl, val, treectrl);
	WX_SETBOOL_EX(*at, ctrl->GetQuickBestSize())
	return true;
}

static bool GetTextCtrl (void* val, DeltaValue* at) 
{
	wxTreeCtrl *ctrl = DLIB_WXTYPECAST_BASE(TreeCtrl, val, treectrl);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, TextCtrl, ctrl->GetEditControl())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "normalImageList",	&GetNormalImageList,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "stateImageList",		&GetStateImageList,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "spacing",			&GetSpacing,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "quickBestSize",		&GetQuickBestSize,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textCtrl",			&GetTextCtrl,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TreeCtrl, treectrl)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(treectrl_construct, 0, 7, Nil)
	wxTreeCtrl *ctrl = (wxTreeCtrl*) 0;
	if (n == 0)
		ctrl = new wxTreeCtrl();
	else {
		DLIB_WXGET_BASE(window, Window, parent)
		int id = wxID_ANY;
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxTreeCtrlNameStr;
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 4) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 5) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 6) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 7) { WX_GETSTRING_DEFINED(name) }
		ctrl = new wxTreeCtrl(parent, id, pos, size, style, *validator, name);
	}
	WX_SET_WINDOW_OBJECT(TreeCtrl, ctrl)
}

WX_FUNC_ARGRANGE_START(treectrl_addroot, 2, 5, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_GETSTRING(text)
	int image = -1, selectedImage = -1;
	wxTreeItemData *data = NULL;
	if (n >= 3) { WX_GETNUMBER(image) }
	if (n >= 4) { WX_GETNUMBER(selectedImage) }
	if (n >= 5) { DLIB_WXGET_BASE(treeitemdata, TreeItemData, _data) data = _data; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->AddRoot(text, image, selectedImage, data)))
}

WX_FUNC_ARGRANGE_START(treectrl_appenditem, 3, 6, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, parent)
	WX_GETSTRING(text)
	int image = -1, selectedImage = -1;
	wxTreeItemData *data = NULL;
	if (n >= 4) { WX_GETNUMBER(image) }
	if (n >= 5) { WX_GETNUMBER(selectedImage) }
	if (n >= 6) { DLIB_WXGET_BASE(treeitemdata, TreeItemData, _data) data = _data; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->AppendItem(*parent, text, image, selectedImage, data)))
}

WX_FUNC_START(treectrl_assignimagelist, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	ctrl->AssignImageList(imageList);
}

WX_FUNC_START(treectrl_assignstateimagelist, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	ctrl->AssignStateImageList(imageList);
}

WX_FUNC_START(treectrl_collapse, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->Collapse(*item);
}

WX_FUNC_START(treectrl_collapseall, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	ctrl->CollapseAll();
}

WX_FUNC_START(treectrl_collapseallchildren, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->CollapseAllChildren(*item);
}

WX_FUNC_START(treectrl_collapseandreset, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->CollapseAndReset(*item);
}

WX_FUNC_ARGRANGE_START(treectrl_create, 2, 8, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(window, Window, parent)
	int id = wxID_ANY;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxTreeCtrlNameStr;
	if (n >= 3) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 8) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(ctrl->Create(parent, id, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(ctrl);
}

WX_FUNC_START(treectrl_delete, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->Delete(*item);
}

WX_FUNC_START(treectrl_deleteallitems, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	ctrl->DeleteAllItems();
}

WX_FUNC_START(treectrl_deletechildren, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->DeleteChildren(*item);
}

WX_FUNC_START(treectrl_editlabel, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->EditLabel(*item);
}

WX_FUNC_ARGRANGE_START(treectrl_endeditlabel, 2, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	bool discardChanges = false;
	if (n >= 3) { WX_GETBOOL_DEFINED(discardChanges) }
	ctrl->EndEditLabel(*item, discardChanges);
}

WX_FUNC_START(treectrl_ensurevisible, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->EnsureVisible(*item);
}

WX_FUNC_START(treectrl_expand, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->Expand(*item);
}

WX_FUNC_START(treectrl_expandall, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	ctrl->ExpandAll();
}

WX_FUNC_START(treectrl_expandallchildren, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->ExpandAllChildren(*item);
}

WX_FUNC_ARGRANGE_START(treectrl_getboundingrect, 3, 4, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_GETTABLE(rect_table)
	wxRect rect;
	bool textOnly = false;
	if (n >= 4) { WX_GETBOOL_DEFINED(textOnly) }
	WX_SETBOOL(ctrl->GetBoundingRect(*item, rect, textOnly))
	DeltaWxRect *retval = DNEWCLASS(DeltaWxRect, (new wxRect(rect)));
	DeltaValue value;
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE_EX(value, Rect, new wxRect(rect))
	WX_SETTABLE_RETVAL(rect_table, value)
}

WX_FUNC_ARGRANGE_START(treectrl_getchildrencount, 2, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	bool recursively = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(recursively) }
	WX_SETNUMBER(ctrl->GetChildrenCount(*item, recursively))
}

WX_FUNC_START(treectrl_getcount, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETNUMBER(ctrl->GetCount())
}

WX_FUNC_START(treectrl_geteditcontrol, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETOBJECT(TextCtrl, ctrl->GetEditControl())
}

WX_FUNC_START(treectrl_getfirstchild, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_GETTABLE(cookie_table)
	wxTreeItemIdValue cookie;
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetFirstChild(*item, cookie)))
	wxTreeItemId fromCookie(cookie);
	DeltaValue value;
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE_EX(value, TreeItemId, new wxTreeItemId(fromCookie))
	WX_SETTABLE_RETVAL(cookie_table, value)
}

WX_FUNC_START(treectrl_getfirstvisibleitem, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetFirstVisibleItem()))
}

WX_FUNC_START(treectrl_getimagelist, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETOBJECT(ImageList, ctrl->GetImageList())
}

WX_FUNC_START(treectrl_getindent, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETNUMBER(ctrl->GetIndent())
}

WX_FUNC_START(treectrl_getitembackgroundcolour, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(ctrl->GetItemBackgroundColour(*item)))
}

WX_FUNC_START(treectrl_getitemdata, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT(TreeItemData, ctrl->GetItemData(*item))
}

WX_FUNC_START(treectrl_getitemfont, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(ctrl->GetItemFont(*item)))
}

WX_FUNC_ARGRANGE_START(treectrl_getitemimage, 2, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	int which = wxTreeItemIcon_Normal;
	if (n >= 3) { WX_GETDEFINE_DEFINED(which) }
	WX_SETNUMBER(ctrl->GetItemImage(*item, (wxTreeItemIcon)which))
}

WX_FUNC_START(treectrl_getitemtext, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETSTRING(ctrl->GetItemText(*item))
}

WX_FUNC_START(treectrl_getitemtextcolour, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(ctrl->GetItemTextColour(*item)))
}

WX_FUNC_START(treectrl_getlastchild, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetLastChild(*item)))
}

WX_FUNC_START(treectrl_getnextchild, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_GETTABLE(cookie_table)
	wxTreeItemIdValue cookie;
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetNextChild(*item, cookie)))
	wxTreeItemId fromCookie(cookie);
	DeltaValue value;
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE_EX(value, TreeItemId, new wxTreeItemId(fromCookie))
	WX_SETTABLE_RETVAL(cookie_table, value)
}

WX_FUNC_START(treectrl_getnextsibling, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetNextSibling(*item)))
}

WX_FUNC_START(treectrl_getnextvisible, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetNextVisible(*item)))
}

WX_FUNC_START(treectrl_getitemparent, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetItemParent(*item)))
}

WX_FUNC_START(treectrl_getprevsibling, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetPrevSibling(*item)))
}

WX_FUNC_START(treectrl_getprevvisible, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetPrevVisible(*item)))
}

WX_FUNC_START(treectrl_getquickbestsize, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETBOOL(ctrl->GetQuickBestSize())
}

WX_FUNC_START(treectrl_getrootitem, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetRootItem()))
}

WX_FUNC_START(treectrl_getselection, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->GetSelection()))
}

WX_FUNC_START(treectrl_getselections, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	wxArrayTreeItemIds selection;
	ctrl->GetSelections(selection);
	DeltaObject *retval = DNEW(DeltaObject);
	for (int i = 0, num = (int)selection.capacity(); i < num; ++i) {
		DeltaValue value;
		WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE_EX(value, TreeItemId, new wxTreeItemId(selection[i]))
		retval->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	DLIB_RETVAL_REF.FromTable(retval);
}

WX_FUNC_START(treectrl_getstateimagelist, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETOBJECT(ImageList, ctrl->GetStateImageList())
}

WX_FUNC_ARGRANGE_START(treectrl_hittest, 2, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGETPOINT_BASE(point)
	int flags;
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->HitTest(*point, flags)))
	if (n >= 3) {
		WX_GETTABLE(flags_table)
		WX_SETTABLE_RETVAL(flags_table, DeltaValue(DeltaNumberValueType(flags)))
	}
}

WX_FUNC_ARGRANGE_START(treectrl_insertitem, 4, 7, Nil)
	wxTreeItemId *retval = (wxTreeItemId*) 0;
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, parent)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		DLIB_WXGET_BASE(treeitemid, TreeItemId, previous)
		WX_GETSTRING(text)
		int image = -1, selImage = -1;
		wxTreeItemData *data = NULL;
		if (n >= 5) { WX_GETDEFINE_DEFINED(image) }
		if (n >= 6) { WX_GETDEFINE_DEFINED(selImage) }
		if (n >= 7) { DLIB_WXGET_BASE(treeitemdata, TreeItemData, _data) data = _data; }
		retval = new wxTreeItemId(ctrl->InsertItem(
			parent, previous, text, image, selImage, data));
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(before)
		WX_GETSTRING(text)
		int image = -1, selImage = -1;
		wxTreeItemData *data = NULL;
		if (n >= 5) { WX_GETDEFINE_DEFINED(image) }
		if (n >= 6) { WX_GETDEFINE_DEFINED(selImage) }
		if (n >= 7) { DLIB_WXGET_BASE(treeitemdata, TreeItemData, _data) data = _data; }
		retval = new wxTreeItemId(ctrl->InsertItem(
			parent, before, text, image, selImage, data));
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, retval)
}

WX_FUNC_START(treectrl_isbold, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETBOOL(ctrl->IsBold(*item))
}

WX_FUNC_START(treectrl_isempty, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_SETBOOL(ctrl->IsEmpty())
}

WX_FUNC_START(treectrl_isexpanded, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETBOOL(ctrl->IsExpanded(*item))
}

WX_FUNC_START(treectrl_isselected, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETBOOL(ctrl->IsSelected(*item))
}

WX_FUNC_START(treectrl_isvisible, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETBOOL(ctrl->IsVisible(*item))
}

WX_FUNC_START(treectrl_itemhaschildren, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_SETBOOL(ctrl->ItemHasChildren(*item))
}

WX_FUNC_ARGRANGE_START(treectrl_prependitem, 3, 6, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, parent)
	WX_GETSTRING(text)
	int image = -1, selImage = -1;
	wxTreeItemData *data = NULL;
	if (n >= 4) { WX_GETDEFINE_DEFINED(image) }
	if (n >= 5) { WX_GETDEFINE_DEFINED(selImage) }
	if (n >= 6) { DLIB_WXGET_BASE(treeitemdata, TreeItemData, _data) data = _data; }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TreeItemId, new wxTreeItemId(ctrl->PrependItem(
			parent, text, image, selImage, data)))
}

WX_FUNC_START(treectrl_scrollto, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->ScrollTo(*item);
}

WX_FUNC_ARGRANGE_START(treectrl_selectitem, 2, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	bool select = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(select) }
	ctrl->SelectItem(*item, select);
}

WX_FUNC_START(treectrl_setindent, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_GETNUMBER(indent)
	ctrl->SetIndent(indent);
}

WX_FUNC_START(treectrl_setimagelist, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	ctrl->SetImageList(imageList);
}

WX_FUNC_START(treectrl_setitembackgroundcolour, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	DLIB_WXGET_BASE(colour, Colour, col)
	ctrl->SetItemBackgroundColour(*item, *col);
}

WX_FUNC_ARGRANGE_START(treectrl_setitembold, 2, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	bool bold = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(bold) }
	ctrl->SetItemBold(*item, bold);
}

WX_FUNC_START(treectrl_setitemdata, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	DLIB_WXGET_BASE(treeitemdata, TreeItemData, data)
	ctrl->SetItemData(*item, data);
}

WX_FUNC_ARGRANGE_START(treectrl_setitemdrophighlight, 2, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	bool highlight = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(highlight) }
	ctrl->SetItemDropHighlight(*item, highlight);
}

WX_FUNC_START(treectrl_setitemfont, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	DLIB_WXGET_BASE(font, Font, font)
	ctrl->SetItemFont(*item, *font);
}

WX_FUNC_ARGRANGE_START(treectrl_setitemhaschildren, 2, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	bool hasChildren = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(hasChildren) }
	ctrl->SetItemHasChildren(*item, hasChildren);
}

WX_FUNC_ARGRANGE_START(treectrl_setitemimage, 3, 4, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_GETDEFINE(image)
	int which = wxTreeItemIcon_Normal;
	if (n >= 4) { WX_GETDEFINE_DEFINED(which) }
	ctrl->SetItemImage(*item, image, (wxTreeItemIcon)which);
}

WX_FUNC_START(treectrl_setitemtext, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	WX_GETSTRING(text)
	ctrl->SetItemText(*item, text);
}

WX_FUNC_START(treectrl_setitemtextcolour, 3, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	DLIB_WXGET_BASE(colour, Colour, col)
	ctrl->SetItemTextColour(*item, *col);
}

WX_FUNC_START(treectrl_setquickbestsize, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_GETBOOL(quickBestSize)
	ctrl->SetQuickBestSize(quickBestSize);
}

WX_FUNC_START(treectrl_setstateimagelist, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(imagelist, ImageList, imageList)
	ctrl->SetStateImageList(imageList);
}

WX_FUNC_START(treectrl_setwindowstyle, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	WX_GETDEFINE(styles)
	ctrl->SetWindowStyle(styles);
}

WX_FUNC_START(treectrl_sortchildren, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->SortChildren(*item);
}

WX_FUNC_START(treectrl_toggle, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->Toggle(*item);
}

WX_FUNC_START(treectrl_toggleitemselection, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->ToggleItemSelection(*item);
}

WX_FUNC_START(treectrl_unselect, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	ctrl->Unselect();
}

WX_FUNC_START(treectrl_unselectall, 1, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	ctrl->UnselectAll();
}

WX_FUNC_START(treectrl_unselectitem, 2, Nil)
	DLIB_WXGET_BASE(treectrl, TreeCtrl, ctrl)
	DLIB_WXGET_BASE(treeitemid, TreeItemId, item)
	ctrl->UnselectItem(*item);
}
