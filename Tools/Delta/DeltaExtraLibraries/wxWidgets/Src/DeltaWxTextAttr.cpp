#include "DeltaWxTextAttr.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(textattr, name)
#define WX_FUNC(name) WX_FUNC1(textattr, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getalignment)
WX_FUNC_DEF(getbackgroundcolour)
WX_FUNC_DEF(getfont)
WX_FUNC_DEF(getleftindent)
WX_FUNC_DEF(getleftsubindent)
WX_FUNC_DEF(getrightindent)
WX_FUNC_DEF(gettabs)
WX_FUNC_DEF(gettextcolour)
WX_FUNC_DEF(hasalignment)
WX_FUNC_DEF(hasbackgroundcolour)
WX_FUNC_DEF(hasfont)
WX_FUNC_DEF(hasleftindent)
WX_FUNC_DEF(hasrightindent)
WX_FUNC_DEF(hastabs)
WX_FUNC_DEF(hastextcolour)
WX_FUNC_DEF(getflags)
WX_FUNC_DEF(isdefault)
WX_FUNC_DEF(merge)
WX_FUNC_DEF(setalignment)
WX_FUNC_DEF(setbackgroundcolour)
WX_FUNC_DEF(setflags)
WX_FUNC_DEF(setfont)
WX_FUNC_DEF(setleftindent)
WX_FUNC_DEF(setrightindent)
WX_FUNC_DEF(settabs)
WX_FUNC_DEF(settextcolour)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getalignment),
	WX_FUNC(getbackgroundcolour),
	WX_FUNC(getfont),
	WX_FUNC(getleftindent),
	WX_FUNC(getleftsubindent),
	WX_FUNC(getrightindent),
	WX_FUNC(gettabs),
	WX_FUNC(gettextcolour),
	WX_FUNC(hasalignment),
	WX_FUNC(hasbackgroundcolour),
	WX_FUNC(hasfont),
	WX_FUNC(hasleftindent),
	WX_FUNC(hasrightindent),
	WX_FUNC(hastabs),
	WX_FUNC(hastextcolour),
	WX_FUNC(getflags),
	WX_FUNC(isdefault),
	WX_FUNC(merge),
	WX_FUNC(setalignment),
	WX_FUNC(setbackgroundcolour),
	WX_FUNC(setflags),
	WX_FUNC(setfont),
	WX_FUNC(setleftindent),
	WX_FUNC(setrightindent),
	WX_FUNC(settabs),
	WX_FUNC(settextcolour)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getalignment", "settextcolour")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(TextAttr, "textattr")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(TextAttr, val, textattr) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetFlags (void* val, DeltaValue* at) 
{
	wxTextAttr *textattr = DLIB_WXTYPECAST_BASE(TextAttr, val, textattr);
	WX_SETNUMBER_EX(*at, textattr->GetFlags())
	return true;
}

static bool GetTextColour (void* val, DeltaValue* at) 
{
	wxTextAttr *textattr = DLIB_WXTYPECAST_BASE(TextAttr, val, textattr);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(textattr->GetTextColour()))
	return true;
}

static bool GetBackgroundColour (void* val, DeltaValue* at) 
{
	wxTextAttr *textattr = DLIB_WXTYPECAST_BASE(TextAttr, val, textattr);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(textattr->GetBackgroundColour()))
	return true;
}

static bool GetFont (void* val, DeltaValue* at) 
{
	wxTextAttr *textattr = DLIB_WXTYPECAST_BASE(TextAttr, val, textattr);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Font, new wxFont(textattr->GetFont()))
	return true;
}

static bool GetAlignment (void* val, DeltaValue* at) 
{
	wxTextAttr *textattr = DLIB_WXTYPECAST_BASE(TextAttr, val, textattr);
	WX_SETNUMBER_EX(*at, textattr->GetAlignment())
	return true;
}

