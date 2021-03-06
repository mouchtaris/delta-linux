#include "DeltaWxFont.h"
#include "DeltaWxObject.h"
#include "DeltaWxSize.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
//

typedef const wxFont* (* DefaultFontGetter) (void);
typedef std::map<std::string, DefaultFontGetter> DefaultFontsMap;

static DefaultFontsMap defaultFontMap;

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(font, name)
#define WX_FUNC(name) WX_FUNC1(font, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(isfixedwidth)
WX_FUNC_DEF(getdefaultencoding)
WX_FUNC_DEF(getfacename)
WX_FUNC_DEF(getfamily)
WX_FUNC_DEF(getnativefontinfodesc)
WX_FUNC_DEF(getnativefontinfouserdesc)
WX_FUNC_DEF(getpointsize)
WX_FUNC_DEF(getstyle)
WX_FUNC_DEF(getunderlined)
WX_FUNC_DEF(getweight)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(setdefaultencoding)
WX_FUNC_DEF(setfacename)
WX_FUNC_DEF(setfamily)
WX_FUNC_DEF(setnativefontinfo)
WX_FUNC_DEF(setnativefontinfouserdesc)
WX_FUNC_DEF(setpointsize)
WX_FUNC_DEF(setstyle)
WX_FUNC_DEF(setunderlined)
WX_FUNC_DEF(setweight)
WX_FUNC_DEF(equal)
WX_FUNC_DEF(notequal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getdefaultencoding),
	WX_FUNC(setdefaultencoding),
	WX_FUNC(isfixedwidth),
	WX_FUNC(getfacename),
	WX_FUNC(getfamily),
	WX_FUNC(getnativefontinfodesc),
	WX_FUNC(getnativefontinfouserdesc),
	WX_FUNC(getpointsize),
	WX_FUNC(getstyle),
	WX_FUNC(getunderlined),
	WX_FUNC(getweight),
	WX_FUNC(isok),
	WX_FUNC(setfacename),
	WX_FUNC(setfamily),
	WX_FUNC(setnativefontinfo),
	WX_FUNC(setnativefontinfouserdesc),
	WX_FUNC(setpointsize),
	WX_FUNC(setstyle),
	WX_FUNC(setunderlined),
	WX_FUNC(setweight),
	WX_FUNC(equal),
	WX_FUNC(notequal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(3, uarraysize(funcs) - 3, "isfixedwidth", "notequal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(Font, "font", Object)

////////////////////////////////////////////////////////////////

static const wxFont* ItalicFontGetter (void)
	{ return wxITALIC_FONT; }

static const wxFont* NormalFontGetter (void)
	{ return wxNORMAL_FONT; }

static const wxFont* SmallFontGetter (void)
	{ return wxSMALL_FONT; }

static const wxFont* SwissFontGetter (void)
	{ return wxSWISS_FONT; }

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Font, val, font) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

void DeltaWxFontInitFunc()
{
	defaultFontMap.insert(DefaultFontsMap::value_type ("ITALIC_FONT",	&ItalicFontGetter));
	defaultFontMap.insert(DefaultFontsMap::value_type ("NORMAL_FONT",	&NormalFontGetter));
	defaultFontMap.insert(DefaultFontsMap::value_type ("SMALL_FONT",	&SmallFontGetter));
	defaultFontMap.insert(DefaultFontsMap::value_type ("SWISS_FONT",	&SwissFontGetter));
	FontUtils::InstallAll(methods);
	DeltaLibraryFuncArgsBinder* binder = DeltaLibraryFuncArgsBinder::New(&CallerChecker);
	DPTR(methods)->Set(DeltaValue("=="), DeltaValue(&font_equal_LibFunc, binder));
	DPTR(methods)->Set(DeltaValue("!="), DeltaValue(&font_notequal_LibFunc, binder));
}

void DeltaWxFontCleanUpFunc()
{
	defaultFontMap.clear();
}

bool DeltaWxFontSearch(std::string str, wxFont *font)
{
	if (str.find("wx") == 0 && str.length() > 0)
		str = str.substr(2);
	DefaultFontsMap::iterator it;
	it = defaultFontMap.find(str);
	if (it == defaultFontMap.end())
		return false;
	*font = *(*it->second)();
	return true;
}

WX_LIBRARY_FUNCS_IMPLEMENTATION_EX(Font, font,
								   DeltaWxFontInitFunc();,
								   DeltaWxFontCleanUpFunc();)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(font_construct, 0, 7, Nil)
	wxFont *font = (wxFont*) 0;
	if (n == 0)
		font = new wxFont();
	else if (n == 1) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			DLIB_WXGET_BASE(font, Font, _font)
			font = new wxFont(*_font);
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_String) {
			std::string str = DPTR(vm)->GetActualArg(_argNo++)->ToString();
			wxFont _font;
			if (DeltaWxFontSearch(str, &_font))
				font = new wxFont(_font);
		}
	} else if (n >= 4) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
			WX_GETNUMBER(pointSize)
			WX_GETDEFINE(family)
			WX_GETDEFINE(style)
			WX_GETDEFINE(weight)
			if (n == 4)
				font = new wxFont((int) pointSize, (wxFontFamily)family,
					(wxFontStyle) style, (wxFontWeight)weight);
			else if (n == 5) {
				WX_GETBOOL(underline)
				font = new wxFont((int) pointSize, (wxFontFamily)family,
					(wxFontStyle) style, (wxFontWeight)weight, underline);
			} else if (n == 6) {
				WX_GETBOOL(underline)
				WX_GETSTRING(faceName)
				font = new wxFont((int) pointSize, (wxFontFamily)family,
					(wxFontStyle) style, (wxFontWeight)weight, underline, faceName);
			} else {
				WX_GETBOOL(underline)
				WX_GETSTRING(faceName)
				WX_GETDEFINE(encoding)
				font = new wxFont((int) pointSize, (wxFontFamily)family,
					(wxFontStyle) style, (wxFontWeight)weight, underline, faceName, (wxFontEncoding)encoding);
			}
		} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
