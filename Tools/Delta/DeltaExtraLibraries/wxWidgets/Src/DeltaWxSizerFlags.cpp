#include "DeltaWxSizerFlags.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(sizerflags, name)
#define WX_FUNC(name) WX_FUNC1(sizerflags, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(align)
WX_FUNC_DEF(border)
WX_FUNC_DEF(center)
WX_FUNC_DEF(centre)
WX_FUNC_DEF(doubleborder)
WX_FUNC_DEF(doublehorzborder)
WX_FUNC_DEF(expand)
WX_FUNC_DEF(getdefaultborder)
WX_FUNC_DEF(left)
WX_FUNC_DEF(fixedminsize)
WX_FUNC_DEF(proportion)
WX_FUNC_DEF(right)
WX_FUNC_DEF(shaped)
WX_FUNC_DEF(tripleborder)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getdefaultborder),
	WX_FUNC(align),
	WX_FUNC(border),
	WX_FUNC(center),
	WX_FUNC(centre),
	WX_FUNC(doubleborder),
	WX_FUNC(doublehorzborder),
	WX_FUNC(expand),
	WX_FUNC(left),
	WX_FUNC(fixedminsize),
	WX_FUNC(proportion),
	WX_FUNC(right),
	WX_FUNC(shaped),
	WX_FUNC(tripleborder)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(2, uarraysize(funcs) - 2, "align", "tripleborder")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(SizerFlags, "sizerflags")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(SizerFlags, val, sizerflags) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetProportion (void* val, DeltaValue* at) 
{
	wxSizerFlags *flags = DLIB_WXTYPECAST_BASE(SizerFlags, val, sizerflags);
	WX_SETNUMBER_EX(*at, flags->GetProportion())
	return true;
}

static bool GetFlags (void* val, DeltaValue* at) 
{
	wxSizerFlags *flags = DLIB_WXTYPECAST_BASE(SizerFlags, val, sizerflags);
	WX_SETNUMBER_EX(*at, flags->GetFlags())
	return true;
}

static bool GetBorderInPixels (void* val, DeltaValue* at) 
{
	wxSizerFlags *flags = DLIB_WXTYPECAST_BASE(SizerFlags, val, sizerflags);
	WX_SETNUMBER_EX(*at, flags->GetBorderInPixels())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "proportion",			&GetProportion,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "flags",				&GetFlags,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "borderInPixels",		&GetBorderInPixels,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(SizerFlags,sizerflags)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(sizerflags_construct, 0, 1, Nil)
	wxSizerFlags *flags = (wxSizerFlags*) 0;
	if (n == 0)
		flags = new wxSizerFlags();
	else {
		WX_GETNUMBER(proportion)
		flags = new wxSizerFlags(proportion);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, flags)
}

WX_FUNC_START(sizerflags_align, 2, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_GETDEFINE(align)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->Align(align)))
}

WX_FUNC_ARGRANGE_START(sizerflags_border, 1, 3, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	int direction = wxALL;
	int borderInPixels = wxSizerFlags::GetDefaultBorder();
	if (n >= 2) { WX_GETDEFINE_DEFINED(direction) }
	if (n >= 3) { WX_GETNUMBER_DEFINED(borderInPixels) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->Border(direction, borderInPixels)))
}

WX_FUNC_START(sizerflags_center, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->Center()))
}

WX_FUNC_START(sizerflags_centre, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->Center()))
}

WX_FUNC_ARGRANGE_START(sizerflags_doubleborder, 1, 2, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	int direction = wxALL;
	if (n >= 2) { WX_GETDEFINE_DEFINED(direction) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->DoubleBorder(direction)))
}

WX_FUNC_START(sizerflags_doublehorzborder, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->DoubleHorzBorder()))
}

WX_FUNC_START(sizerflags_expand, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->Expand()))
}

WX_FUNC_START(sizerflags_getdefaultborder, 0, Nil)
	WX_SETNUMBER(wxSizerFlags::GetDefaultBorder())
}

WX_FUNC_START(sizerflags_left, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->Left()))
}

WX_FUNC_START(sizerflags_fixedminsize, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->FixedMinSize()))
}

WX_FUNC_START(sizerflags_proportion, 2, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_GETNUMBER(proportion)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->Proportion(proportion)))
}

WX_FUNC_START(sizerflags_right, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->Right()))
}

WX_FUNC_START(sizerflags_shaped, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->Shaped()))
}

WX_FUNC_ARGRANGE_START(sizerflags_tripleborder, 1, 2, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, sizerflags)
	int direction = wxALL;
	if (n >= 2) { WX_GETDEFINE_DEFINED(direction) }
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(SizerFlags, new wxSizerFlags(sizerflags->TripleBorder(direction)))
}
