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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(acceleratorentry, name)
#define WX_FUNC(name) WX_FUNC1(acceleratorentry, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getcommand)
WX_FUNC_DEF(getflags)
WX_FUNC_DEF(getkeycode)
WX_FUNC_DEF(set)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getcommand),
	WX_FUNC(getflags),
	WX_FUNC(getkeycode),
	WX_FUNC(set)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getcommand", "set")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(AcceleratorEntry, "acceleratorentry")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(AcceleratorEntry, val, acceleratorentry) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetFlags (void* val, DeltaValue* at) 
{
	wxAcceleratorEntry *entry = DLIB_WXTYPECAST_BASE(AcceleratorEntry, val, acceleratorentry);
	at->FromNumber(entry->GetFlags());
	return true;
}

static bool GetKeyCode (void* val, DeltaValue* at) 
{
	wxAcceleratorEntry *entry = DLIB_WXTYPECAST_BASE(AcceleratorEntry, val, acceleratorentry);
	at->FromNumber(entry->GetKeyCode());
	return true;
}

static bool GetCommand (void* val, DeltaValue* at) 
{
	wxAcceleratorEntry *entry = DLIB_WXTYPECAST_BASE(AcceleratorEntry, val, acceleratorentry);
	at->FromNumber(entry->GetCommand());
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "flags",				&GetFlags,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "keyCode",			&GetKeyCode,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "command",			&GetCommand,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(AcceleratorEntry,acceleratorentry)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(acceleratorentry_construct, 0, 3, Nil)
	wxAcceleratorEntry *accelentry = (wxAcceleratorEntry*) 0;
	if (n == 0) {
		accelentry = new wxAcceleratorEntry();
	} else if (n == 3) {
		WX_GETDEFINE(flags)
		WX_GETDEFINE(keyCode)
		WX_GETDEFINE(cmd)
		accelentry = new wxAcceleratorEntry(flags, keyCode, cmd);
	} else {
		DPTR(vm)->PrimaryError(
			"Wrong number of args (%d passed) to '%s'",
			vm->TotalActualArgs(),
			CURR_FUNC
		);
		RESET_EMPTY
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(AcceleratorEntry, accelentry)
}

WX_FUNC_START(acceleratorentry_getcommand, 1, Nil)
	DLIB_WXGET_BASE(acceleratorentry, AcceleratorEntry, accelentry)
	WX_SETNUMBER(accelentry->GetCommand());
}

WX_FUNC_START(acceleratorentry_getflags, 1, Nil)
	DLIB_WXGET_BASE(acceleratorentry, AcceleratorEntry, accelentry)
	WX_SETNUMBER(accelentry->GetFlags());
}

WX_FUNC_START(acceleratorentry_getkeycode, 1, Nil)
	DLIB_WXGET_BASE(acceleratorentry, AcceleratorEntry, accelentry)
	WX_SETNUMBER(accelentry->GetKeyCode());
}

WX_FUNC_START(acceleratorentry_set, 4, Nil)
	DLIB_WXGET_BASE(acceleratorentry, AcceleratorEntry, accelentry)
	WX_GETDEFINE(flags)
	WX_GETDEFINE(keyCode)
	WX_GETDEFINE(cmd)
	accelentry->Set(flags, keyCode, cmd);
}
