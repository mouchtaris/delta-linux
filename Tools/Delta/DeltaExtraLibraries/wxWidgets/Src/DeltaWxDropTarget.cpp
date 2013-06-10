#include "DeltaWxDropTarget.h"
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

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(droptarget, name)
#define WX_FUNC(name) WX_FUNC1(droptarget, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(getdata)
WX_FUNC_DEF(ondata)
WX_FUNC_DEF(ondataadd)
WX_FUNC_DEF(ondataremove)
WX_FUNC_DEF(ondrop)
WX_FUNC_DEF(ondropadd)
WX_FUNC_DEF(ondropremove)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(getdata),
	WX_FUNC(ondata),
	WX_FUNC(ondataadd),
	WX_FUNC(ondataremove),
	WX_FUNC(ondrop),
	WX_FUNC(ondropadd),
	WX_FUNC(ondropremove)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "ondropremove")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(DropTarget, "droptarget")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(DropTarget, val, droptarget) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetDefaultAction (void* val, DeltaValue* at) 
{
	wxDropTarget *target = DLIB_WXTYPECAST_BASE(DropTarget, val, droptarget);
	WX_SETNUMBER_EX(*at, target->GetDefaultAction())
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "defaultAction",		&GetDefaultAction,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(DropTarget,droptarget)

////////////////////////////////////////////////////////////////

WX_FUNC_START(droptarget_construct, 0, Nil)
	context.UpdateContext(_argNo, _sig1, _sig2);
	if (SetValueFromNativeInstance
			<DeltaWxDropTargetClassId, DeltaWxDropTarget, wxDropTargetDerived>
			(new wxDropTargetDerived(), DLIB_RETVAL_PTR, &LetWrapperLive,
			&DropTargetUtils::GetGetter, &DropTargetUtils::GetMethods, &context)) {
		context.UpdateLocals(NULL, NULL, &_argNo, &_sig1, &_sig2);
	} else
		return;
}

WX_FUNC_START(droptarget_destruct, 1, Nil)
	DLIB_WXDELETE(droptarget, DropTarget, target)
}

WX_FUNC_START(droptarget_getdata, 1, Nil)
	DLIB_WXGET_BASE(droptarget, DropTarget, target)
	target->GetData();
}

WX_FUNC_START(droptarget_ondata, 4, Nil)
	DLIB_WXGET_BASE(droptarget, DropTarget, target)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	WX_GETNUMBER(def)
	WX_SETNUMBER(target->OnData(x, y, (wxDragResult)(int)def))
}

static void droptarget_ondataadd_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("droptarget_ondataadd");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(droptarget, DropTarget, _droptarget)
	wxDropTargetDerived *droptarget = (wxDropTargetDerived*)_droptarget;
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable())
			droptarget->AddOnData(arg);
	}
}

static void droptarget_ondataremove_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("droptarget_ondataremove");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(droptarget, DropTarget, _droptarget)
	wxDropTargetDerived *droptarget = (wxDropTargetDerived*)_droptarget;
	DeltaValue *onData = droptarget->GetOnDataFunc();
	DeltaValue *arg = DPTR(vm)->GetActualArg(1);
	if (arg->IsCallable()) {
		if (arg->Equal(onData)) {
			droptarget->AddOnData(NULL);
		}
	}
}

WX_FUNC_START(droptarget_ondrop, 3, Nil)
	DLIB_WXGET_BASE(droptarget, DropTarget, target)
	WX_GETNUMBER(x)
	WX_GETNUMBER(y)
	WX_SETBOOL(target->OnDrop(x, y))
}

static void droptarget_ondropadd_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("droptarget_ondropadd");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(droptarget, DropTarget, _droptarget)
	wxDropTargetDerived *droptarget = (wxDropTargetDerived*)_droptarget;
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable())
			droptarget->AddOnDrop(*arg);
	}
}

static void droptarget_ondropremove_LibFunc (DeltaVirtualMachine* vm)
{
	ISSUE_FUNC("droptarget_ondropremove");
	util_ui32 _argNo = 0;
	DeltaNilReset reseter(vm);
	std::string _sig1, _sig2;
	DeltaAtLeastTotalArgsCheck(2, CURR_FUNC, RESET_EMPTY)
	int n = DPTR(vm)->TotalActualArgs();
	WX_CREATE_CONTEXT(context);
	DLIB_WXGET_BASE(droptarget, DropTarget, _droptarget)
	wxDropTargetDerived *droptarget = (wxDropTargetDerived*)_droptarget;
	std::list<DeltaValue> *onDrop = droptarget->GetOnDropFuncs();
	for (int i = 1; i < n; ++i) {
		DeltaValue *arg = DPTR(vm)->GetActualArg(i);
		if (arg->IsCallable()) {
			std::list<DeltaValue>::iterator it = onDrop->begin();
			for (; it != onDrop->end(); it++) {
				if (arg->Equal(*it)) {
					onDrop->erase(it);
					break;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////

wxDragResult wxDropTargetDerived::NotifyData(wxCoord x, wxCoord y, wxDragResult def)
{
	std::list<DeltaValue> args;
	args.push_front(DeltaValue((double)x));
	args.push_front(DeltaValue((double)y));
	args.push_front(DeltaValue((double)def));
	DeltaValue dataFunc = *(this->GetOnDataFunc());
	DeltaValue obj_result;
	DASSERT(dataFunc.IsCallable());
	dataFunc(args, &obj_result);
	if(obj_result.Type() == DeltaValue_Number)
		return (wxDragResult)(int)obj_result.ToNumber();
	return wxDragError;
}

bool wxDropTargetDerived::NotifyDrop(wxCoord x, wxCoord y)
{
	bool retval = true;
	std::list<DeltaValue> args;
	args.push_front(DeltaValue((double)x));
	args.push_front(DeltaValue((double)y));
	std::list<DeltaValue>::iterator it;
	std::list<DeltaValue> *dropFuncs = this->GetOnDropFuncs();
	for (it = dropFuncs->begin(); it != dropFuncs->end(); ++it) {
		DeltaValue obj = *it, obj_result;
		DASSERT(obj.IsCallable());
		obj(args, &obj_result);
		if(obj_result.Type() == DeltaValue_Bool)
			retval = retval && obj_result.ToBool();
	}
	return retval;
}
