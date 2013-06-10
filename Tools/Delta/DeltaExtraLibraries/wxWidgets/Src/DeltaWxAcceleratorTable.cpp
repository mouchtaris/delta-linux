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
WX_FUNC_DEF(isok)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(isok)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "isok", "isok")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(AcceleratorTable, "acceleratortable", Object)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(AcceleratorTable, val, acceleratortable) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Object, val)
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Object",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(AcceleratorTable,acceleratortable)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(acceleratortable_construct, 0, 2, Nil)
	wxAcceleratorTable *acceltable = (wxAcceleratorTable*) 0;
	if (n == 0) {
		acceltable = new wxAcceleratorTable();
	} else if (n == 1) {
#if defined (__WXMSW__)
		WX_GETSTRING(resource)
		acceltable = new wxAcceleratorTable(resource);
#else
		DLIB_ERROR_CHECK(
			true,
			"This function overload is only available on MS Windows implementation of wxWidgets."
		);
#endif //__WXMSW__
	} else if (n == 2) {
		WX_GETNUMBER(num_d)
		WX_GETTABLE(entr)
		const size_t num = num_d;
		wxAcceleratorEntry *entries = DNEWARR(wxAcceleratorEntry, num);
		for (int i = 0; i < (int) num; ++i) {
			DeltaValue value;
			entr->Get(DeltaValue((DeltaNumberValueType)i), &value);
			if (value.Type() == DeltaValue_ExternId) {
				util_ui32 serial_no = (util_ui32)value.ToExternId();
				if (DLIB_WXISBASE(AcceleratorEntry, serial_no, acceleratorentry, accelentry)) {
					entries[i] = *accelentry;
				}
			}
		}
		acceltable = new wxAcceleratorTable(num, entries);
		DDELARR(entries);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(AcceleratorTable, acceltable)
}

WX_FUNC_START(acceleratortable_isok, 1, Nil)
	DLIB_WXGET_BASE(acceleratortable, AcceleratorTable, acceltable)
	WX_SETBOOL(acceltable->IsOk());
}