static bool GetTabs (void* val, DeltaValue* at) 
{
	wxTextAttr *textattr = DLIB_WXTYPECAST_BASE(TextAttr, val, textattr);
	wxArrayInt tabs = textattr->GetTabs();
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, n = (int)tabs.GetCount(); i < n; ++i) {
		DeltaValue value;
		WX_SETNUMBER_EX(value, tabs[i]);
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static bool GetLeftIndent (void* val, DeltaValue* at) 
{
	wxTextAttr *textattr = DLIB_WXTYPECAST_BASE(TextAttr, val, textattr);
	WX_SETNUMBER_EX(*at, textattr->GetLeftIndent())
	return true;
}

static bool GetLeftSubIndent (void* val, DeltaValue* at) 
{
	wxTextAttr *textattr = DLIB_WXTYPECAST_BASE(TextAttr, val, textattr);
	WX_SETNUMBER_EX(*at, textattr->GetLeftSubIndent())
	return true;
}

static bool GetRightIndent (void* val, DeltaValue* at) 
{
	wxTextAttr *textattr = DLIB_WXTYPECAST_BASE(TextAttr, val, textattr);
	WX_SETNUMBER_EX(*at, textattr->GetRightIndent())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "flags",				&GetFlags,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textColour",			&GetTextColour,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "backgroundColour",	&GetBackgroundColour,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "font",				&GetFont,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "textAlignment",		&GetAlignment,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "tabs",				&GetTabs,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "leftIndent",			&GetLeftIndent,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "leftSubIndent",		&GetLeftSubIndent,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "rightIndent",		&GetRightIndent,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(TextAttr,textattr)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(textattr_construct, 0, 4, Nil)
	wxTextAttr *attr = (wxTextAttr*) 0;
	if (n == 0) {
		attr = new wxTextAttr();
	} else {
		DLIB_WXGET_BASE(colour, Colour, colText)
		wxColour colBack = wxNullColour;
		wxFont font = wxNullFont;
		int alignment = wxTEXT_ALIGNMENT_DEFAULT;
		if (n >= 2) { DLIB_WXGET_BASE(colour, Colour, colour) colBack = *colour; }
		if (n >= 3) { DLIB_WXGET_BASE(font, Font, f) font = *f; }
		if (n >= 4) { WX_GETDEFINE_DEFINED(alignment) }
		attr = new wxTextAttr(*colText, colBack, font, (wxTextAttrAlignment)alignment);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(TextAttr, attr)
}

WX_FUNC_START(textattr_getalignment, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETNUMBER(attr->GetAlignment())
}

WX_FUNC_START(textattr_getbackgroundcolour, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(attr->GetBackgroundColour()))
}

WX_FUNC_START(textattr_getfont, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(attr->GetFont()))
}

WX_FUNC_START(textattr_getleftindent, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETNUMBER(attr->GetLeftIndent())
}

WX_FUNC_START(textattr_getleftsubindent, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETNUMBER(attr->GetLeftSubIndent())
}

WX_FUNC_START(textattr_getrightindent, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETNUMBER(attr->GetRightIndent())
}

WX_FUNC_START(textattr_gettabs, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	wxArrayInt tabs = attr->GetTabs();
	DLIB_RETVAL_REF.FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, n = (int)tabs.capacity(); i < n; ++i) {
		DLIB_RETVAL_REF.ToTable()->Set(DeltaValue((DeltaNumberValueType)i), DeltaValue(DeltaNumberValueType(tabs[i])));
	}
}

WX_FUNC_START(textattr_gettextcolour, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(attr->GetTextColour()))
}

WX_FUNC_START(textattr_hasalignment, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETBOOL(attr->HasAlignment())
}

WX_FUNC_START(textattr_hasbackgroundcolour, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETBOOL(attr->HasBackgroundColour())
}

WX_FUNC_START(textattr_hasfont, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETBOOL(attr->HasFont())
}

WX_FUNC_START(textattr_hasleftindent, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETBOOL(attr->HasLeftIndent())
}

WX_FUNC_START(textattr_hasrightindent, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETBOOL(attr->HasRightIndent())
}

WX_FUNC_START(textattr_hastabs, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETBOOL(attr->HasTabs())
}

WX_FUNC_START(textattr_hastextcolour, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETBOOL(attr->HasTextColour())
}

WX_FUNC_START(textattr_getflags, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETNUMBER(attr->GetFlags())
}

WX_FUNC_START(textattr_isdefault, 1, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_SETBOOL(attr->IsDefault())
}

WX_FUNC_START(textattr_merge, 2, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	DLIB_WXGET_BASE(textattr, TextAttr, overlay)
	attr->Merge(*overlay);
}

WX_FUNC_START(textattr_setalignment, 2, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_GETDEFINE(alignment)
	attr->SetAlignment((wxTextAttrAlignment)alignment);
}

WX_FUNC_START(textattr_setbackgroundcolour, 2, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	DLIB_WXGET_BASE(colour, Colour, colour)
	attr->SetBackgroundColour(*colour);
}

WX_FUNC_START(textattr_setflags, 2, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_GETDEFINE(flags)
	attr->SetFlags(flags);
}

WX_FUNC_START(textattr_setfont, 2, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	DLIB_WXGET_BASE(font, Font, font)
	attr->SetFont(*font);
}

WX_FUNC_ARGRANGE_START(textattr_setleftindent, 2, 3, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_GETNUMBER(indent)
	int subindent = 0;
	if (n >= 3) { WX_GETNUMBER_DEFINED(subindent) }
	attr->SetLeftIndent(indent, subindent);
}

WX_FUNC_START(textattr_setrightindent, 2, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_GETNUMBER(indent)
	attr->SetRightIndent(indent);
}

WX_FUNC_START(textattr_settabs, 2, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	WX_GETTABLE(tab_table)
	wxArrayInt tabs;
	for (int i = 0, n = tab_table->Total(); i < n; ++i) {
		DeltaValue value;
		tab_table->Get(DeltaValue((DeltaNumberValueType)i), &value);
		if (value.Type() == DeltaValue_Number) {
			tabs.Insert((int)value.ToNumber(), i);
		}
	}
	attr->SetTabs(tabs);
}

WX_FUNC_START(textattr_settextcolour, 2, Nil)
	DLIB_WXGET_BASE(textattr, TextAttr, attr)
	DLIB_WXGET_BASE(colour, Colour, colour)
	attr->SetTextColour(*colour);
}
