#include "DeltaWxStatusBar.h"
#include "DeltaWxWindow.h"
#include "DeltaWxRect.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(statusbar, name)
#define WX_FUNC(name) WX_FUNC1(statusbar, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getfieldrect)
WX_FUNC_DEF(getfieldscount)
WX_FUNC_DEF(getstatustext)
WX_FUNC_DEF(popstatustext)
WX_FUNC_DEF(pushstatustext)
WX_FUNC_DEF(setfieldscount)
WX_FUNC_DEF(setminheight)
WX_FUNC_DEF(setstatustext)
WX_FUNC_DEF(setstatuswidths)
WX_FUNC_DEF(setstatusstyles)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(create),
	WX_FUNC(getfieldrect),
	WX_FUNC(getfieldscount),
	WX_FUNC(getstatustext),
	WX_FUNC(popstatustext),
	WX_FUNC(pushstatustext),
	WX_FUNC(setfieldscount),
	WX_FUNC(setminheight),
	WX_FUNC(setstatustext),
	WX_FUNC(setstatuswidths),
	WX_FUNC(setstatusstyles)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "create", "setstatusstyles")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(StatusBar, "statusbar", Window)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(StatusBar, val, statusbar) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Window, val)
	return true;
}

static bool GetFieldsCount (void* val, DeltaValue* at) 
{
	wxStatusBar *statusbar = DLIB_WXTYPECAST_BASE(StatusBar, val, statusbar);
	WX_SETNUMBER_EX(*at, statusbar->GetFieldsCount())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Window",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "numberOfFiels",		&GetFieldsCount,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(StatusBar,statusbar)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(statusbar_construct, 0, 4, Nil)
	wxStatusBar *stbar = (wxStatusBar*) 0;
	if (n == 0) {
		stbar = new wxStatusBar();
	} else {
		DLIB_WXGET_BASE(window, Window, parent)
		int id = -1;
		long style = 0x0010;
		if (n >= 2) { WX_GETDEFINE_DEFINED(id) }
		if (n >= 3) { WX_GETDEFINE_DEFINED(style) }
		if (n == 4) {
			WX_GETSTRING(name)
			stbar = new wxStatusBar(parent, id, style, name);
		} else {
			stbar = new wxStatusBar(parent, id, style);
		}
	}
	WX_SET_WINDOW_OBJECT(StatusBar, stbar)
}

WX_FUNC_ARGRANGE_START(statusbar_create, 2, 5, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	DLIB_WXGET_BASE(window, Window, parent)
	int id = -1;
	long style = 0x0010;
	if (n >= 3) { WX_GETDEFINE_DEFINED(id) }
	if (n >= 4) { WX_GETDEFINE_DEFINED(style) }
	if (n == 5) {
		WX_GETSTRING(name)
		WX_SETBOOL(stbar->Create(parent, id, style, name))
	} else {
		WX_SETBOOL(stbar->Create(parent, id, style))
	}
	SetWrapperChild<DeltaWxWindowClassId,DeltaWxWindow,wxWindow>(stbar);
}

WX_FUNC_START(statusbar_getfieldrect, 3, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	WX_GETNUMBER(i)
	DLIB_WXGET_BASE(rect, Rect, rect)
	WX_SETBOOL(stbar->GetFieldRect(i, *rect))
}

WX_FUNC_START(statusbar_getfieldscount, 1, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	WX_SETNUMBER(stbar->GetFieldsCount())
}

WX_FUNC_ARGRANGE_START(statusbar_getstatustext, 1, 2, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	if (n == 1) {
		WX_SETSTRING(stbar->GetStatusText())
	} else {
		WX_GETNUMBER(i)
		WX_SETSTRING(stbar->GetStatusText(i))
	}
}

WX_FUNC_ARGRANGE_START(statusbar_popstatustext, 1, 2, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	if (n == 1) {
		stbar->PopStatusText();
	} else {
		WX_GETNUMBER(field)
		stbar->PopStatusText(field);
	}
}

WX_FUNC_ARGRANGE_START(statusbar_pushstatustext, 2, 3, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	WX_GETSTRING(string)
	if (n == 2) {
		stbar->PushStatusText(string);
	} else {
		WX_GETNUMBER(field)
		stbar->PushStatusText(string, field);
	}
}

WX_FUNC_ARGRANGE_START(statusbar_setfieldscount, 1, 3, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	if (n == 1) {
		stbar->SetFieldsCount();
	} else if (n == 2) {
		WX_GETNUMBER(number)
		stbar->SetFieldsCount(number);
	} else {
		WX_GETNUMBER(number)
		WX_GETTABLE(d_widths)
		int d_widths_size = d_widths->Total();
		int *widths = DNEWARR(int, d_widths_size);
		DeltaValue content;
		for (int i = 0; i < d_widths_size; ++i) {
			d_widths->Get(DeltaValue((DeltaNumberValueType)i), &content);
			if (content.Type() == DeltaValue_Number)
				widths[i] = (int)content.ToNumber();
		}
		stbar->SetFieldsCount(number, widths);
		DDELARR(widths);
	}
}

WX_FUNC_START(statusbar_setminheight, 2, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	WX_GETNUMBER(height)
	stbar->SetMinHeight(height);
}

WX_FUNC_ARGRANGE_START(statusbar_setstatustext, 2, 3, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	WX_GETSTRING(text)
	if (n == 2) {
		stbar->SetStatusText(text);
	} else {
		WX_GETNUMBER(i)
		stbar->SetStatusText(text, i);
	}
}

WX_FUNC_START(statusbar_setstatuswidths, 3, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	WX_GETNUMBER(n)
	WX_GETTABLE(d_widths)
	int d_widths_size = d_widths->Total();
	int *widths = DNEWARR(int, d_widths_size);
	DeltaValue content;
	for (int i = 0; i < d_widths_size; ++i) {
		d_widths->Get(DeltaValue((DeltaNumberValueType)i), &content);
		if (content.Type() == DeltaValue_Number)
			widths[i] = (int)content.ToNumber();
	}
	stbar->SetStatusWidths(n, widths);
	DDELARR(widths);
}

WX_FUNC_START(statusbar_setstatusstyles, 3, Nil)
	DLIB_WXGET_BASE(statusbar, StatusBar, stbar)
	WX_GETNUMBER(n)
	WX_GETTABLE(d_styles)
	int d_styles_size = d_styles->Total();
	int *styles = DNEWARR(int, d_styles_size);
	DeltaValue content;
	for (int i = 0; i < d_styles_size; ++i) {
		d_styles->Get(DeltaValue((DeltaNumberValueType)i), &content);
		WX_GETDEFINE_DEFINED(styles[i])
	}
	stbar->SetStatusWidths(n, styles);
	DDELARR(styles);
}
