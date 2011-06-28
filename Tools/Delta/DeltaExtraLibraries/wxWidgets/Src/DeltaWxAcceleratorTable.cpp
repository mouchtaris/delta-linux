#include "DeltaWxAcceleratorTable.h"
#include "DeltaWxObject.h"
#include "DeltaWxAcceleratorEntry.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(acceleratortable, name)
#define WX_FUNC(name) WX_FUNC1(acceleratortable, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(isok)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(isok)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "isok")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(AcceleratorTable, "acceleratortable", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(AcceleratorTable, val, acceleratortable) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxObject *_parent = DLIB_WXTYPECAST_BASE(Object, val, object);
	DeltaWxObject *parent = DNEWCLASS(DeltaWxObject, (_parent));
	WX_SETOBJECT_EX(*at, Object, parent)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(AcceleratorTable, acceleratortable);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(acceleratortable_construct, 0, 2, Nil)
	wxAcceleratorTable *wxacceltable = (wxAcceleratorTable*) 0;
	DeltaWxAcceleratorTable *acceltable = (DeltaWxAcceleratorTable*) 0;
	if (n == 0) {
		wxacceltable = new wxAcceleratorTable();
	} else if (n == 1) {
#if defined (__WXMSW__)
		WX_GETSTRING(resource)
		wxacceltable = new wxAcceleratorTable(resource);
#endif //__WXMSW__
	} else if (n == 2) {
		WX_GETNUMBER(num)
		WX_GETTABLE(entr)
		wxAcceleratorEntry *entries = DNEWARR(wxAcceleratorEntry, num);
		for (int i = 0; i < (int) num; ++i) {
			DeltaValue value;
			entr->Get(DeltaValue((DeltaNumberValueType)i), &value);
			if (value.Type() == DeltaValue_ExternId) {
				util_ui32 serial_no = (util_ui32)value.ToExternId();
				if (DLIB_WXISBASE(AcceleratorEntry, serial_no, acceleratorentry, entry)) {
					wxAcceleratorEntry *accelentry = (wxAcceleratorEntry*) entry->GetCastToNativeInstance();
					entries[i] = *accelentry;
				}
			}
		}
		wxacceltable = new wxAcceleratorTable(num, entries);
		DDELARR(entries);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	if (wxacceltable) acceltable = DNEWCLASS(DeltaWxAcceleratorTable, (wxacceltable));
	WX_SETOBJECT(AcceleratorTable, acceltable)
}

DLIB_FUNC_START(acceleratortable_destruct, 1, Nil)
	DLIB_WXDELETE(acceleratortable, AcceleratorTable, acceltable)
}

DLIB_FUNC_START(acceleratortable_isok, 1, Nil)
	DLIB_WXGET_BASE(acceleratortable, AcceleratorTable, acceltable)
	WX_SETBOOL(acceltable->IsOk());
}
