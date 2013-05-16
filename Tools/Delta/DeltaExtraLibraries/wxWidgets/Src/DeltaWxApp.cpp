#include "DeltaWxApp.h"
#include "DeltaWxEvtHandler.h"
#include "DeltaWxWindow.h"
#include "DeltaValue.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"

////////////////////////////////////////////////////////////////
// Custom WIN32 handling to activate the propert execution context
// and allow the dll manifest load the correct version of comctl32.
#ifdef WIN32

static HMODULE module;
BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: module = (HMODULE) hModule; break;
		case DLL_PROCESS_DETACH: module = (HMODULE) INVALID_HANDLE_VALUE; break;
	}
	return true;
}

class CMyContext {
public:
	BOOL Init() {
		BOOL bRet = FALSE;
		OSVERSIONINFO info = { 0 };
		info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		//do special XP theme activation code only on XP or higher...
		if (GetVersionEx(&info) && info.dwMajorVersion >= 5 && info.dwMinorVersion >= 1 && info.dwPlatformId == VER_PLATFORM_WIN32_NT) {
			TCHAR szModule[MAX_PATH] = {0};
			::GetModuleFileName(module, szModule, MAX_PATH);
			ACTCTX actctx = {
				sizeof(ACTCTX),
				ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID,
				szModule, 0, 0, 0, MAKEINTRESOURCE(2), 0, module
			};
			
			bRet = (m_hActCtx = ::CreateActCtx(&actctx)) != INVALID_HANDLE_VALUE;
		}
		return bRet;
	}

	CMyContext() : m_ulActivationCookie(0), m_hActCtx(0) {
		if (Init() && m_hActCtx && (m_hActCtx != INVALID_HANDLE_VALUE))
			ActivateActCtx(m_hActCtx, &m_ulActivationCookie);
	}
	~CMyContext() {
		if (m_hActCtx && (m_hActCtx != INVALID_HANDLE_VALUE)) {
			DeactivateActCtx(0, m_ulActivationCookie);
			ReleaseActCtx(m_hActCtx);
		}
	}
private: 
	ULONG_PTR m_ulActivationCookie;
	HANDLE m_hActCtx;
};

#endif

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(app, name)
#define WX_FUNC(name) WX_FUNC1(app, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(dispatch)
WX_FUNC_DEF(exitmainloop)
WX_FUNC_DEF(getappname)
WX_FUNC_DEF(getclassname)
WX_FUNC_DEF(getinstance)
WX_FUNC_DEF(getexitonframedelete)
WX_FUNC_DEF(gettopwindow)
WX_FUNC_DEF(getusebestvisual)
WX_FUNC_DEF(getvendorname)
WX_FUNC_DEF(isactive)
WX_FUNC_DEF(ismainlooprunning)
WX_FUNC_DEF(oninit)
WX_FUNC_DEF(oninitadd)
WX_FUNC_DEF(oninitremove)
WX_FUNC_DEF(onexit)
WX_FUNC_DEF(onexitadd)
WX_FUNC_DEF(onexitremove)
WX_FUNC_DEF(onrun)
WX_FUNC_DEF(onrunadd)
WX_FUNC_DEF(onrunremove)
WX_FUNC_DEF(pending)
WX_FUNC_DEF(setappname)
WX_FUNC_DEF(setclassname)
WX_FUNC_DEF(setexitonframedelete)
WX_FUNC_DEF(settopwindow)
WX_FUNC_DEF(setvendorname)
WX_FUNC_DEF(setusebestvisual)
WX_FUNC_DEF(start)
WX_FUNC_DEF(yield)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(ismainlooprunning),
	WX_FUNC(getinstance),
	WX_FUNC(destruct),
	WX_FUNC(dispatch),
	WX_FUNC(exitmainloop),
	WX_FUNC(getappname),
	WX_FUNC(getclassname),
	WX_FUNC(getexitonframedelete),
	WX_FUNC(gettopwindow),
	WX_FUNC(getusebestvisual),
	WX_FUNC(getvendorname),
	WX_FUNC(isactive),
	WX_FUNC(oninit),
	WX_FUNC(oninitadd),
	WX_FUNC(oninitremove),
	WX_FUNC(onexit),
	WX_FUNC(onexitadd),
	WX_FUNC(onexitremove),
	WX_FUNC(onrun),
	WX_FUNC(onrunadd),
	WX_FUNC(onrunremove),
	WX_FUNC(pending),
	WX_FUNC(setappname),
	WX_FUNC(setclassname),
	WX_FUNC(setexitonframedelete),
	WX_FUNC(settopwindow),
	WX_FUNC(setvendorname),
	WX_FUNC(setusebestvisual),
	WX_FUNC(start),
	WX_FUNC(yield)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(3, uarraysize(funcs) - 3, "destruct", "yield")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(AppAdapter, "app", EvtHandler)

