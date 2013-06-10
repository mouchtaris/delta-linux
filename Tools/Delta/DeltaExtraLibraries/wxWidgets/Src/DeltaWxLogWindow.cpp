#include "DeltaWxLogWindow.h"
#include "DeltaWxLogChain.h"
#include "DeltaWxFrame.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(logwindow, name)
#define WX_FUNC(name) WX_FUNC1(logwindow, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(onframecreate)
WX_FUNC_DEF(onframecreateadd)
WX_FUNC_DEF(onframecreateremove)
WX_FUNC_DEF(onframeclose)
WX_FUNC_DEF(onframecloseadd)
WX_FUNC_DEF(onframecloseremove)
WX_FUNC_DEF(onframedelete)
WX_FUNC_DEF(onframedeleteadd)
WX_FUNC_DEF(onframedeleteremove)
WX_FUNC_DEF(show)
WX_FUNC_DEF(getframe)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(onframecreate),
	WX_FUNC(onframecreateadd),
	WX_FUNC(onframecreateremove),
	WX_FUNC(onframeclose),
	WX_FUNC(onframecloseadd),
	WX_FUNC(onframecloseremove),
	WX_FUNC(onframedelete),
	WX_FUNC(onframedeleteadd),
	WX_FUNC(onframedeleteremove),
	WX_FUNC(show),
	WX_FUNC(getframe)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "getframe")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS(LogWindow, "logwindow", LogChain)

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(LogWindow, val, logwindow) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetBaseClass (void* val, DeltaValue* at) 
{
	WX_SET_BASECLASS_GETTER(at, LogChain, val)
	return true;
}

static bool GetLogFrame (void* val, DeltaValue* at) 
{
	wxLogWindow *window = DLIB_WXTYPECAST_BASE(LogWindow, val, logwindow);
	wxFrame *frame = window->GetFrame();
	DeltaWxFrame *retval = frame ? DNEWCLASS(DeltaWxFrame, (frame)) : (DeltaWxFrame*) 0;
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "LogChain",			&GetBaseClass,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "logFrame",			&GetLogFrame,			DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(LogWindow,logwindow)

////////////////////////////////////////////////////////////////

WX_FUNC_ARGRANGE_START(logwindow_construct, 2, 4, Nil)
	DLIB_WXGET_BASE(frame, Frame, parent)
	WX_GETSTRING(title)
	bool show = true, passtoold = true;
	if (n >= 3) { WX_GETBOOL_DEFINED(show) }
	if (n >= 4) { WX_GETBOOL_DEFINED(passtoold) }
	WX_SETOBJECT(LogWindow, new wxLogWindowDerived(parent, title, show, passtoold))
}

WX_FUNC_START(logwindow_destruct, 1, Nil)
	DLIB_WXDELETE(logwindow, LogWindow, log)
}

WX_FUNC_START(logwindow_onframecreate, 2, Nil)
	DLIB_WXGET_BASE(logwindow, LogWindow, win)
	DLIB_WXGET_BASE(frame, Frame, frame)
	win->OnFrameCreate(frame);
}

static void logwindow_onframecreateadd_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("logwindow_onframecreateadd");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(logwindow, LogWindow, _win)
	wxLogWindowDerived *win = (wxLogWindowDerived*)_win;
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable())
			win->AddOnCreate(*arg);
	}
}

static void logwindow_onframecreateremove_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("logwindow_onframecreateremove");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(logwindow, LogWindow, _win)
	wxLogWindowDerived *win = (wxLogWindowDerived*)_win;
	std::list<DeltaValue> *onCreate = win->GetOnCreateFuncs();
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable()) {
			std::list<DeltaValue>::iterator it = onCreate->begin();
			for (; it != onCreate->end(); it++) {
				if (arg->Equal(*it)) {
					onCreate->erase(it);
					break;
				}
			}
		}
	}
}


WX_FUNC_START(logwindow_onframeclose, 2, Nil)
	DLIB_WXGET_BASE(logwindow, LogWindow, win)
	DLIB_WXGET_BASE(frame, Frame, frame)
	WX_SETBOOL(win->OnFrameClose(frame))
}

