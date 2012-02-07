#include "DeltaWxFileDialog.h"
#include "DeltaWxDialog.h"
#include "DeltaWxWindow.h"
#include "DeltaWxPoint.h"
#include "DeltaWxSize.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(filedialog, name)
#define WX_FUNC(name) WX_FUNC1(filedialog, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getdirectory)
WX_FUNC_DEF(getfilename)
WX_FUNC_DEF(getfilenames)
WX_FUNC_DEF(getfilterindex)
WX_FUNC_DEF(getmessage)
WX_FUNC_DEF(getpath)
WX_FUNC_DEF(getpaths)
WX_FUNC_DEF(getwildcard)
WX_FUNC_DEF(setdirectory)
WX_FUNC_DEF(setfilename)
WX_FUNC_DEF(setfilterindex)
WX_FUNC_DEF(setmessage)
WX_FUNC_DEF(setpath)
WX_FUNC_DEF(setwildcard)
WX_FUNC_DEF(showmodal)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getdirectory),
	WX_FUNC(getfilename),
	WX_FUNC(getfilenames),
	WX_FUNC(getfilterindex),
	WX_FUNC(getmessage),
	WX_FUNC(getpath),
	WX_FUNC(getpaths),
	WX_FUNC(getwildcard),
	WX_FUNC(setdirectory),
	WX_FUNC(setfilename),
	WX_FUNC(setfilterindex),
	WX_FUNC(setmessage),
	WX_FUNC(setpath),
	WX_FUNC(setwildcard),
	WX_FUNC(showmodal)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "showmodal")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(FileDialog, "filedialog", Dialog)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(FileDialog, val, filedialog) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	wxDialog *_parent = DLIB_WXTYPECAST_BASE(Dialog, val, dialog);
	DeltaWxDialog *parent = DNEWCLASS(DeltaWxDialog, (_parent));
	WX_SETOBJECT_EX(*at, Dialog, parent)
	return true;
}

static bool GetMessage (void* val, DeltaValue* at) 
{
	wxFileDialog *dialog = DLIB_WXTYPECAST_BASE(FileDialog, val, filedialog);
	WX_SETSTRING_EX(*at, dialog->GetMessage())
	return true;
}

static bool GetDirectory (void* val, DeltaValue* at) 
{
	wxFileDialog *dialog = DLIB_WXTYPECAST_BASE(FileDialog, val, filedialog);
	WX_SETSTRING_EX(*at, dialog->GetDirectory())
	return true;
}

static bool GetPath (void* val, DeltaValue* at) 
{
	wxFileDialog *dialog = DLIB_WXTYPECAST_BASE(FileDialog, val, filedialog);
	WX_SETSTRING_EX(*at, dialog->GetPath())
	return true;
}

static bool GetFilename (void* val, DeltaValue* at) 
{
	wxFileDialog *dialog = DLIB_WXTYPECAST_BASE(FileDialog, val, filedialog);
	WX_SETSTRING_EX(*at, dialog->GetFilename())
	return true;
}

static bool GetWildcard (void* val, DeltaValue* at) 
{
	wxFileDialog *dialog = DLIB_WXTYPECAST_BASE(FileDialog, val, filedialog);
	WX_SETSTRING_EX(*at, dialog->GetWildcard())
	return true;
}

static bool GetFilterIndex (void* val, DeltaValue* at) 
{
	wxFileDialog *dialog = DLIB_WXTYPECAST_BASE(FileDialog, val, filedialog);
	WX_SETNUMBER_EX(*at, dialog->GetFilterIndex())
	return true;
}

