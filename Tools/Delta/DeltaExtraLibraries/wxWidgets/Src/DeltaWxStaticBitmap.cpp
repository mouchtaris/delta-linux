#include "DeltaWxStaticBitmap.h"
#include "DeltaWxControl.h"
#include "DeltaWxBitmap.h"
#include "DeltaWxIcon.h"
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
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(staticbitmap, name)
#define WX_FUNC(name) WX_FUNC1(staticbitmap, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(create)
WX_FUNC_DEF(getbitmap)
WX_FUNC_DEF(geticon)
WX_FUNC_DEF(setbitmap)
WX_FUNC_DEF(seticon)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(create),
	WX_FUNC(getbitmap),
	WX_FUNC(geticon),
	WX_FUNC(setbitmap),
	WX_FUNC(seticon)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "seticon")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(StaticBitmap, "staticbitmap", Control)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(StaticBitmap, val, staticbitmap) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxControl *_parent = DLIB_WXTYPECAST_BASE(Control, val, control);
	DeltaWxControl *parent = DNEWCLASS(DeltaWxControl, (_parent));
	WX_SETOBJECT_EX(*at, Control, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Control",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(StaticBitmap,staticbitmap)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(staticbitmap_construct, 0, 7, Nil)
	wxStaticBitmap *wxbitmap = (wxStaticBitmap*) 0;
	DeltaWxStaticBitmap *bitmap = (DeltaWxStaticBitmap*) 0;
	if (n == 0) {
		wxbitmap = new wxStaticBitmap();
	} else if (n >= 3) {
		DLIB_WXGET_BASE(window, Window, parent)
		WX_GETDEFINE(id)
		DLIB_WXGET_BASE(bitmap, Bitmap, label)
		DLIB_ERROR_CHECK(
			!label,
			uconstructstr(
				"invalid argument passed (%s)."
				" Expected externid of type 'bitmap'",
				DPTR(vm)->GetActualArg(2)->TypeStr()
			)
		);
		wxPoint pos = wxDefaultPosition;
		wxSize size = wxDefaultSize;
		long style = 0;
		wxString name = wxStaticBitmapNameStr;
		if (n >= 4) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
		if (n >= 5) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
		if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
		if (n >= 7) { WX_GETSTRING_DEFINED(name) }
		wxbitmap = new wxStaticBitmap(parent, id, *label, pos, size, style, name);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxbitmap) bitmap = DNEWCLASS(DeltaWxStaticBitmap, (wxbitmap));
	WX_SETOBJECT(StaticBitmap, bitmap)
}

DLIB_FUNC_START(staticbitmap_destruct, 1, Nil)
	DLIB_WXDELETE(staticbitmap, StaticBitmap, bitmap)
}

WX_FUNC_ARGRANGE_START(staticbitmap_create, 4, 8, Nil)
	DLIB_WXGET_BASE(staticbitmap, StaticBitmap, bitmap)
	DLIB_WXGET_BASE(window, Window, parent)
	WX_GETDEFINE(id)
	DLIB_WXGET_BASE(bitmap, Bitmap, label)
	DLIB_ERROR_CHECK(
		!label,
		uconstructstr(
			"invalid argument passed (%s)."
			" Expected externid of type 'bitmap'",
			DPTR(vm)->GetActualArg(3)->TypeStr()
		)
	);
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	long style = 0;
	wxString name = wxStaticBitmapNameStr;
	if (n >= 5) { DLIB_WXGETPOINT_BASE(_pos) pos = *_pos; }
	if (n >= 6) { DLIB_WXGETSIZE_BASE(_size) size = *_size; }
	if (n >= 7) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 8) { WX_GETSTRING_DEFINED(name) }
	WX_SETBOOL(bitmap->Create(parent, id, *label, pos, size, style, name))
}

DLIB_FUNC_START(staticbitmap_getbitmap, 1, Nil)
	DLIB_WXGET_BASE(staticbitmap, StaticBitmap, bitmap)
	DeltaWxBitmap *retval = DNEWCLASS(DeltaWxBitmap, (new wxBitmap(bitmap->GetBitmap())));
	WX_SETOBJECT(Bitmap, retval)
}

DLIB_FUNC_START(staticbitmap_geticon, 1, Nil)
	DLIB_WXGET_BASE(staticbitmap, StaticBitmap, bitmap)
	DeltaWxIcon *retval = DNEWCLASS(DeltaWxIcon, (new wxIcon(bitmap->GetIcon())));
	WX_SETOBJECT(Icon, retval)
}

DLIB_FUNC_START(staticbitmap_setbitmap, 2, Nil)
	DLIB_WXGET_BASE(staticbitmap, StaticBitmap, staticbitmap)
	DLIB_WXGET_BASE(bitmap, Bitmap, bitmap)
	DLIB_ERROR_CHECK(
		!bitmap,
		uconstructstr(
			"invalid argument passed (%s)."
			" Expected externid of type 'bitmap'",
			DPTR(vm)->GetActualArg(1)->TypeStr()
		)
	);
	staticbitmap->SetBitmap(*bitmap);
}

DLIB_FUNC_START(staticbitmap_seticon, 2, Nil)
	DLIB_WXGET_BASE(staticbitmap, StaticBitmap, bitmap)
	DLIB_WXGET_BASE(icon, Icon, icon)
	DLIB_ERROR_CHECK(
		!icon,
		uconstructstr(
			"invalid argument passed (%s)."
			" Expected externid of type 'icon'",
			DPTR(vm)->GetActualArg(1)->TypeStr()
		)
	);
	bitmap->SetIcon(*icon);
}
