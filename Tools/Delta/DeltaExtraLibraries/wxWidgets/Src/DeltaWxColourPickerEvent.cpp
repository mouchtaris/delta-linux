#include "DeltaWxColourPickerEvent.h"
#include "DeltaWxCommandEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(colourpickerevent, name)
#define WX_FUNC(name) WX_FUNC1(colourpickerevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getcolour)
WX_FUNC_DEF(setcolour)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getcolour),
	WX_FUNC(setcolour)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getcolour", "setcolour")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(ColourPickerEvent, "colourpickerevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(ColourPickerEvent, val, colourpickerevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetColour (void* val, DeltaValue* at) 
{
	wxColourPickerEvent *ev = DLIB_WXTYPECAST_BASE(ColourPickerEvent, val, colourpickerevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Colour, new wxColour(ev->GetColour()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "colour",				&GetColour,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(ColourPickerEvent,colourpickerevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(colourpickerevent_construct, 0, 3, Nil)
	wxColourPickerEvent *evt = (wxColourPickerEvent*) 0;
	if (n == 0)
		evt = new wxColourPickerEvent();
	else if (n == 3) {
		DLIB_WXGET_BASE(object, Object, generator)
		WX_GETDEFINE(id)
		DLIB_WXGET_BASE(colour, Colour, colour)
		evt = new wxColourPickerEvent(generator, id, *colour);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(ColourPickerEvent, evt)
}

WX_FUNC_START(colourpickerevent_getcolour, 1, Nil)
	DLIB_WXGET_BASE(colourpickerevent, ColourPickerEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Colour, new wxColour(evt->GetColour()))
}

WX_FUNC_START(colourpickerevent_setcolour, 2, Nil)
	DLIB_WXGET_BASE(colourpickerevent, ColourPickerEvent, evt)
	DLIB_WXGET_BASE(colour, Colour, colour)
	evt->SetColour(*colour);
}
