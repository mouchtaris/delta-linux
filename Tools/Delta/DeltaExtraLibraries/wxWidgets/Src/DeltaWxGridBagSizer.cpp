#include "DeltaWxGridBagSizer.h"
#include "DeltaWxFlexGridSizer.h"
#include "DeltaWxSizerItem.h"
#include "DeltaWxWindow.h"
#include "DeltaWxSizer.h"
#include "DeltaWxSize.h"
#include "DeltaWxPoint.h"
#include "DeltaWxGBPosition.h"
#include "DeltaWxGBSpan.h"
#include "DeltaWxGBSizerItem.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(gridbagsizer, name)
#define WX_FUNC(name) WX_FUNC1(gridbagsizer, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(add)
WX_FUNC_DEF(calcmin)
WX_FUNC_DEF(checkforintersection)
WX_FUNC_DEF(finditem)
WX_FUNC_DEF(finditematpoint)
WX_FUNC_DEF(finditematposition)
WX_FUNC_DEF(getcellsize)
WX_FUNC_DEF(getemptycellsize)
WX_FUNC_DEF(getitemposition)
WX_FUNC_DEF(getitemspan)
WX_FUNC_DEF(recalcsizes)
WX_FUNC_DEF(setemptycellsize)
WX_FUNC_DEF(setitemposition)
WX_FUNC_DEF(setitemspan)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(add),
	WX_FUNC(calcmin),
	WX_FUNC(checkforintersection),
	WX_FUNC(finditem),
	WX_FUNC(finditematpoint),
	WX_FUNC(finditematposition),
	WX_FUNC(getcellsize),
	WX_FUNC(getemptycellsize),
	WX_FUNC(getitemposition),
	WX_FUNC(getitemspan),
	WX_FUNC(recalcsizes),
	WX_FUNC(setemptycellsize),
	WX_FUNC(setitemposition),
	WX_FUNC(setitemspan)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setitemspan")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(GridBagSizer, "gridbagsizer", FlexGridSizer)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(GridBagSizer, val, gridbagsizer) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, FlexGridSizer, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "FlexGridSizer",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(GridBagSizer,gridbagsizer)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(gridbagsizer_construct, 0, 2, Nil)
	wxGridBagSizer *sizer = (wxGridBagSizer*) 0;
	int vgap = 0;
	int hgap = 0;
	if (n >= 1)	{ WX_GETNUMBER_DEFINED(vgap) }
	if (n >= 2)	{ WX_GETNUMBER_DEFINED(hgap) }
	sizer = new wxGridBagSizer(vgap, hgap);
	WX_SETOBJECT(GridBagSizer, sizer)
}

WX_FUNC_START(gridbagsizer_destruct, 1, Nil)
	DLIB_WXDELETE(gridbagsizer, GridBagSizer, sizer)
}