static void logwindow_onframecloseadd_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("logwindow_onframecloseadd");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(logwindow, LogWindow, _win)
	wxLogWindowDerived *win = (wxLogWindowDerived*)_win;
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable())
			win->AddOnClose(*arg);
	}
}

static void logwindow_onframecloseremove_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("logwindow_onframecloseremove");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(logwindow, LogWindow, _win)
	wxLogWindowDerived *win = (wxLogWindowDerived*)_win;
	std::list<DeltaValue> *onClose = win->GetOnCloseFuncs();
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable()) {
			std::list<DeltaValue>::iterator it = onClose->begin();
			for (; it != onClose->end(); it++) {
				if (arg->Equal(*it)) {
					onClose->erase(it);
					break;
				}
			}
		}
	}
}

WX_FUNC_START(logwindow_onframedelete, 2, Nil)
	DLIB_WXGET_BASE(logwindow, LogWindow, win)
	DLIB_WXGET_BASE(frame, Frame, frame)
	win->OnFrameDelete(frame);
}

static void logwindow_onframedeleteadd_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("logwindow_onframedeleteadd");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(logwindow, LogWindow, _win)
	wxLogWindowDerived *win = (wxLogWindowDerived*)_win;
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable())
			win->AddOnDelete(*arg);
	}
}

static void logwindow_onframedeleteremove_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("logwindow_onframedeleteremove");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(logwindow, LogWindow, _win)
	wxLogWindowDerived *win = (wxLogWindowDerived*)_win;
	std::list<DeltaValue> *onDelete = win->GetOnDeleteFuncs();
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable()) {
			std::list<DeltaValue>::iterator it = onDelete->begin();
			for (; it != onDelete->end(); it++) {
				if (arg->Equal(*it)) {
					onDelete->erase(it);
					break;
				}
			}
		}
	}
}

WX_FUNC_ARGRANGE_START(logwindow_show, 1, 2, Nil)
	DLIB_WXGET_BASE(logwindow, LogWindow, win)
	bool show = true;
	if (n >= 2) { WX_GETBOOL_DEFINED(show) }
	win->Show(show);
}

WX_FUNC_START(logwindow_getframe, 1, Nil)
	DLIB_WXGET_BASE(logwindow, LogWindow, win)
	wxFrame* retval	= win->GetFrame();
	WX_SETOBJECT(Frame, retval)
}

////////////////////////////////////////////////////////////////

void wxLogWindowDerived::NotifyCreate(wxFrame *wxframe)
{
	wxFrame *frame = wxframe;
	DeltaValue arg;
	WX_SETOBJECT_NO_CONTEXT_EX(arg, Frame, frame)
	std::list<DeltaValue>::iterator it;
	std::list<DeltaValue> *createFuncs = this->GetOnCreateFuncs();
	for (it = createFuncs->begin(); it != createFuncs->end(); ++it) {
		DeltaValue obj = *it;
		DASSERT(obj.IsCallable());
		obj(arg);
	}
}

bool wxLogWindowDerived::NotifyClose(wxFrame *wxframe)
{
	bool retval = true;
	wxFrame *frame = wxframe;
	DeltaValue arg;
	WX_SETOBJECT_NO_CONTEXT_EX(arg, Frame, frame)
	std::list<DeltaValue>::iterator it;
	std::list<DeltaValue> *closeFuncs = this->GetOnCloseFuncs();
	for (it = closeFuncs->begin(); it != closeFuncs->end(); ++it) {
		DeltaValue obj = *it, obj_result;
		DASSERT(obj.IsCallable());
		obj(arg, &obj_result);
		if(obj_result.Type() == DeltaValue_Bool)
			retval = retval && obj_result.ToBool();
	}
	return retval;
}

void wxLogWindowDerived::NotifyDelete(wxFrame *wxframe)
{
	wxFrame *frame = wxframe;
	DeltaValue arg;
	WX_SETOBJECT_NO_CONTEXT_EX(arg, Frame, frame)
	std::list<DeltaValue>::iterator it;
	std::list<DeltaValue> *deleteFuncs = this->GetOnDeleteFuncs();
	for (it = deleteFuncs->begin(); it != deleteFuncs->end(); ++it) {
		DeltaValue obj = *it;
		DASSERT(obj.IsCallable());
		obj(arg);
	}
}