#include "DeltaWxIndividualLayoutConstraint.h"
#include "DeltaWxObject.h"
#include "DeltaWxWindow.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(individuallayoutconstraint, name)
#define WX_FUNC(name) WX_FUNC1(individuallayoutconstraint, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(above)
WX_FUNC_DEF(absolute)
WX_FUNC_DEF(asis)
WX_FUNC_DEF(below)
WX_FUNC_DEF(unconstrained)
WX_FUNC_DEF(leftof)
WX_FUNC_DEF(percentof)
WX_FUNC_DEF(rightof)
WX_FUNC_DEF(sameas)
WX_FUNC_DEF(set)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(above),
	WX_FUNC(absolute),
	WX_FUNC(asis),
	WX_FUNC(below),
	WX_FUNC(unconstrained),
	WX_FUNC(leftof),
	WX_FUNC(percentof),
	WX_FUNC(rightof),
	WX_FUNC(sameas),
	WX_FUNC(set)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "set")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(IndividualLayoutConstraint, "individuallayoutconstraint", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(IndividualLayoutConstraint, val, individuallayoutconstraint) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static bool GetOtherWindow (void* val, DeltaValue* at) 
{
	wxIndividualLayoutConstraint *constr =
		DLIB_WXTYPECAST_BASE(IndividualLayoutConstraint, val, individuallayoutconstraint);
	wxWindow *window = (wxWindow*)constr->GetOtherWindow();
	DeltaWxWindow *retval = window ? DNEWCLASS(DeltaWxWindow, (window)) : (DeltaWxWindow*) 0;
	WX_SETOBJECT_EX(*at, Window, retval)
	return true;
}

static bool GetMyEdge (void* val, DeltaValue* at) 
{
	wxIndividualLayoutConstraint *constr =
		DLIB_WXTYPECAST_BASE(IndividualLayoutConstraint, val, individuallayoutconstraint);
	WX_SETNUMBER_EX(*at, constr->GetMyEdge())
	return true;
}

static bool GetRelationship (void* val, DeltaValue* at) 
{
	wxIndividualLayoutConstraint *constr =
		DLIB_WXTYPECAST_BASE(IndividualLayoutConstraint, val, individuallayoutconstraint);
	WX_SETNUMBER_EX(*at, constr->GetRelationship())
	return true;
}

static bool GetMargin (void* val, DeltaValue* at) 
{
	wxIndividualLayoutConstraint *constr =
		DLIB_WXTYPECAST_BASE(IndividualLayoutConstraint, val, individuallayoutconstraint);
	WX_SETNUMBER_EX(*at, constr->GetMargin())
	return true;
}

static bool GetValue (void* val, DeltaValue* at) 
{
	wxIndividualLayoutConstraint *constr =
		DLIB_WXTYPECAST_BASE(IndividualLayoutConstraint, val, individuallayoutconstraint);
	WX_SETNUMBER_EX(*at, constr->GetValue())
	return true;
}

static bool GetPercent (void* val, DeltaValue* at) 
{
	wxIndividualLayoutConstraint *constr =
		DLIB_WXTYPECAST_BASE(IndividualLayoutConstraint, val, individuallayoutconstraint);
	WX_SETNUMBER_EX(*at, constr->GetPercent())
	return true;
}

static bool GetOtherEdge (void* val, DeltaValue* at) 
{
	wxIndividualLayoutConstraint *constr =
		DLIB_WXTYPECAST_BASE(IndividualLayoutConstraint, val, individuallayoutconstraint);
	WX_SETNUMBER_EX(*at, constr->GetOtherEdge())
	return true;
}

static bool GetDone (void* val, DeltaValue* at) 
{
	wxIndividualLayoutConstraint *constr =
		DLIB_WXTYPECAST_BASE(IndividualLayoutConstraint, val, individuallayoutconstraint);
	WX_SETBOOL_EX(*at, constr->GetDone())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "otherWindow",		&GetOtherWindow,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "myEdge",				&GetMyEdge,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "relationship",		&GetRelationship,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "margin",				&GetMargin,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "value",				&GetValue,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "percent",			&GetPercent,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "otherEdge",			&GetOtherEdge,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "done",				&GetDone,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(IndividualLayoutConstraint,individuallayoutconstraint)

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(individuallayoutconstraint_construct, 0, Nil)
	DeltaWxIndividualLayoutConstraint *retval = DNEWCLASS(DeltaWxIndividualLayoutConstraint,
		(new wxIndividualLayoutConstraint()));
	WX_SETOBJECT(IndividualLayoutConstraint, retval)
}

DLIB_FUNC_START(individuallayoutconstraint_destruct, 1, Nil)
	DLIB_WXDELETE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
}

