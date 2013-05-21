/**
 *	DeltaVM.h
 *
 *	-- IDE Delta Virtual Machine Component --
 *
 *	Virtual Machine component of the Delta language.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef IDE_DELTA_VIRTUAL_MACHINE_COMPONENT_H_
#define IDE_DELTA_VIRTUAL_MACHINE_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "BinaryBuffer.h"
#include "ComponentHandle.h"
#include "IDEComponent.h"

#include "DeltaBreakPoint.h"
#include "UndefineAllWin32Shit.h"
#include "DeltaVMFuncReturnTypes.h"
#include "DebugClient.h"
#include <boost/thread/mutex.hpp>
#include <boost/foreach.hpp>

#pragma warning(disable: 4251)

class DeltaVirtualMachine;

namespace ide
{
	//----------------------------
	//-- class DeltaVM

	class DeltaVM :
		public IDEComponent
	{
		DECLARE_IDE_COMPONENT(DeltaVM);

	public:
		///--- type definitions

		///--- constructors / destructor
		DeltaVM(void);
		~DeltaVM(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));
		DECLARE_EXPORTED_STATIC(bool, CanDestroy, (void));

		DECLARE_EXPORTED_STATIC_(const String, LoadAndRun, (const String& binary, const String& source, const String& vmId),
			_("Loads and runs the script leaving it in an active state"));
		DECLARE_EXPORTED_STATIC_(bool, CallGlobalFunc, (const String& vmId, const String& func),
			_("Calls the global function of the script"));
		DECLARE_EXPORTED_STATIC_(bool, DeleteVM, (const String& vmId),
			_("Deletes the vm with the given id"));
		DECLARE_EXPORTED_STATIC_(StringList, ExtractBuildDependencies, (const String& source, const String& bytecodePath),
			_("Extract the byte code file dependencies (resulating from 'using') of a source file"));
		DECLARE_EXPORTED_STATIC_(void, RunConsole, (const String& uri, const String& options, const String& directory),
			_("Execute the specified script as console program, in a separate process"));
		DECLARE_EXPORTED_STATIC_(void, DebugConsole, (const String& binary, const String& source, const String& options, const String& directory),
			_("Start debugging the specified script as a console program, in a separate process"));
		DECLARE_EXPORTED_STATIC_(void, DebugConsoleWithConsoleDebugger, (const String& binary, const String& source, const String& options, const String& directory),
			_("Start debugging the specified script as a console program, in a separate process and with a separate console debugger"));
		DECLARE_EXPORTED_STATIC_(void, DebugLocalScript, (const String& cmdLine, const String& source, const String& directory),
			_("Start debugging of a script at a local executing host"));
		DECLARE_EXPORTED_STATIC_(unsigned long, DebugCompilerInvocation, (const String& uri, const String& options, const String& directory, const UIntList& buildId, const Handle& script),
			_("Start debugging of a compiler invocation"));
		DECLARE_EXPORTED_STATIC_(unsigned long, DebugAspectCompilerInvocation, (const String& uri, const StringList& transformations, const String& options, const String& directory, const UIntList& buildId, const Handle& script),
			_("Start debugging of a compiler invocation"));
		DECLARE_EXPORTED_STATIC_(void, DebugLocalScriptWithConsoleDebugger, (const String& cmdLine, const String& source, const String& directory),
			_("Start debugging of a script at a local executing host using a separate console debugger"));
		DECLARE_EXPORTED_STATIC_(void, DebugAttach, (const String& host, int port),
			_("Attach to the debugger running at a specific host"));
		DECLARE_EXPORTED_STATIC_(void, StopDebug, (void),
			_("Stop debugging of the currently running script"));


		DECLARE_EXPORTED_STATIC_(bool, ToggleBreakpoint, (const String& symbolic, int line),
			_("Toggle a breakpoint at the specified location. Returns true if breakpoint is set, false otherwise"));
		DECLARE_EXPORTED_STATIC_(bool, ToggleBreakpointStatus, (const String& symbolic, int line),
			_("Enable or disable a breakpoint at the specified location. Returns true if breakpoint status was toggled"));
		DECLARE_EXPORTED_STATIC_(void, AddBreakpoint, (const String& symbolic, int line, const String& condition),
			_("Insert a breakpoint at the specified location"));
		DECLARE_EXPORTED_STATIC_(void, RemoveBreakpoint, (const String& symbolic, int line),
			_("Remove a breakpoint at the specified location"));
		DECLARE_EXPORTED_STATIC_(void, EnableBreakpoint, (const String& symbolic, int line),
			_("Enable a breakpoint at the specified location"));
		DECLARE_EXPORTED_STATIC_(void, DisableBreakpoint, (const String& symbolic, int line),
			_("Disable a breakpoint at the specified location"));
		DECLARE_EXPORTED_STATIC_(void, ChangeBreakpointCondition, (const String& symbolic, int line, const String& condition),
			_("Change the condition of the breakpoint at the specified location"));
		DECLARE_EXPORTED_STATIC_(void, ChangeBreakpointLine, (const String& symbolic, int line, int newLine),
			_("Change the line of the breakpoint at the specified location"));

		DECLARE_EXPORTED_STATIC_(StringListList, GetBreakpoints, (const String& uri),
		_("Returns a list with the breakpoints for the given script (each breakpoint represented as a list of strings)"));
		DECLARE_EXPORTED_STATIC_(StringListList, GetAllBreakpoints, (void),
			_("Returns a list with all breakpoints in the workspace (each breakpoint represented as a list of strings)"));

		DECLARE_EXPORTED_STATIC_(void, DeleteAllSourceBreakpoints, (const String& symbolic),
			_("Delete all breakpoints of the given source"));
		DECLARE_EXPORTED_STATIC_(void, DeleteAllBreakpoints, (void),
			_("Delete all breakpoints in the workspace"));
		DECLARE_EXPORTED_STATIC_(void, EnableAllBreakpoints, (void),
			_("Enable all breakpoints in the workspace"));
		DECLARE_EXPORTED_STATIC_(void, DisableAllBreakpoints, (void),
			_("Disable all breakpoints in the workspace"));

		DECLARE_EXPORTED_STATIC_(bool, LoadBreakpoints, (const String& uri),
			_("Load breakpoint information from the given file"));
		DECLARE_EXPORTED_STATIC_(bool, SaveBreakpoints, (const String& uri),
			_("Saves current breakpoint information to the given file"));

		DECLARE_EXPORTED_STATIC_(StringBoolPair, EvalExpr, (const String& expr),
			_("Evaluate an expression returning its value in a string format"));
		DECLARE_EXPORTED_STATIC_(StringBoolPair, EvalExprEx, (const String& format, const String& expr),
			_("Evaluate an expression returning its contents in a string with the specified format"));
		DECLARE_EXPORTED_STATIC_(StringList, EvalExprList, (const StringList& exprs),
			_("Evaluate a list of expressions returning their values in a list of string formats"));
		DECLARE_EXPORTED_STATIC_(StringList, EvalExprListEx, (const String& format, const StringList& exprs),
			_("Evaluate a list of expressions returning their contents in a list of strings with the specified formats"));
		DECLARE_EXPORTED_STATIC_(ObjectGraphData, EvalObjectGraph, (const String& expr, int depth, bool directed),
			_("Evaluate the object graph of the given depth for the given expression"));

		DECLARE_EXPORTED_STATIC_(InputBinaryBuffer, EvalExprBinaryData, (const String& expr),
			_("Evaluate an expression for a native value and extra it a binary buffer"));
		DECLARE_EXPORTED_STATIC_(String, GetEvalExprBinaryDataError, (void),
			_("Returns the error message for the most recent respective evaluation"));

		DECLARE_EXPORTED_STATIC_(void, Go, (void),
			_("Continue the program execution"));
		DECLARE_EXPORTED_STATIC_(void, Break, (void),
			_("Break the program execution, stopping at the next instruction to be executed"));
		DECLARE_EXPORTED_STATIC_(void, ShowNextStatement, (void),
			_("Show the next statement to be executed"));
		DECLARE_EXPORTED_STATIC_(void, StepIn, (void),
			_("Make a step in the debugging, visiting function calls"));
		DECLARE_EXPORTED_STATIC_(void, SelectiveStepIn, (void),
			_("Make a selective step in the debugging, visiting the selected function call"));
		DECLARE_EXPORTED_STATIC_(void, StepOver, (void),
			_("Make a step in the debugging, without entering in any function call"));
		DECLARE_EXPORTED_STATIC_(void, StepOut, (void),
			_("Make a step in the debugging, exiting the current function"));
		DECLARE_EXPORTED_STATIC_(void, RunToCursor, (void),
			_("Run the program in debug mode, until the line of the cursor is reached"));

		DECLARE_EXPORTED_STATIC_(uint, GetTotalStackFrames, (void),
			_("Return total number of stack frames"));
		DECLARE_EXPORTED_STATIC_(uint, GetCurrentStackFrameIndex, (void),
			_("Return index of the current stack frame"));
		DECLARE_EXPORTED_STATIC_(String, GetStackFrame, (uint index),
			_("Return information description of the specified stack frame"));
		DECLARE_EXPORTED_STATIC_(void, MoveStackFrame, (uint index),
			_("Move the stack frame to the specified index"));
		DECLARE_EXPORTED_STATIC_(StringListList, GetCurrentContextVariables, (void),
			_("Return a list with the variables of the current context (each variable represented as a list of strings)"));
		
		DECLARE_EXPORTED_STATIC_(String, GetStopPointSource, (void),
			_("Return the stop point source file (used to set the trace arrow)"));
		DECLARE_EXPORTED_STATIC_(uint, GetStopPointLine, (void),
			_("Return the stop point line (used to set the trace arrow)"));

		DECLARE_EXPORTED_STATIC_(const String, GetURIFromSymbolic, (const String& symbolic),
			_("Extract a full uri from a symbolic"));
		DECLARE_EXPORTED_STATIC_(const String, GetSymbolicURIFromURI, (const String& uri),
			_("Extract a symbolic uri from a symbolic"));
		DECLARE_EXPORTED_STATIC_(const String, GetSymbolicURIFromEditor, (const Handle& editor),
			_("Extract a symbolic uri from the given editor"));
		DECLARE_EXPORTED_STATIC_(const Handle, GetEditorFromSymbolicURI, (const String& symbolic),
			_("Extract an editor from the given symbolic uri"));
		DECLARE_EXPORTED_STATIC_(void, GotoSymbolicDocument, (const String& symbolic, int line),
			_("Go to the specified line of the symbolic document"));

		DECLARE_EXPORTED_STATIC(void, Configure, (void));

		DECLARE_EXPORTED_STATIC(void, RecordFailedConfigAndExtensionScripts, (const String& uri, const String& error));
		DECLARE_EXPORTED_STATIC(void, ClearFailedConfigAndExtensionScripts, (void));
		DECLARE_EXPORTED_STATIC(const StringList, GetFailedConfigAndExtensionScriptURIs, (void));
		DECLARE_EXPORTED_STATIC(const StringList, GetFailedConfigAndExtensionScriptErrors, (void));

		DECLARE_EXPORTED_STATIC(const String, GetConfigScriptDirectory, (void));
		DECLARE_EXPORTED_STATIC(const String, GetExtensionScriptDirectory, (void));

		///--- exported SLOTS
		DECLARE_EXPORTED_STATIC_(void, OnEditorClosing, (const Handle& editor),
			_("Upon closing an editor check perform bookkeeping in case it contained dynamic code"));
		DECLARE_EXPORTED_STATIC_(void, OnEditorMarginMarksClicked, (const Handle& editor, int line),
			_("Upon clicking on the editor margin marks perform a ToggleBreakpoint"));
		DECLARE_EXPORTED_STATIC_(void, OnFileNameChanged, (const Handle& editor, const String& uri),
			_("Upon changing the file name of an editor load proper information for the editor"));
		DECLARE_EXPORTED_STATIC_(void, OnTreeItemSymbolicURIChanged, (const Handle& handle, const String& oldUri, const String& newUri),
			_("Upon changing the symbolic uri of a script make the appropriate changes for it's breakpoints"));
		DECLARE_EXPORTED_STATIC_(void, OnEditLinesChangedBy, (const Handle& editor, int fromLine, int offset),
			_("Upon changing lines during editing move breakpoints accordingly"));
		DECLARE_EXPORTED_STATIC_(void, OnScriptSourceAdded, (const Handle& script, const Handle& addedSource,
			const StringList& encodedLineMappings, const String& type, uint index),
			_("Upon addition of a script intermediate source"));
		DECLARE_EXPORTED_STATIC_(void, OnUserListSelected, (const Handle& editor, int listType, const String& selection),
			_("Notification about the user list selection (used for the selective step in)"));

	protected:
		//--- component signals
		virtual void ComponentAppliedChangedProperties (const conf::PropertyTable& old, const conf::PropertyIdVec& changed);
		virtual void ComponentAddedProperty(const std::string& id, conf::Property* prop);
	private:
		///--- private type definitions
		typedef boost::tuple<String, int, String>	Breakpoint;
		typedef std::list<Breakpoint>				BreakpointList;

		typedef boost::tuple<std::string, uint, uint, uint, std::string> FuncInfo;
		static bool	IsLibraryFunction (const FuncInfo& info);

			//tuple<function name & vmid, definition line, call line, scope level, call params>
		typedef std::pair<std::string, int> VarDeclInfo;	// <name, line>

		typedef boost::function<void (void)> EvaluationFunction;

		typedef boost::tuple<unsigned long, std::string, uint, String, String, bool> DebugServer;	//<pid, host, port, source, directory, isCompilation>
		typedef std::stack<DebugServer> DebugServerStack;

		enum DebugState {
			DEBUG_IDLE		= 0,
			DEBUG_TRYING	= 1,
			DEBUG_RUNNING	= 2,
			DEBUG_STOPPED	= 3
		};

		///--- private API
		void HandleNewProperties (const conf::PropertyIdVec& changed);

		static const DebugServer LocalDebugServer(unsigned long pid, const String& source, const String& directory, bool isCompilation);
		static void DebugProcess (unsigned long pid, const String& source, const String& directory, bool isCompilation);

		static const std::string GetExtensionAndConfigScriptsReservedByteCodeLoadingPath(void);
		static const std::string GetExtensionAndConfigScriptsReservedDllImportPath(void);

		static void SetToStringMaxLength (void);
		static void SetBreakOnThrownException (void);
		static void SetObjectGraphConfiguration (void);
		static void SetExpressionEvaluationFormat (void);

		static void ResumeDebug(void);

		static void ReceiveFunction(FuncInfo& info);
		static void UpdateReturnValue(const std::string& func, const std::string& value);
		static void ReceiveFuncResults(void);
		static void ReceiveVariables(const FuncInfo& info);
		static void ReceiveStack(const FuncInfo& topFunction);
		static bool StackDown(FuncInfo& info);
		static bool StackUp(FuncInfo& info);
		static void MultipleStackDown(FuncInfo& info, unsigned n);
		static void MultipleStackUp(FuncInfo& info, unsigned n);

		static void OnStopRequested(void);

		static void InitializeVM(void);
		static void CleanUpVM(void);

		typedef void (*DebugFunc)(const String& cmdLine, const String& source, const String& directory);
		static void DebugConsoleWithFunc(DebugFunc func, const String& binary, const String& source, const String& options, const String& directory);

		static bool InitializeDebugClient(const DebugServer& server, bool initial);
		static void CleanUpDebugClient (const DebugServer& server, bool final);
		static bool ActivateMostRecentDebugServer(bool previousFinished);
		static void DebugClientThread(const DebugServer& server);

		static void AddBreakpointSynchronously (const String& symbolic, int line, const String& condition);
		static void GenerateBreakpoints (const std::string original, const String& generated,  const StringList& lineMappings,
			void (*adderFunc)(const String&, int, const String&), BreakpointList& breakpoints, bool deleteOriginal = false);
		static void GeneratedFinalSourceBreakpoints (void);
		static void ClearBreakpointList (BreakpointList& breakpoints);
		static void RemoveSourceFromBreakpointList (BreakpointList& breakpoints, const String& symbolic);

		static void HandleBreakpointHit(void);
		static void HandleVMError(void);
		static void HandleVMWarning(void);
		static void HandleVMStop(void);
		static void	MessageConditionError(const String& cond, util_ui32 line);
		static void	MessageConditionSatisifed(const String& cond, util_ui32 line);
		static void HandleInvalidBreakpoint(void);
		static void HandleValidBreakpointAdded(void);
		static void CommitAddBreakpoint(const String& symbolic, int line, const String& condition);

		static const std::string MakeContext(const std::string& name, uint defLine);
		static const VarDeclInfo ExtractVarDeclInfo(const std::string& data);	//<name:line>

		static void CreateDynamicCodeFile(const String& uri, const String& code);

		///--- thread safe signal triggers
		static void onBreakpointAdded(const String& uri, const String& symbolic, int line, const String& condition);
		static void onBreakpointRemoved(const String& uri, const String& symbolic, int line, bool enabled);
		static void onBreakpointEnabled(const String& uri, const String& symbolic, int line);
		static void onBreakpointDisabled(const String& uri, const String& symbolic, int line);
		static void onBreakpointConditionChanged(const String& uri, const String& symbolic, int line, const String& condition);
		static void onBreakpointLineChanged(const String& uri, const String& symbolic, int line, int newLine);
		static void onBreakpointHit(const String& uri, const String& symbolic, int line);
		static void onFirstBreakpoint(void);
		static void onFirstBreakpointEnabled(void);
		static void onFirstBreakpointDisabled(void);
		static void onNoBreakpoints(void);
		static void onNoBreakpointsEnabled(void);
		static void onNoBreakpointsDisabled(void);

		static void onPushStackFrame(const String& record, uint defLine, uint callLine, uint scope, const String& params);
		static void onStackFrameMoved(uint index);
		static void onAddFunctionReturnValue(const String& func, const String& value);
		static void onAddVariable(const String& name, const String& value, int line, bool newValue);

		static void onDebugStarted(const String& uri);
		static void onDebugStopped(const String& uri);
		static void onDebugResumed(void);

		typedef std::vector<FuncInfo> CallStack;
		static CallStack callstack;

		typedef std::pair<String, int> StopPointInfo;	// <source, line>
		static StopPointInfo stopPoint;

		static BreakpointList	finalGeneratedBreakpoints, stageGeneratedBreakpoints;
		
		enum VariableState {
			VARIABLE_DEFAULT	= 0,
			VARIABLE_NEW		= 1,
			VARIABLE_MODIFIED	= 2
		};

		typedef boost::tuple<std::string, std::string, VariableState> VarInfo;
		typedef std::list<VarInfo> VariableList;
		typedef std::map<std::string, VariableList> VariableMap;
		static VariableMap variables;

		typedef std::pair<std::string, std::string> FuncReturnValue;
		typedef std::list<FuncReturnValue> FuncReturnValueList;
		static FuncReturnValueList returnValues;

		static std::string currentContext;

		static DeltaVirtualMachine* mainVM;
		static DeltaVirtualMachine* debugVM;
		static bool go;
		static bool stopped;

		static bool inGlobal;
		static bool inBreakPoint;				///< a breakpoint is hit
		static volatile bool waitingBreakpointValidation;
		static DebugState debugRunningState;	///< state of debugging VM

		static uint stackFrameIndex;	///< index of the currently active stack frame
		static uint guid;

		static DebugServerStack debugServers;
		static DebugServer currDebugServer;

		typedef std::map<String, String> VMIdSourceMapping;
		static VMIdSourceMapping vmIds;

		static StringList userSelectionList;
		static StringList failedURIs;
		static StringList failingURIErrors;
	};
}

#endif	//IDE_DELTA_VIRTUAL_MACHINE_COMPONENT_H_
