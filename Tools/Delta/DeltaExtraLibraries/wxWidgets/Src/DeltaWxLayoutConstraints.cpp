#include "DeltaWxLayoutConstraints.h"
#include "DeltaWxObject.h"
#include "DeltaWxIndividualLayoutConstraint.h"
#include "DeltaValue.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(layoutconstraints, name)
#define WX_FUNC(name) WX_FUNC1(layoutconstraints, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getbottom)
WX_FUNC_DEF(getcentrex)
WX_FUNC_DEF(getcentrey)
WX_FUNC_DEF(getheight)
WX_FUNC_DEF(getleft)
WX_FUNC_DEF(getright)
WX_FUNC_DEF(gettop)
WX_FUNC_DEF(getwidth)
WX_FUNC_DEF(setbottom)
WX_FUNC_DEF(setcentrex)
WX_FUNC_DEF(setcentrey)
WX_FUNC_DEF(setheight)
WX_FUNC_DEF(setleft)
WX_FUNC_DEF(setright)
WX_FUNC_DEF(settop)
WX_FUNC_DEF(setwidth)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getbottom),
	WX_FUNC(getcentrex),
	WX_FUNC(getcentrey),
	WX_FUNC(getheight),
	WX_FUNC(getleft),
	WX_FUNC(getright),
	WX_FUNC(gettop),
	WX_FUNC(getwidth),
	WX_FUNC(setbottom),
	WX_FUNC(setcentrex),
	WX_FUNC(setcentrey),
	WX_FUNC(setheight),
	WX_FUNC(setleft),
	WX_FUNC(setright),
	WX_FUNC(settop),
	WX_FUNC(setwidth)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setwidth")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(LayoutConstraints, "layoutconstraints", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(LayoutConstraints, val, layoutconstraints) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetLeft (void* val, DeltaValue* at) 
{
	wxLayoutConstraints *constr = DLIB_WXTYPECAST_BASE(LayoutConstraints, val, layoutconstraints);
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(constr->left)));
	WX_SETOBJECT_EX(*at, IndividualLayoutConstraint, retval)
	return true;
}

static bool GetTop (void* val, DeltaValue* at) 
{
	wxLayoutConstraints *constr = DLIB_WXTYPECAST_BASE(LayoutConstraints, val, layoutconstraints);
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(constr->top)));
	WX_SETOBJECT_EX(*at, IndividualLayoutConstraint, retval)
	return true;
}

static bool GetRight (void* val, DeltaValue* at) 
{
	wxLayoutConstraints *constr = DLIB_WXTYPECAST_BASE(LayoutConstraints, val, layoutconstraints);
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(constr->right)));
	WX_SETOBJECT_EX(*at, IndividualLayoutConstraint, retval)
	return true;
}

static bool GetBottom (void* val, DeltaValue* at) 
{
	wxLayoutConstraints *constr = DLIB_WXTYPECAST_BASE(LayoutConstraints, val, layoutconstraints);
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(constr->bottom)));
	WX_SETOBJECT_EX(*at, IndividualLayoutConstraint, retval)
	return true;
}

static bool GetWidth (void* val, DeltaValue* at) 
{
	wxLayoutConstraints *constr = DLIB_WXTYPECAST_BASE(LayoutConstraints, val, layoutconstraints);
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(constr->width)));
	WX_SETOBJECT_EX(*at, IndividualLayoutConstraint, retval)
	return true;
}

static bool GetHeight (void* val, DeltaValue* at) 
{
	wxLayoutConstraints *constr = DLIB_WXTYPECAST_BASE(LayoutConstraints, val, layoutconstraints);
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(constr->height)));
	WX_SETOBJECT_EX(*at, IndividualLayoutConstraint, retval)
	return true;
}

static bool GetCentreX (void* val, DeltaValue* at) 
{
	wxLayoutConstraints *constr = DLIB_WXTYPECAST_BASE(LayoutConstraints, val, layoutconstraints);
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(constr->centreX)));
	WX_SETOBJECT_EX(*at, IndividualLayoutConstraint, retval)
	return true;
}

