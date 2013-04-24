#include "DeltaWxMemoryDC.h"
#include "DeltaWxDC.h"
#include "DeltaWxBitmap.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(memorydc, name)
#define WX_FUNC(name) WX_FUNC1(memorydc, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(selectobject)
WX_FUNC_DEF(selectobjectassource)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(selectobject),
	WX_FUNC(selectobjectassource)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "selectobjectassource")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(MemoryDC, "memorydc", DC)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(MemoryDC, val, memorydc) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDC *_parent = DLIB_WXTYPECAST_BASE(DC, val, dc);
	DeltaWxDC *parent = DNEWCLASS(DeltaWxDC, (_parent));
	WX_SETOBJECT_EX(*at, DC, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "DC",					&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(MemoryDC,memorydc)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(memorydc_construct, 0, 1, Nil)
	wxMemoryDC *wxdc = (wxMemoryDC*) 0;
	DeltaWxMemoryDC *dc = (DeltaWxMemoryDC*) 0;
	if (n == 0)
		wxdc = new wxMemoryDC();
	else {
		DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
		wxdc = new wxMemoryDC(*bmp);
	}
	if (wxdc) dc = DNEWCLASS(DeltaWxMemoryDC, (wxdc));
	WX_SETOBJECT(MemoryDC, dc)
}

DLIB_FUNC_START(memorydc_destruct, 1, Nil)
	DLIB_WXDELETE(memorydc, MemoryDC, dc)
}

DLIB_FUNC_START(memorydc_selectobject, 2, Nil)
	DLIB_WXGET_BASE(memorydc, MemoryDC, dc)
	DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
	dc->SelectObject(*bmp);
}

DLIB_FUNC_START(memorydc_selectobjectassource, 2, Nil)
	DLIB_WXGET_BASE(memorydc, MemoryDC, dc)
	DLIB_WXGET_BASE(bitmap, Bitmap, bmp)
	dc->SelectObjectAsSource(*bmp);
}
