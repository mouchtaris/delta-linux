#include "DeltaWxSplashScreen.h"
#include "DeltaWxFrame.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
#include "DeltaWxCloseEvent.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(splashscreen, name)
#define WX_FUNC(name) WX_FUNC1(splashscreen, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(onclosewindow)
WX_FUNC_DEF(getbitmap)
WX_FUNC_DEF(getsplashstyle)
WX_FUNC_DEF(gettimeout)
WX_FUNC_DEF(setbitmap)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(onclosewindow),
	WX_FUNC(getbitmap),
	WX_FUNC(getsplashstyle),
	WX_FUNC(gettimeout),
	WX_FUNC(setbitmap)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setbitmap")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(SplashScreen, "splashscreen", Frame)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SplashScreen, val, splashscreen) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxFrame *_parent = DLIB_WXTYPECAST_BASE(Frame, val, frame);
	DeltaWxFrame *parent = DNEWCLASS(DeltaWxFrame, (_parent));
	WX_SETOBJECT_EX(*at, Frame, parent)
	return true;
}

static bool GetSplashStyle (void* val, DeltaValue* at) 
{
	wxSplashScreen *splash = DLIB_WXTYPECAST_BASE(SplashScreen, val, splashscreen);
	WX_SETNUMBER_EX(*at, splash->GetSplashStyle())
	return true;
}

static bool GetMilliseconds (void* val, DeltaValue* at) 
{
	wxSplashScreen *splash = DLIB_WXTYPECAST_BASE(SplashScreen, val, splashscreen);
	WX_SETNUMBER_EX(*at, splash->GetTimeout())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Frame",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "splashStyle",		&GetSplashStyle,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "milliseconds",		&GetMilliseconds,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SplashScreen, splashscreen);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(splashscreen_construct, 5, 8, Nil)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	WX_GETDEFINE(splashStyle)
	WX_GETNUMBER(milliseconds)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP;
	if (n >= 6) { DLIB_WXGET_BASE(point, Point, _pos) pos = *_pos; }
	if (n >= 7) { DLIB_WXGET_BASE(size, Size, _size) size = *_size; }
	if (n >= 8) { WX_GETDEFINE_DEFINED(style) }
	DeltaWxSplashScreen *splash = DNEWCLASS(DeltaWxSplashScreen,
		(new wxSplashScreen(*bitmap, splashStyle, milliseconds, parent, id, pos, size, style)));
	WX_SETOBJECT(SplashScreen, splash)
}

DLIB_FUNC_START(splashscreen_destruct, 1, Nil)
	DLIB_WXDELETE(splashscreen, SplashScreen, splash)
}

DLIB_FUNC_START(splashscreen_onclosewindow, 2, Nil)
	DLIB_WXGET_BASE(splashscreen, SplashScreen, splash)
	DLIB_WXGET_BASE(closeevent, CloseEvent, evt)
	splash->OnCloseWindow(*evt);
}

DLIB_FUNC_START(splashscreen_getbitmap, 1, Nil)
	DLIB_WXGET_BASE(splashscreen, SplashScreen, splash)
	DeltaWxBitmap *retval = DNEWCLASS(DeltaWxBitmap, (
		new wxBitmap(splash->GetSplashWindow()->GetBitmap())));
	WX_SETOBJECT(Bitmap, retval)
}

DLIB_FUNC_START(splashscreen_getsplashstyle, 1, Nil)
	DLIB_WXGET_BASE(splashscreen, SplashScreen, splash)
	WX_SETNUMBER(splash->GetSplashStyle())
}

DLIB_FUNC_START(splashscreen_gettimeout, 1, Nil)
	DLIB_WXGET_BASE(splashscreen, SplashScreen, splash)
	WX_SETNUMBER(splash->GetTimeout())
}

DLIB_FUNC_START(splashscreen_setbitmap, 2, Nil)
	DLIB_WXGET_BASE(splashscreen, SplashScreen, splash)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	splash->GetSplashWindow()->SetBitmap(*bitmap);
}
