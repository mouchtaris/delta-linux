/**
 *	Workspace.h
 *
 *	-- IDE Profile Manager Component --
 *
 *	Workspace component is the root component of
 *	the Project Manager's tree view, representing
 *	a workspace container of projects.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 *  Added support for console debugger invocation, AS, August 2009. 
 * FIXME: all these string constants for commands appearing in multiple files
 * to be changed to macros somehow.
 */
#include "Workspace.h"
#include "IDEDialogs.h"

#include "StringUtils.h"
#include "Properties.h"
#include "PropertyUtils.h"
#include "XMLPropertyVisitor.h"
#include "GenericDialogs.h"
#include "FileChangeWatcher.h"

#include "ComponentEntry.h"
#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "DelayedCaller.h"

#include "PropertyTable.h"

#include "BitmapRegistry.h"
#include "ComponentConfigurationDialog.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <wx/filename.h>

#include <windows.h>

#include "Icons/build_workspace.xpm"
#include "Icons/clean_workspace.xpm"
#include "Icons/cancel_work.xpm"
#include "Icons/debug_attach.xpm"
#include "Icons/debug_executable.xpm"
#include "Icons/buildlog_enable.xpm"
#include "Icons/buildlog_disable.xpm"
#include "Icons/buildlog_delete.xpm"

#include "Icons/go.xpm"
#include "Icons/break.xpm"
#include "Icons/stop_debug.xpm"
#include "Icons/show_next_statement.xpm"
#include "Icons/step_in.xpm"
#include "Icons/selective_step_in.xpm"
#include "Icons/step_over.xpm"
#include "Icons/step_out.xpm"
#include "Icons/run_to_cursor.xpm"
#include "BuildLog.h"

namespace ide
{
	static const char *defaultChildren[] = { "Project", "AspectProject" };

	//-------------------------------------------------------//
	//---- static members -----------------------------------//

	std::string Workspace::debuggerId;
	Handle Workspace::debuggerHandle;


	//-------------------------------------------------------//
	//---- class Workspace ----------------------------------//

	COMPONENT_METADATA(
		Workspace,
		_("Workspace"),
		_("Workspace is the root container of projects"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("0.1")
		);
	IMPLEMENT_COMPONENT_(Workspace, Container);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(Workspace, table)
	{
		StringVec options;
		options.push_back(_T("Workspace"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of workspace in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SIGNAL(Workspace, WorkspaceLoaded, (const String& uri));
	EXPORTED_SIGNAL(Workspace, WorkspaceClosed, (const String& uri));
	EXPORTED_STATIC_SIGNAL(Workspace, WorkStarted, (const Handle& root, const String& task));
	EXPORTED_STATIC_SIGNAL(Workspace, WorkCompleted, (const Handle& root, const String& task));
	EXPORTED_STATIC_SIGNAL(Workspace, WorkCanceled, (const Handle& root, const String& task));

	//-----------------------------------------------------------------------

	Workspace::Workspace(void) : m_buildingEditorScript(false)
	{
		StringVec options;
		options.push_back(_T("debug"));
		options.push_back(_T("release"));

		AddInstanceProperty("config", new conf::EnumStringProperty(_("Build Configuration"), options, 0,
			_("Build configuration of workspace"))
		);
		AddInstanceProperty("breakpoints", new conf::StringProperty(_("Breakpoints file"), _("break.dat"),
			_("File storing the user breakpoints of the workspace"))
		);
	}

	//-----------------------------------------------------------------------

	Workspace::~Workspace(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, Initialize, (void))
	{
		Call<void (const std::string&, const std::string&)>
			Register(s_classId, "TreeItemComponent", "RegisterChildType");
		BOOST_FOREACH(const char* child, defaultChildren)
			Register(s_classId, child);

		BitmapRegistry::Instance().Insert(_T("build_workspace"), build_workspace_xpm);
		BitmapRegistry::Instance().Insert(_T("clean_workspace"), clean_workspace_xpm);
		BitmapRegistry::Instance().Insert(_T("cancel_work"), cancel_work_xpm);
		BitmapRegistry::Instance().Insert(_T("debug_attach"), debug_attach_xpm);
		BitmapRegistry::Instance().Insert(_T("debug_executable"), debug_executable_xpm);
		BitmapRegistry::Instance().Insert(_T("buildlog_enable"), buildlog_enable_xpm);
		BitmapRegistry::Instance().Insert(_T("buildlog_disable"), buildlog_disable_xpm);
		BitmapRegistry::Instance().Insert(_T("buildlog_delete"), buildlog_delete_xpm);

		BitmapRegistry::Instance().Insert(_T("go"), go_xpm);
		BitmapRegistry::Instance().Insert(_T("break"), break_xpm);
		BitmapRegistry::Instance().Insert(_T("stop_debug"), stop_debug_xpm);
		BitmapRegistry::Instance().Insert(_T("show_next_statement"), show_next_statement_xpm);
		BitmapRegistry::Instance().Insert(_T("step_in"), step_in_xpm);
		BitmapRegistry::Instance().Insert(_T("selective_step_in"), selective_step_in_xpm);
		BitmapRegistry::Instance().Insert(_T("step_over"), step_over_xpm);
		BitmapRegistry::Instance().Insert(_T("step_out"), step_out_xpm);
		BitmapRegistry::Instance().Insert(_T("run_to_cursor"), run_to_cursor_xpm);

		Call<void (const String& path, const UserCommandDesc& desc)> AddCommand("DeltaVM", "Shell", "AddCommand");

		AddCommand(
			_("/{100}Build/{10}Build Workspace\tF7"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "BuildCtx"), false, UC_MAIN, false, true, _T("build_workspace"))
		);
		AddCommand(
			_("/{100}Build/{11}Build Script\tCtrl+F7"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "BuildScript"), false, UC_MAIN, false, true, _T("build"))
		);
		AddCommand(
			_("/{100}Build/{13}Build Script with Debugging\tCtrl+Alt+F7"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "BuildScriptWithDebugging"), false, UC_MAIN, false, true, _T("build"))
		);
		AddCommand(
			_("/{100}Build/{15}Clean Workspace\tCtrl+Shift+F7"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "CleanCtx"), false, UC_MAIN, false, true, _T("clean_workspace"))
		);

		AddLogCommands();

		AddCommand(
			_("/{100}Build/{19}Delete Log"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "DeleteWorkspaceBuildLog"), false, UC_MAIN, false, true, _T("buildlog_delete"))
		);