static bool GetFilenames (void* val, DeltaValue* at) 
{
	wxFileDialog *dialog = DLIB_WXTYPECAST_BASE(FileDialog, val, filedialog);
	at->FromTable(DNEW(DELTA_OBJECT));
	wxArrayString filenames;
	dialog->GetFilenames(filenames);
	for (int i = 0, n = (int)filenames.GetCount(); i < n; ++i) {
		DeltaValue value;
		WX_SETSTRING_EX(value, filenames.Item(i))
		at->ToTable()->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Dialog",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "message",			&GetMessage,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "directory",			&GetDirectory,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "path",				&GetPath,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "filename",			&GetFilename,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "wildcard",			&GetWildcard,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "filterIndex",		&GetFilterIndex,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "filenames",			&GetFilenames,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(FileDialog, filedialog);

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(filedialog_construct, 1, 9, Nil)
	DLIB_WXGET_BASE(window, Window, parent)
	wxString message = wxFileSelectorPromptStr, defaultDir = wxEmptyString, defaultFile = wxEmptyString,
		wildCard = wxFileSelectorDefaultWildcardStr, name = wxFileDialogNameStr;
	long style = wxFD_DEFAULT_STYLE;
	wxPoint pos = wxDefaultPosition;
	wxSize size = wxDefaultSize;
	if (n >= 2) { WX_GETSTRING_DEFINED(message) }
	if (n >= 3) { WX_GETSTRING_DEFINED(defaultDir) }
	if (n >= 4) { WX_GETSTRING_DEFINED(defaultFile) }
	if (n >= 5) { WX_GETSTRING_DEFINED(wildCard) }
	if (n >= 6) { WX_GETDEFINE_DEFINED(style) }
	if (n >= 7) { DLIB_WXGETPOINT_BASE(pt) pos = *pt; }
	if (n >= 8) { DLIB_WXGETSIZE_BASE(sz) size = *sz; }
	if (n >= 9) { WX_GETSTRING_DEFINED(name) }
	DeltaWxFileDialog *dialog = DNEWCLASS(DeltaWxFileDialog,
		(new wxFileDialog(parent, message, defaultDir, defaultFile, wildCard, style, pos, size, name)));
	WX_SETOBJECT(FileDialog, dialog)
}

DLIB_FUNC_START(filedialog_destruct, 1, Nil)
	DLIB_WXDELETE(filedialog, FileDialog, dialog)
}

DLIB_FUNC_START(filedialog_getdirectory, 1, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_SETSTRING(dialog->GetDirectory())
}

DLIB_FUNC_START(filedialog_getfilename, 1, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_SETSTRING(dialog->GetFilename())
}

DLIB_FUNC_START(filedialog_getfilenames, 1, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	wxArrayString filenames;
	dialog->GetFilenames(filenames);
	DeltaObject *retval = DNEW(DeltaObject);
	for (int i = 0, n = (int)filenames.GetCount(); i < n; ++i) {
		DeltaValue value;
		value.FromString(std::string(filenames.Item(i).mb_str(wxConvUTF8)));
		retval->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	DLIB_RETVAL_REF.FromTable(retval);
}

DLIB_FUNC_START(filedialog_getfilterindex, 1, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_SETNUMBER(dialog->GetFilterIndex())
}

DLIB_FUNC_START(filedialog_getmessage, 1, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_SETSTRING(dialog->GetMessage())
}

DLIB_FUNC_START(filedialog_getpath, 1, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_SETSTRING(dialog->GetPath())
}

DLIB_FUNC_START(filedialog_getpaths, 1, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	wxArrayString paths;
	dialog->GetPaths(paths);
	DeltaObject *retval = DNEW(DeltaObject);
	for (int i = 0, n = (int)paths.GetCount(); i < n; ++i) {
		DeltaValue value;
		value.FromString(std::string(paths.Item(i).mb_str(wxConvUTF8)));
		retval->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	DLIB_RETVAL_REF.FromTable(retval);
}

DLIB_FUNC_START(filedialog_getwildcard, 1, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_SETSTRING(dialog->GetWildcard())
}

DLIB_FUNC_START(filedialog_setdirectory, 2, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_GETSTRING(directory)
	dialog->SetDirectory(directory);
}

DLIB_FUNC_START(filedialog_setfilename, 2, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_GETSTRING(filename)
	dialog->SetFilename(filename);
}

DLIB_FUNC_START(filedialog_setfilterindex, 2, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_GETNUMBER(index)
	dialog->SetFilterIndex(index);
}

DLIB_FUNC_START(filedialog_setmessage, 2, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_GETSTRING(message)
	dialog->SetMessage(message);
}

DLIB_FUNC_START(filedialog_setpath, 2, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_GETSTRING(path)
	dialog->SetPath(path);
}

DLIB_FUNC_START(filedialog_setwildcard, 2, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_GETSTRING(wildcard)
	dialog->SetWildcard(wildcard);
}

DLIB_FUNC_START(filedialog_showmodal, 1, Nil)
	DLIB_WXGET_BASE(filedialog, FileDialog, dialog)
	WX_SETNUMBER(dialog->ShowModal())
}
