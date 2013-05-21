/**
 *	DeltaVM.cpp
 *
 *	-- IDE Delta Virtual Machine Component --
 *
 *	Virtual Machine component of the Delta language.
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	Bug finding and corrections by AS (Savidis) and JL (Lilis).
 *	June 2007. Continuous extensions by JL.
 *  Added support for console debugger invocation, AS, August 2009. 
 */
#include "DeltaVM.h"

#include "Call.h"
#include "ComponentFunctionCallerSafe.h"
#include "ComponentRegistry.h"
#include "ComponentFactory.h"
#include "ComponentUserCommandDesc.h"
#include "ComponentConfigurationDialog.h"
#include "PropertyUtils.h"
#include "DelayedCaller.h"
#include "GenericDialogs.h"
#include "StringUtils.h"
#include "DockableComponent.h"

#include "Streams.h"
#include "ConsoleHost.h"

#include "ControlAPI.h"
#include "BuildDependencies.h"
#include "ConstructionAPI.h"
#include "CommonGUIUtils.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaPureVMFacade.h"
#ifdef	DELTA_DEBUGGED_VM
#include "DeltaDebuggedVMFacade.h"
#else
#include "DeltaDebugDynamicActivator.h"
#endif
#include "DeltaDebugClientBreakPoints.h"
#include "DebugServer.h"
#include "DeltaVMRegistry.h"
#include "DeltaDebugExtensionsInit.h"
#include "uinit.h"
#include "ustrings.h"
#include "uerrorclass.h"

#include "DynamicCodeManager.h"
#include "DebugAsyncEvaluator.h"

#include "UndefineAllWin32Shit.h"
#include <wx/app.h>
#include <wx/filename.h>
#include "wx/wx.h"

#include <windows.h>
#include "UndefineAllWin32Shit.h"

#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/xtime.hpp>

#ifndef NO_VLD
#include <vld.h>
#endif

#define CONFIG_SCRIPTS_DIRECTORY		"config_scripts"
#define EXTENSION_SCRIPTS_DIRECTORY		"extension_scripts"

#define	SPARROW_OUTPUT_MESSAGE(msg)											\
		Call<void (const String&), SafeCall>(s_classId, "Output", "Append") \
		(_("\n") +  util::std2str(msg) + _("\n"))

#define CALL_DELAYED(call)	timer::DelayedCaller::Instance().PostDelayedCall(call)

#define SELECTIVE_STEP_IN_LIST 1	//Scintilla user list identifier (don't use 0, it is reserved)

namespace ide
{
	class WindowFocuser
	{
	private:
#ifdef WIN32
		static HWND window;

		static BOOL CALLBACK FindWindowCallback(HWND hwnd, LPARAM lParam)
		{
			DWORD pid = 0;
			if (GetWindowThreadProcessId(hwnd, &pid))
			{
				if (pid == (DWORD) lParam)
				{
					window = hwnd;
					return FALSE;
				}
			}
			return TRUE;
		} 
#endif	// WIN32

	public:
		static void FocusSparrow(void)
		{
#ifdef WIN32
			SetForegroundWindow((HWND) wxTheApp->GetTopWindow()->GetHandle());
#endif	// WIN32
		}

		static void FocusWindowByProcessId(unsigned long pid)
		{
#ifdef WIN32
			window = NULL;
			EnumWindows (FindWindowCallback, (LPARAM) pid);
			if (window)
				SetForegroundWindow(window);
#endif	// WIN32
		}
	};

#ifdef WIN32
	HWND WindowFocuser::window;
#endif	// WIN32

	//-------------------------------------------------------//
	//---- static members -----------------------------------//
			
	static Component *instance;
	static boost::mutex stackProcessingMutex;
	static boost::mutex selectiveStepMutex;
	static boost::mutex debugTryingMutex;
	static boost::mutex debugClientThreadFinishedMutex;
	static String debugClientThreadSource;
	static volatile bool debugClientThreadFinished = false;

	DeltaVM::StopPointInfo DeltaVM::stopPoint;
	DeltaVM::CallStack DeltaVM::callstack;
	DeltaVM::FuncReturnValueList DeltaVM::returnValues;
	DeltaVM::VariableMap DeltaVM::variables;
	std::string DeltaVM::currentContext;

	DeltaVirtualMachine* DeltaVM::debugVM	= 0;

	bool DeltaVM::go			= false;
	bool DeltaVM::stopped		= false;
	bool DeltaVM::inGlobal		= true;
	bool DeltaVM::inBreakPoint	= false;

	DeltaVM::DebugState	DeltaVM::debugRunningState = DeltaVM::DEBUG_IDLE;

	static bool shouldNotEnterTraceCall = false;

	uint DeltaVM::stackFrameIndex		= 0;
	uint DeltaVM::guid					= 0;

	DeltaVM::DebugServerStack DeltaVM::debugServers;
	DeltaVM::DebugServer DeltaVM::currDebugServer(0, std::string(), 0, String(), String(), false);

	DeltaVM::VMIdSourceMapping DeltaVM::vmIds;

	StringList DeltaVM::userSelectionList;
	StringList DeltaVM::failedURIs;
	StringList DeltaVM::failingURIErrors;


	//-------------------------------------------------------//
	//---- class DeltaVM ------------------------------------//

