#include "DeltaWxLogChain.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(logchain, name)
#define WX_FUNC(name) WX_FUNC1(logchain, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getoldlog)
WX_FUNC_DEF(ispassingmessages)
WX_FUNC_DEF(passmessages)
WX_FUNC_DEF(setlog)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getoldlog),
	WX_FUNC(ispassingmessages),
	WX_FUNC(passmessages),
	WX_FUNC(setlog)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setlog")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(LogChain, "logchain", Log)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(LogChain, val, logchain) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, Log, val)
	return true;
}

static bool GetOldLog (void* val, DeltaValue* at) 
{
	wxLogChain *chain = DLIB_WXTYPECAST_BASE(LogChain, val, logchain);
	WX_SETOBJECT_NO_CONTEXT_EX(*at, Log, chain->GetOldLog())
	return true;
}

static bool GetIsPassingMessages (void* val, DeltaValue* at) 
{
	wxLogChain *chain = DLIB_WXTYPECAST_BASE(LogChain, val, logchain);
	WX_SETBOOL_EX(*at, chain->IsPassingMessages())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "Log",				&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "oldLog",				&GetOldLog,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "isPassingMessages",	&GetIsPassingMessages,	DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(LogChain,logchain)

////////////////////////////////////////////////////////////////

WX_FUNC_START(logchain_construct, 1, Nil)
	DLIB_WXGET_BASE(log, Log, logger)
	WX_SETOBJECT(LogChain, new wxLogChain(logger))
}

WX_FUNC_START(logchain_destruct, 1, Nil)
	DLIB_WXDELETE(logchain, LogChain, log)
}

WX_FUNC_START(logchain_getoldlog, 1, Nil)
	DLIB_WXGET_BASE(logchain, LogChain, log)
	wxLog* retval	= log->GetOldLog();
	WX_SETOBJECT(Log, retval)
}

WX_FUNC_START(logchain_ispassingmessages, 1, Nil)
	DLIB_WXGET_BASE(logchain, LogChain, log)
	WX_SETBOOL(log->IsPassingMessages())
}

WX_FUNC_START(logchain_passmessages, 2, Nil)
	DLIB_WXGET_BASE(logchain, LogChain, log)
	WX_GETBOOL(pass)
	log->PassMessages(pass);
}

WX_FUNC_START(logchain_setlog, 2, Nil)
	DLIB_WXGET_BASE(logchain, LogChain, log)
	DLIB_WXGET_BASE(log, Log, logger)
	log->SetLog(logger);
}