		AddCommand(
			_("/{110}Debug/{10}Run\tCtrl+F5"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "Run"), false, UC_ALL, false, true, _T("run_project"))
		);
		AddCommand(
			_("/{110}Debug/{20}Debug (Zen, graphical)\tF5"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "Debug"), false, UC_ALL, false, true, _T("debug_project"))
		);

		AddCommand(
			_("/{110}Debug/{30}Debug (Disco, console)\tCtrl+Shift+F5"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "DebugWithConsoleDebugger"), false, UC_ALL, false, true, _T("debug_project_console"))
		);

		AddCommand(
			_("/{110}Debug/{200}--Toggle Breakpoint\tF9"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "ToggleBreakpoint"), false, UC_ALL)
		);
		AddCommand(
			_("/{110}Debug/{210}Enable-Disable Breakpoint\tCtrl+F9"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "ToggleBreakpointStatus"), false, UC_ALL)
		);
		AddCommand(
			_("/{110}Debug/{220}New Breakpoint\tCtrl+B"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "NewBreakpoint"), false, UC_ALL)
		);
		AddCommand(
			_("/{110}Debug/{230}Delete All Breakpoints\tCtrl+Shift+F9"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "DeleteAllBreakpoints"), false, UC_ALL, false)
		);
		AddCommand(
			_("/{110}Debug/{10000}--Attach to debugger..."),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "DebugAttach"), false, UC_MAIN, true, true, _T("debug_attach"))
		);
		AddCommand(
			_("/{110}Debug/{10001}Debug executable..."),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "DebugExecutable"), false, UC_MAIN, true, true, _T("debug_executable"))
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, CleanUp, (void))
	{
		Call<void (const String& path), SafeCall> RemoveCommand("DeltaVM", "Shell", "RemoveCommand");

		RemoveCommand(_("/Build/Build Workspace\tF7"));
		RemoveCommand(_("/Build/Build Script\tCtrl+F7"));
		RemoveCommand(_("/Build/Build Script with Debugging\tCtrl+Alt+F7"));
		RemoveCommand(_("/Build/Clean Workspace\tCtrl+Shift+F7"));
		RemoveCommand(_("/Build/Disable Log"));
		RemoveCommand(_("/Build/Enable Log"));
		RemoveCommand(_("/Build/Delete Log"));
		RemoveCommand(_("/Debug/Run\tCtrl+F5"));
		RemoveCommand(_("/Debug/Debug (Zen, graphical)\tF5"));
		RemoveCommand(_("/Debug/Debug (Disco, console)\tCtrl+Shift+F5"));
		RemoveCommand(_("/Debug/Toggle Breakpoint\tF9"));
		RemoveCommand(_("/Debug/Enable-Disable Breakpoint\tCtrl+F9"));
		RemoveCommand(_("/Debug/New Breakpoint\tCtrl+B"));
		RemoveCommand(_("/Debug/Delete All Breakpoints\tCtrl+Shift+F9"));
		RemoveCommand(_("/Debug/Attach to debugger..."));
		RemoveCommand(_("/Debug/Debug executable..."));

		BitmapRegistry::Instance().Remove(_T("build_workspace"));
		BitmapRegistry::Instance().Remove(_T("clean_workspace"));
		BitmapRegistry::Instance().Remove(_T("cancel_work"));
		BitmapRegistry::Instance().Remove(_T("debug_attach"));
		BitmapRegistry::Instance().Remove(_T("debug_executable"));

		BitmapRegistry::Instance().Remove(_T("go"));
		BitmapRegistry::Instance().Remove(_T("break"));
		BitmapRegistry::Instance().Remove(_T("stop_debug"));
		BitmapRegistry::Instance().Remove(_T("show_next_statement"));
		BitmapRegistry::Instance().Remove(_T("step_in"));
		BitmapRegistry::Instance().Remove(_T("selective_step_in"));
		BitmapRegistry::Instance().Remove(_T("step_over"));
		BitmapRegistry::Instance().Remove(_T("step_out"));
		BitmapRegistry::Instance().Remove(_T("run_to_cursor"));

		Call<void (const std::string&, const std::string&), SafeCall>
			Unregister(s_classId, "TreeItemComponent", "UnregisterChildType");
		BOOST_FOREACH(const char* child, defaultChildren)
			Unregister(s_classId, child);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, bool, CanDestroy, (void))
	{
		SaveAll();
		return Container::CanDestroy();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, bool, Load, (const String& uri))
	{
		if (!Container::Load(uri))
			return false;

		const String breakpoints = conf::get_prop_value<conf::StringProperty>(GetInstanceProperty("breakpoints"), String());
		if(!breakpoints.empty())	//if loading of breakpoint file fails, just create an empty breapoint file for that path
			if(!Call<bool (const String&)>(this, "DeltaVM", "LoadBreakpoints")(GetPath() + breakpoints))
				Call<bool (const String&)>(this, "DeltaVM", "SaveBreakpoints")(GetPath() + breakpoints);

		//-- trigger signal
		sigWorkspaceLoaded(this, GetURI());
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, bool, SaveAs, (const String& uri))
	{
		bool status = Container::SaveAs(uri);
		return SaveBreakpoints() && status;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, bool, Close, (void))
	{
		SaveAll();
		Destroy();
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, void, BuildScript, (void))
	{
		Handle resource;
		if ((resource = GetActiveResource()) && (resource.GetClassId() == "Script" || resource.GetClassId() == "Aspect")) {
			m_buildingEditorScript = true;
			Call<void (void), SafeCall>(this, resource, "BuildCtx")();
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, void, BuildScriptWithDebugging, (void))
	{
		Handle resource;
		if ((resource = GetActiveResource()) && (resource.GetClassId() == "Script" || resource.GetClassId() == "Aspect"))
			Call<void (void), SafeCall>(this, resource, "DebugBuildCtx")();
	}



	/*****************************************************
	**  We add a menu button depending the log's existance.
	******************************************************/

	EXPORTED_STATIC(Workspace, void, AddLogCommands, (void))
	{
		Call<void (const String& path, const UserCommandDesc& desc)> AddCommand("DeltaVM", "Shell", "AddCommand");
		if (__BL.IsEnabled()){
			AddCommand(
				_("/{100}Build/{18}Disable Log"),
				UserCommandDesc(UserCommandDesc::Callback("Workspace", "DisableWorkspaceBuildLog"), false, UC_MAIN, false, true, _T("buildlog_disable"))
			);
		}
		else{
			AddCommand(
				_("/{100}Build/{18}Enable Log"),
				UserCommandDesc(UserCommandDesc::Callback("Workspace", "EnableWorkspaceBuildLog"), false, UC_MAIN, false, true, _T("buildlog_enable"))
			);
		}
	}

	/*********************************************************
	**  Callback for disabling the buildlog for the workspace
	**********************************************************/

	EXPORTED_FUNCTION(Workspace, void, DisableWorkspaceBuildLog, (void))
	{
		__BL.DisableBuildLog();

		Call<void (const String& path), SafeCall> RemoveCommand("DeltaVM", "Shell", "RemoveCommand");
		Call<void (const String& path, const UserCommandDesc& desc)> AddCommand("DeltaVM", "Shell", "AddCommand");

		RemoveCommand(_("/Build/Disable Log"));
		AddCommand(
			_("/{100}Build/{18}Enable Log"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "EnableWorkspaceBuildLog"), false, UC_MAIN, false, true, _T("buildlog_enable"))
		);
	}

	/*********************************************************
	**  Callback for enabling the buildlog for the workspace
	**********************************************************/

	EXPORTED_FUNCTION(Workspace, void, EnableWorkspaceBuildLog, (void))
	{
		__BL.EnableBuildLog();

		Call<void (const String& path), SafeCall> RemoveCommand("DeltaVM", "Shell", "RemoveCommand");
		Call<void (const String& path, const UserCommandDesc& desc)> AddCommand("DeltaVM", "Shell", "AddCommand");
		RemoveCommand(_("/Build/Enable Log"));
		AddCommand(
			_("/{100}Build/{18}Disable Log"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "DisableWorkspaceBuildLog"), false, UC_MAIN, false, true, _T("buildlog_disable"))
		);
	}

	/*********************************************************
	**  Callback for deleting the buildlog for the workspace
	**********************************************************/

	EXPORTED_FUNCTION(Workspace, void, DeleteWorkspaceBuildLog, (void))
	{
		string message = __BL.LogExists(GetPath(),GetName())?"Build log successfully deleted.":"Build log does not exist.";
		Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")(
				_("Delete Log"), util::std2str(message)
			);
		__BL.DeleteBuildLog(this->GetPath(),this->GetName());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, void, Debug, (void))
	{
		Debug(_("Debugging Workspace...\n"), "Debug");
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, void, DebugWithConsoleDebugger, (void))
	{
		Debug(_("Debugging Workspace (via console debugger)...\n"), "DebugWithConsoleDebugger");
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, DebugAttach, (void))
	{
		HostPortDialog dlg(0, _("Attach to debugger..."), _T("localhost"), 0);

		if (dlg.ShowModal() != wxID_OK)
			return;

		debuggerId = "DeltaVM";
		Call<void (void), SafeCall>(s_classId, debuggerId, "StopDebug")();
		Call<void (const String&, int), SafeCall>(s_classId, debuggerId, "DebugAttach")(dlg.GetHost(), dlg.GetPort());
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, DebugExecutable, (void))
	{
		FileDialog dlg(0, _T("Debug executable..."), String(), String(), _("Delta Binary Files (*.dbc)|*.dbc"));

		if (dlg.ShowModal() != wxID_OK)
			return;

		debuggerId = "DeltaVM";
		Call<void (void), SafeCall>(s_classId, debuggerId, "StopDebug")();
		
		wxFileName filename(dlg.GetPath());
		Call<void (const String&, const String&, const String&), SafeCall>(s_classId, debuggerId, "DebugConsole")
			(filename.GetFullPath(), _("Executable"), filename.GetPath());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, void, EnableWorkCommands, (void))
	{
		Container::EnableWorkCommands();
		Call<void (const String& path)>("DeltaVM", "Shell", "RemoveCommand")(_("/Build/Cancel\tCtrl+F12"));
		Call<void (const String& path, const UserCommandDesc& desc)> AddCommand("DeltaVM", "Shell", "AddCommand");

		Call<void (const String& path)> EnableCommand("DeltaVM", "Shell", "EnableCommand");
		EnableCommand(_("/Build/Build Workspace\tF7"));
		EnableCommand(_("/Build/Build Script\tCtrl+F7"));
		EnableCommand(_("/Build/Build Script with Debugging\tCtrl+Alt+F7"));
		EnableCommand(_("/Build/Clean Workspace\tCtrl+Shift+F7"));
		EnableCommand(_("/Build/Enable Log"));
		EnableCommand(_("/Build/Disable Log"));
		EnableCommand(_("/Build/Delete Log"));
		EnableCommand(_("/Debug/Run\tCtrl+F5"));
		EnableCommand(_("/Debug/Debug (Zen, graphical)\tF5"));
		EnableCommand(_("/Debug/Debug (Disco, console)\tCtrl+Shift+F5"));
		EnableCommand(_("/Debug/Attach to debugger..."));
		EnableCommand(_("/Debug/Debug executable..."));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, void, DisableWorkCommands, (void))
	{
		Container::DisableWorkCommands();
		Call<void (const String& path, const UserCommandDesc& desc)>("DeltaVM", "Shell", "AddCommand")(
			_("/{100}Build/{20}Cancel\tCtrl+F12"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "CancelWork"), false, UC_MAIN, true, true, _T("cancel_work"))
		);

		Call<void (const String& path)> DisableCommand("DeltaVM", "Shell", "DisableCommand");
		DisableCommand(_("/Build/Build Workspace\tF7"));
		DisableCommand(_("/Build/Build Script\tCtrl+F7"));
		DisableCommand(_("/Build/Build Script with Debugging\tCtrl+Alt+F7"));
		DisableCommand(_("/Build/Clean Workspace\tCtrl+Shift+F7"));
		DisableCommand(_("/Build/Disable Log"));
		DisableCommand(_("/Build/Enable Log"));
		DisableCommand(_("/Build/Delete Log"));
		DisableCommand(_("/Debug/Run\tCtrl+F5"));
		DisableCommand(_("/Debug/Debug (Zen, graphical)\tF5"));
		DisableCommand(_("/Debug/Debug (Disco, console)\tCtrl+Shift+F5"));
		DisableCommand(_("/Debug/Attach to debugger..."));
		DisableCommand(_("/Debug/Debug executable..."));
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Workspace, void, OnDebugStarted, (const std::string& classId, const String& uri),
	"DebugStarted")
	{
		Call<void (const String& path)> RemoveCommand("DeltaVM", "Shell", "RemoveCommand");
		RemoveCommand(_("/Debug/Run\tCtrl+F5"));
		RemoveCommand(_("/Debug/Debug (Zen, graphical)\tF5"));

		debuggerHandle = Call<Handle (const std::string&, int)>("DeltaVM", "Shell", "AddComponent")("DeltaVM", 0);
		Call<void (const String& path, const UserCommandDesc& desc)> AddCommand("DeltaVM", debuggerHandle, "AddCommand");
		
		AddCommand(
			_("/{110}Debug/{40}--Continue\tF5"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "Go"), false, UC_ALL, true, true, _("go"))
		);
		AddCommand(
			_("/{110}Debug/{50}Break\tCtrl+Alt+Break"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "Break"), false, UC_ALL, true, true, _("break"))
		);
		AddCommand(
			_("/{110}Debug/{60}Stop Debugging\tShift+F5"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "StopDebug"), false, UC_ALL, true, true, _("stop_debug"))
		);
		AddCommand(
			_("/{110}Debug/{100}--Show Next Statement"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "ShowNextStatement"), false, UC_ALL, true, true, _("show_next_statement"))
		);
		AddCommand(
			_("/{110}Debug/{110}Step In\tF11"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "StepIn"), false, UC_ALL, true, true, _("step_in"))
		);
		AddCommand(
			_("/{110}Debug/{110}Selective Step In\tCtrl+F11"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "SelectiveStepIn"), false, UC_ALL, true, true, _("selective_step_in"))
		);
		AddCommand(
			_("/{110}Debug/{110}Step Over\tF10"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "StepOver"), false, UC_ALL, true, true, _("step_over"))
		);
		AddCommand(
			_("/{110}Debug/{110}Step Out\tShift+F11"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "StepOut"), false, UC_ALL, true, true, _("step_out"))
		);
		AddCommand(
			_("/{110}Debug/{120}Run to Cursor\tCtrl+F10--"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "RunToCursor"), false, UC_ALL, true, true, _("run_to_cursor"))
		);
		debuggerId = Call<const std::string& (void)>(this, "Script", "GetDebuggerId")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Workspace, void, OnDebugStopped, (const std::string& classId, const String& uri),
	"DebugStopped")
	{
		Call<void (const String& path)> RemoveCommand("DeltaVM", debuggerHandle, "RemoveCommand");
		RemoveCommand(_("/Debug/Continue\tF5"));
		RemoveCommand(_("/Debug/Break\tCtrl+Alt+Break"));
		RemoveCommand(_("/Debug/Stop Debugging\tShift+F5"));
		RemoveCommand(_("/Debug/Show Next Statement"));
		RemoveCommand(_("/Debug/Step In\tF11"));
		RemoveCommand(_("/Debug/Selective Step In\tCtrl+F11"));
		RemoveCommand(_("/Debug/Step Over\tF10"));
		RemoveCommand(_("/Debug/Step Out\tShift+F11"));
		RemoveCommand(_("/Debug/Run to Cursor\tCtrl+F10"));

		Call<void (const String& path, const UserCommandDesc& desc)> AddCommand("DeltaVM", "Shell", "AddCommand");
		AddCommand(
			_("/{110}Debug/{10}--Run\tCtrl+F5"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "Run"), false, UC_ALL, false, true, _T("run_project"))
		);
		AddCommand(
			_("/{110}Debug/{20}Debug (Zen, graphical)\tF5"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "Debug"), false, UC_ALL, false, true, _T("debug_project"))
		);

		debuggerId = "";
		Call<bool (const Handle&)>("DeltaVM", "Shell", "RemoveComponent")(debuggerHandle);
		debuggerHandle = Handle();
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(Workspace, void, OnFirstBreakpoint, (void), "FirstBreakpoint")
	{
		Call<void (const String& path)>("DeltaVM", "Shell", "EnableCommand")
			(_("/Debug/Delete All Breakpoints\tCtrl+Shift+F9"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(Workspace, void, OnFirstBreakpointEnabled, (void), "FirstBreakpointEnabled")
	{
		Call<void (const String& path, const UserCommandDesc& desc)>("DeltaVM", "Shell", "AddCommand")(
			_("/{110}Debug/{250}Disable All Breakpoints"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "DisableAllBreakpoints"), false, UC_ALL)
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(Workspace, void, OnFirstBreakpointDisbled, (void), "FirstBreakpointDisabled")
	{
		Call<void (const String& path, const UserCommandDesc& desc)>("DeltaVM", "Shell", "AddCommand")(
			_("/{110}Debug/{240}Enable All Breakpoints"),
			UserCommandDesc(UserCommandDesc::Callback("Workspace", "EnableAllBreakpoints"), false, UC_ALL)
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(Workspace, void, OnNoBreakpoints, (void), "NoBreakpoints")
	{
		Call<void (const String& path)>("DeltaVM", "Shell", "DisableCommand")
			(_("/Debug/Delete All Breakpoints\tCtrl+Shift+F9"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(Workspace, void, OnNoBreakpointsEnabled, (void), "NoBreakpointsEnabled")
	{
		Call<void (const String& path)>("DeltaVM", "Shell", "RemoveCommand")(_("/Debug/Disable All Breakpoints"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_STATIC(Workspace, void, OnNoBreakpointsDisabled, (void), "NoBreakpointsDisabled")
	{
		Call<void (const String& path)>("DeltaVM", "Shell", "RemoveCommand")(_("/Debug/Enable All Breakpoints"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, const Handle&, GetRootWorkingResource, (void))
	{
		return m_rootWorkingResource;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, void, ReadWorkspaceLog, (void))
	{			
		/*
		**  We must read and update each script's directory
		**  information in case the user moved the
		**  workspace's location.
		*/

		if (__BL.IsEnabled()){
			__BL.Read( GetPath(), GetName() );
			ide::Component::List children;
			GetChildrenRecursively(children);
			BOOST_FOREACH(Component* child, children){
				std::string type = child->GetClassId();
				if (( type=="Script" || type=="StageResult" || type=="StageSource" || type=="Aspect")){
					Script* tmp = static_cast<Script*>(child);
					tmp->UpdateLogDirectoryInformation();
				}
			}			
		}
	}
	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Workspace, void, StartWorking, (const Handle& root, const String& task))
	{
		if (!m_rootWorkingResource) {
			if (util::str2std(task)=="Build")ReadWorkspaceLog();
			m_rootWorkingResource = root;
			m_task = task;
			SetWorkspaceWorkCommandsStatus(false);
			sigWorkStarted(m_rootWorkingResource, m_task);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Workspace, void, OnResourceWorkCompleted, (const Handle& resource,
		const String& task, const UIntList& workId), "ResourceWorkCompleted")
	{
		Container::OnResourceWorkCompleted(resource, task, workId);
		if (resource == m_rootWorkingResource && task == m_task) {
			FocusComponent("ErrorList");
			SetWorkspaceWorkCommandsStatus(true);
			if (m_buildingEditorScript)
				if (Component* comp = ComponentRegistry::Instance().GetFocusedInstance("Editor")) {
					comp->Focus();
					if (wxWindow* win = comp->GetWindow())
						win->SetFocus();
				}

			timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(OnWorkCompleted, m_rootWorkingResource, m_task));
			m_buildingEditorScript = false;
			m_rootWorkingResource = Handle();
			m_task.clear();
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, Go, (void))
	{
		if (!debuggerId.empty())
			Call<void (void)>(s_classId, debuggerId, "Go")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, Break, (void))
	{
		if (!debuggerId.empty())
			Call<void (void)>(s_classId, debuggerId, "Break")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, StopDebug, (void))
	{
		if (!debuggerId.empty())
			Call<void (void)>(s_classId, debuggerId, "StopDebug")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, ShowNextStatement, (void))
	{
		if (!debuggerId.empty())
			Call<void (void)>(s_classId, debuggerId, "ShowNextStatement")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, StepIn, (void))
	{
		if (!debuggerId.empty())
			Call<void (void)>(s_classId, debuggerId, "StepIn")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, SelectiveStepIn, (void))
	{
		if (!debuggerId.empty())
			Call<void (void)>(s_classId, debuggerId, "SelectiveStepIn")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, StepOver, (void))
	{
		if (!debuggerId.empty())
			Call<void (void)>(s_classId, debuggerId, "StepOver")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, StepOut, (void))
	{
		if (!debuggerId.empty())
			Call<void (void)>(s_classId, debuggerId, "StepOut")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, RunToCursor, (void))
	{
		if (!debuggerId.empty())
			Call<void (void)>(s_classId, debuggerId, "RunToCursor")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, CancelWork, (void))
	{
		Component *comp = ComponentRegistry::Instance().GetFocusedInstance(s_classId);
		if (comp) {
			Workspace *workspace = static_cast<Workspace *>(comp);
			if (workspace->GetRootWorkingResource()) {
				sigWorkCanceled(workspace->m_rootWorkingResource, workspace->m_task);
				FocusComponent("Output");
				workspace->SetWorkspaceWorkCommandsStatus(true);
				workspace->m_rootWorkingResource = Handle();
				workspace->m_task.clear();
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, ToggleBreakpoint, (void))
	{
		if (const Handle editor = GetActiveEditor()) {
			int line = Call<int (void)>(s_classId, editor, "GetCurrentLine")();
			const String symbolic = Call<const String (const Handle&)>(s_classId, "DeltaVM", "GetSymbolicURIFromEditor")(editor);
			if (!symbolic.empty())
				Call<bool (const String&, int)>(s_classId, "DeltaVM", "ToggleBreakpoint")(symbolic, line);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, ToggleBreakpointStatus, (void))
	{
		if (const Handle editor = GetActiveEditor()) {
			int line = Call<int (void)>(s_classId, editor, "GetCurrentLine")();
			const String symbolic = Call<const String (const Handle&)>(s_classId, "DeltaVM", "GetSymbolicURIFromEditor")(editor);
			if (!symbolic.empty())
				Call<bool (const String&, int)>(s_classId, "DeltaVM", "ToggleBreakpointStatus")(symbolic, line);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(Workspace, NewBreakpoint, _("/{110}Debug/{220}New Breakpoint\tCtrl+B"), MT_MAIN, "")
	{
		conf::PropertyTable table;
		conf::StringProperty* uriProp = new conf::StringProperty(_T("Symbolic source"), String(),
			_("Symbolic source uri for the breakpoint"));
		conf::IntProperty* lineProp = new conf::IntProperty(_T("Line"), int(),
			_("Line for the breakpoint"));
		conf::StringProperty* conditionProp = new conf::StringProperty(_T("Condition"), String(),
			_("Condition for the breakpoint"));

		table.AddProperty(util::str2std(uriProp->GetLabel()), uriProp);
		table.AddProperty(util::str2std(lineProp->GetLabel()), lineProp);
		table.AddProperty(util::str2std(conditionProp->GetLabel()), conditionProp);

		conf::PropertyIdVec changed;
		conf::DefaultGUIGenerator dgg;
		dgg.ShowDialogFromProperties(0, table, changed, _("New Breakpoint"));

		if(!changed.empty())
			Call<void (const String&, int, const String&)>(s_classId, "DeltaVM", "AddBreakpoint")
				(uriProp->GetValue(), lineProp->GetValue(), conditionProp->GetValue());
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(Workspace, DeleteAllBreakpoints, _("/{110}Debug/{230}Delete All Breakpoints\tCtrl+Shift+F9"), MT_MAIN, "")
	{
		if(gui::getConfirmation(0, _("Confirmation"), _("Do you want to delete all breakpoints?")))
			Call<void (void)>(s_classId, "DeltaVM", "DeleteAllBreakpoints")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, EnableAllBreakpoints, (void))
	{
		Call<void (void)>(s_classId, "DeltaVM", "EnableAllBreakpoints")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Workspace, void, DisableAllBreakpoints, (void))
	{
		Call<void (void)>(s_classId, "DeltaVM", "DisableAllBreakpoints")();
	}

	//-----------------------------------------------------------------------

	void Workspace::ComponentDestroyed(void)
	{
		SaveBreakpoints();
		const String& uri = GetURI();
		if (!uri.empty())
			sigWorkspaceClosed(this, uri);
		Container::ComponentDestroyed();
	}

	//-----------------------------------------------------------------------

	void Workspace::SetWorkspaceWorkCommandsStatus(bool enabled)
	{
		const std::string func = enabled ? "EnableWorkCommands" : "DisableWorkCommands";
		List items(1, this);
		assert(m_rootWorkingResource);
		m_rootWorkingResource->GetChildrenRecursively(items);
		BOOST_FOREACH(Component* comp, items)
			Call<void (void), SafeCall>(this, comp, func)();
	}

	//-----------------------------------------------------------------------

	void Workspace::Debug(const String& msg, const std::string& debugFunc)
	{
		Call<void (const String&), SafeCall>(s_classId, "Output", "Append")(msg);
		const String startup = GetStartupResource();
		Handle resource;
		if (!startup.empty() && (resource = GetChildByURI(startup)))
			Call<void (void), SafeCall>(this, resource, debugFunc)();
		else
			Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")(
				_("Execution Error"), _("Startup project for the workspace could not be found!")
			);
	}

	//-----------------------------------------------------------------------

	bool Workspace::SaveBreakpoints(void)
	{
		const String breakpoints = conf::get_prop_value<conf::StringProperty>(GetInstanceProperty("breakpoints"), String());
		return	breakpoints.empty() ? true : 
				Call<bool (const String&)>(this, "DeltaVM", "SaveBreakpoints")(GetPath() + breakpoints);
	}

	//-----------------------------------------------------------------------

	void Workspace::FocusComponent(const std::string& classId)
	{
		Component* comp = ComponentRegistry::Instance().GetFocusedInstance(classId);
		if (comp)
			comp->Focus();
	}

	//-----------------------------------------------------------------------

	const Handle Workspace::GetActiveResource(void)
	{
		const ComponentEntry& workspace = ComponentRegistry::Instance().GetComponentEntry("Workspace");
		const ComponentEntry& editorMgr = ComponentRegistry::Instance().GetComponentEntry("EditorManager");
		if (!workspace.GetFocusedInstance() || !editorMgr.GetFocusedInstance())
			return Handle();

		const Handle& editor =
			Call<const Handle& (void)>(s_classId, editorMgr.GetFocusedInstance(), "GetFocusedEditor")();
		if (!editor)
			return Handle();

		Handle script = Call<const Handle& (void)>(s_classId, editor, "GetAssociatedComponent")();
		if (!script) {
			const String& uri = Call<const String& (void)>(s_classId, editor, "GetURI")();
			script = Call<Handle (const String&)>(s_classId, workspace.GetFocusedInstance(), "GetChildByURI")(uri);
		}
		return script;
	}

	//-----------------------------------------------------------------------

	const Handle Workspace::GetActiveEditor(void)
	{
		const ComponentEntry& editorMgr = ComponentRegistry::Instance().GetComponentEntry("EditorManager");
		return editorMgr.GetFocusedInstance() ?
			Call<const Handle& (void)>(s_classId, editorMgr.GetFocusedInstance(), "GetFocusedEditor")() : Handle();
	}

	//-----------------------------------------------------------------------

	void Workspace::OnWorkCompleted(const Handle& root, const String& task)
	{
		/* 
		**  We save the log upon completion of the build proccess.
		**  Cleaning does not affect the log.                   
		*/
		if (__BL.IsEnabled() && task==util::std2str("Build"))__BL.Save();
		sigWorkCompleted(root, task);
	}

	//-----------------------------------------------------------------------
}
