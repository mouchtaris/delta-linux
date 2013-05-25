#include "DeltaWxCaret.h"
#include "DeltaWxWindow.h"
#include "DeltaWxSize.h"
#include "DeltaWxPoint.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(caret, name)
#define WX_FUNC(name) WX_FUNC1(caret, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getblinktime)
WX_FUNC_DEF(getposition)
WX_FUNC_DEF(getsize)
WX_FUNC_DEF(getwindow)
WX_FUNC_DEF(hide)
WX_FUNC_DEF(isok)
WX_FUNC_DEF(isvisible)
WX_FUNC_DEF(move)
WX_FUNC_DEF(setblinktime)
WX_FUNC_DEF(setsize)
WX_FUNC_DEF(show)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getblinktime),
	WX_FUNC(setblinktime),
	WX_FUNC(create),
	WX_FUNC(getposition),
	WX_FUNC(getsize),
	WX_FUNC(getwindow),
	WX_FUNC(hide),
	WX_FUNC(isok),
	WX_FUNC(isvisible),
	WX_FUNC(move),
	WX_FUNC(setsize),
	WX_FUNC(show)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(3, uarraysize(funcs) - 3, "create", "show")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(Caret, "caret")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(Caret, val, caret) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetWidth (void* val, DeltaValue* at) 
{
	wxCaret *caret = DLIB_WXTYPECAST_BASE(Caret, val, caret);
	at->FromNumber(caret->GetSize().x);
	return true;
}

static bool GetHeight (void* val, DeltaValue* at) 
{
	wxCaret *caret = DLIB_WXTYPECAST_BASE(Caret, val, caret);
	at->FromNumber(caret->GetSize().y);
	return true;
}

static bool GetX (void* val, DeltaValue* at) 
{
	wxCaret *caret = DLIB_WXTYPECAST_BASE(Caret, val, caret);
	at->FromNumber(caret->GetPosition().x);
	return true;
}

static bool GetY (void* val, DeltaValue* at) 
{
	wxCaret *caret = DLIB_WXTYPECAST_BASE(Caret, val, caret);
	at->FromNumber(caret->GetPosition().y);
	return true;
}

static bool GetWindow (void* val, DeltaValue* at) 
{
	wxCaret *caret = DLIB_WXTYPECAST_BASE(Caret, val, caret);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Window, caret->GetWindow())
	return true;
}

static bool GetIsVisible (void* val, DeltaValue* at) 
{
	wxCaret *caret = DLIB_WXTYPECAST_BASE(Caret, val, caret);
	at->FromBool(caret->IsVisible());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "width",				&GetWidth,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "height",				&GetHeight,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "x",					&GetX,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "y",					&GetY,					DELTA_GETBYSTRING_NO_PRECOND	},
	{ "window",				&GetWindow,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isVisible",			&GetIsVisible,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(Caret,caret)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(caret_construct, 0, 3, Nil)
	wxCaret *caret = (wxCaret*) 0;
	if (n == 0) {
		caret = new wxCaret();
	} else if (n == 2) {
		DLIB_WXGET_BASE(window, Window, window)
		DLIB_WXGETSIZE_BASE(size)
		caret = new wxCaret(window, *size);
	} else if (n == 3) {
		DLIB_WXGET_BASE(window, Window, window)
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		caret = new wxCaret(window, width, height);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SETOBJECT(Caret, caret)
}

WX_FUNC_ARGRANGE_START(caret_create, 3, 4, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	DLIB_WXGET_BASE(window, Window, window)
	if (n == 3) {
		DLIB_WXGETSIZE_BASE(size)
		WX_SETBOOL(caret->Create(window, *size));
	} else {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		WX_SETBOOL(caret->Create(window, width, height));
	}
}

WX_FUNC_START(caret_getblinktime, 0, Nil)
	WX_SETNUMBER(wxCaret::GetBlinkTime());
}

WX_FUNC_START(caret_getposition, 1, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(caret->GetPosition()))
}

WX_FUNC_START(caret_getsize, 1, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(caret->GetSize()))
}

WX_FUNC_START(caret_getwindow, 1, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	wxWindow* retval	= caret->GetWindow();;
	WX_SETOBJECT(Window, retval)
}

WX_FUNC_START(caret_hide, 1, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	caret->Hide();
}

WX_FUNC_START(caret_isok, 1, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	WX_SETBOOL(caret->IsOk());
}

WX_FUNC_START(caret_isvisible, 1, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	WX_SETBOOL(caret->IsVisible());
}

WX_FUNC_ARGRANGE_START(caret_move, 2, 3, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	if (n == 2) {
		DLIB_WXGETPOINT_BASE(pt)
		caret->Move(*pt);
	} else {
		WX_GETNUMBER(x)
		WX_GETNUMBER(y)
		caret->Move(x, y);
	}
}

WX_FUNC_START(caret_setblinktime, 1, Nil)
	WX_GETNUMBER(milliseconds)
	wxCaret::SetBlinkTime(milliseconds);
}

WX_FUNC_ARGRANGE_START(caret_setsize, 2, 3, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	if (n == 2) {
		DLIB_WXGETSIZE_BASE(size)
		caret->SetSize(*size);
	} else {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		caret->SetSize(width, height);
	}
}

WX_FUNC_ARGRANGE_START(caret_show, 1, 2, Nil)
	DLIB_WXGET_BASE(caret, Caret, caret)
	if (n == 1) {
		caret->Show();
	} else {
		WX_GETBOOL(show)
		caret->Show(show);
	}
}
