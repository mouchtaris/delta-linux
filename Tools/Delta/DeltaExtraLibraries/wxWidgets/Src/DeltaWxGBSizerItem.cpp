#include "DeltaWxGBSizerItem.h"
#include "DeltaWxSizer.h"
#include "DeltaWxWindow.h"
#include "DeltaWxGBPosition.h"
#include "DeltaWxGBSpan.h"
#include "DeltaWxSize.h"
#include "DeltaWxPoint.h"
#include "DeltaWxRect.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(gbsizeritem, name)
#define WX_FUNC(name) WX_FUNC1(gbsizeritem, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getendpos)
WX_FUNC_DEF(getpos)
WX_FUNC_DEF(getspan)
WX_FUNC_DEF(intersects)
WX_FUNC_DEF(setpos)
WX_FUNC_DEF(setspan)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getendpos),
	WX_FUNC(getpos),
	WX_FUNC(getspan),
	WX_FUNC(intersects),
	WX_FUNC(setpos),
	WX_FUNC(setspan)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setspan")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(GBSizerItem, "gbsizeritem", SizerItem)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(GBSizerItem, val, gbsizeritem) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, SizerItem, val)
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxGBSizerItem *item = DLIB_WXTYPECAST_BASE(GBSizerItem, val, gbsizeritem);
	int row, col;
	item->GetPos(row, col);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, GBPosition, new wxGBPosition(row, col))
	return true;
}

static bool GetEndPosition (void* val, DeltaValue* at) 
{
	wxGBSizerItem *item = DLIB_WXTYPECAST_BASE(GBSizerItem, val, gbsizeritem);
	int row, col;
	item->GetEndPos(row, col);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, GBPosition, new wxGBPosition(row, col))
	return true;
}

static bool GetSpan (void* val, DeltaValue* at) 
{
	wxGBSizerItem *item = DLIB_WXTYPECAST_BASE(GBSizerItem, val, gbsizeritem);
	int rowSpan, colSpan;
	item->GetSpan(rowSpan, colSpan);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, GBSpan, new wxGBSpan(rowSpan, colSpan))
	return true;
}

////////////////////////////////////////////////////////////////

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "SizerItem",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "position",			&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "endPosition",		&GetEndPosition,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "span",				&GetSpan,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(GBSizerItem,gbsizeritem)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(gbsizeritem_construct, 0, 6, Nil)
	wxGBSizerItem *gbitem = (wxGBSizerItem*) 0;
	if (n == 0)
		gbitem = new wxGBSizerItem();
	else if (n == 6) {
		WX_GETNUMBER(width)
		WX_GETNUMBER(height)
		DLIB_WXGET_BASE(gbposition, GBPosition, pos)
		DLIB_WXGET_BASE(gbspan, GBSpan, span)
		WX_GETDEFINE(flag)
		WX_GETNUMBER(border)
		gbitem = new wxGBSizerItem(width, height, *pos, *span, flag, border, NULL);
	} else if (n == 5) {
		if (DPTR(vm)->GetActualArg(_argNo)->Type() == DeltaValue_ExternId) {
			util_ui32 serial_no = (util_ui32)DPTR(vm)->GetActualArg(_argNo++)->ToExternId();
			if (DLIB_WXISBASE(Window, serial_no, window, window)) {
				DLIB_WXGET_BASE(gbposition, GBPosition, pos)
				DLIB_WXGET_BASE(gbspan, GBSpan, span)
				WX_GETDEFINE(flag)
				WX_GETNUMBER(border)
				gbitem = new wxGBSizerItem(window, *pos, *span, flag, border, NULL);
			} else
			if (DLIB_WXISBASE(Sizer, serial_no, sizer, sizer)) {
				DLIB_WXGET_BASE(gbposition, GBPosition, pos)
				DLIB_WXGET_BASE(gbspan, GBSpan, span)
				WX_GETDEFINE(flag)
				WX_GETNUMBER(border)
				gbitem = new wxGBSizerItem(sizer, *pos, *span, flag, border, NULL);
			}
		}
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			DPTR(vm)->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SETOBJECT(GBSizerItem, gbitem)
}

WX_FUNC_START(gbsizeritem_destruct, 1, Nil)
	DLIB_WXDELETE(gbsizeritem, GBSizerItem, item)
}

WX_FUNC_START(gbsizeritem_getendpos, 1, Nil)
	DLIB_WXGET_BASE(gbsizeritem, GBSizerItem, item)
	int row, col;
	item->GetEndPos(row, col);
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(GBPosition, new wxGBPosition(row,col))
}

WX_FUNC_START(gbsizeritem_getpos, 1, Nil)
	DLIB_WXGET_BASE(gbsizeritem, GBSizerItem, item)
	int row, col;
	item->GetPos(row, col);
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(GBPosition, new wxGBPosition(row,col))
}

WX_FUNC_START(gbsizeritem_getspan, 1, Nil)
	DLIB_WXGET_BASE(gbsizeritem, GBSizerItem, item)
	int rowSpan, colSpan;
	item->GetSpan(rowSpan, colSpan);
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(GBSpan, new wxGBSpan(rowSpan,colSpan))
}

WX_FUNC_ARGRANGE_START(gbsizeritem_intersects, 2, 3, Nil)
	DLIB_WXGET_BASE(gbsizeritem, GBSizerItem, item)
	if (n == 2) {
		DLIB_WXGET_BASE(gbsizeritem, GBSizerItem, other)
		WX_SETBOOL(item->Intersects(*other))
	} else if (n == 3) {
		DLIB_WXGET_BASE(gbposition, GBPosition, pos)
		DLIB_WXGET_BASE(gbspan, GBSpan, span)
		WX_SETBOOL(item->Intersects(*pos, *span))
	}
}

WX_FUNC_START(gbsizeritem_setpos, 2, Nil)
	DLIB_WXGET_BASE(gbsizeritem, GBSizerItem, item)
	DLIB_WXGET_BASE(gbposition, GBPosition, pos)
	WX_SETBOOL(item->SetPos(*pos))
}

WX_FUNC_START(gbsizeritem_setspan, 2, Nil)
	DLIB_WXGET_BASE(gbsizeritem, GBSizerItem, item)
	DLIB_WXGET_BASE(gbspan, GBSpan, span)
	WX_SETBOOL(item->SetSpan(*span))
}
