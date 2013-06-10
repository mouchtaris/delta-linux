#include "DeltaWxListItemAttr.h"
#include "DeltaWxColour.h"
#include "DeltaWxFont.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(listitemattr, name)
#define WX_FUNC(name) WX_FUNC1(listitemattr, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getbackgroundcolour)
WX_FUNC_DEF(getfont)
WX_FUNC_DEF(gettextcolour)
WX_FUNC_DEF(hasbackgroundcolour)
WX_FUNC_DEF(hasfont)
WX_FUNC_DEF(hastextcolour)
WX_FUNC_DEF(setbackgroundcolour)
WX_FUNC_DEF(setfont)
WX_FUNC_DEF(settextcolour)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getbackgroundcolour),
	WX_FUNC(getfont),
	WX_FUNC(gettextcolour),
	WX_FUNC(hasbackgroundcolour),
	WX_FUNC(hasfont),
	WX_FUNC(hastextcolour),
	WX_FUNC(setbackgroundcolour),
	WX_FUNC(setfont),
	WX_FUNC(settextcolour)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getbackgroundcolour", "settextcolour")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(ListItemAttr, "listitemattr")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ListItemAttr, val, listitemattr) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBackgroundColour (void* val, DeltaValue* at) 
{
	wxListItemAttr *attr = DLIB_WXTYPECAST_BASE(ListItemAttr, val, listitemattr);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(attr->GetBackgroundColour()))
	return true;
}

static bool GetTextColour (void* val, DeltaValue* at) 
{
	wxListItemAttr *attr = DLIB_WXTYPECAST_BASE(ListItemAttr, val, listitemattr);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(attr->GetTextColour()))
	return true;
}

static bool GetFont (void* val, DeltaValue* at) 
{
	wxListItemAttr *attr = DLIB_WXTYPECAST_BASE(ListItemAttr, val, listitemattr);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Font, new wxFont(attr->GetFont()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "backgroundColour",	&GetBackgroundColour,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textColour",			&GetTextColour,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "font",				&GetFont,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ListItemAttr,listitemattr)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(listitemattr_construct, 0, 3, Nil)
	wxListItemAttr *attr = (wxListItemAttr*) 0;
	if (n == 0) {
		attr = new wxListItemAttr();
	} else if (n == 3) {
		DLIB_WXGET_BASE(colour, Colour, colText)
		DLIB_WXGET_BASE(colour, Colour, colBack)
		DLIB_WXGET_BASE(font, Font, font)
		attr = new wxListItemAttr(*colText, *colBack, *font);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ListItemAttr, attr)
}

WX_FUNC_START(listitemattr_getbackgroundcolour, 1, Nil)
	DLIB_WXGET_BASE(listitemattr, ListItemAttr, attr)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(attr->GetBackgroundColour()))
}

WX_FUNC_START(listitemattr_getfont, 1, Nil)
	DLIB_WXGET_BASE(listitemattr, ListItemAttr, attr)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(attr->GetFont()))
}

WX_FUNC_START(listitemattr_gettextcolour, 1, Nil)
	DLIB_WXGET_BASE(listitemattr, ListItemAttr, attr)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(attr->GetTextColour()))
}

WX_FUNC_START(listitemattr_hasbackgroundcolour, 1, Nil)
	DLIB_WXGET_BASE(listitemattr, ListItemAttr, attr)
	WX_SETBOOL(attr->HasBackgroundColour())
}

WX_FUNC_START(listitemattr_hasfont, 1, Nil)
	DLIB_WXGET_BASE(listitemattr, ListItemAttr, attr)
	WX_SETBOOL(attr->HasFont())
}

WX_FUNC_START(listitemattr_hastextcolour, 1, Nil)
	DLIB_WXGET_BASE(listitemattr, ListItemAttr, attr)
	WX_SETBOOL(attr->HasTextColour())
}

WX_FUNC_START(listitemattr_setbackgroundcolour, 2, Nil)
	DLIB_WXGET_BASE(listitemattr, ListItemAttr, attr)
	DLIB_WXGET_BASE(colour, Colour, colour)
	attr->SetBackgroundColour(*colour);
}

WX_FUNC_START(listitemattr_setfont, 2, Nil)
	DLIB_WXGET_BASE(listitemattr, ListItemAttr, attr)
	DLIB_WXGET_BASE(font, Font, font)
	attr->SetFont(*font);
}

WX_FUNC_START(listitemattr_settextcolour, 2, Nil)
	DLIB_WXGET_BASE(listitemattr, ListItemAttr, attr)
	DLIB_WXGET_BASE(colour, Colour, colour)
	attr->SetTextColour(*colour);
}