void wxAppInstallLibraryMethods()
{
	AppAdapterUtils::InstallAll(methods);
}

DELTA_LIBRARY_SUBAPI_INSTALLER_EX(DeltaWxAppAdapter, "app", "wx::app_",
								  methods = DeltaObject::NativeCodeHelpers::NewObject();
								  DELTALIBFUNC_SET_METHODS_TABLE_CREATION_INFO(methods, apiClassName);
								  wxAppInstallLibraryMethods(),
								  DELTALIBFUNC_DESTROY_METHODS_TABLE())

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(app_construct, 0, Nil)
DeltaWxAppAdapter *wxapp;
if (wxApp* app = (wxApp*) wxApp::GetInstance())
	wxapp = DNEWCLASS(DeltaWxAppAdapter, (new wxAppAdapter(app)));
else
	wxapp = DNEWCLASS(DeltaWxAppAdapter, (new wxAppAdapter()));
WX_SETOBJECT(AppAdapter, wxapp)
DLIB_FUNC_END

DLIB_FUNC_START(app_destruct, 1, Nil)
DLIB_WXDELETE(app, AppAdapter, app)
DLIB_FUNC_END

DLIB_FUNC_START(app_dispatch, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	app->Dispatch();
}

DLIB_FUNC_START(app_exitmainloop, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	app->ExitMainLoop();
}

DLIB_FUNC_START(app_getappname, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_SETSTRING(app->GetAppName())
}

DLIB_FUNC_START(app_getclassname, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_SETSTRING(app->GetClassName())
}

DLIB_FUNC_START(app_getinstance, 0, Nil)
	if (wxApp* app = (wxApp*) wxApp::GetInstance()) {
		DeltaWxAppAdapter *wxapp = DNEWCLASS(DeltaWxAppAdapter, (new wxAppAdapter(app)));
		WX_SETOBJECT(AppAdapter, wxapp)
	} else {
		DLIB_RETVAL_REF.FromNil();
	}
}

DLIB_FUNC_START(app_getexitonframedelete, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_SETBOOL(app->GetExitOnFrameDelete())
}

DLIB_FUNC_START(app_gettopwindow, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	DeltaWxWindow *retval = DNEWCLASS(DeltaWxWindow, (app->GetTopWindow()));
	WX_SETOBJECT(Window, retval)
}

DLIB_FUNC_START(app_getusebestvisual, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_SETBOOL(app->GetUseBestVisual())
}

DLIB_FUNC_START(app_getvendorname, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_SETSTRING(app->GetVendorName())
}

DLIB_FUNC_START(app_isactive, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_SETBOOL(app->IsActive())
}

DLIB_FUNC_START(app_ismainlooprunning, 0, Nil)
	WX_SETBOOL(wxApp::IsMainLoopRunning())
}

DLIB_FUNC_START(app_oninit, 1, Nil)
DLIB_WXGET_BASE(app, AppAdapter, wxVar)
WX_SETBOOL(wxVar->OnInit());
DLIB_FUNC_END

static void app_oninitadd_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("app_oninitadd");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	DLIB_WXGET_BASE(app, AppAdapter, _app)
	wxAppAdapter *app = (wxAppAdapter*)_app;
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable())
			app->AddOnInit(*arg);
	}
}

static void app_oninitremove_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("app_oninitremove");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	DLIB_WXGET_BASE(app, AppAdapter, _app)
	wxAppAdapter *app = (wxAppAdapter*)_app;
	std::list<DeltaValue> *onInit = app->GetOnInitFuncs();
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable()) {
			std::list<DeltaValue>::iterator it = onInit->begin();
			for (; it != onInit->end(); it++) {
				if (arg->Equal(*it)) {
					onInit->erase(it);
					break;
				}
			}
		}
	}
}

DLIB_FUNC_START(app_onexit, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, wxVar)
	WX_SETNUMBER(wxVar->OnExit());
}

static void app_onexitadd_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("app_onexitadd");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	DLIB_WXGET_BASE(app, AppAdapter, app)
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable())
			app->AddOnExit(*arg);
	}
}

static void app_onexitremove_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("app_onexitremove");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	DLIB_WXGET_BASE(app, AppAdapter, app)
	std::list<DeltaValue> *onExit = app->GetOnExitFuncs();
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable()) {
			std::list<DeltaValue>::iterator it = onExit->begin();
			for (; it != onExit->end(); it++) {
				if (arg->Equal(*it)) {
					onExit->erase(it);
					break;
				}
			}
		}
	}
}

