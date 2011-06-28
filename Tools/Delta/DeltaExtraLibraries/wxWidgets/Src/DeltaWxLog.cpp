#include "DeltaWxLog.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(log, name)
#define WX_FUNC(name) WX_FUNC1(log, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(addtracemask)
WX_FUNC_DEF(cleartracemasks)
WX_FUNC_DEF(gettracemasks)
WX_FUNC_DEF(onlog)
WX_FUNC_DEF(getactivetarget)
WX_FUNC_DEF(setactivetarget)
WX_FUNC_DEF(suspend)
WX_FUNC_DEF(resume)
WX_FUNC_DEF(dontcreateondemand)
WX_FUNC_DEF(flush)
WX_FUNC_DEF(flushactive)
WX_FUNC_DEF(getverbose)
WX_FUNC_DEF(setverbose)
WX_FUNC_DEF(getloglevel)
WX_FUNC_DEF(setloglevel)
WX_FUNC_DEF(getrepetitioncounting)
WX_FUNC_DEF(setrepetitioncounting)
WX_FUNC_DEF(gettimestamp)
WX_FUNC_DEF(settimestamp)
WX_FUNC_DEF(gettracemask)
WX_FUNC_DEF(settracemask)
WX_FUNC_DEF(isallowedtracemask)
WX_FUNC_DEF(removetracemask)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(addtracemask),
	WX_FUNC(cleartracemasks),
	WX_FUNC(gettracemasks),
	WX_FUNC(onlog),
	WX_FUNC(getactivetarget),
	WX_FUNC(setactivetarget),
	WX_FUNC(suspend),
	WX_FUNC(resume),
	WX_FUNC(dontcreateondemand),
	WX_FUNC(flushactive),
	WX_FUNC(getverbose),
	WX_FUNC(setverbose),
	WX_FUNC(getloglevel),
	WX_FUNC(setloglevel),
	WX_FUNC(getrepetitioncounting),
	WX_FUNC(setrepetitioncounting),
	WX_FUNC(gettimestamp),
	WX_FUNC(settimestamp),
	WX_FUNC(gettracemask),
	WX_FUNC(settracemask),
	WX_FUNC(isallowedtracemask),
	WX_FUNC(removetracemask),
	WX_FUNC(destruct),
	WX_FUNC(flush)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(23, uarraysize(funcs) - 23, "destruct", "flush")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(Log, "log")

void wxLogInstallLibraryMethods()
{
	LogUtils::InstallAll(methods);
}

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaWxLog, "log", "wx::log_",
								  methods = DeltaObject::NativeCodeHelpers::NewObject();
								  DELTALIBFUNC_SET_METHODS_TABLE_CREATION_INFO(methods, apiClassName);
								  wxLogInstallLibraryMethods(),
								  DELTALIBFUNC_DESTROY_METHODS_TABLE())

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(log_construct, 0, Nil)
	DeltaWxLog *log = DNEWCLASS(DeltaWxLog, (new wxLog()));
	WX_SETOBJECT(Log, log)
}

DLIB_FUNC_START(log_destruct, 1, Nil)
	DLIB_WXDELETE(log, Log, log)
}

DLIB_FUNC_START(log_addtracemask, 1, Nil)
	WX_GETSTRING(mask)
	wxLog::AddTraceMask(mask);
}

DLIB_FUNC_START(log_cleartracemasks, 0, Nil)
	wxLog::ClearTraceMasks();
}

DLIB_FUNC_START(log_gettracemasks, 0, Nil)
	wxArrayString masks = wxLog::GetTraceMasks();
	DeltaObject *retval = DNEW(DeltaObject);
	for (int i = 0, n = (int)masks.GetCount(); i < n; ++i) {
		DeltaValue value;
		value.FromString(std::string(masks.Item(i).mb_str(wxConvUTF8)));
		retval->Set(DeltaValue((DeltaNumberValueType)i), value);
	}
	DLIB_RETVAL_REF.FromTable(retval);
}

DLIB_FUNC_START(log_onlog, 3, Nil)
	WX_GETDEFINE(level)
	WX_GETSTRING(message)
	WX_GETNUMBER(t)
	wxLog::OnLog(level, message, t);
}

DLIB_FUNC_START(log_getactivetarget, 0, Nil)
	WXNEWCLASS(DeltaWxLog, retval, wxLog, wxLog::GetActiveTarget())
	WX_SETOBJECT(Log, retval)
}

DLIB_FUNC_START(log_setactivetarget, 1, Nil)
	DLIB_WXGET_BASE(log, Log, logtarget)
	WXNEWCLASS(DeltaWxLog, retval, wxLog, wxLog::SetActiveTarget(logtarget))
	WX_SETOBJECT(Log, retval)
}

DLIB_FUNC_START(log_suspend, 0, Nil)
	wxLog::Suspend();
}

DLIB_FUNC_START(log_resume, 0, Nil)
	wxLog::Resume();
}

DLIB_FUNC_START(log_dontcreateondemand, 0, Nil)
	wxLog::DontCreateOnDemand();
}

DLIB_FUNC_START(log_flush, 1, Nil)
	DLIB_WXGET_BASE(log, Log, log)
	log->Flush();
}

DLIB_FUNC_START(log_flushactive, 0, Nil)
	wxLog::FlushActive();
}

DLIB_FUNC_START(log_getverbose, 0, Nil)
	WX_SETBOOL(wxLog::GetVerbose())
}

WX_FUNC_ARGRANGE_START(log_setverbose, 0, 1, Nil)
	bool verbose = true;
	if (n >= 1) { WX_GETBOOL_DEFINED(verbose) }
	wxLog::SetVerbose(verbose);
}

DLIB_FUNC_START(log_getloglevel, 0, Nil)
	WX_SETNUMBER(wxLog::GetLogLevel())
}

DLIB_FUNC_START(log_setloglevel, 1, Nil)
	WX_GETDEFINE(level)
	wxLog::SetLogLevel(level);
}

DLIB_FUNC_START(log_getrepetitioncounting, 0, Nil)
	WX_SETBOOL(wxLog::GetRepetitionCounting())
}

WX_FUNC_ARGRANGE_START(log_setrepetitioncounting, 0, 1, Nil)
	bool counting = true;
	if (n >= 1) { WX_GETBOOL_DEFINED(counting) }
	wxLog::SetRepetitionCounting(counting);
}

DLIB_FUNC_START(log_gettimestamp, 0, Nil)
	WX_SETSTRING(wxString(wxLog::GetTimestamp()))
}

DLIB_FUNC_START(log_settimestamp, 1, Nil)
	WX_GETSTRING(format)
	wxLog::SetTimestamp(format);
}

DLIB_FUNC_START(log_gettracemask, 0, Nil)
	WX_SETNUMBER(wxLog::GetTraceMask())
}

DLIB_FUNC_START(log_settracemask, 1, Nil)
	WX_GETDEFINE(mask)
	wxLog::SetTraceMask(mask);
}

DLIB_FUNC_START(log_isallowedtracemask, 1, Nil)
	WX_GETSTRING(mask)
	WX_SETBOOL(wxLog::IsAllowedTraceMask(mask))
}

DLIB_FUNC_START(log_removetracemask, 1, Nil)
	WX_GETSTRING(mask)
	wxLog::RemoveTraceMask(mask);
}
