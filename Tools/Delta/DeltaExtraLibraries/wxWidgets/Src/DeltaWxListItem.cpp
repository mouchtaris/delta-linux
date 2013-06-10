#include "DeltaWxListItem.h"
#include "DeltaWxColour.h"
#include "DeltaWxFont.h"
#include "DeltaWxObject.h"
#include "DeltaWxListItemAttr.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(listitem, name)
#define WX_FUNC(name) WX_FUNC1(listitem, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(clear)
WX_FUNC_DEF(getalign)
WX_FUNC_DEF(getbackgroundcolour)
WX_FUNC_DEF(getcolumn)
WX_FUNC_DEF(getfont)
WX_FUNC_DEF(getid)
WX_FUNC_DEF(getimage)
WX_FUNC_DEF(getmask)
WX_FUNC_DEF(getstate)
WX_FUNC_DEF(gettext)
WX_FUNC_DEF(gettextcolour)
WX_FUNC_DEF(getwidth)
WX_FUNC_DEF(setalign)
WX_FUNC_DEF(setbackgroundcolour)
WX_FUNC_DEF(setcolumn)
WX_FUNC_DEF(setfont)
WX_FUNC_DEF(setid)
WX_FUNC_DEF(setimage)
WX_FUNC_DEF(setmask)
WX_FUNC_DEF(setstate)
WX_FUNC_DEF(setstatemask)
WX_FUNC_DEF(settext)
WX_FUNC_DEF(settextcolour)
WX_FUNC_DEF(setwidth)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(clear),
	WX_FUNC(getalign),
	WX_FUNC(getbackgroundcolour),
	WX_FUNC(getcolumn),
	WX_FUNC(getfont),
	WX_FUNC(getid),
	WX_FUNC(getimage),
	WX_FUNC(getmask),
	WX_FUNC(getstate),
	WX_FUNC(gettext),
	WX_FUNC(gettextcolour),
	WX_FUNC(getwidth),
	WX_FUNC(setalign),
	WX_FUNC(setbackgroundcolour),
	WX_FUNC(setcolumn),
	WX_FUNC(setfont),
	WX_FUNC(setid),
	WX_FUNC(setimage),
	WX_FUNC(setmask),
	WX_FUNC(setstate),
	WX_FUNC(setstatemask),
	WX_FUNC(settext),
	WX_FUNC(settextcolour),
	WX_FUNC(setwidth)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "clear", "setwidth")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(ListItem, "listitem")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ListItem, val, listitem) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetAttributes (void* val, DeltaValue* at) 
{
	wxListItem *item = DLIB_WXTYPECAST_BASE(ListItem, val, listitem);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, ListItemAttr, item->GetAttributes())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "attribute",			&GetAttributes,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ListItem,listitem)

////////////////////////////////////////////////////////////////

WX_FUNC_START(listitem_construct, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ListItem, new wxListItem())
}

WX_FUNC_START(listitem_clear, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	item->Clear();
}

WX_FUNC_START(listitem_getalign, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETNUMBER(item->GetAlign())
}

WX_FUNC_START(listitem_getbackgroundcolour, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(item->GetBackgroundColour()))
}

WX_FUNC_START(listitem_getcolumn, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETNUMBER(item->GetColumn())
}

WX_FUNC_START(listitem_getfont, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(item->GetFont()))
}

WX_FUNC_START(listitem_getid, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETNUMBER(item->GetId())
}

WX_FUNC_START(listitem_getimage, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETNUMBER(item->GetImage())
}

WX_FUNC_START(listitem_getmask, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETNUMBER(item->GetMask())
}

WX_FUNC_START(listitem_getstate, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETNUMBER(item->GetState())
}

WX_FUNC_START(listitem_gettext, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETSTRING(item->GetText())
}

WX_FUNC_START(listitem_gettextcolour, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(item->GetTextColour()))
}

WX_FUNC_START(listitem_getwidth, 1, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_SETNUMBER(item->GetWidth())
}

WX_FUNC_START(listitem_setalign, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_GETNUMBER(align)
	item->SetAlign((wxListColumnFormat)(int)align);
}

WX_FUNC_START(listitem_setbackgroundcolour, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	DLIB_WXGET_BASE(colour, Colour, colBack)
	item->SetBackgroundColour(*colBack);
}

WX_FUNC_START(listitem_setcolumn, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_GETNUMBER(col)
	item->SetColumn(col);
}

WX_FUNC_START(listitem_setfont, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	DLIB_WXGET_BASE(font, Font, font)
	item->SetFont(*font);
}

WX_FUNC_START(listitem_setid, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_GETNUMBER(id)
	item->SetId(id);
}

WX_FUNC_START(listitem_setimage, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_GETNUMBER(image)
	item->SetImage(image);
}

WX_FUNC_START(listitem_setmask, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_GETNUMBER(mask)
	item->SetMask(mask);
}

WX_FUNC_START(listitem_setstate, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_GETNUMBER(state)
	item->SetState(state);
}

WX_FUNC_START(listitem_setstatemask, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_GETNUMBER(statemask)
	item->SetStateMask(statemask);
}

WX_FUNC_START(listitem_settext, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_GETSTRING(text)
	item->SetText(text);
}

WX_FUNC_START(listitem_settextcolour, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	DLIB_WXGET_BASE(colour, Colour, colText)
	item->SetTextColour(*colText);
}

WX_FUNC_START(listitem_setwidth, 2, Nil)
	DLIB_WXGET_BASE(listitem, ListItem, item)
	WX_GETNUMBER(width)
	item->SetWidth(width);
}
