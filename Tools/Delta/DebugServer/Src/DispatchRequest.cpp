// DispatchRequest.cpp
// Implementation of debug request dispatcher.
// ScriptFighter Project.
// A. Savidis, February 2002.
//

#include "DebugServer.h"
#include "DeltaVMRegistry.h"
#include "DeltaVirtualMachine.h"
#include "DebugExprParserAPI.h"
#include "DeltaDebugExtensions.h"
#include "DeltaTable.h"
#include "DeltaDebugServerBreakPoints.h"
#include "ufiles.h"

#include <string>

//////////////////////////////////////////////////////////////////

bool DeltaDebugServer::insideBreakPoint		= false;
bool DeltaDebugServer::startRequestReceived = false;
void (*DeltaDebugServer::startFunc)(void) = &DeltaDebugServer::DefaultOnStart;
DeltaDebugServer::HandlerMap*  DeltaDebugServer::handlers = (HandlerMap*) 0;

void DeltaDebugServer::DefaultOnStart (void)
	{ DASSERT(!startRequestReceived); startRequestReceived = true; }

void DeltaDebugServer::SetStartServeFunction (void (*f) (void))  {
	if (startRequestReceived) {
		DASSERT(startFunc == &DefaultOnStart);	// Can set only once after it is received.
		if (f)					
			(*f)();				// Called explicitly.
	}
	else
		startFunc = f;
}

//////////////////////////////////////////////////////////////////

#define	DEBUGVM	DPTR(DeltaDebugExtensions::GetCurrentDebugContext())
#define	BREAKVM	DPTR(DeltaDebugExtensions::BreakPointVM())

#define	BREAKVM_DEBUGGER GetDebugger(BREAKVM)
#define	DEBUGVM_DEBUGGER GetDebugger(DEBUGVM)

//////////////////////////////////////////////////////////////////

