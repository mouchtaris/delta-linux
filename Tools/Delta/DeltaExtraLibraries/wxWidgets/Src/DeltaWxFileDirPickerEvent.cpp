#include "DeltaWxFileDirPickerEvent.h"
#include "DeltaWxCommandEvent.h"
#include "DeltaWxObject.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(filedirpickerevent, name)
#define WX_FUNC(name) WX_FUNC1(filedirpickerevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getpath)
WX_FUNC_DEF(setpath)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getpath),
	WX_FUNC(setpath)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getpath", "setpath")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FileDirPickerEvent, "filedirpickerevent", CommandEvent)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FileDirPickerEvent, val, filedirpickerevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, CommandEvent, val)
	return true;
}

static bool GetPath (void* val, DeltaValue* at) 
{
	wxFileDirPickerEvent *ev = DLIB_WXTYPECAST_BASE(FileDirPickerEvent, val, filedirpickerevent);
	WX_SETSTRING_EX(*at, ev->GetPath())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "CommandEvent",		&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "path",				&GetPath,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FileDirPickerEvent,filedirpickerevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(filedirpickerevent_construct, 0, 4, Nil)
	wxFileDirPickerEvent *evt = (wxFileDirPickerEvent*) 0;
	if (n == 0)
		evt = new wxFileDirPickerEvent();
	else if (n == 4) {
		WX_GETDEFINE(type)
		DLIB_WXGET_BASE(object, Object, generator)
		WX_GETDEFINE(id)
		WX_GETSTRING(path)
		evt = new wxFileDirPickerEvent(type, generator, id, path);
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(FileDirPickerEvent, evt)
}

WX_FUNC_START(filedirpickerevent_getpath, 1, Nil)
	DLIB_WXGET_BASE(filedirpickerevent, FileDirPickerEvent, evt)
	WX_SETSTRING(evt->GetPath())
}

WX_FUNC_START(filedirpickerevent_setpath, 2, Nil)
	DLIB_WXGET_BASE(filedirpickerevent, FileDirPickerEvent, evt)
	WX_GETSTRING(path)
	evt->SetPath(path);
}
