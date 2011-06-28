#include "DeltaWxColourData.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(colourdata, name)
#define WX_FUNC(name) WX_FUNC1(colourdata, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getchoosefull)
WX_FUNC_DEF(getcolour)
WX_FUNC_DEF(getcustomcolour)
WX_FUNC_DEF(setchoosefull)
WX_FUNC_DEF(setcolour)
WX_FUNC_DEF(setcustomcolour)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getchoosefull),
	WX_FUNC(getcolour),
	WX_FUNC(getcustomcolour),
	WX_FUNC(setchoosefull),
	WX_FUNC(setcolour),
	WX_FUNC(setcustomcolour)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setcustomcolour")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ColourData, "colourdata", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ColourData, val, colourdata) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetColour (void* val, DeltaValue* at) 
{
	wxColourData *data = DLIB_WXTYPECAST_BASE(ColourData, val, colourdata);
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(data->GetColour())));
	WX_SETOBJECT_EX(*at, Colour, retval)
	return true;
}

static bool GetCustomColours (void* val, DeltaValue* at) 
{
	wxColourData *data = DLIB_WXTYPECAST_BASE(ColourData, val, colourdata);
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0; i < 16; ++i) {
		DeltaValue value;
		wxColour colour = data->GetCustomColour(i);
		DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(colour)));
		WX_SETOBJECT_EX(value, Colour, retval)
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static bool GetChooseFull (void* val, DeltaValue* at) 
{
	wxColourData *data = DLIB_WXTYPECAST_BASE(ColourData, val, colourdata);
	at->FromBool(data->GetChooseFull());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "dataColour",			&GetColour,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "customColours",		&GetCustomColours,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "chooseFull",			&GetChooseFull,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ColourData, colourdata);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(colourdata_construct, 0, Nil)
	DeltaWxColourData *data = DNEWCLASS(DeltaWxColourData, (new wxColourData()));
	WX_SETOBJECT(ColourData, data)
}

DLIB_FUNC_START(colourdata_destruct, 1, Nil)
	DLIB_WXDELETE(colourdata, ColourData, data)
}

DLIB_FUNC_START(colourdata_getchoosefull, 1, Nil)
	DLIB_WXGET_BASE(colourdata, ColourData, data)
	WX_SETBOOL(data->GetChooseFull())
}

DLIB_FUNC_START(colourdata_getcolour, 1, Nil)
	DLIB_WXGET_BASE(colourdata, ColourData, data)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(data->GetColour())));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(colourdata_getcustomcolour, 2, Nil)
	DLIB_WXGET_BASE(colourdata, ColourData, data)
	WX_GETNUMBER(i)
	DeltaWxColour *retval = DNEWCLASS(DeltaWxColour, (new wxColour(data->GetCustomColour(i))));
	WX_SETOBJECT(Colour, retval)
}

DLIB_FUNC_START(colourdata_setchoosefull, 2, Nil)
	DLIB_WXGET_BASE(colourdata, ColourData, data)
	WX_GETBOOL(flag)
	data->SetChooseFull(flag);
}

DLIB_FUNC_START(colourdata_setcolour, 2, Nil)
	DLIB_WXGET_BASE(colourdata, ColourData, data)
	DLIB_WXGET_BASE(colour, Colour, col)
	data->SetColour(*col);
}

DLIB_FUNC_START(colourdata_setcustomcolour, 3, Nil)
	DLIB_WXGET_BASE(colourdata, ColourData, data)
	WX_GETNUMBER(i)
	DLIB_WXGET_BASE(colour, Colour, col)
	data->SetCustomColour(i, *col);
}
