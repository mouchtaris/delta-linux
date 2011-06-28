#include "DeltaWxVisualAttributes.h"
#include "DeltaWxFont.h"
#include "DeltaWxColour.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(visualattributes, name)
#define WX_FUNC(name) WX_FUNC1(visualattributes, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getcolbg)
WX_FUNC_DEF(getcolfg)
WX_FUNC_DEF(getfont)
WX_FUNC_DEF(setcolbg)
WX_FUNC_DEF(setcolfg)
WX_FUNC_DEF(setfont)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getcolbg),
	WX_FUNC(getcolfg),
	WX_FUNC(getfont),
	WX_FUNC(setcolbg),
	WX_FUNC(setcolfg),
	WX_FUNC(setfont)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setfont")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(VisualAttributes, "visualattributes")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(VisualAttributes, val, visualattributes) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetFont (void* val, DeltaValue* at) 
{
	wxVisualAttributes *attr = DLIB_WXTYPECAST_BASE(VisualAttributes, val, visualattributes);
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(attr->font)));
	WX_SETOBJECT_EX(*at, Font, retval)
	return true;
}

static bool GetForegroundColour (void* val, DeltaValue* at) 
{
	wxVisualAttributes *attr = DLIB_WXTYPECAST_BASE(VisualAttributes, val, visualattributes);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->colFg)));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static bool GetBackgroundColour (void* val, DeltaValue* at) 
{
	wxVisualAttributes *attr = DLIB_WXTYPECAST_BASE(VisualAttributes, val, visualattributes);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->colBg)));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "font",				&GetFont,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "foregroundColour",	&GetForegroundColour,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "backgroundColour",	&GetBackgroundColour,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(VisualAttributes, visualattributes);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(visualattributes_construct, 0, 3, Nil)
	wxVisualAttributes *wxattr = (wxVisualAttributes*) 0;
	DeltaWxVisualAttributes *attr = (DeltaWxVisualAttributes*) 0;
	if (n == 0)
		wxattr = new wxVisualAttributes();
	else if (n == 3) {
		DLIB_WXGET_BASE(font, Font, font)
		DLIB_WXGET_BASE(colour, Colour, colFg)
		DLIB_WXGET_BASE(colour, Colour, colBg)
		wxattr = new wxVisualAttributes();
		wxattr->font = *font;
		wxattr->colFg = *colFg;
		wxattr->colBg = *colBg;
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxattr) attr = DNEWCLASS(DeltaWxVisualAttributes, (wxattr));
	WX_SETOBJECT(VisualAttributes, attr)
}

DLIB_FUNC_START(visualattributes_destruct, 1, Nil)
	DLIB_WXDELETE(visualattributes, VisualAttributes, attr)
}

DLIB_FUNC_START(visualattributes_getcolbg, 1, Nil)
	DLIB_WXGET_BASE(visualattributes, VisualAttributes, attr)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->colBg)));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(visualattributes_getcolfg, 1, Nil)
	DLIB_WXGET_BASE(visualattributes, VisualAttributes, attr)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(attr->colFg)));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(visualattributes_getfont, 1, Nil)
	DLIB_WXGET_BASE(visualattributes, VisualAttributes, attr)
	DeltaWxFont *retval = DNEWCLASS(DeltaWxFont, (new wxFont(attr->font)));
	WX_SETOBJECT(Font, retval)
}

DLIB_FUNC_START(visualattributes_setcolbg, 2, Nil)
	DLIB_WXGET_BASE(visualattributes, VisualAttributes, attr)
	DLIB_WXGET_BASE(colour, Colour, colour)
	attr->colBg = *colour;
}

DLIB_FUNC_START(visualattributes_setcolfg, 2, Nil)
	DLIB_WXGET_BASE(visualattributes, VisualAttributes, attr)
	DLIB_WXGET_BASE(colour, Colour, colour)
	attr->colFg = *colour;
}

DLIB_FUNC_START(visualattributes_setfont, 2, Nil)
	DLIB_WXGET_BASE(visualattributes, VisualAttributes, attr)
	DLIB_WXGET_BASE(font, Font, font)
	attr->font = *font;
}