WX_FUNC_ARGRANGE_START(gridbagsizer_add, 2, 7, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	wxSizerItem *retval = (wxSizerItem*) 0;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			if (n == 2) {
				DPTR(vm)->PrimaryError(
					"Wrong number of args (%d passed) to '%s'",
					DPTR(vm)->TotalActualArgs(),
					CURR_FUNC
				);
				RESET_EMPTY
			} else {
				DLIB_WXGET_BASE(gbposition, GBPosition, pos)
				wxGBSpan span	= wxDefaultSpan;
				int flag		= 0;
				int border		= 0;
				if (n >= 4) { DLIB_WXGET_BASE(gbspan, GBSpan, _span) span = *_span; }
				if (n >= 5) { WX_GETDEFINE_DEFINED(flag) }
				if (n >= 6) { WX_GETNUMBER_DEFINED(border) }
				retval	= sizer->Add(window, *pos, span, flag, border);
			}
		} else
		if (DLIB_WXISBASE(Sizer, serial_no, sizer, szr)) {
			if (n == 2) {
				DPTR(vm)->PrimaryError(
					"Wrong number of args (%d passed) to '%s'",
					DPTR(vm)->TotalActualArgs(),
					CURR_FUNC
				);
				RESET_EMPTY
			} else {
				DLIB_WXGET_BASE(gbposition, GBPosition, pos)
				wxGBSpan span	= wxDefaultSpan;
				int flag		= 0;
				int border		= 0;
				if (n >= 4) { DLIB_WXGET_BASE(gbspan, GBSpan, _span) span = *_span; }
				if (n >= 5) { WX_GETDEFINE_DEFINED(flag) }
				if (n >= 6) { WX_GETNUMBER_DEFINED(border) }
				retval	= sizer->Add(szr, *pos, span, flag, border);
			}
		} else
		if (DLIB_WXISBASE(GBSizerItem, serial_no, gbsizeritem, szrItem)) {
			retval	= (sizer->Add(szrItem));;
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		if (n <= 3) {
			DPTR(vm)->PrimaryError(
				"Wrong number of args (%d passed) to '%s'",
				DPTR(vm)->TotalActualArgs(),
				CURR_FUNC
			);
			RESET_EMPTY
		} else {
			WX_GETNUMBER(width)
			WX_GETNUMBER(height)
			DLIB_WXGET_BASE(gbposition, GBPosition, pos)
			wxGBSpan span	= wxDefaultSpan;
			int flag		= 0;
			int border		= 0;
			if (n >= 5) { DLIB_WXGET_BASE(gbspan, GBSpan, _span) span = *_span; }
			if (n >= 6) { WX_GETDEFINE_DEFINED(flag) }
			if (n >= 7) { WX_GETNUMBER_DEFINED(border) }
			retval	= sizer->Add(width, height, *pos, span, flag, border);
		}
	}
	WX_SETOBJECT(SizerItem, retval)
}

WX_FUNC_START(gridbagsizer_calcmin, 1, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(sizer->CalcMin()))
}

WX_FUNC_ARGRANGE_START(gridbagsizer_checkforintersection, 2, 4, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	DLIB_ERROR_CHECK(
		DPTR(vm)->GetActualArg(_argNo)->Type() != DeltaValue_ExternId &&
		DPTR(vm)->GetActualArg(_argNo)->Type() != DeltaValue_Nil,
		uconstructstr(
			"invalid argument passed (%s). Expected externid",
			DPTR(vm)->GetActualArg(_argNo)->TypeStr()
		)
	)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(GBSizerItem, serial_no, gbsizeritem, sizeritem)) {
			wxGBSizerItem *excludeitem = NULL;
			if (n >= 3) {
				DLIB_WXGET_BASE(gbsizeritem, GBSizerItem, _excludeitem)
				excludeitem = _excludeitem;
			}
			WX_SETBOOL(sizer->CheckForIntersection(sizeritem, excludeitem))
		} else
		if (DLIB_WXISBASE(GBPosition, serial_no, gbposition, pos)) {
			if (n <= 2) {
				DPTR(vm)->PrimaryError(
					"Wrong number of args (%d passed) to '%s'",
					DPTR(vm)->TotalActualArgs(),
					CURR_FUNC
				);
				RESET_EMPTY
			} else {
				DLIB_WXGET_BASE(gbspan, GBSpan, span)
				wxGBSizerItem *excludeitem = NULL;
				if (n >= 4) {
					DLIB_WXGET_BASE(gbsizeritem, GBSizerItem, _excludeitem)
					excludeitem = _excludeitem;
				}
				WX_SETBOOL(sizer->CheckForIntersection(*pos, *span, excludeitem))
			}
		}
	}
}

WX_FUNC_START(gridbagsizer_finditem, 2, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	wxGBSizerItem *retval;
	DLIB_ERROR_CHECK(
		DPTR(vm)->GetActualArg(_argNo)->Type() != DeltaValue_ExternId &&
		DPTR(vm)->GetActualArg(_argNo)->Type() != DeltaValue_Nil,
		uconstructstr(
			"invalid argument passed (%s). Expected externid",
			DPTR(vm)->GetActualArg(_argNo)->TypeStr()
		)
	)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			retval	= sizer->FindItem(window);
		} else
		if (DLIB_WXISBASE(Sizer, serial_no, sizer, other)) {
			retval	= sizer->FindItem(other);
		}
	}
	WX_SETOBJECT(GBSizerItem, retval)
}

