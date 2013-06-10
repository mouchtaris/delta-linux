#include "DeltaWxFontData.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(fontdata, name)
#define WX_FUNC(name) WX_FUNC1(fontdata, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(enableeffects)
WX_FUNC_DEF(getallowsymbols)
WX_FUNC_DEF(getcolour)
WX_FUNC_DEF(getchosenfont)
WX_FUNC_DEF(getenableeffects)
WX_FUNC_DEF(getinitialfont)
WX_FUNC_DEF(getshowhelp)
WX_FUNC_DEF(setallowsymbols)
WX_FUNC_DEF(setchosenfont)
WX_FUNC_DEF(setcolour)
WX_FUNC_DEF(setinitialfont)
WX_FUNC_DEF(setrange)
WX_FUNC_DEF(setshowhelp)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(enableeffects),
	WX_FUNC(getallowsymbols),
	WX_FUNC(getcolour),
	WX_FUNC(getchosenfont),
	WX_FUNC(getenableeffects),
	WX_FUNC(getinitialfont),
	WX_FUNC(getshowhelp),
	WX_FUNC(setallowsymbols),
	WX_FUNC(setchosenfont),
	WX_FUNC(setcolour),
	WX_FUNC(setinitialfont),
	WX_FUNC(setrange),
	WX_FUNC(setshowhelp)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "enableeffects", "setshowhelp")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FontData, "fontdata", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FontData, val, fontdata) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static bool GetFontColour (void* val, DeltaValue* at) 
{
	wxFontData *data = DLIB_WXTYPECAST_BASE(FontData, val, fontdata);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(data->GetColour()))
	return true;
}

static bool GetShowHelp (void* val, DeltaValue* at) 
{
	wxFontData *data = DLIB_WXTYPECAST_BASE(FontData, val, fontdata);
	WX_SETBOOL_EX(*at, data->GetShowHelp())
	return true;
}

static bool GetAllowSymbols (void* val, DeltaValue* at) 
{
	wxFontData *data = DLIB_WXTYPECAST_BASE(FontData, val, fontdata);
	WX_SETBOOL_EX(*at, data->GetAllowSymbols())
	return true;
}

static bool GetEnableEffects (void* val, DeltaValue* at) 
{
	wxFontData *data = DLIB_WXTYPECAST_BASE(FontData, val, fontdata);
	WX_SETBOOL_EX(*at, data->GetEnableEffects())
	return true;
}

static bool GetInitialFont (void* val, DeltaValue* at) 
{
	wxFontData *data = DLIB_WXTYPECAST_BASE(FontData, val, fontdata);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Font, new wxFont(data->GetInitialFont()))
	return true;
}

static bool GetChosenFont (void* val, DeltaValue* at) 
{
	wxFontData *data = DLIB_WXTYPECAST_BASE(FontData, val, fontdata);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Font, new wxFont(data->GetChosenFont()))
	return true;
}

static bool GetMinSize (void* val, DeltaValue* at) 
{
	wxFontData *data = DLIB_WXTYPECAST_BASE(FontData, val, fontdata);
	WX_SETNUMBER_EX(*at, data->m_minSize)
	return true;
}

static bool GetMaxSize (void* val, DeltaValue* at) 
{
	wxFontData *data = DLIB_WXTYPECAST_BASE(FontData, val, fontdata);
	WX_SETNUMBER_EX(*at, data->m_maxSize)
	return true;
}

static bool GetEncoding (void* val, DeltaValue* at) 
{
	wxFontData *data = DLIB_WXTYPECAST_BASE(FontData, val, fontdata);
	WX_SETNUMBER_EX(*at, data->GetEncoding())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "fontColour",			&GetFontColour,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "showHelp",			&GetShowHelp,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "allowSymbols",		&GetAllowSymbols,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "enableEffects",		&GetEnableEffects,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "initialFont",		&GetInitialFont,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "chosenFont",			&GetChosenFont,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "minSize",			&GetMinSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "maxSize",			&GetMaxSize,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "encoding",			&GetEncoding,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FontData,fontdata)

////////////////////////////////////////////////////////////////

WX_FUNC_START(fontdata_construct, 0, Nil)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(FontData, new wxFontData())
}

WX_FUNC_START(fontdata_enableeffects, 2, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_GETBOOL(enable)
	data->EnableEffects(enable);
}

WX_FUNC_START(fontdata_getallowsymbols, 1, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_SETBOOL(data->GetAllowSymbols())
}

WX_FUNC_START(fontdata_getcolour, 1, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(data->GetColour()))
}

WX_FUNC_START(fontdata_getchosenfont, 1, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(data->GetChosenFont()))
}

WX_FUNC_START(fontdata_getenableeffects, 1, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_SETBOOL(data->GetEnableEffects())
}

WX_FUNC_START(fontdata_getinitialfont, 1, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(data->GetInitialFont()))
}

WX_FUNC_START(fontdata_getshowhelp, 1, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_SETBOOL(data->GetShowHelp())
}

WX_FUNC_START(fontdata_setallowsymbols, 2, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_GETBOOL(symbols)
	data->SetAllowSymbols(symbols);
}

WX_FUNC_START(fontdata_setchosenfont, 2, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	DLIB_WXGET_BASE(font, Font, font)
	data->SetChosenFont(*font);
}

WX_FUNC_START(fontdata_setcolour, 2, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	DLIB_WXGET_BASE(colour, Colour, colour)
	data->SetColour(*colour);
}

WX_FUNC_START(fontdata_setinitialfont, 2, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	DLIB_WXGET_BASE(font, Font, font)
	data->SetInitialFont(*font);
}

WX_FUNC_START(fontdata_setrange, 3, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_GETNUMBER(min)
	WX_GETNUMBER(max)
	data->SetRange(min, max);
}

WX_FUNC_START(fontdata_setshowhelp, 2, Nil)
	DLIB_WXGET_BASE(fontdata, FontData, data)
	WX_GETBOOL(help)
	data->SetShowHelp(help);
}