#if defined (__WXMSW__)
			DLIB_WXGETSIZE_BASE(pixelSize)
			WX_GETDEFINE(family)
			WX_GETDEFINE(style)
			WX_GETDEFINE(weight)
			if (n == 4)
				font = new wxFont(*pixelSize, (wxFontFamily)family,
					(wxFontStyle) style, (wxFontWeight)weight);
			else if (n == 5) {
				WX_GETBOOL(underline)
				font = new wxFont(*pixelSize, (wxFontFamily)family,
					(wxFontStyle) style, (wxFontWeight)weight, underline);
			} else if (n == 6) {
				WX_GETBOOL(underline)
				WX_GETSTRING(faceName)
				font = new wxFont(*pixelSize, (wxFontFamily)family,
					(wxFontStyle) style, (wxFontWeight)weight, underline, faceName);
			} else {
				WX_GETBOOL(underline)
				WX_GETSTRING(faceName)
				WX_GETDEFINE(encoding)
				font = new wxFont(*pixelSize, (wxFontFamily)family,
					(wxFontStyle) style, (wxFontWeight)weight, underline, faceName, (wxFontEncoding)encoding);
			}
#else
			DLIB_ERROR_CHECK(
				true,
				"This function overload is only available on MS Windows implementation of wxWidgets."
			);
#endif //__WXMSW__
		} else {
			DPTR(vm)->PrimaryError(
				"Wrong type of arg (%s passed) to '%s'",
				DPTR(vm)->GetActualArg(_argNo)->TypeStr(),
				CURR_FUNC
			);
			RESET_EMPTY
		}
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, font)
}

WX_FUNC_START(font_isfixedwidth, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETBOOL(font->IsFixedWidth());
}

WX_FUNC_START(font_getdefaultencoding, 0, Nil)
	WX_SETNUMBER(wxFont::GetDefaultEncoding());
}

WX_FUNC_START(font_getfacename, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETSTRING(font->GetFaceName())
}

WX_FUNC_START(font_getfamily, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETNUMBER(font->GetFamily());
}

WX_FUNC_START(font_getnativefontinfodesc, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETSTRING(font->GetNativeFontInfoDesc())
}

WX_FUNC_START(font_getnativefontinfouserdesc, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETSTRING(font->GetNativeFontInfoUserDesc())
}

WX_FUNC_START(font_getpointsize, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETNUMBER(font->GetPointSize());
}

WX_FUNC_START(font_getstyle, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETNUMBER(font->GetStyle());
}

WX_FUNC_START(font_getunderlined, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETBOOL(font->GetUnderlined());
}

WX_FUNC_START(font_getweight, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETNUMBER(font->GetWeight());
}

WX_FUNC_START(font_isok, 1, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_SETBOOL(font->IsOk());
}

WX_FUNC_START(font_setdefaultencoding, 1, Nil)
	WX_GETDEFINE(encoding)
	wxFont::SetDefaultEncoding((wxFontEncoding)encoding);
}

WX_FUNC_START(font_setfacename, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_GETSTRING(faceName)
	WX_SETBOOL(font->SetFaceName(faceName));
}

WX_FUNC_START(font_setfamily, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_GETDEFINE(family)
	font->SetFamily((wxFontFamily)family);
}

WX_FUNC_START(font_setnativefontinfo, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_GETSTRING(info)
	WX_SETBOOL(font->SetNativeFontInfo(info));
}

WX_FUNC_START(font_setnativefontinfouserdesc, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_GETSTRING(info)
	WX_SETBOOL(font->SetNativeFontInfoUserDesc(info));
}

WX_FUNC_START(font_setpointsize, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_GETNUMBER(pointSize)
	font->SetPointSize(pointSize);
}

WX_FUNC_START(font_setstyle, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_GETDEFINE(style)
	font->SetStyle(style);
}

WX_FUNC_START(font_setunderlined, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_GETBOOL(underlined)
	font->SetUnderlined(underlined);
}

WX_FUNC_START(font_setweight, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	WX_GETDEFINE(weight)
	font->SetWeight((wxFontWeight)weight);
}

WX_FUNC_START(font_equal, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	DLIB_WXGET_BASE(font, Font, font2)
	WX_SETBOOL(font->operator==(*font2))
}

WX_FUNC_START(font_notequal, 2, Nil)
	DLIB_WXGET_BASE(font, Font, font)
	DLIB_WXGET_BASE(font, Font, font2)
	WX_SETBOOL(font->operator!=(*font2))
}