	COMPONENT_METADATA(
		DeltaVM,
		_("Delta Virtual Machine Invoker"),
		_("Runs the Delta VM to execute byte code"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("beta")
	);
	IMPLEMENT_COMPONENT_(DeltaVM, IDEComponent);

	//-----------------------------------------------------------------------

	EXPORTED_STATIC_SIGNAL(DeltaVM, PushStackFrame, (const String& record, uint defLine, uint callLine, uint scope, const String& params));
	EXPORTED_STATIC_SIGNAL(DeltaVM, StackFrameMoved, (uint index));
	EXPORTED_STATIC_SIGNAL(DeltaVM, AddFunctionReturnValue, (const String& func, const String& value));

	EXPORTED_STATIC_SIGNAL(DeltaVM, DebugStarted, (const String& uri));
	EXPORTED_STATIC_SIGNAL(DeltaVM, DebugStopped, (const String& uri));
	EXPORTED_STATIC_SIGNAL(DeltaVM, DebugResumed, (void));

	EXPORTED_STATIC_SIGNAL(DeltaVM, RunStarted, (const String& uri));
	EXPORTED_STATIC_SIGNAL(DeltaVM, RunStopped, (const String& uri));

	EXPORTED_STATIC_SIGNAL(DeltaVM, LibraryDefinitionsChanged, (const StringList& newDefinitions));
	EXPORTED_STATIC_SIGNAL(DeltaVM, ExpressionEvaluationFormatChanged, (const String& format));

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(DeltaVM, table)
	{
		using namespace conf;
		AggregateProperty* debugger = new AggregateProperty(_("Debugger"), _("Debugger configuration data"));
		debugger->AddProperty("tostring_maxlength", new IntProperty(_("Max debug string length"),
			64, _("The maximum number of characters for string conversion of values")));
		debugger->AddProperty("break_on_thrown_exception", new BoolProperty(_("Break on thrown exceptions"),
			false, _("Defines if thrown exceptions will trigger a break point")));
		debugger->AddProperty("receive_variables", new BoolProperty(_("Receive variables"),
			true, _("Flag indicating to receive or not the variables during debugging")));

		AggregateProperty* graph = new AggregateProperty(_("Object graph"), _("Object graph configuration data"));
		graph->AddProperty("expandLibraryFuncs", new BoolProperty(_("Expand Library Functions"), false,
			_("Flag indicating to expand library functions")));
		graph->AddProperty("expandMethodFuncs", new BoolProperty(_("Expand Method Functions"), false,
			_("Flag indicating to expand method functions")));
		graph->AddProperty("expandProgramFuncs", new BoolProperty(_("Expand Program Functions"), false,
			_("Flag indicating to expand program functions")));
		graph->AddProperty("expandFuncVMs", new BoolProperty(_("Expand Function VMs"), false,
			_("Flag indicating to expand the function's virtual machines")));
		debugger->AddProperty("object_graph", graph);

		StringVec options;
		options.push_back(_T("rc"));
		options.push_back(_T("xml"));
		options.push_back(_T("json"));
		EnumStringProperty* format = new EnumStringProperty(_("Expression Evaluation Format"), options, 0,
			_("Option for the communication format between debugger frontend and backend"));
		debugger->AddProperty("expression_evaluation_format", format);
		table.AddProperty("debugger", debugger);

		AggregateProperty* vm = new AggregateProperty(_("Virtual Machine"), _("Virtual machine configuration data"));
		AggregateProperty *listType = new AggregateProperty(_("Library definition"), _("Single library definition"));
		listType->AddProperty("name", new StringProperty(_("Name")));
		listType->AddProperty("path", new FileProperty(_("Path")));
		vm->AddProperty("libdefs", new AggregateListProperty(_T("Library registry"), listType,
			_T("Registry for the library file definitions")));

		table.AddProperty("vm", vm);
	}

	//-----------------------------------------------------------------------

	DeltaVM::DeltaVM(void)
	{
	}

	//-----------------------------------------------------------------------

	DeltaVM::~DeltaVM(void)
	{
	}

	//-----------------------------------------------------------------------

	static void VMErrorReport (const char* msg)
	{
		Call<void (const String&), SafeCall>("DeltaVM", "Output", "Append")(util::std2str(msg));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, Initialize, (void))
	{
		InitializeVM();
		Call<void (const String& path, const UserCommandDesc& desc)>("DeltaVM", "Shell", "AddCommand")(
			_("/{2}Configure/DeltaVM..."),
			UserCommandDesc(UserCommandDesc::Callback("DeltaVM", "Configure"), false, UC_MAIN, true, true, String())
		);
		instance = ComponentFactory::Instance().CreateComponent(s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, CleanUp, (void))
	{
		instance->Destroy();
		//FIXME: Call<void (const String& path)>("DeltaVM", "Shell", "RemoveCommand")(_("/Configure/Debugger..."));
		CleanUpVM();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, bool, CanDestroy, (void))
	{
		bool inactive;
		{
			boost::mutex::scoped_lock lock(debugClientThreadFinishedMutex);
			inactive = debugRunningState == DEBUG_IDLE;
			debugClientThreadFinished = false;
		}

		if (inactive)
			return true;
		else if (gui::getConfirmation(0, _("DeltaVM"), 
			_("A debugging session is currently active. Do you wish to stop debugging?"))
		){
			bool forcedStop;
			{
				boost::mutex::scoped_lock lock(debugClientThreadFinishedMutex);
				forcedStop = debugClientThreadFinished == false;
			}
			StopDebug();
			while(!debugClientThreadFinished)
				Sleep(100);
			if (forcedStop) {
				onDebugResumed();
				onDebugStopped(debugClientThreadSource);
				debugClientThreadSource.clear();
			}
			return true;
		}
		else
			return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, const String, LoadAndRun, (const String& binary, const String& source, const String& vmId))
	{
		//-- Load binary code
		const std::string basename = util::str2std(wxFileName(binary).GetName());	
		const std::string id = vmId.empty() ?
			basename + "::" + boost::lexical_cast<std::string>(++guid) : util::str2std(vmId);
		DeltaVirtualMachine* vm = DNEWCLASS(DeltaVirtualMachine, (id.c_str()));
		String error;
		if (!DPTR(vm)->Load(util::str2std(binary).c_str())) {
			DDELETE(vm);
			DASSERT(UERROR_ISRAISED());
			error = util::std2str(UERROR_GETREPORT());
			UERROR_CLEAR();
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")(error);
		}
		else {
			//-- Start running VM
			DeltaVirtualMachine::SetErrorCallback(VMErrorReport);

			util_ui32 sn = DPTR(vm)->GetSerialNo();
			DPTR(vm)->PushSerialNoPointer(&sn);
			DPTR(vm)->Run();

			if (sn) {
				DPTR(vm)->PopSerialNoPointer(&sn);
				if (DPTR(vm)->HasProducedError() && (DELTA_NO_VM_CALLER_FAILED)) {
					DASSERT(UERROR_ISRAISED());
					error = util::std2str(UERROR_GETREPORT());
					DeltaVirtualMachine::ResetRunTimeErrors();
					DDELETE(vm);
				}
				else if (!source.empty())
					vmIds[util::std2str(id)] = source;
				UERROR_CLEAR();
			}
			else {
				DASSERT(UERROR_ISRAISED());
				error = util::std2str(UERROR_GETREPORT());
				DBGOUT << error << DBGENDL;
				gui::displayMessage(
					0, 
					_("Delta Sparrow Script Runtime Error"),
					_("Sparrow IDE script '")									+ 
					binary														+ 
					_T("' unexpectedly terminated due to a runtime error:\n\n")	+
					error														+
					_T("\n")
				);
				UERROR_CLEAR();
			}
		}
		if (error.empty())
			return util::std2str(id);
		else {
			RecordFailedConfigAndExtensionScripts(source, error);
			return String();
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, bool, CallGlobalFunc, (const String& vmId, const String& func))
	{
		bool retval = false;
		if (DeltaVirtualMachine* vm = VMRegistry().Get(util::str2std(vmId).c_str())) {
			const std::string cfunc = util::str2std(func);
			String error;
			if (DPTR(vm)->GlobalFuncExists(cfunc.c_str())) {
				util_ui32 sn = DPTR(vm)->GetSerialNo();
				DPTR(vm)->PushSerialNoPointer(&sn);
				DPTR(vm)->ExtCallGlobalFunc(cfunc.c_str());
				if (sn) {
					DPTR(vm)->PopSerialNoPointer(&sn);
					if (DPTR(vm)->HasProducedError() && (DELTA_NO_VM_CALLER_FAILED)) {
						DASSERT(UERROR_ISRAISED());
						error = util::std2str(UERROR_GETREPORT());
						DeltaVirtualMachine::ResetRunTimeErrors();
					}
					UERROR_CLEAR();
					retval = true;
				}
				else {
					DASSERT(UERROR_ISRAISED());
					error = util::std2str(UERROR_GETREPORT());
					DBGOUT << error << DBGENDL;
					gui::displayMessage(
						0, 
						_("Delta Sparrow Script Runtime Error"),
						_("Sparrow IDE script '")									+ 
						vmId														+ 
						_T("' unexpectedly terminated due to a runtime error:\n\n")	+
						error														+
						_T("\n")
					);
					UERROR_CLEAR();
				}
			}
			if (!error.empty()) {
				VMIdSourceMapping::const_iterator i = vmIds.find(vmId);
				const String uri = i == vmIds.end() ? String() : i->second;
				RecordFailedConfigAndExtensionScripts(uri, error);
			}
		}
		return retval;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, bool, DeleteVM, (const String& vmId))
	{
		if (DeltaVirtualMachine* vm = VMRegistry().Get(util::str2std(vmId).c_str())) {
			DDELETE(vm);
			return true;
		}
		else
			return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, StringList, ExtractBuildDependencies, (const String& source, const String& bytecodePath))
	{
		typedef DeltaBuildDependencies::Dependencies Dependencies;
		DeltaBuildDependencies::Dependencies deps;
		DeltaBuildDependencies dependencies;
		dependencies.Extract(util::str2std(bytecodePath), util::str2std(source), &deps);

		StringList result;
		for (Dependencies::iterator i = deps.begin(); i != deps.end(); ++i) {
			result.push_back(util::std2str(i->first));
			switch (i->second) {
				case DeltaBuildDependencies::NotFound	: result.push_back(_T(DELTA_DEPENDENCY_NOT_FOUND));	break;
				case DeltaBuildDependencies::OneFound	: result.push_back(_T(DELTA_DEPENDENCY_ONE_FOUND));	break;
				case DeltaBuildDependencies::ManyFound	: result.push_back(_T(DELTA_DEPENDENCY_MANY_FOUND));	break;
				default : assert(false);
			}
		}
		return result;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, RunConsole, (const String& uri, const String& options, const String& directory))
	{
#ifndef NDEBUG
		const std::string executable = "DeltaVM_d";
#else
		const std::string executable = "DeltaVM";
#endif
		const std::string curi = util::str2std(uri);
		const std::string coptions = util::str2std(options);
		const std::string cdirectory = util::str2std(directory);
		const std::string sparrowDir = IDECore::GetInstallationDir();
		util::ConsoleHost().Execute(sparrowDir + executable + " \"" + curi + "\"" + coptions, cdirectory);
		Call<void (const String&), SafeCall>(s_classId, "Output", "Append")(_("Running externally: ") + uri + _T("\n"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, DebugConsole, (const String& binary, const String& source, const String& options, const String& directory))
	{
		DebugConsoleWithFunc(DeltaVM::DebugLocalScript, binary, source, options, directory);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, DebugConsoleWithConsoleDebugger, (const String& binary, const String& source, const String& options, const String& directory))
	{
		DebugConsoleWithFunc(DeltaVM::DebugLocalScriptWithConsoleDebugger, binary, source, options, directory);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, DebugLocalScriptWithConsoleDebugger, (const String& cmdLine, const String& source, const String& directory))
	{
#ifndef NDEBUG
		const String debugger = _T("DeltaConsoleDebugger_d");
#else 
		const String debugger = _T("DeltaConsoleDebugger");
#endif
		const String sparrowDir	= util::std2str(IDECore::GetInstallationDir());

		util_ui32 negotiationPort = DeltaDebugClient::ServerPortNegotiation::Start();
		if (!negotiationPort) {
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")(
				_("Unable to open a negotiation port with the debuggee.")
			);
			return;
		}
		
		unsigned long pid = util::ConsoleHost().Execute(
			util::str2std(cmdLine) + DeltaDebugClient::ServerPortNegotiation::ToArg(negotiationPort),
			util::normalizepath(util::str2std(directory))
		);

		if (pid) {
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")(
				_T("Debugging (via console debugger)") + source + _T(".\n")
			);

			// Run the console debugger supplying as argument the server port. The console
			// debugger will run on the project directory.

			if (util_ui32 serverPort = DeltaDebugClient::ServerPortNegotiation::Finish()) {

				std::string breakpoints = util::str2std(directory);
				char		last		= breakpoints[breakpoints.length() - 1];

				if (last != '\\' && last != '/' )
					breakpoints.append("\\");

				breakpoints.append("BreakpointsConsole.txt");
				DeltaDebugClientBreakPoints::Get().StoreText(breakpoints.c_str());

				// Make the command line for console debugger execution.

				const String cmmdLine =	sparrowDir	+
										debugger	+
										util::std2str(
											uconstructstr(
												" --port %u --breakpoints \"%s\"", 
												serverPort, 
												breakpoints.c_str()
											)
										);

				if (!util::ConsoleHost().Execute(util::str2std(cmmdLine), util::normalizepath(util::str2std(directory))))
					Call<void (const String&), SafeCall>(s_classId, "Output", "Append")(
						_T("Failed to launch the console debugger.\n")
					);
			}
			else
				Call<void (const String&), SafeCall>(s_classId, "Output", "Append")(
					_T("Could not get the debuggee server port. Try to attach manually.\n")
				);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, DebugLocalScript, (const String& cmdLine, const String& source, const String& directory))
	{
		boost::mutex::scoped_lock lock(debugTryingMutex);
		if (debugRunningState != DEBUG_IDLE)
			return;

		util_ui32 negotiationPort = DeltaDebugClient::ServerPortNegotiation::Start();
		if (!negotiationPort)
		{
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
				(_("Unable to open a negotiation port."));
			DeltaDebugClient::ServerPortNegotiation::Finish();
			return;
		}
		unsigned long pid = util::ConsoleHost().Execute(
			util::str2std(cmdLine) + DeltaDebugClient::ServerPortNegotiation::ToArg(negotiationPort),
			util::normalizepath(util::str2std(directory))
		);
		DebugProcess(pid, source, directory, false);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, unsigned long, DebugCompilerInvocation, (const String& uri, const String& options,
		const String& directory, const UIntList& buildId, const Handle& script))
	{
		boost::mutex::scoped_lock lock(debugTryingMutex);
		util_ui32 negotiationPort = DeltaDebugClient::ServerPortNegotiation::Start();
		if (!negotiationPort)
		{
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
				(_("Unable to open a negotiation port."));
			DeltaDebugClient::ServerPortNegotiation::Finish();
			return 0;
		}

		const String debugOptions = _T(" --debugged ") + util::std2str(DeltaDebugClient::ServerPortNegotiation::ToArg(negotiationPort));

		unsigned long pid  = Call<unsigned long (const String&, const String&, const String&, const UIntList&, const Handle&)>
			(script.Resolve(), "DeltaCompiler", "Compile")(uri, debugOptions + options, directory, buildId, script);
		DebugProcess(pid, uri, directory, true);
		return pid;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, unsigned long, DebugAspectCompilerInvocation, (const String& uri, const StringList& transformations,
		const String& options, const String& directory, const UIntList& buildId, const Handle& script))
	{
		boost::mutex::scoped_lock lock(debugTryingMutex);
		util_ui32 negotiationPort = DeltaDebugClient::ServerPortNegotiation::Start();
		if (!negotiationPort)
		{
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
				(_("Unable to open a negotiation port."));
			DeltaDebugClient::ServerPortNegotiation::Finish();
			return 0;
		}

		const String debugOptions = _T(" --debugged ") + util::std2str(DeltaDebugClient::ServerPortNegotiation::ToArg(negotiationPort));

		unsigned long pid  = Call<unsigned long (const String&, const StringList&, const String&, const String&, const UIntList&, const Handle&)>
			(script.Resolve(), "DeltaCompiler", "AspectTransformation")(uri, transformations, debugOptions + options, directory, buildId, script);
		DebugProcess(pid, uri, directory, false);
		return pid;
	}

	//-----------------------------------------------------------------------

	const DeltaVM::DebugServer DeltaVM::LocalDebugServer(unsigned long pid, const String& source, const String& directory, bool isCompilation) {
		util_ui32 serverPort = DeltaDebugClient::ServerPortNegotiation::Finish();
		String dir;
		if (!(dir = util::normalizepath(directory)).empty() && dir.GetChar(dir.length() - 1) != _T('/')	)
			dir += _T('/');
		return DebugServer(pid, "localhost", serverPort, source, directory, isCompilation);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::DebugProcess (unsigned long pid, const String& source, const String& directory, bool isCompilation)
	{
		const DebugServer info = LocalDebugServer(pid, source, directory, isCompilation);
		if (info.get<0>()) {
			debugServers.push(info);
			if (debugServers.size() == 1) {
				Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
					(_T("Debugging ") + source + _T("\n"));
				if (info.get<2>()) {
					currDebugServer = info;
					debugRunningState = DEBUG_TRYING;
					CALL_DELAYED(boost::bind(&DeltaVM::onDebugStarted, source));
					boost::thread debugClient(boost::bind(&DeltaVM::DebugClientThread, currDebugServer));
					while (debugRunningState == DEBUG_TRYING)	// Busy wait.
						Sleep(100);
				}
				else {
					Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
						(_("Can not determine debug server port. Try to attach manually.\n"));
					debugServers.pop();
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, DebugAttach, (const String& host, int port))
	{
		if (debugRunningState != DEBUG_IDLE)
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
				(_("Cannot attach while a debug session is active\n"));
		else {
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
				(_("Debugging remote script at host: ") + host + _T("\n"));

			const String source = _T("remote script");
			debugRunningState = DEBUG_TRYING;
			onDebugStarted(source);
			DebugServer server(0, util::str2std(host), uint(port), source, String());
			boost::thread debugClient(boost::bind(&DeltaVM::DebugClientThread, server));
			while (debugRunningState == DEBUG_TRYING)	// Busy wait.
				Sleep(100);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, StopDebug, (void))
	{
		if (debugRunningState == DEBUG_TRYING || debugRunningState == DEBUG_RUNNING)
		{
			debugRunningState = DEBUG_STOPPED;
			DeltaDebugClient::DoStop();

			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
				(_("\nExecution stopped by user!\n"));
		}
	}

	//-----------------------------------------------------------------------

#define	RETURN_EMPTY
#define	SAFE_TRACECOMMAND_IGNORE(ret) \
		if (debugRunningState != DEBUG_RUNNING || !inBreakPoint || shouldNotEnterTraceCall ) \
			return ret;

	EXPORTED_STATIC(DeltaVM, void, Go, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(RETURN_EMPTY);
		shouldNotEnterTraceCall = true;

		DeltaDebugClient::DoGo();
		ResumeDebug();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, Break, (void))
	{
		//Do NOT use the SAFE_TRACE_COMMAND_IGNORE here.
		if (debugRunningState == DEBUG_RUNNING && !inBreakPoint)
			DeltaDebugClient::DoBreakExecution();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, ShowNextStatement, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(RETURN_EMPTY);
		MoveStackFrame(callstack.size() - 1);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, StepIn, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(RETURN_EMPTY);
		shouldNotEnterTraceCall = true;

		DeltaDebugClient::DoStepIn();
		ResumeDebug();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, SelectiveStepIn, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(RETURN_EMPTY);
		shouldNotEnterTraceCall = true;

		typedef std::pair<std::string, std::string> StringPair;
		typedef std::list<StringPair> StringPairList;
		StringPairList calls;

		{
			boost::mutex::scoped_lock lock(selectiveStepMutex);
			DeltaDebugClient::DoGetAllPossibleCalls();
			DeltaDebugClient::WaitAnyMessage();
			DeltaDebugClient::GetInfoAllPossibleCalls(calls);
			DeltaDebugClient::ResponseProcessed();
		}

		if (calls.empty())
		{
			DeltaDebugClient::DoStepIn();	//Normal StepIn
			ResumeDebug();
		}
		else
		{
			BOOST_FOREACH(const StringPair& pair, calls)
				userSelectionList.push_back(
					util::std2str(pair.first) + _T(" (") + util::std2str(pair.second) + _T(")")
				);
			GotoSymbolicDocument(stopPoint.first, stopPoint.second);
			Call<void (int, const StringList&), SafeCall>(s_classId, "Editor", "UserListShow")
				(SELECTIVE_STEP_IN_LIST, userSelectionList);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, StepOver, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(RETURN_EMPTY);
		shouldNotEnterTraceCall = true;

		DeltaDebugClient::DoStepOver();
		ResumeDebug();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, StepOut, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(RETURN_EMPTY);
		shouldNotEnterTraceCall = true;

		DeltaDebugClient::DoStepOut();
		ResumeDebug();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, RunToCursor, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(RETURN_EMPTY);
		shouldNotEnterTraceCall = true;

		//-- Get editor's current line
		Component* editorMgr = ComponentRegistry::Instance().GetFocusedInstance("EditorManager");
		if (!editorMgr)
			return;
		Handle editor = Call<const Handle& (void)>(s_classId, editorMgr, "GetFocusedEditor")();
		int line = Call<int (void)>(s_classId, editor, "GetCurrentLine")();

		DeltaDebugClient::DoRunTo(line);
		ResumeDebug();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, StringBoolPair, EvalExpr, (const String& expr))
	{
		SAFE_TRACECOMMAND_IGNORE(StringBoolPair(String(), false));
		return	DebugAsyncEvaluator::Eval(
					EvalExprAsyncEvaluation(
						boost::bind(&DeltaDebugClient::DoGetExpr, util::str2std(expr))
					)
				);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, StringBoolPair, EvalExprEx, (const String& format, const String& expr))
	{
		SAFE_TRACECOMMAND_IGNORE(StringBoolPair(String(), false));

		return  DebugAsyncEvaluator::Eval(
					EvalExprAsyncEvaluation(
						boost::bind(&DeltaDebugClient::DoGetExprTypeData, util::str2std(format), util::str2std(expr))
					)
				);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, StringList, EvalExprList, (const StringList& exprs))
	{
		SAFE_TRACECOMMAND_IGNORE(StringList());
		StdStringList stdExprs;
		BOOST_FOREACH(const String& expr, exprs)
			stdExprs.push_back(util::str2std(expr));

		return	DebugAsyncEvaluator::Eval(
					EvalMultipleExprAsyncEvaluation(
						boost::bind(&DeltaDebugClient::DoGetExprMany, stdExprs)
					)
				);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, StringList, EvalExprListEx, (const String& format, const StringList& exprs))
	{
		SAFE_TRACECOMMAND_IGNORE(StringList());
		StdStringList stdExprs;
		BOOST_FOREACH(const String& expr, exprs)
			stdExprs.push_back(util::str2std(expr));

		return	DebugAsyncEvaluator::Eval(
					EvalMultipleExprAsyncEvaluation(
						boost::bind(DeltaDebugClient::DoGetExprTypeDataMany, util::str2std(format), stdExprs)
					)
				);
	}

	//-----------------------------------------------------------------------

	static String evalExprBinaryDataError;

	EXPORTED_STATIC(DeltaVM, InputBinaryBuffer, EvalExprBinaryData, (const String& expr)) {

		SAFE_TRACECOMMAND_IGNORE(InputBinaryBuffer());

		evalExprBinaryDataError.clear();
		EvalBinaryDataAsyncEvaluation eval(boost::bind(DeltaDebugClient::DoGetBinaryData, util::str2std(expr)));
		const InputBinaryBuffer& result = DebugAsyncEvaluator::Eval(eval);
		if (eval.HasError())
			evalExprBinaryDataError = util::std2str(eval.GetError());
		return result;
	}

	EXPORTED_STATIC(DeltaVM, String, GetEvalExprBinaryDataError, (void)) 
		{ return evalExprBinaryDataError; }

	EXPORTED_STATIC(DeltaVM, ObjectGraphData, EvalObjectGraph, (const String& expr, int depth, bool directed))
	{
		SAFE_TRACECOMMAND_IGNORE(ObjectGraphData());

		return	DebugAsyncEvaluator::Eval(
					EvalGraphAsyncEvaluation(
						boost::bind(DeltaDebugClient::DoGetObjectGraph, util::str2std(expr), depth, directed)
					)
				);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, uint, GetTotalStackFrames, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(0);

		return callstack.size();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, uint, GetCurrentStackFrameIndex, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(0);

		return stackFrameIndex;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, String, GetStackFrame, (uint index))
	{
		SAFE_TRACECOMMAND_IGNORE(String());

		//Don't assert because commands can be invoked from the introspection with invalid arguments
		if (index >= callstack.size())
			return String();

		return
			util::std2str(boost::get<0>(callstack[index])) + _T("#") +
			boost::lexical_cast<String>(boost::get<1>(callstack[index])) + _T("#") +
			boost::lexical_cast<String>(boost::get<2>(callstack[index])) + _T("#") +
			boost::lexical_cast<String>(boost::get<3>(callstack[index])) + _T("#") +
			GetURIFromSymbolic(util::std2str(boost::get<4>(callstack[index])));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, MoveStackFrame, (uint index))
	{
		SAFE_TRACECOMMAND_IGNORE(RETURN_EMPTY);
		shouldNotEnterTraceCall = true;
		boost::mutex::scoped_lock lock(stackProcessingMutex);

		// Do not 'assert(index < callstack.size())' because commands can
		// be invoked from the introspection with invalid arguments.

		if (index < callstack.size() && index != stackFrameIndex) {

			FuncInfo funcInfo;
			if (index < stackFrameIndex)
				DeltaVM::MultipleStackDown(funcInfo, stackFrameIndex - index);
			else
				DeltaVM::MultipleStackUp(funcInfo, index - stackFrameIndex);
			stackFrameIndex = index;

			ReceiveVariables(funcInfo);
		}

		if (index < callstack.size())	//valid stack frame movement
			CALL_DELAYED(boost::bind(&DeltaVM::onStackFrameMoved, index));
		
		shouldNotEnterTraceCall = false;
}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, StringListList, GetCurrentContextVariables, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(StringListList());
		
		DASSERT(variables.find(currentContext) != variables.end());
		StringListList vars;
		BOOST_FOREACH(VarInfo& varInfo, variables[currentContext]) {
			if (varInfo.get<0>() == DBG_GETVARIABLES_NAME_IS_GROUP) {
				StringList group;
				group.push_back(util::std2str(varInfo.get<0>()));
				group.push_back(util::std2str(varInfo.get<1>()));
				vars.push_back(group);
			}
			else {
				VarDeclInfo info = ExtractVarDeclInfo(varInfo.get<0>());
				if (!info.first.empty()) {
					StringList var;
					var.push_back(util::std2str(info.first));
					var.push_back(util::std2str(varInfo.get<1>()));
					var.push_back(boost::lexical_cast<String>(info.second));
					var.push_back(boost::lexical_cast<String>((int)varInfo.get<2>()));
					vars.push_back(var);
				}
			}
		}
		return vars;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, String, GetStopPointSource, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(String());

		return stopPoint.first;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, uint, GetStopPointLine, (void))
	{
		SAFE_TRACECOMMAND_IGNORE(0);

		return stopPoint.second;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, const String, GetURIFromSymbolic, (const String& symbolic))
	{
		const Handle& script = Call<const Handle& (const String&)>(s_classId, "ProjectManager", "GetResourceBySymbolicURI")(symbolic);
		if (script)	{		//first try for a script within the wsp
			//TODO: this code corresponds to the final source version getting the same symbolic as the original but this
			//is not the case right now. Determine if the symbolic should be the same or different and change accordingly.
			//if (debugRunningState == DEBUG_RUNNING)
			//	return Call<const String& (void)>(s_classId, script, "GetFinalSourceURI")();
			//else
			return Call<const String& (void)>(s_classId, script, "GetURI")();
		}

		const String comp = Call<const String (const String&)>(s_classId, "DeltaComponentDirectory", "GetComponentFromSymbolicURI")(symbolic);
		if (!comp.empty())	//then try for a sparrow component
			return Call<const String (const String&)>(s_classId, "DeltaComponentDirectory", "GetURIFromComponent")(comp);

		const String dynamicURI = util::std2str(DynamicCodeManager::GetURI(util::str2std(symbolic)));
		if (!dynamicURI.empty())	//then try for a dynamic source
			return dynamicURI;
		
		return currDebugServer.get<4>() + symbolic;	//finally assume an external script residing at the executing directory
	}


	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, const String, GetSymbolicURIFromURI, (const String& uri))
	{
		const Handle& script = Call<const Handle& (const String&)>(s_classId, "ProjectManager", "GetResourceByURI")(uri);
		if (script)
			return Call<const String& (void)>(s_classId, script, "GetSymbolicURI")();
		else
			return util::std2str(DynamicCodeManager::GetSymbolicURI(util::str2std(uri)));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, const String, GetSymbolicURIFromEditor, (const Handle& editor))
	{
		//Don't assert because commands can be invoked from the introspection with invalid arguments
		if (editor.GetClassId() != "Editor")
			return String();

		const std::string symbolic = DynamicCodeManager::GetSymbolicURI(editor);
		if (!symbolic.empty())
			return util::std2str(symbolic);

		Handle script = Call<const Handle& (void)>(s_classId, editor, "GetAssociatedComponent")();
		if (!script) {
			const String& uri = Call<const String& (void)>(s_classId, editor, "GetURI")();
			script = Call<const Handle& (const String&)>(s_classId, "ProjectManager", "GetResourceByURI")(uri);
		}
		return script ? Call<const String& (void)>(s_classId, script, "GetSymbolicURI")() : String();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, const Handle, GetEditorFromSymbolicURI, (const String& symbolic))
	{
		const Handle editor = DynamicCodeManager::GetEditor(util::str2std(symbolic));
		if (editor.GetSerial())
			return editor;

		const String uri = GetURIFromSymbolic(symbolic);
		return uri.empty() ?
			Handle() : Call<const Handle& (const String&)>(s_classId, "EditorManager", "GetEditor")(uri);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, GotoSymbolicDocument, (const String& symbolic, int line))
	{
		DockableComponent::EnsureVisibility("EditorManager");
		const String uri = GetURIFromSymbolic(symbolic);
		if (!uri.empty())
			Call<void (const String&, int)>(s_classId, "EditorManager", "GotoDocument")(uri, line);
		else {
			Handle editor = DynamicCodeManager::GetEditor(util::str2std(symbolic));
			if (!editor && debugRunningState == DEBUG_RUNNING) {
				std::string code;
				DeltaDebugClient::DoGetDynamicCode();
				DeltaDebugClient::WaitAnyMessage();
				DeltaDebugClient::GetInfoDynamicCode(&code);
				DeltaDebugClient::ResponseProcessed();
				if (!code.empty()) {
					DynamicCodeManager::Register(util::str2std(symbolic), code);
					editor = DynamicCodeManager::GetEditor(util::str2std(symbolic));
				}
			}
			if (editor) {
				Call<void (const Handle&)>(s_classId, "EditorManager", "FocusEditor")(editor);
				Call<void (int)>(s_classId, editor, "GotoLine")(line);
				Call<void (int)>(s_classId, editor, "EnsureLineVisible")(line);
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, Configure, (void))
		{ ComponentConfigurationDialog().Configure(s_classId); }

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, RecordFailedConfigAndExtensionScripts, (const String& uri, const String& error))
	{ 
		if (!uri.empty())
			failedURIs.push_back(uri);
		failingURIErrors.push_back(error);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, void, ClearFailedConfigAndExtensionScripts, (void))
	{
		failedURIs.clear();
		failingURIErrors.clear();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, const StringList, GetFailedConfigAndExtensionScriptURIs, (void))
		{ return failedURIs; }

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, const StringList, GetFailedConfigAndExtensionScriptErrors, (void))
		{ return failingURIErrors; }

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, const String, GetConfigScriptDirectory, (void))
		{ return util::std2str(IDECore::GetInstallationDir() + CONFIG_SCRIPTS_DIRECTORY + "/"); }

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaVM, const String, GetExtensionScriptDirectory, (void))
		{ return util::std2str(IDECore::GetInstallationDir() + EXTENSION_SCRIPTS_DIRECTORY + "/"); }

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(DeltaVM, void, OnEditorClosing, (const Handle& editor), "EditorClosing")
	{
		DynamicCodeManager::Cancel(editor);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(DeltaVM, void, OnUserListSelected,
		(const Handle& editor, int listType, const String& selection), "UserListSelected")
	{
		if (listType == SELECTIVE_STEP_IN_LIST)
		{
			if (selection.empty())
				shouldNotEnterTraceCall = false;
			else
			{
				StringList::iterator i = std::find(userSelectionList.begin(), userSelectionList.end(), selection);
				DASSERT(i != userSelectionList.end());
				DeltaDebugClient::DoSelectiveStepIn(std::distance(userSelectionList.begin(), i));
				ResumeDebug();
			}
			userSelectionList.clear();
		}
	}

	//-----------------------------------------------------------------------

	void DeltaVM::ComponentAppliedChangedProperties (const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
	{
		HandleNewProperties(changed);
		IDEComponent::ComponentAppliedChangedProperties(old, changed);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::ComponentAddedProperty(const std::string& id, conf::Property* prop)
	{
		HandleNewProperties(conf::PropertyIdVec(1, id));
		IDEComponent::ComponentAddedProperty(id, prop);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::HandleNewProperties (const conf::PropertyIdVec& changed) {
		using namespace conf;
		if (std::find(changed.begin(), changed.end(), "vm.libdefs") != changed.end()) {
			const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(s_classId);
			const AggregateProperty* vm = safe_prop_cast<const AggregateProperty>(entry.GetProperty("vm"));
			const AggregateListProperty* libdefs = safe_prop_cast<const AggregateListProperty>(vm->GetProperty("libdefs"));

			StringList newDefinitions;
			BOOST_FOREACH(const AggregateProperty* p, libdefs->GetPropertyList())
				newDefinitions.push_back(get_prop_value<StringProperty>(p->GetProperty("name")));
			sigLibraryDefinitionsChanged(newDefinitions);
		}

		if (debugRunningState == DEBUG_RUNNING) {
			bool debuggerChanged = std::find(changed.begin(), changed.end(), "debugger") != changed.end();
			if (debuggerChanged || std::find(changed.begin(), changed.end(), "debugger.tostring_maxlength") != changed.end())
				SetToStringMaxLength();
			if (debuggerChanged || std::find(changed.begin(), changed.end(), "debugger.break_on_thrown_exception") != changed.end())
				SetBreakOnThrownException();
			if (debuggerChanged || std::find(changed.begin(), changed.end(), "debugger.object_graph") != changed.end())
				SetObjectGraphConfiguration();
			if (debuggerChanged || std::find(changed.begin(), changed.end(), "debugger.expression_evaluation_format") != changed.end())
				SetExpressionEvaluationFormat();
		}
	}

	//-----------------------------------------------------------------------

	const std::string DeltaVM::GetExtensionAndConfigScriptsReservedByteCodeLoadingPath(void)
	{
		const std::string sparrowDir = IDECore::GetInstallationDir();
		return	sparrowDir + "scripts/bin;"					+ 
				sparrowDir + CONFIG_SCRIPTS_DIRECTORY + ";"	+ 
				sparrowDir + EXTENSION_SCRIPTS_DIRECTORY;
	}

	//-----------------------------------------------------------------------

	const std::string DeltaVM::GetExtensionAndConfigScriptsReservedDllImportPath(void)
	{
		return IDECore::GetInstallationDir() + "extension_dlls";
	}

	//-----------------------------------------------------------------------

	void DeltaVM::SetToStringMaxLength(void)
	{
		using namespace conf;
		const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(s_classId);
		const AggregateProperty* debugger = safe_prop_cast<const AggregateProperty>(entry.GetProperty("debugger"));
		const Property *maxLenProp = debugger->GetProperty("tostring_maxlength");
		util_i32 maxLen = conf::get_prop_value<conf::IntProperty>(maxLenProp);
		DeltaDebugClient::DoSetToStringMaxLength(maxLen);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::SetBreakOnThrownException(void)
	{
		using namespace conf;
		const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(s_classId);
		const AggregateProperty* debugger = safe_prop_cast<const AggregateProperty>(entry.GetProperty("debugger"));
		if (const Property *p = debugger->GetProperty("break_on_thrown_exception")) {
			bool val = conf::get_prop_value<conf::BoolProperty>(p);
			DeltaDebugClient::DoSetBreakOnThrownException(val);
		}
	}

	//-----------------------------------------------------------------------

	void DeltaVM::SetObjectGraphConfiguration(void)
	{
		using namespace conf;
		const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(s_classId);
		const AggregateProperty* debugger = safe_prop_cast<const AggregateProperty>(entry.GetProperty("debugger"));
		const AggregateProperty* graph = safe_prop_cast<const AggregateProperty>(debugger->GetProperty("object_graph"));
		DeltaDebugClient::DoSetObjectGraphConfiguration(
			get_prop_value<BoolProperty>(graph->GetProperty("expandLibraryFuncs")),
			get_prop_value<BoolProperty>(graph->GetProperty("expandMethodFuncs")),
			get_prop_value<BoolProperty>(graph->GetProperty("expandProgramFuncs")),
			get_prop_value<BoolProperty>(graph->GetProperty("expandFuncVMs"))
		);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::SetExpressionEvaluationFormat(void)
	{
		using namespace conf;
		const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(s_classId);
		const AggregateProperty* debugger = safe_prop_cast<const AggregateProperty>(entry.GetProperty("debugger"));
		const Property *exprEvaluationFormat = debugger->GetProperty("expression_evaluation_format");				
		sigExpressionEvaluationFormatChanged(get_prop_value<conf::EnumStringProperty>(exprEvaluationFormat));
	}

	//-----------------------------------------------------------------------

	void DeltaVM::ResumeDebug(void)
	{
		CALL_DELAYED(&DeltaVM::onDebugResumed);
		if (unsigned long pid = currDebugServer.get<0>())
			WindowFocuser::FocusWindowByProcessId(pid);
		//Do not clear the variables here to maintain old/new value information
		callstack.clear();
		inBreakPoint = false;
	}

	//-----------------------------------------------------------------------

	bool DeltaVM::IsLibraryFunction (const FuncInfo& info)
		{ return DeltaDebugClient::IsLineOfLibraryFunction(info.get<1>()); }

	void DeltaVM::ReceiveFunction(FuncInfo& info)
	{
		if (DeltaDebugClient::GetResponseType() == Debug_InfoGlobalContext) {
			boost::get<0>(info) = "global scope";
			return;
		}

		DASSERT(DeltaDebugClient::GetResponseType() == Debug_InfoCurrFunction);
		DeltaDebugClient::GetInfoCurrFunction(
			&boost::get<0>(info),	///< function name, vmid
			&boost::get<1>(info),	///< definition line
			&boost::get<2>(info),	///< call line
			&boost::get<3>(info),	///< scope level
			&boost::get<4>(info)	///< call params
		);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::UpdateReturnValue(const std::string& func, const std::string& value) {

		FuncReturnValueList::iterator i = std::find_if(
			returnValues.begin(),
			returnValues.end(),
			std::bind2nd(uequal_first<FuncReturnValue>(), func)
		);
		if (i != returnValues.end())
			returnValues.erase(i);
		returnValues.push_back(FuncReturnValue(func, value));
	}

	//-----------------------------------------------------------------------

	void DeltaVM::ReceiveFuncResults(void)
	{
		returnValues.clear();

		DeltaDebugClient::WaitAnyMessage();
		DASSERT(DeltaDebugClient::GetResponseType() == Debug_InfoMostRecentFuncResults);

		FuncReturnValueList results;
		if (!DeltaDebugClient::GetMostRecentFuncResults(results))
			DASSERT(false);

		DeltaDebugClient::ResponseProcessed();

		BOOST_FOREACH(const FuncReturnValue& f, results)
			UpdateReturnValue(f.first, f.second);
		results.clear();

		BOOST_FOREACH(FuncReturnValue& info, returnValues)
			CALL_DELAYED(
				boost::bind(&DeltaVM::onAddFunctionReturnValue, 
					util::std2str(info.first),
					util::std2str(info.second)
				)
			);
	}

	//-----------------------------------------------------------------------
	
	void DeltaVM::ReceiveVariables(const FuncInfo& info) {
		currentContext = MakeContext(info.get<0>(), info.get<1>());
		if (IsLibraryFunction(info)) {
			DASSERT(variables.find(currentContext) == variables.end() || variables[currentContext].empty());
			variables[currentContext];	//create empty context
			return;
		}

		const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(s_classId);
		const conf::AggregateProperty* debugger =
			conf::safe_prop_cast<const conf::AggregateProperty>(entry.GetProperty("debugger"));
		if (conf::get_prop_value<conf::BoolProperty>(debugger->GetProperty("receive_variables"))) {
			DeltaDebugClient::DoGetVariables();
			DeltaDebugClient::WaitAnyMessage();
			VariableList& oldVars = variables[currentContext];

			typedef std::pair<std::string, std::string> StringPair;
			typedef std::list<StringPair> StringPairList;

			VariableList newVars;
			StringPairList l;
			bool status = DeltaDebugClient::GetInfoVariables(l);
			DASSERT(status);

			BOOST_FOREACH(const StringPair& p, l) {
				VariableState state = VARIABLE_DEFAULT;
				if (p.first != DBG_GETVARIABLES_NAME_IS_GROUP) {
					VariableList::const_iterator i;
					for (i = oldVars.begin(); i != oldVars.end(); ++i)
						if (i->get<0>() == p.first)
							break;
					if (i == oldVars.end())
						state = VARIABLE_NEW;
					else if (i->get<1>() != p.second)
						state = VARIABLE_MODIFIED;
				}
				newVars.push_back(VarInfo(p.first, p.second, state));
			}
			oldVars = newVars;

			DeltaDebugClient::ResponseProcessed();
		}
		else
			variables[currentContext].clear();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::ReceiveStack (const FuncInfo& topFunction)
	{
		FuncInfo info;
		std::stack<FuncInfo> cs;
		while (StackDown(info))
			cs.push(info);
		DASSERT(inGlobal);

		stackFrameIndex = 0;
		callstack.clear();
		while (!cs.empty())
			{ callstack.push_back(cs.top()); cs.pop(); }
		callstack.push_back(topFunction);

		while (StackUp(info))
			;
	
		BOOST_FOREACH(FuncInfo& info, callstack)
			CALL_DELAYED(
				boost::bind(&DeltaVM::onPushStackFrame,
					util::std2str(boost::get<0>(info)),
					boost::get<1>(info),
					boost::get<2>(info),
					boost::get<3>(info),
					util::std2str(boost::get<4>(info))
				)
			);
	}

	//-----------------------------------------------------------------------

	bool DeltaVM::StackDown(FuncInfo& info)
	{
		DeltaDebugClient::DoStackDown();
		DeltaDebugClient::WaitAnyMessage();

		if (DeltaDebugClient::GetResponseType() == Debug_InfoGlobalContext)
			inGlobal = true;
		else {
			ReceiveFunction(info);
			--stackFrameIndex;
			inGlobal = false;
		}

		DeltaDebugClient::ResponseProcessed();
		return !inGlobal;
	}

	//-----------------------------------------------------------------------

	void DeltaVM::MultipleStackDown(FuncInfo& info, util_ui32 n) {

		DeltaDebugClient::DoMultipleStackDown(n);
		DeltaDebugClient::WaitAnyMessage();

		if (DeltaDebugClient::GetResponseType() == Debug_InfoGlobalContext)
			inGlobal = true;
		else {
			ReceiveFunction(info);
			inGlobal = false;
		}

		DeltaDebugClient::ResponseProcessed();
	}

	//-----------------------------------------------------------------------

	bool DeltaVM::StackUp(FuncInfo& info)
	{
		DeltaDebugClient::DoStackUp();
		DeltaDebugClient::WaitAnyMessage();
		bool more = true;

		if (DeltaDebugClient::GetResponseType() != Debug_InfoTopContext) {
			ReceiveFunction(info);
			++stackFrameIndex;
		}
		else
			more = false;
		
		DeltaDebugClient::ResponseProcessed();
		return more;
	}

	//-----------------------------------------------------------------------

	void DeltaVM::MultipleStackUp(FuncInfo& info, util_ui32 n) {

		DeltaDebugClient::DoMultipleStackUp(n);
		DeltaDebugClient::WaitAnyMessage();

		if (DeltaDebugClient::GetResponseType() != Debug_InfoTopContext)
			ReceiveFunction(info);
		
		DeltaDebugClient::ResponseProcessed();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::OnStopRequested(void)
	{
		exit(0);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::InitializeVM(void)
	{
		DeltaPureVMFacade::Initialise();
		DeltaPureVMFacade::SetByteCodeLoadingPath(GetExtensionAndConfigScriptsReservedByteCodeLoadingPath(), true);
		DeltaPureVMFacade::SetDllImportPath(GetExtensionAndConfigScriptsReservedDllImportPath(), true);

		//-- Initialize sub-system
#ifdef	DELTA_DEBUGGED_VM
		char **argv = DNEWARR(char*, wxTheApp->argc + 1);
		for (unsigned i = 0; i < (unsigned) wxTheApp->argc; ++i)
			argv[i] = ucopystr(util::str2std(wxTheApp->argv[i]));
		argv[wxTheApp->argc] = (char *) 0;
		DeltaDebuggedVMFacade::Initialise(DeltaDebuggedVMFacade::NegotiationPortFromArguments(wxTheApp->argc, argv));
		for(unsigned i = 0; i < (unsigned) wxTheApp->argc; ++i)
			DDELARR(argv[i]);
		DDELARR(argv);
#else

#ifdef	_DEBUG
#define	DEBUGGER_COMMON		"DebuggerCommonD.dll"
#define	DEBUGGER_BACKEND	"DebuggerBackendD.dll"
#else
#define	DEBUGGER_COMMON		"DebuggerCommon.dll"
#define	DEBUGGER_BACKEND	"DebuggerBackend.dll"
#endif

		DeltaDebugDynamicActivator::GetSingleton().AddLibrary(
			DEBUGGER_COMMON, DELTA_DEBUG_DYNAMIC_ACTIVATOR_NO_INIT, DELTA_DEBUG_DYNAMIC_ACTIVATOR_NO_CLEANUP
		);
		DeltaDebugDynamicActivator::GetSingleton().AddLibrary(
			DEBUGGER_BACKEND, "DeltaDebuggedVMFacade_Initialise", "DeltaDebuggedVMFacade_CleanUp"
		);

#undef	DEBUGGER_COMMON	
#undef	DEBUGGER_BACKEND

#endif
		DeltaDebugClientBreakPoints::Create();

		//-- Load Delta Library Functions
		ControlAPI::Install();
		ConstructionAPI::Install();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::CleanUpVM(void)
	{
		//-- Unload Delta Library Functions
		ConstructionAPI::Uninstall();
		ControlAPI::Uninstall();

		//-- CleanUp sub-system
		DeltaDebugClientBreakPoints::Destroy();
#ifdef	DELTA_DEBUGGED_VM
		DeltaDebuggedVMFacade::CleanUp();
#endif
		DeltaPureVMFacade::CleanUp();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::DebugConsoleWithFunc(DebugFunc func, const String& binary, const String& source, const String& options, const String& directory)
	{
#ifndef NDEBUG
		const String executable = _T("DeltaVMDebug_d");
#else 
		const String executable = _T("DeltaVMDebug");
#endif
		const String sparrowDir = util::std2str(IDECore::GetInstallationDir());
		assert(func);
		(*func)(
			sparrowDir + executable + _T(" ") + util::quotepath(util::normalizepath(binary)) + options,
			util::normalizepath(source),
			util::normalizepath(directory)
		);
	}

	//-----------------------------------------------------------------------

	bool DeltaVM::InitializeDebugClient(const DebugServer& server, bool initial) {
		DeltaDebugClient::Initialise();
		if (initial)
			GeneratedFinalSourceBreakpoints();

		if (!DeltaDebugClient::Connect(server.get<1>(), server.get<2>())) {
			DeltaDebugClient::CleanUp();
			debugRunningState = DEBUG_IDLE;
			if (initial)
				CALL_DELAYED(boost::bind(&DeltaVM::onDebugStopped, server.get<3>()));
			return false;
		}

		if (debugRunningState != DEBUG_STOPPED) {	//was not stopped before connection was established
			debugRunningState = DEBUG_RUNNING;
			inBreakPoint = false;
			SetToStringMaxLength();
			SetBreakOnThrownException();
			SetObjectGraphConfiguration();
			DeltaDebugClient::DoStart();
		}
		return true;
	}

	//-----------------------------------------------------------------------

	void DeltaVM::CleanUpDebugClient (const DebugServer& server, bool final) {
		DebugAsyncEvaluator::CancelAll();

		boost::mutex::scoped_lock lock(debugClientThreadFinishedMutex);
		DeltaDebugClient::CleanUp();

		debugRunningState = DEBUG_IDLE;

		inBreakPoint					= false;
		waitingBreakpointValidation		= false;

		if (final) {
			ClearBreakpointList(finalGeneratedBreakpoints);
			if (server.get<5>())
				ClearBreakpointList(stageGeneratedBreakpoints);
		}

		CALL_DELAYED(&DeltaVM::onDebugResumed);
		if (final)
			CALL_DELAYED(boost::bind(&DeltaVM::onDebugStopped, server.get<3>()));
		variables.clear();
		callstack.clear();
		stackFrameIndex = 0;
		debugClientThreadSource = server.get<3>();
		debugClientThreadFinished = true;
	}

	//-----------------------------------------------------------------------

	bool DeltaVM::ActivateMostRecentDebugServer(bool previousFinished) {
		CleanUpDebugClient(currDebugServer, debugServers.size() == 1);
		currDebugServer = DebugServer(0, std::string(), 0, String(), String(), false);
		if (previousFinished) {
			DASSERT(!debugServers.empty());
			debugServers.pop();
		}
		while (!debugServers.empty())
			if (InitializeDebugClient(debugServers.top(), false)) {
				currDebugServer = debugServers.top();
				return true;
			}
			else
				debugServers.pop();
		return false;
	}

	//-----------------------------------------------------------------------

	void DeltaVM::DebugClientThread(const DebugServer& server)
	{
		if (!InitializeDebugClient(server, true)) {
			Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
				(_("Can not connect with the debug server.\n"));
			return;
		}
		do {
			while (debugRunningState == DEBUG_RUNNING) {

				if (DeltaDebugClient::IsConnectionBroken()) {
					SPARROW_OUTPUT_MESSAGE("Stopping debugging since debugger connection is broken!");
					break;
				}

				boost::mutex::scoped_lock lock(stackProcessingMutex);
				try {

					boost::mutex::scoped_lock lock(selectiveStepMutex);

					if (!DeltaDebugClient::IsResponsePending()) {
						uprocesssleep(100);
						DebugServer& top = debugServers.top();
						if (top.get<1>() != currDebugServer.get<1>() || top.get<2>() != currDebugServer.get<2>()) {
							DASSERT(debugServers.size() > 1);
							if (!ActivateMostRecentDebugServer(false)) {
								SPARROW_OUTPUT_MESSAGE("Stopping debugging since unable to connect to any available debugger!");
								break;
							}
						}
						continue;
					}
					DeltaDebugClient::ReceiveResponse();
					switch (DeltaDebugClient::GetResponseType()) {
						case Debug_InfoStopPoint:
							HandleBreakpointHit();
							break;

						case Debug_InfoInvalidBreakPoint:
						case Debug_InfoBreakPointConditionError:
							HandleInvalidBreakpoint();
							break;

						case Debug_InfoValidBreakPoint:
							HandleValidBreakpointAdded();
							break;

						case Debug_InfoError:
							HandleVMError();
							break;

						case Debug_InfoWarning:
							HandleVMWarning();
							break;

						case Debug_InfoStop:
							HandleVMStop();
							break;

						case Debug_InfoDynamicCode:
							DeltaDebugClient::ResponseProcessed();	//Ignore the async message.
							break;

						// All expression evaluation requests are handled asynchronously. 
						case Debug_InfoValue				:
						case Debug_InfoExprTypeData			:
						case Debug_InfoErrorValue			:
						case Debug_InfoValueMany			:
						case Debug_InfoExprTypeDataMany		:
						case Debug_InfoObjectGraph			:
						case Debug_InfoBinaryData			: 
						
							{ DebugAsyncEvaluator::Receive(DeltaDebugClient::GetResponseType()); break; }

						default: {
							DS2CR response = DeltaDebugClient::GetResponseType();
							SPARROW_OUTPUT_MESSAGE(uconstructstr("Unexpected message: %d", response));
							DeltaDebugClient::ResponseProcessed();
							break;
						}
					}
				}
				catch (DeltaDebugClient::BrokenConnection) {
					SPARROW_OUTPUT_MESSAGE("Stopping debugging since debugger connection is broken!");
					debugRunningState = DEBUG_IDLE;
				}
				catch (...) {
					SPARROW_OUTPUT_MESSAGE("Unknown error occured!");
					DASSERT(false);
				}
			}
		} while (ActivateMostRecentDebugServer(true));
	}

	//-----------------------------------------------------------------------

	void DeltaVM::MessageConditionSatisifed (const String& cond, util_ui32 line) {
		WindowFocuser::FocusSparrow();
		const String message = wxString::Format(
			_("\n( %s ) == true (breakpoint condition).\nBreaking execution at line %u!"),
			cond.c_str(),
			line
		);
		wxMessageBox(message, wxT("Debugger message"), wxOK);	
	}

	//-----------------------------------------------------------------------

	void DeltaVM::HandleBreakpointHit(void) {

		std::string	source, cond;
		util_ui32	line;

		DeltaDebugClient::GetInfoStopPoint(&source, &line, &inGlobal, &cond);
		DeltaDebugClient::ResponseProcessed();
		
		if (!cond.empty())
			CALL_DELAYED(boost::bind(&DeltaVM::MessageConditionSatisifed, util::std2str(cond), line));

		String symbolic	= util::std2str(source);
		String uri		= GetURIFromSymbolic(symbolic);

		//-- Make sure the editor manager can accept the BreakpointHit signal
		CALL_DELAYED(boost::bind(&DockableComponent::EnsureVisibility, "EditorManager"));

		DeltaDebugClient::WaitAnyMessage();
		FuncInfo topFunc;
		ReceiveFunction(topFunc);
		DeltaDebugClient::ResponseProcessed();

		ReceiveFuncResults();
		ReceiveVariables(topFunc);
		ReceiveStack(topFunc);

		//-- Focus Sparrow
		WindowFocuser::FocusSparrow();

		if (ustrprefix(DBG_DYNAMIC_SOURCE, source.c_str())) {
			uri = util::std2str(source) + _(".dsc");
			const std::string csymbolic = source;
			symbolic = util::std2str(csymbolic);
			DeltaDebugClient::DoGetDynamicCode();
			DeltaDebugClient::WaitAnyMessage();
			DeltaDebugClient::GetInfoDynamicCode(&source);
			DeltaDebugClient::ResponseProcessed();

			if (DynamicCodeManager::GetEditor(csymbolic).Resolve())
				CALL_DELAYED(boost::bind(&DynamicCodeManager::Update, csymbolic, source));
			else
				CALL_DELAYED(boost::bind(&DynamicCodeManager::Register, csymbolic, source));
		}

		Call<void (const String&), SafeCall>(s_classId, "Output", "Append")(
			_("\nStopped at line: ")			+
			boost::lexical_cast<String>(line)	+
			_(", vm: ") + symbolic + _(", ")	+
			(inGlobal ? _("global") : _("local")) + _(" scope.\n")
		);

		stopPoint = StopPointInfo(symbolic, line);
		inBreakPoint = true;
		shouldNotEnterTraceCall = false;

		CALL_DELAYED(boost::bind(&DeltaVM::onBreakpointHit, uri, symbolic, (int) line));
	}

	//-----------------------------------------------------------------------

	void DeltaVM::HandleVMError(void)
	{
		std::string error;
		DeltaDebugClient::GetInfoError(&error);
		DeltaDebugClient::ResponseProcessed();

		Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
			(util::std2str(std::string("\n") + error + "\n"));
	}

	//-----------------------------------------------------------------------

	void DeltaVM::HandleVMWarning(void)
	{
		std::string warning;
		DeltaDebugClient::GetInfoWarning(&warning);
		DeltaDebugClient::ResponseProcessed();

		Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
			(util::std2str(std::string("\n") + warning + "\n"));
	}

	//-----------------------------------------------------------------------

	void DeltaVM::HandleVMStop(void)
	{
		Call<void (const String&), SafeCall>(s_classId, "Output", "Append")
			(_("\nExecution finished gracefully.\n"));
		debugRunningState = DEBUG_IDLE;

		DeltaDebugClient::ResponseProcessed();
	}

	//-----------------------------------------------------------------------

	void DeltaVM::MessageConditionError (const String& cond, util_ui32 line) {
		WindowFocuser::FocusSparrow();
		const String message = wxString::Format(
			_("( %s ) evaluation error (breakpoint condition).\nBreakpoint at line %u was disabled."),
			cond.c_str(),
			line
		);
		wxMessageBox(message, wxT("Debugger message"), wxOK | wxICON_ERROR);	
	}

	//-----------------------------------------------------------------------

	const std::string DeltaVM::MakeContext (const std::string& name, uint defLine)
	{
		return uconstructstr("%s, line %d", name.c_str(), defLine);
	}

	//-----------------------------------------------------------------------
	
	const DeltaVM::VarDeclInfo DeltaVM::ExtractVarDeclInfo(const std::string& data)
	{
		std::string::size_type colon = data.find(":");
		std::string::size_type scope = data.find("::");

		// This signifies something has changed on the backend side regarding the
		// format of var names.

		if (colon == std::string::npos)
			return VarDeclInfo(
				data + "<failed to parse>",
				DELTA_CANTBE_A_SOURCE_LINE
			);

		while(colon == scope) {
			colon = data.find(":", colon + 2), scope = data.find("::", scope + 2);
			DASSERT(colon != std::string::npos);
		}

		DASSERT(colon != std::string::npos);
		return VarDeclInfo(
			data.substr(0, colon),
			boost::lexical_cast<int>(data.substr(colon + 1))
		);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onPushStackFrame(const String& record, uint defLine, uint callLine, uint scope, const String& params)
	{
		sigPushStackFrame(record, defLine, callLine, scope, params);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onStackFrameMoved(uint index)
	{
		sigStackFrameMoved(index);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onAddFunctionReturnValue(const String& func, const String& value)
	{
		sigAddFunctionReturnValue(func, value);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onDebugStarted(const String& uri)
	{
		Component* shell = ComponentRegistry::Instance().GetFocusedInstance("Shell");
		if(shell) {
			GUI_SCOPED_FREEZE(shell->GetWindow());
			sigDebugStarted(uri);
		}
		else
			sigDebugStarted(uri);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onDebugStopped(const String& uri)
	{
		Component* shell = ComponentRegistry::Instance().GetFocusedInstance("Shell");
		if(shell) {
			GUI_SCOPED_FREEZE(shell->GetWindow());
			sigDebugStopped(uri);
		}
		else
			sigDebugStopped(uri);
	}

	//-----------------------------------------------------------------------

	void DeltaVM::onDebugResumed(void)
	{
		sigDebugResumed();
	}

	//-----------------------------------------------------------------------
}
