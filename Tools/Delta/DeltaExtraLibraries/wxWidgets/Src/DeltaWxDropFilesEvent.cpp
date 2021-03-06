#include "DeltaWxDropFilesEvent.h"
#include "DeltaWxEvent.h"
#include "DeltaWxPoint.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(dropfilesevent, name)
#define WX_FUNC(name) WX_FUNC1(dropfilesevent, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(getfiles)
WX_FUNC_DEF(getnumberoffiles)
WX_FUNC_DEF(getposition)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(getfiles),
	WX_FUNC(getnumberoffiles),
	WX_FUNC(getposition)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "getfiles", "getposition")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(DropFilesEvent, "dropfilesevent", Event)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DropFilesEvent, val, dropfilesevent) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Event, val)
	return true;
}

static bool GetNoFiles (void* val, DeltaValue* at) 
{
	wxDropFilesEvent *ev = DLIB_WXTYPECAST_BASE(DropFilesEvent, val, dropfilesevent);
	WX_SETNUMBER_EX(*at, ev->GetNumberOfFiles())
	return true;
}

static bool GetPosition (void* val, DeltaValue* at) 
{
	wxDropFilesEvent *ev = DLIB_WXTYPECAST_BASE(DropFilesEvent, val, dropfilesevent);
	WX_SETOBJECT_NO_CONTEXT_COLLECTABLE_NATIVE_INSTANCE_EX(*at, Point, new wxPoint(ev->GetPosition()))
	return true;
}

static bool GetFiles (void* val, DeltaValue* at) 
{
	wxDropFilesEvent *ev = DLIB_WXTYPECAST_BASE(DropFilesEvent, val, dropfilesevent);
	wxString *files = ev->GetFiles();
	at->FromTable(DNEW(DeltaObject));
	for (int i = 0, n = ev->GetNumberOfFiles(); i < n; ++i) {
		DeltaValue value;
		WX_SETSTRING_EX(value, files[i])
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Event",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "noFiles",			&GetNoFiles,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "pos",				&GetPosition,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "files",				&GetFiles,				DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DropFilesEvent,dropfilesevent)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(dropfilesevent_construct, 0, 3, Nil)
	int type = wxEVT_NULL, noFiles = 0;
	wxString *files = (wxString *) NULL;
	if (n >= 1) { WX_GETDEFINE_DEFINED(type) }
	if (n >= 2) { WX_GETNUMBER_DEFINED(noFiles) }
	if (n >= 3) {
		WX_GETTABLE(strings)
		int strings_size = strings->Total();
		files = DNEWARR(wxString, strings_size);
		for (int i = 0, n = strings_size; i < n; ++i) {
			DeltaValue value;
			strings->Get(DeltaValue((DeltaNumberValueType)i), &value);
			if (value.Type() == DeltaValue_String) {
				wxString str = wxString(value.ToString().c_str(), wxConvUTF8);
				files[i] = str;
			}
		}
	}
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(DropFilesEvent, new wxDropFilesEvent(type, noFiles, files))
	if (n >= 3) { DDELARR(files); }
}

WX_FUNC_START(dropfilesevent_getfiles, 1, Nil)
	DLIB_WXGET_BASE(dropfilesevent, DropFilesEvent, evt)
	wxString *strings = evt->GetFiles();
	DeltaObject *retval = DNEW(DeltaObject);
	for (int i = 0, n = evt->GetNumberOfFiles(); i < n; ++i) {
		DeltaValue value;
		value.FromString(std::string(strings[i].mb_str(wxConvUTF8)));
		retval->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	DLIB_RETVAL_REF.FromTable(retval);
}

WX_FUNC_START(dropfilesevent_getnumberoffiles, 1, Nil)
	DLIB_WXGET_BASE(dropfilesevent, DropFilesEvent, evt)
	WX_SETNUMBER(evt->GetNumberOfFiles())
}

WX_FUNC_START(dropfilesevent_getposition, 1, Nil)
	DLIB_WXGET_BASE(dropfilesevent, DropFilesEvent, evt)
	WX_SETOBJECT_COLLECTABLE_NATIVE_INSTANCE(Point, new wxPoint(evt->GetPosition()))
}