WX_FUNC_START(gridbagsizer_finditematpoint, 2, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	DLIB_WXGETPOINT_BASE(pt)
	WX_SETOBJECT(GBSizerItem, sizer->FindItemAtPoint(*pt))
}

WX_FUNC_START(gridbagsizer_finditematposition, 2, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	DLIB_WXGET_BASE(gbposition, GBPosition, pos)
	WX_SETOBJECT(GBSizerItem, sizer->FindItemAtPosition(*pos))
}

WX_FUNC_START(gridbagsizer_getcellsize, 3, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	WX_GETNUMBER(row)
	WX_GETNUMBER(column)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(sizer->GetCellSize(row, column)))
}

WX_FUNC_START(gridbagsizer_getemptycellsize, 1, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Size, new wxSize(sizer->GetEmptyCellSize()))
}

WX_FUNC_START(gridbagsizer_getitemposition, 2, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	wxGBPosition *retval;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			wxGBPosition _retval = sizer->GetItemPosition(window);
			retval = new wxGBPosition(_retval.GetRow(), _retval.GetCol());
		} else
		if (DLIB_WXISBASE(Sizer, serial_no, sizer, other)) {
			wxGBPosition _retval = sizer->GetItemPosition(other);
			retval = new wxGBPosition(_retval.GetRow(), _retval.GetCol());
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		wxGBPosition _retval = sizer->GetItemPosition(index);
		retval = new wxGBPosition(_retval.GetRow(), _retval.GetCol());
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(GBPosition, retval)
}

WX_FUNC_START(gridbagsizer_getitemspan, 2, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	wxGBSpan *retval;
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			wxGBSpan _retval = sizer->GetItemSpan(window);
			retval = new wxGBSpan(_retval.GetRowspan(), _retval.GetColspan());
		} else
		if (DLIB_WXISBASE(Sizer, serial_no, sizer, other)) {
			wxGBSpan _retval = sizer->GetItemSpan(other);
			retval = new wxGBSpan(_retval.GetRowspan(), _retval.GetColspan());
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		wxGBSpan _retval = sizer->GetItemSpan(index);
		retval = new wxGBSpan(_retval.GetRowspan(), _retval.GetColspan());
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(GBSpan, retval)
}

WX_FUNC_START(gridbagsizer_recalcsizes, 1, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	sizer->RecalcSizes();
}

WX_FUNC_START(gridbagsizer_setemptycellsize, 2, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	DLIB_WXGETSIZE_BASE(size)
	sizer->SetEmptyCellSize(*size);
}

WX_FUNC_START(gridbagsizer_setitemposition, 3, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			DLIB_WXGET_BASE(gbposition, GBPosition, pos)
			WX_SETBOOL(sizer->SetItemPosition(window, *pos))
		} else
		if (DLIB_WXISBASE(Sizer, serial_no, sizer, other)) {
			DLIB_WXGET_BASE(gbposition, GBPosition, pos)
			WX_SETBOOL(sizer->SetItemPosition(other, *pos))
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		DLIB_WXGET_BASE(gbposition, GBPosition, pos)
		WX_SETBOOL(sizer->SetItemPosition(index, *pos))
	}
}

WX_FUNC_START(gridbagsizer_setitemspan, 3, Nil)
	DLIB_WXGET_BASE(gridbagsizer, GridBagSizer, sizer)
	if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
		util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
		if (DLIB_WXISBASE(Window, serial_no, window, window)) {
			DLIB_WXGET_BASE(gbspan, GBSpan, span)
			WX_SETBOOL(sizer->SetItemSpan(window, *span))
		} else
		if (DLIB_WXISBASE(Sizer, serial_no, sizer, other)) {
			DLIB_WXGET_BASE(gbspan, GBSpan, span)
			WX_SETBOOL(sizer->SetItemSpan(other, *span))
		}
	} else if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_Number) {
		WX_GETNUMBER(index)
		DLIB_WXGET_BASE(gbspan, GBSpan, span)
		WX_SETBOOL(sizer->SetItemSpan(index, *span))
	}
}