WX_FUNC_ARGRANGE_START(individuallayoutconstraint_above, 2, 3, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	DLIB_WXGET_BASE(window, Window, otherWin)
	if (n == 2) {
		indlayconstr->Above(otherWin);
	} else {
		WX_GETNUMBER(margin)
		indlayconstr->Above(otherWin, margin);
	}
}

DLIB_FUNC_START(individuallayoutconstraint_absolute, 2, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	WX_GETNUMBER(value)
	indlayconstr->Absolute(value);
}

DLIB_FUNC_START(individuallayoutconstraint_asis, 1, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	indlayconstr->AsIs();
}

WX_FUNC_ARGRANGE_START(individuallayoutconstraint_below, 2, 3, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	DLIB_WXGET_BASE(window, Window, otherWin)
	if (n == 2) {
		indlayconstr->Below(otherWin);
	} else {
		WX_GETNUMBER(margin)
		indlayconstr->Below(otherWin, margin);
	}
}

DLIB_FUNC_START(individuallayoutconstraint_unconstrained, 1, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	indlayconstr->Unconstrained();
}

WX_FUNC_ARGRANGE_START(individuallayoutconstraint_leftof, 2, 3, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	DLIB_WXGET_BASE(window, Window, otherWin)
	if (n == 2) {
		indlayconstr->LeftOf(otherWin);
	} else {
		WX_GETNUMBER(margin)
		indlayconstr->LeftOf(otherWin, margin);
	}
}

DLIB_FUNC_START(individuallayoutconstraint_percentof, 4, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	DLIB_WXGET_BASE(window, Window, otherWin)
	WX_GETDEFINE(edge)
	WX_GETNUMBER(per)
	indlayconstr->PercentOf(otherWin, (wxEdge)edge, per);
}

WX_FUNC_ARGRANGE_START(individuallayoutconstraint_rightof, 2, 3, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	DLIB_WXGET_BASE(window, Window, otherWin)
	if (n == 2) {
		indlayconstr->RightOf(otherWin);
	} else {
		WX_GETNUMBER(margin)
		indlayconstr->RightOf(otherWin, margin);
	}
}

WX_FUNC_ARGRANGE_START(individuallayoutconstraint_sameas, 3, 4, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	DLIB_WXGET_BASE(window, Window, otherWin)
	WX_GETDEFINE(edge)
	if (n == 3) {
		indlayconstr->SameAs(otherWin, (wxEdge)edge);
	} else {
		WX_GETNUMBER(margin)
		indlayconstr->SameAs(otherWin, (wxEdge)edge, margin);
	}
}

WX_FUNC_ARGRANGE_START(individuallayoutconstraint_set, 4, 6, Nil)
	DLIB_WXGET_BASE(individuallayoutconstraint, IndividualLayoutConstraint, indlayconstr)
	WX_GETDEFINE(rel)
	DLIB_WXGET_BASE(window, Window, otherWin)
	WX_GETDEFINE(otherEdge)
	if (n == 4) {
		indlayconstr->Set((wxRelationship)rel, otherWin, (wxEdge)otherEdge);
	} else if (n == 5) {
		WX_GETNUMBER(value)
		indlayconstr->Set((wxRelationship)rel, otherWin, (wxEdge)otherEdge, value);
	} else {
		WX_GETNUMBER(value)
		WX_GETNUMBER(margin)
		indlayconstr->Set((wxRelationship)rel, otherWin, (wxEdge)otherEdge, value, margin);
	}
}
