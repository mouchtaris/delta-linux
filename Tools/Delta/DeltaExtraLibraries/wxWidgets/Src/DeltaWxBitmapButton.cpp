#include "DeltaWxBitmapButton.h"
#include "DeltaWxButton.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxValidator.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
#include "DeltaWxLibraryTemplates_SetWrapperChild.h" // FIXME
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(bitmapbutton, name)
#define WX_FUNC(name) WX_FUNC1(bitmapbutton, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getbitmapdisabled)
WX_FUNC_DEF(getbitmapfocus)
WX_FUNC_DEF(getbitmaphover)
WX_FUNC_DEF(getbitmaplabel)
WX_FUNC_DEF(getbitmapselected)
WX_FUNC_DEF(setbitmapdisabled)
WX_FUNC_DEF(setbitmapfocus)
WX_FUNC_DEF(setbitmaphover)
WX_FUNC_DEF(setbitmaplabel)
WX_FUNC_DEF(setbitmapselected)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getbitmapdisabled),
	WX_FUNC(getbitmapfocus),
	WX_FUNC(getbitmaphover),
	WX_FUNC(getbitmaplabel),
	WX_FUNC(getbitmapselected),
	WX_FUNC(setbitmapdisabled),
	WX_FUNC(setbitmapfocus),
	WX_FUNC(setbitmaphover),
	WX_FUNC(setbitmaplabel),
	WX_FUNC(setbitmapselected)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "setbitmapselected")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(BitmapButton, "bitmapbutton", Button)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(BitmapButton, val, bitmapbutton) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Button, val)
	return true;
}

static bool GetBmpNormal (void* val, DeltaValue* at) 
{
	wxBitmapButton *bmpbutton = DLIB_WXTYPECAST_BASE(BitmapButton, val, bitmapbutton);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Bitmap, new wxBitmap(bmpbutton->GetBitmapLabel()))
	return true;
}

static bool GetBmpSelected (void* val, DeltaValue* at) 
{
	wxBitmapButton *bmpbutton = DLIB_WXTYPECAST_BASE(BitmapButton, val, bitmapbutton);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Bitmap, new wxBitmap(bmpbutton->GetBitmapSelected()))
	return true;
}

static bool GetBmpFocus (void* val, DeltaValue* at) 
{
	wxBitmapButton *bmpbutton = DLIB_WXTYPECAST_BASE(BitmapButton, val, bitmapbutton);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Bitmap, new wxBitmap(bmpbutton->GetBitmapFocus()))
	return true;
}

static bool GetBmpDisabled (void* val, DeltaValue* at) 
{
	wxBitmapButton *bmpbutton = DLIB_WXTYPECAST_BASE(BitmapButton, val, bitmapbutton);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Bitmap, new wxBitmap(bmpbutton->GetBitmapDisabled()))
	return true;
}

static bool GetBmpHover (void* val, DeltaValue* at) 
{
	wxBitmapButton *bmpbutton = DLIB_WXTYPECAST_BASE(BitmapButton, val, bitmapbutton);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Bitmap, new wxBitmap(bmpbutton->GetBitmapHover()))
	return true;
}

static bool GetMarginX (void* val, DeltaValue* at) 
{
	wxBitmapButton *bmpbutton = DLIB_WXTYPECAST_BASE(BitmapButton, val, bitmapbutton);
	at->FromNumber(bmpbutton->GetMarginX());
	return true;
}

static bool GetMarginY (void* val, DeltaValue* at) 
{
	wxBitmapButton *bmpbutton = DLIB_WXTYPECAST_BASE(BitmapButton, val, bitmapbutton);
	at->FromNumber(bmpbutton->GetMarginY());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Button",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bmpNormal",			&GetBmpNormal,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bmpSelected",		&GetBmpSelected,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bmpFocus",			&GetBmpFocus,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bmpDisabled",		&GetBmpDisabled,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bmpHover",			&GetBmpHover,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "marginX",			&GetMarginX,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "marginY",			&GetMarginY,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(BitmapButton,bitmapbutton)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(bitmapbutton_construct, 0, 8, Nil)
	wxBitmapButton *bmpbutton = (wxBitmapButton*) 0;
	if (n == 0) {
		bmpbutton = new wxBitmapButton();
	} else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = wxBU_AUTODRAW;
		wxValidator *validator = (wxValidator*)&wxDefaultValidator;
		wxString name = wxButtonNameStr;
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
		if (n >= 8) { WX_GETSTRING_DEFINED(name) }
		bmpbutton = new wxBitmapButton(parent, id, *bitmap, pos, size, style, *validator, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SET_WINDOW_OBJECT(BitmapButton, bmpbutton)
}

WX_FUNC_ARGRANGE_START(bitmapbutton_create, 4, 9, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxBU_AUTODRAW;
	wxValidator *validator = (wxValidator*)&wxDefaultValidator;
	wxString name = wxButtonNameStr;
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { DLIB_WXGET_BASE(validator, Validator, val) validator = val; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(bmpbutton->Create(parent, id, *bitmap, pos, size, style, *validator, name))
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(bmpbutton);
}

WX_FUNC_START(bitmapbutton_getbitmapdisabled, 1, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	WX_SETOBJECT(Bitmap, new wxBitmap(bmpbutton->GetBitmapDisabled()))
}

WX_FUNC_START(bitmapbutton_getbitmapfocus, 1, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	WX_SETOBJECT(Bitmap, new wxBitmap(bmpbutton->GetBitmapFocus()))
}

WX_FUNC_START(bitmapbutton_getbitmaphover, 1, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	WX_SETOBJECT(Bitmap, new wxBitmap(bmpbutton->GetBitmapHover()))
}

WX_FUNC_START(bitmapbutton_getbitmaplabel, 1, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	WX_SETOBJECT(Bitmap, new wxBitmap(bmpbutton->GetBitmapLabel()))
}

WX_FUNC_START(bitmapbutton_getbitmapselected, 1, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	WX_SETOBJECT(Bitmap, new wxBitmap(bmpbutton->GetBitmapSelected()))
}

WX_FUNC_START(bitmapbutton_setbitmapdisabled, 2, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	bmpbutton->SetBitmapDisabled(*bitmap);
}

WX_FUNC_START(bitmapbutton_setbitmapfocus, 2, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	bmpbutton->SetBitmapFocus(*bitmap);
}

WX_FUNC_START(bitmapbutton_setbitmaphover, 2, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	bmpbutton->SetBitmapHover(*bitmap);
}

WX_FUNC_START(bitmapbutton_setbitmaplabel, 2, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	bmpbutton->SetBitmapLabel(*bitmap);
}

WX_FUNC_START(bitmapbutton_setbitmapselected, 2, Nil)
	DLIB_WXGET_BASE(bitmapbutton, BitmapButton, bmpbutton)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	bmpbutton->SetBitmapSelected(*bitmap);
}

