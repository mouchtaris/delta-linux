#include "DeltaWxStaticText.h"
#include "DeltaWxControl.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(statictext, name)
#define WX_FUNC(name) WX_FUNC1(statictext, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getlabel)
WX_FUNC_DEF(setlabel)
WX_FUNC_DEF(wrap)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getlabel),
	WX_FUNC(setlabel),
	WX_FUNC(wrap)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "wrap")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(StaticText, "statictext", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(StaticText, val, statictext) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Control, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(StaticText,statictext)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(statictext_construct, 0, 7, Nil)
	wxStaticText *text = (wxStaticText*) 0;
	if (n == 0) {
		text = new wxStaticText();
	} else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		WX_GETSTRING(label)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxString name = wxStaticTextNameStr;
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { WX_GETSTRING_DEFINED(name) }
		text = new wxStaticText(parent, id, label, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(StaticText, text)
}

WX_FUNC_ARGRANGE_START(statictext_create, 4, 8, Nil)
	DLIB_WXGET_BASE(statictext, StaticText, text)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	WX_GETSTRING(label)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = 0;
	wxString name = wxStaticTextNameStr;
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(text->Create(parent, id, label, pos, size, style, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(text);
}

WX_FUNC_START(statictext_getlabel, 1, Nil)
	DLIB_WXGET_BASE(statictext, StaticText, text)
	WX_SETSTRING(text->GetLabel())
}

WX_FUNC_START(statictext_setlabel, 2, Nil)
	DLIB_WXGET_BASE(statictext, StaticText, text)
	WX_GETSTRING(label)
	text->SetLabel(label);
}

WX_FUNC_START(statictext_wrap, 2, Nil)
	DLIB_WXGET_BASE(statictext, StaticText, text)
	WX_GETNUMBER(width)
	text->Wrap(width);
}
