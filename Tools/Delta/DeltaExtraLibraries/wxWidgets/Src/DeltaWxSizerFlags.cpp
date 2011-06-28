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
WX_FUNC_DEF(destruct)
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
	WX_FUNC(destruct),
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

DELTALIBFUNC_DECLARECONSTS(2, uarraysize(funcs) - 2, "destruct", "tripleborder")

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

WX_LIBRARY_FUNCS_IMPLEMENTATION(SizerFlags, sizerflags);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(sizerflags_construct, 0, 1, Nil)
	wxSizerFlags *wxflags = (wxSizerFlags*) 0;
	DeltaWxSizerFlags *flags = (DeltaWxSizerFlags*) 0;
	if (n == 0)
		wxflags = new wxSizerFlags();
	else {
		WX_GETNUMBER(proportion)
		wxflags = new wxSizerFlags(proportion);
	}
	if (wxflags) flags = DNEWCLASS(DeltaWxSizerFlags, (wxflags));
	WX_SETOBJECT(SizerFlags, flags)
}

DLIB_FUNC_START(sizerflags_destruct, 1, Nil)
	DLIB_WXDELETE(sizerflags, SizerFlags, flags)
}

DLIB_FUNC_START(sizerflags_align, 2, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	WX_GETDEFINE(align)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Align(align))));
	WX_SETOBJECT(SizerFlags, retval)
}

WX_FUNC_ARGRANGE_START(sizerflags_border, 1, 3, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = (DeltaWxSizerFlags*) 0;
	if (n == 1)
		retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Border())));
	else if (n == 2) {
		WX_GETDEFINE(direction)
		retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Border(direction))));
	} else {
		WX_GETDEFINE(direction)
		WX_GETNUMBER(borderinpixels)
		retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Border(direction, borderinpixels))));
	}
	WX_SETOBJECT(SizerFlags, retval)
}

DLIB_FUNC_START(sizerflags_center, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Center())));
	WX_SETOBJECT(SizerFlags, retval)
}

DLIB_FUNC_START(sizerflags_centre, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Center())));
	WX_SETOBJECT(SizerFlags, retval)
}

WX_FUNC_ARGRANGE_START(sizerflags_doubleborder, 1, 2, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = (DeltaWxSizerFlags*) 0;
	if (n == 1)
		retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->DoubleBorder())));
	else {
		WX_GETDEFINE(direction)
		retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->DoubleBorder(direction))));
	}
	WX_SETOBJECT(SizerFlags, retval)
}

DLIB_FUNC_START(sizerflags_doublehorzborder, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->DoubleHorzBorder())));
	WX_SETOBJECT(SizerFlags, retval)
}

DLIB_FUNC_START(sizerflags_expand, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Expand())));
	WX_SETOBJECT(SizerFlags, retval)
}

DLIB_FUNC_START(sizerflags_getdefaultborder, 0, Nil)
	WX_SETNUMBER(wxSizerFlags::GetDefaultBorder())
}

DLIB_FUNC_START(sizerflags_left, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Left())));
	WX_SETOBJECT(SizerFlags, retval)
}

DLIB_FUNC_START(sizerflags_fixedminsize, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->FixedMinSize())));
	WX_SETOBJECT(SizerFlags, retval)
}

DLIB_FUNC_START(sizerflags_proportion, 2, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	WX_GETNUMBER(proportion)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Proportion(proportion))));
	WX_SETOBJECT(SizerFlags, retval)
}

DLIB_FUNC_START(sizerflags_right, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Right())));
	WX_SETOBJECT(SizerFlags, retval)
}

DLIB_FUNC_START(sizerflags_shaped, 1, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->Shaped())));
	WX_SETOBJECT(SizerFlags, retval)
}

WX_FUNC_ARGRANGE_START(sizerflags_tripleborder, 1, 2, Nil)
	DLIB_WXGET_BASE(sizerflags, SizerFlags, flags)
	DeltaWxSizerFlags *retval = (DeltaWxSizerFlags*) 0;
	if (n == 1)
		retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->TripleBorder())));
	else {
		WX_GETDEFINE(direction)
		retval = DNEWCLASS(DeltaWxSizerFlags, (new wxSizerFlags(flags->TripleBorder(direction))));
	}
	WX_SETOBJECT(SizerFlags, retval)
}
