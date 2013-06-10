#include "DeltaWxFontPickerEvent.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(fontpickerevent, name)
#define WX_FUNC(name) WX_FUNC1(fontpickerevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getfont)
WX_FUNC_DEF(setfont)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getfont),
	WX_FUNC(setfont)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getfont", "setfont")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FontPickerEvent, "fontpickerevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FontPickerEvent, val, fontpickerevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetFont (void* val, DeltaValue* at) 
{
	wxFontPickerEvent *ev = DLIB_WXTYPECAST_BASE(FontPickerEvent, val, fontpickerevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Font, new wxFont(ev->GetFont()))
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "font",				&GetFont,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FontPickerEvent,fontpickerevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(fontpickerevent_construct, 0, 3, Nil)
	wxFontPickerEvent *evt = (wxFontPickerEvent*) 0;
	if (n == 0)
		evt = new wxFontPickerEvent();
	else if (n == 3) {
		DLIB_WXGET_BASE(object, Object, generator)
		WX_GETDEFINE(id)
		DLIB_WXGET_BASE(font, Font, font)
		evt = new wxFontPickerEvent(generator, id, *font);
	}
	WX_SETOBJECT(FontPickerEvent, evt)
}

WX_FUNC_START(fontpickerevent_getfont, 1, Nil)
	DLIB_WXGET_BASE(fontpickerevent, FontPickerEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Font, new wxFont(evt->GetFont()))
}

WX_FUNC_START(fontpickerevent_setfont, 2, Nil)
	DLIB_WXGET_BASE(fontpickerevent, FontPickerEvent, evt)
	DLIB_WXGET_BASE(font, Font, font)
	evt->SetFont(*font);
}