#define	ADD_HANDLER(cmmd) \
	(*DPTR(handlers))[Debug_##cmmd] = &DeltaDebugServer::Handle##cmmd

void DeltaDebugServer::InstallRequestHandlers (void) {
	ADD_HANDLER(Go);
	ADD_HANDLER(StepOver);	
	ADD_HANDLER(StepIn);	
	ADD_HANDLER(SelectiveStepIn);	
	ADD_HANDLER(RunTo);		
	ADD_HANDLER(StepOut);	
	ADD_HANDLER(StackUp);		
	ADD_HANDLER(StackDown);		
	ADD_HANDLER(MultipleStackUp);		
	ADD_HANDLER(MultipleStackDown);		
	ADD_HANDLER(GetLocal);		
	ADD_HANDLER(GetGlobal);		
	ADD_HANDLER(GetActual);
	ADD_HANDLER(GetExpr);
	ADD_HANDLER(GetExprMany);
	ADD_HANDLER(GetExprTypeData);
	ADD_HANDLER(GetExprTypeDataMany);
	ADD_HANDLER(GetBinaryData);
	ADD_HANDLER(SetObjectGraphConfiguration);
	ADD_HANDLER(GetObjectGraph);
	ADD_HANDLER(GetVariables);
	ADD_HANDLER(GetDynamicCode);
	ADD_HANDLER(GetAllPossibleCalls);
	ADD_HANDLER(AddBreakPoint);			
	ADD_HANDLER(RemoveBreakPoint);		
	ADD_HANDLER(ChangeBreakPointCondition);		
	ADD_HANDLER(EnableBreakPoint);		
	ADD_HANDLER(DisableBreakPoint);		
	ADD_HANDLER(EnableAllBreakPoints);	
	ADD_HANDLER(DisableAllBreakPoints);	
	ADD_HANDLER(RemoveAllBreakPoints);	
	ADD_HANDLER(Stop);	
	ADD_HANDLER(Start);	
	ADD_HANDLER(BreakExecution);	
	ADD_HANDLER(SetToStringMaxLength);	
	ADD_HANDLER(SetBreakOnThrownException);	
}

//////////////////////////////////////////////////////////

void DeltaDebugServer::DispatchRequest (DebugClientToServerRequest request) {
	(*(*DPTR(handlers))[request])();
}

//////////////////////////////////////////////////////////

void DeltaDebugServer::HandleGo (void) {
	if (DeltaDebugServer::InsideBreakPoint()) {
		DeltaDebugServer::ResetInsideBreakPoint();
		BREAKVM_DEBUGGER.Continue();
	}
}

//************************

void DeltaDebugServer::HandleStepOver (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	DeltaDebugServer::ResetInsideBreakPoint();
	BREAKVM_DEBUGGER.Step();
}

//************************

void DeltaDebugServer::HandleStepIn (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	DeltaDebugServer::ResetInsideBreakPoint();
	BREAKVM_DEBUGGER.StepIn();
}

//************************

void DeltaDebugServer::HandleSelectiveStepIn (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	DeltaDebugServer::ResetInsideBreakPoint();

	util_ui32 callOrder;
	DeltaDebugServer::GetSelectiveStepIn(&callOrder);
	BREAKVM_DEBUGGER.SelectiveStepIn(callOrder);
}

//************************

void DeltaDebugServer::HandleRunTo (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	DeltaDebugServer::ResetInsideBreakPoint();

	util_ui32 line;
	DeltaDebugServer::GetRunTo(&line);
	if (!BREAKVM_DEBUGGER.IsValidBreakPointLine(line))
		line = BREAKVM_DEBUGGER.GetNextValidBreakPointLine(line);

	if (line)
		BREAKVM_DEBUGGER.RunTo(line);
	else
		BREAKVM_DEBUGGER.Continue();
}

//************************

void DeltaDebugServer::HandleStepOut (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	DeltaDebugServer::ResetInsideBreakPoint();
	BREAKVM_DEBUGGER.StepOut();
}

//************************

void DeltaDebugServer::HandleStackUp (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	if (DEBUGVM_DEBUGGER.StackUp())
		DEBUGVM_DEBUGGER.SendCurrentContext();
	else
		DeltaDebugServer::DoInfoTopContext();
}

//************************

void DeltaDebugServer::HandleMultipleStackUp (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	util_ui32 n = 0;
	GetUnsignedInt(Debug_MultipleStackUp, &n);

	while (n--)
		if (!DEBUGVM_DEBUGGER.StackUp())
			DeltaDebugServer::DoInfoTopContext();

	DEBUGVM_DEBUGGER.SendCurrentContext();
}

//************************

void DeltaDebugServer::HandleStackDown (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	if (!DEBUGVM_DEBUGGER.StackDown())
		DeltaDebugServer::DoInfoGlobalContext();
	else
		DEBUGVM_DEBUGGER.SendCurrentContext();
}

//************************

void DeltaDebugServer::HandleMultipleStackDown (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	util_ui32 n = 0;
	GetUnsignedInt(Debug_MultipleStackDown, &n);

	while (n--)
		if (!DEBUGVM_DEBUGGER.StackDown())
			DeltaDebugServer::DoInfoGlobalContext();

	DEBUGVM_DEBUGGER.SendCurrentContext();
}

//************************

#define	CHECK_DEBUG_INFO_IN_WATCH() \
	if (!DEBUGVM->HasDebugInfo()) \
		{ DeltaDebugServer::DoInfoValue(DBG_NO_INFO); return; }

void DeltaDebugServer::HandleGetLocal (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	CHECK_DEBUG_INFO_IN_WATCH();

	char* name;
	DeltaDebugServer::GetGetLocal(&name);
	DeltaValue* obj = DEBUGVM_DEBUGGER.GetLocalVar(name);

	if (obj)
		DeltaDebugServer::DoInfoValue(obj->ConvertToStringBySkippingUserCode().c_str());
	else
		DeltaDebugServer::DoInfoValue(DBG_ID_NOTFOUND);
}

//************************


void DeltaDebugServer::HandleGetGlobal (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	CHECK_DEBUG_INFO_IN_WATCH();

	char* name;
	DeltaDebugServer::GetGetGlobal(&name);

	if (DeltaValue* obj = DEBUGVM_DEBUGGER.GetGlobalVar(name, ustrprefix(DELTA_STATICVAR_NAME_PREFIX, name)))
		DeltaDebugServer::DoInfoValue(obj->ConvertToStringBySkippingUserCode().c_str());
	else
		DeltaDebugServer::DoInfoValue(DBG_ID_NOTFOUND);
}

//************************

void DeltaDebugServer::HandleGetActual (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	CHECK_DEBUG_INFO_IN_WATCH();

	char*	name;
	DeltaDebugServer::GetGetActual(&name);
	DeltaValue* obj = DEBUGVM_DEBUGGER.GetActualArg(name);

	if (obj)
		DeltaDebugServer::DoInfoValue(obj->ConvertToStringBySkippingUserCode().c_str());
	else
		DeltaDebugServer::DoInfoValue(DBG_ID_NOTFOUND);
}

//************************

void DeltaDebugServer::HandleGetExpr (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	CHECK_DEBUG_INFO_IN_WATCH();

	char* expr;
	DeltaDebugServer::GetGetExpr(&expr);

	std::pair<std::string, bool> result = DebugExprEvaluator::Evaluate(DEBUGVM, expr);
	if (result.second)
		DeltaDebugServer::DoInfoValue(result.first.c_str());
	else
		DeltaDebugServer::DoInfoErrorValue(result.first.c_str());
}

//************************

void DeltaDebugServer::HandleGetExprMany (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	CHECK_DEBUG_INFO_IN_WATCH();

	std::list<std::string> exprs;
	DeltaDebugServer::GetGetExprMany(exprs);

	std::list< std::pair<std::string, bool> > results;

	for (std::list<std::string>::iterator i = exprs.begin(); i != exprs.end(); ++i)
		results.push_back(
			DebugExprEvaluator::Evaluate(DEBUGVM, i->c_str())
		);

	DeltaDebugServer::DoInfoValueMany(results);
}

//************************

void DeltaDebugServer::HandleGetExprTypeData (void) {
	
	if (!DeltaDebugServer::InsideBreakPoint())
		return;
	CHECK_DEBUG_INFO_IN_WATCH();

	char* formatId;
	char* expr;
	DeltaDebugServer::GetGetExprTypeData(&formatId, &expr);

	std::pair<std::string, bool> result = DebugExprEvaluator::EvaluateToTypeData(DEBUGVM, formatId, expr);
	if (result.second)
		DeltaDebugServer::DoInfoExprTypeData(result.first.c_str());
	else
		DeltaDebugServer::DoInfoErrorValue(result.first.c_str());
}

//************************

void DeltaDebugServer::HandleGetExprTypeDataMany (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;
	CHECK_DEBUG_INFO_IN_WATCH();

	char* formatId;
	std::list<std::string> exprs;
	DeltaDebugServer::GetGetExprTypeDataMany(&formatId, exprs);

	std::list< std::pair<std::string, bool> > results;

	for (std::list<std::string>::iterator i = exprs.begin(); i != exprs.end(); ++i)
		results.push_back(
			DebugExprEvaluator::EvaluateToTypeData(DEBUGVM, formatId, i->c_str())
		);

	DeltaDebugServer::DoInfoExprTypeDataMany(results);
}

//************************

void DeltaDebugServer::HandleGetBinaryData (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	CHECK_DEBUG_INFO_IN_WATCH();

	char* expr;
	DeltaDebugServer::GetGetBinaryData(&expr);

	std::string error;
	if (DeltaValue* result = DebugExprEvaluator::Evaluate(DEBUGVM, expr, error))
		if (result->Type() != DeltaValue_ExternId)
			DeltaDebugServer::DoInfoErrorValue(
				uconstructstr(
					"Expression '%s' not an %s; can't request binary data!", 
					expr, 
					result->GetTypeStr(DeltaValue_ExternId)
				).c_str()
			);
		else 
		if (const DeltaExternIdFieldGetter* fieldGetter = result->GetExternIdFieldGetter()) {
			util_ui32	n			= 0;
			bool		wasCopied	= false;
			if (void* data = DPTR(fieldGetter)->GetBinaryData(result->ToExternId(), &n, &wasCopied)) {
				DoInfoBinaryData(data, n);
				if (wasCopied)
					DeltaExternIdFieldGetter::ClearBinaryData(data);
			}
			else
				DeltaDebugServer::DoInfoErrorValue(
					uconstructstr(
						"%s '%s' of type '%s' provides no binary data!", 
						result->TypeStr(),
						expr,
						result->GetExternIdTypeString().c_str()
					).c_str()
				);
		}
		else
			DeltaDebugServer::DoInfoErrorValue(
				uconstructstr(
					"%s '%s' of type '%s' lacks a debugger field getter; can't request binary data!", 
					result->TypeStr(),
					expr,
					result->GetExternIdTypeString().c_str()
				).c_str()
			);
	else
		DeltaDebugServer::DoInfoErrorValue(error.c_str());
}

//************************

void DeltaDebugServer::HandleSetObjectGraphConfiguration(void) {

	bool expandLibraryFuncs;
	bool expandMethodFuncs;
	bool expandProgramFuncs;
	bool expandFuncVMs;

	DeltaDebugServer::GetSetObjectGraphConfiguration(
		&expandLibraryFuncs,
		&expandMethodFuncs, 
		&expandProgramFuncs, 
		&expandFuncVMs
	);

	DebugExprEvaluator::SetObjectGraphConfiguration(
		expandLibraryFuncs, 
		expandMethodFuncs, 
		expandProgramFuncs, 
		expandFuncVMs
	);
}

//************************

void DeltaDebugServer::HandleGetObjectGraph (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;
	CHECK_DEBUG_INFO_IN_WATCH();

	char*		expr;
	util_ui32	depth;
	bool		directed;
	DeltaDebugServer::GetGetObjectGraph(&expr, &depth, &directed);

	ObjectGraph graph;
	std::string simpleOrError;
	bool		succeeded;
	if (directed)
		succeeded = DebugExprEvaluator::EvaluateToObjectGraph(DEBUGVM, expr, graph, depth, simpleOrError);
	else
		succeeded = DebugExprEvaluator::EvaluateToUndirectedObjectGraph(DEBUGVM, expr, graph, depth, simpleOrError);
	if (!succeeded)
		DeltaDebugServer::DoInfoErrorValue(simpleOrError.c_str());	// Error
	else
	if (!simpleOrError.empty())
		DeltaDebugServer::DoInfoValue(simpleOrError.c_str());		// Simple value
	else
		DeltaDebugServer::DoInfoObjectGraph(graph);					// Graph
}

//************************

void DeltaDebugServer::HandleGetVariables (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	CHECK_DEBUG_INFO_IN_WATCH();

	std::list< std::pair<std::string, std::string> > vars;
	DEBUGVM_DEBUGGER.GetVariablesAtCurrentContext(vars);
	DoInfoVariables(vars);
}

//************************

void DeltaDebugServer::HandleGetDynamicCode (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	CHECK_DEBUG_INFO_IN_WATCH();

	DoInfoDynamicCode(
		!DEBUGVM_DEBUGGER.IsDynamicCode() ? 
			DBG_NO_INFO: 
			DEBUGVM_DEBUGGER.GetDynamicCode().c_str()
	);
}	

//************************

void DeltaDebugServer::HandleGetAllPossibleCalls (void) {

	if (!DeltaDebugServer::InsideBreakPoint())
		return;

	std::list< std::pair<std::string, std::string> > calls;
	DEBUGVM_DEBUGGER.GetAllPossibleCalls(calls);
	DoInfoAllPossibleCalls(calls);
}

//************************

DeltaVirtualMachine* DeltaDebugServer::GetFirstMatchingVM (const char* source) {
	
	for (util_ui16 i = 0; i < VMRegistry().Total(); ++i) {
		DeltaVirtualMachine* vm = VMRegistry().Get(i);
		if (ufilepathsequal(DPTR(vm)->Source(), source))
			return vm;
	}
	return (DeltaVirtualMachine*) 0;
}

//************************

void DeltaDebugServer::HandleAddBreakPoint (void) {

	char*		source;
	util_ui32	line;
	char*		cond;

	DeltaDebugServer::GetAddBreakPoint(&source, &line, &cond);
	DASSERT(source && line && cond);

	// Get a VM from this source, if any, to validate the bpt.

	if (DeltaVirtualMachine* vm = GetFirstMatchingVM(source))

		if (!GetDebugger(vm).IsValidBreakPointLine(line)) {	// Try with the closest next valid line.

			util_ui32 newLine = GetDebugger(vm).GetNextValidBreakPointLine(line);

			if (newLine && !DeltaDebugServerBreakPoints::Get().HasBreakPoint(source, newLine))
				DeltaDebugServerBreakPoints::Get().Add(source, newLine, cond);

			DeltaDebugServer::DoInfoInvalidBreakPoint(	// Invalidated, possibly changed.
				Debug_InfoInvalidBreakPoint, 
				source, 
				line, 
				newLine, 
				cond
			);
			return;
		}

	// Fallback (valid or no vm to judge it).
	//
	DeltaDebugServerBreakPoints::Get().Add(source, line, cond);
	DeltaDebugServer::DoInfoValidBreakPoint(	// Validated.
		source, 
		line, 
		cond
	);
}

//************************

void DeltaDebugServer::HandleChangeBreakPointCondition (void) {

	char*		source;
	util_ui32	line;
	char*		cond;

	DeltaDebugServer::GetChangeBreakPointCondition(&source, &line, &cond);
	DASSERT(source && line && cond);

	// Get a VM from this source, if any, to validate the bpt.

	if (DeltaVirtualMachine* vm = GetFirstMatchingVM(source)) {
		if (GetDebugger(vm).IsValidBreakPointLine(line))
			DeltaDebugServerBreakPoints::Get().Change(source, line, cond);
	}
	else
	if (DeltaDebugServerBreakPoints::Get().HasBreakPoint(source, line))
		DeltaDebugServerBreakPoints::Get().Change(source, line, cond);
}

//************************

#define	ERROR_INVALID_BREAKPOINT(op)											\
		if (true) {																\
		DoInfoBreakPointError(													\
			uconstructstr(														\
				"%s break point: not found for source '%s' and line '%d'!",		\
				op, source, line												\
			).c_str()															\
		); } else

void DeltaDebugServer::HandleRemoveBreakPoint (void) {

	char*		source;
	util_ui32	line;

	DeltaDebugServer::GetRemoveBreakPoint(&source, &line);
	if (DeltaDebugServerBreakPoints::Get().HasBreakPoint(source, line))
		DeltaDebugServerBreakPoints::Get().Remove(source, line);
	else 
		ERROR_INVALID_BREAKPOINT("Remove");		
}

//************************

void DeltaDebugServer::HandleEnableBreakPoint (void) {

	char*		source;
	util_ui32	line;

	DeltaDebugServer::GetEnableBreakPoint(&source, &line);
	if (DeltaDebugServerBreakPoints::Get().HasBreakPoint(source, line))
		DeltaDebugServerBreakPoints::Get().Enable(source, line);
	else 
		ERROR_INVALID_BREAKPOINT("Enable");		
}

//************************

void DeltaDebugServer::HandleDisableBreakPoint (void) {

	char*		source;
	util_ui32	line;

	DeltaDebugServer::GetDisableBreakPoint(&source, &line);
	if (DeltaDebugServerBreakPoints::Get().HasBreakPoint(source, line))
		DeltaDebugServerBreakPoints::Get().Disable(source, line);
	else 
		ERROR_INVALID_BREAKPOINT("Disable");		
}

//************************

void DeltaDebugServer::HandleEnableAllBreakPoints (void) {

	char* source;
	DeltaDebugServer::GetEnableAllBreakPoints(&source);
	DeltaDebugServerBreakPoints::Get().EnableAll(source);
}

//************************

void DeltaDebugServer::HandleDisableAllBreakPoints (void) {

	char* source;
	DeltaDebugServer::GetDisableAllBreakPoints(&source);
	DeltaDebugServerBreakPoints::Get().DisableAll(source);
}

//************************

void DeltaDebugServer::HandleRemoveAllBreakPoints (void) {

	char* source;
	DeltaDebugServer::GetRemoveAllBreakPoints(&source);
	DeltaDebugServerBreakPoints::Get().RemoveAll(source);
}

//************************

void DeltaDebugServer::HandleStop (void) 
	{ DeltaDebugServer::NotifyExit(); }

void DeltaDebugServer::HandleStart (void) {
	DASSERT(startFunc);
	(*startFunc)(); 
	if (startFunc != &DeltaDebugServer::DefaultOnStart)
		unullify(startFunc);
}

void DeltaDebugServer::HandleBreakExecution (void) 
	{ DeltaDebugExtensions::BreakExecution(); }

//////////////////////////////////////////////////////////

void DeltaDebugServer::HandleSetToStringMaxLength (void) {
	util_ui32 maxLen;
	DeltaDebugServer::GetSetToStringMaxLength(&maxLen);
	DeltaValue::SetMaxLengthForConvertToStringBySkippingUserCode(maxLen);
}

void DeltaDebugServer::HandleSetBreakOnThrownException (void) {
	bool val;
	DeltaDebugServer::GetSetBreakOnThrownException(&val);
	DeltaDebugExtensions::SetBreakOnThrownException(val);
}

//////////////////////////////////////////////////////////

bool DeltaDebugServer::InsideBreakPoint (void) 
	{ return insideBreakPoint; }

void DeltaDebugServer::ResetInsideBreakPoint (void) 
	{ insideBreakPoint = false;	}

//////////////////////////////////////////////////////////