static bool GetCentreY (void* val, DeltaValue* at) 
{
	wxLayoutConstraints *constr = DLIB_WXTYPECAST_BASE(LayoutConstraints, val, layoutconstraints);
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(constr->centreY)));
	WX_SETOBJECT_EX(*at, IndividualLayoutConstraint, retval)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "left",				&GetLeft,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "top",				&GetTop,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "right",				&GetRight,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "bottom",				&GetBottom,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "width",				&GetWidth,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "height",				&GetHeight,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "centreX",			&GetCentreX,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "centreY",			&GetCentreY,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(LayoutConstraints,layoutconstraints)

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(layoutconstraints_construct, 0, Nil)
	DeltaWxLayoutConstraints *retval = DNEWCLASS(DeltaWxLayoutConstraints, (new wxLayoutConstraints()));
	WX_SETOBJECT(LayoutConstraints, retval)
}

DLIB_FUNC_START(layoutconstraints_destruct, 1, Nil)
	DLIB_WXDELETE(layoutconstraints, LayoutConstraints, layoutconstr)
}

DLIB_FUNC_START(layoutconstraints_getbottom, 1, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(layoutconstr->bottom)));
	WX_SETOBJECT(IndividualLayoutConstraint, retval)
}

DLIB_FUNC_START(layoutconstraints_getcentrex, 1, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(layoutconstr->centreX)));
	WX_SETOBJECT(IndividualLayoutConstraint, retval)
}

DLIB_FUNC_START(layoutconstraints_getcentrey, 1, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(layoutconstr->centreY)));
	WX_SETOBJECT(IndividualLayoutConstraint, retval)
}

DLIB_FUNC_START(layoutconstraints_getheight, 1, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(layoutconstr->height)));
	WX_SETOBJECT(IndividualLayoutConstraint, retval)
}

DLIB_FUNC_START(layoutconstraints_getleft, 1, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(layoutconstr->left)));
	WX_SETOBJECT(IndividualLayoutConstraint, retval)
}

DLIB_FUNC_START(layoutconstraints_getright, 1, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(layoutconstr->right)));
	WX_SETOBJECT(IndividualLayoutConstraint, retval)
}

DLIB_FUNC_START(layoutconstraints_gettop, 1, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(layoutconstr->top)));
	WX_SETOBJECT(IndividualLayoutConstraint, retval)
}

DLIB_FUNC_START(layoutconstraints_getwidth, 1, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint(layoutconstr->width)));
	WX_SETOBJECT(IndividualLayoutConstraint, retval)
}

DLIB_FUNC_START(layoutconstraints_setbottom, 2, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indconstr)
	layoutconstr->bottom = *indconstr;
}

DLIB_FUNC_START(layoutconstraints_setcentrex, 2, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indconstr)
	layoutconstr->centreX = *indconstr;
}

DLIB_FUNC_START(layoutconstraints_setcentrey, 2, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indconstr)
	layoutconstr->centreY = *indconstr;
}

DLIB_FUNC_START(layoutconstraints_setheight, 2, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indconstr)
	layoutconstr->height = *indconstr;
}

DLIB_FUNC_START(layoutconstraints_setleft, 2, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indconstr)
	layoutconstr->left = *indconstr;
}

DLIB_FUNC_START(layoutconstraints_setright, 2, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indconstr)
	layoutconstr->right = *indconstr;
}

DLIB_FUNC_START(layoutconstraints_settop, 2, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indconstr)
	layoutconstr->top = *indconstr;
}

DLIB_FUNC_START(layoutconstraints_setwidth, 2, Nil)
	DLIB_WXGET_BASE(layoutconstraints, LayoutConstraints, layoutconstr)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indconstr)
	layoutconstr->width = *indconstr;
}