DLIB_FUNC_START(app_onrun, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, wxVar)
	WX_SETNUMBER(wxVar->OnRun());
}

static void app_onrunadd_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("app_onrunadd");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	DLIB_WXGET_BASE(app, AppAdapter, app)
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable())
			app->AddOnRun(*arg);
	}
}

static void app_onrunremove_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("app_onrunremove");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	DLIB_WXGET_BASE(app, AppAdapter, app)
	std::list<DeltaValue> *onRun = app->GetOnRunFuncs();
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable()) {
			std::list<DeltaValue>::iterator it = onRun->begin();
			for (; it != onRun->end(); it++) {
				if (arg->Equal(*it)) {
					onRun->erase(it);
					break;
				}
			}
		}
	}
}

DLIB_FUNC_START(app_pending, 1, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_SETBOOL(app->Pending())
}

DLIB_FUNC_START(app_setappname, 2, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_GETSTRING(name)
	app->SetAppName(name);
}

DLIB_FUNC_START(app_setclassname, 2, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_GETSTRING(name)
	app->SetClassName(name);
}

DLIB_FUNC_START(app_setexitonframedelete, 2, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_GETBOOL(flag)
	app->SetExitOnFrameDelete(flag);
}

DLIB_FUNC_START(app_settopwindow, 2, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	DLIB_WXGET_BASE(window, Window, window)
	app->SetTopWindow(window);
}

DLIB_FUNC_START(app_setvendorname, 2, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_GETSTRING(name)
	app->SetVendorName(name);
}

WX_FUNC_ARGRANGE_START(app_setusebestvisual, 2, 3, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	WX_GETBOOL(flag)
	bool forceTrueColour = false;
	if (n >= 3) { WX_GETBOOL_DEFINED(forceTrueColour) }
	app->SetUseBestVisual(flag, forceTrueColour);
}

DLIB_FUNC_START(app_start, 1, Nil)
DLIB_WXGET_BASE(app, AppAdapter, app)
if (!app->IsDelegate()) {
#ifdef WIN32
	CMyContext con;
#endif
	wxApp::SetInstance(app->GetApp());
#if wxUSE_UNICODE
	static char* argv[] = {0};
	wxEntry(app->GetApp()->argc, argv);
#else
	wxEntry(app->GetApp()->argc, app->GetApp()->argv);
#endif
}
DLIB_FUNC_END

WX_FUNC_ARGRANGE_START(app_yield, 1, 2, Nil)
	DLIB_WXGET_BASE(app, AppAdapter, app)
	bool onlyIfNeeded = false;
	if (n >= 2) { WX_GETBOOL_DEFINED(onlyIfNeeded) }
	WX_SETBOOL(app->Yield(onlyIfNeeded))
}

////////////////////////////////////////////////////////////////

bool wxAppAdapter::NotifyInit()
{
	DeltaValue arg;
	DeltaWxAppAdapter *app = DNEWCLASS(DeltaWxAppAdapter, (this));
	WX_SETOBJECT_EX(arg, AppAdapter, app)

	bool retval = true;
	std::list<DeltaValue>::iterator it;
	std::list<DeltaValue> *initFuncs = this->GetOnInitFuncs();
	for (it = initFuncs->begin(); it != initFuncs->end(); ++it) {
		DeltaValue obj = *it, obj_result;
		DASSERT(obj.IsCallable());
		obj(arg, &obj_result);
		if(obj_result.Type() == DeltaValue_Bool)
			retval = retval && obj_result.ToBool();
	}

	DDELETE(app);
	return retval;
}

void wxAppAdapter::NotifyExit()
{
	DeltaValue arg;
	DeltaWxAppAdapter *app = DNEWCLASS(DeltaWxAppAdapter, (this));
	WX_SETOBJECT_EX(arg, AppAdapter, app)

	std::list<DeltaValue>::iterator it;
	std::list<DeltaValue> *exitFuncs = this->GetOnExitFuncs();
	for (it = exitFuncs->begin(); it != exitFuncs->end(); ++it) {
		DeltaValue obj = *it;
		DASSERT(obj.IsCallable());
		obj(arg);
	}

	DDELETE(app);
}

void wxAppAdapter::NotifyRun()
{
	DeltaValue arg;
	DeltaWxAppAdapter *app = DNEWCLASS(DeltaWxAppAdapter, (this));
	WX_SETOBJECT_EX(arg, AppAdapter, app)

	std::list<DeltaValue>::iterator it;
	std::list<DeltaValue> *runFuncs = this->GetOnRunFuncs();
	for (it = runFuncs->begin(); it != runFuncs->end(); ++it) {
		DeltaValue obj = *it;
		DASSERT(obj.IsCallable());
		obj(arg);
	}

	DDELETE(app);
}
