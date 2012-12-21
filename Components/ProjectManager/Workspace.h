/**
 *	Workspace.h
 *
 *	-- IDE Workspace Component --
 *
 *	Workspace component is the root component of
 *	the Project Manager's tree view, representing
 *	a workspace container of projects.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#ifndef PROJECT_MANAGER_WORKSPACE_H_
#define PROJECT_MANAGER_WORKSPACE_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

#include "Container.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "xml.h"

namespace ide
{
	class Workspace :
		public Container
	{
		DECLARE_IDE_COMPONENT(Workspace);

	public:
		///--- type definitions

		///--- constructors / destructor
		Workspace(void);
		~Workspace(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION(bool, CanDestroy, (void));

		DECLARE_EXPORTED_FUNCTION_(bool, Load, (const String& uri), _("Load workspace from disk"));
		DECLARE_EXPORTED_FUNCTION_(bool, SaveAs, (const String& uri), _("Save workspace to a new file"));
		DECLARE_EXPORTED_FUNCTION_(bool, Close, (void), _("Close workspace"));

		DECLARE_EXPORTED_FUNCTION_(void, BuildScript, (void), _("Build Current Script"));
		DECLARE_EXPORTED_FUNCTION_(void, BuildScriptWithDebugging, (void), _("Build Current Script with Debugging"));

		DECLARE_EXPORTED_FUNCTION_(void, Debug, (void),	_("Debug Workspace"));
		DECLARE_EXPORTED_FUNCTION_(void, DebugWithConsoleDebugger, (void),	_("Debug Workspace (console)"));
		DECLARE_EXPORTED_STATIC_(void, DebugAttach, (void),	_("Attach to a remote debugger"));
		DECLARE_EXPORTED_STATIC_(void, DebugExecutable, (void),	_("Debug an executable"));

		DECLARE_EXPORTED_FUNCTION(virtual void, EnableWorkCommands, (void));
		DECLARE_EXPORTED_FUNCTION(virtual void, DisableWorkCommands, (void));

		DECLARE_EXPORTED_MEMBER(void, OnDebugStarted, (const std::string& classId, const String& uri));
		DECLARE_EXPORTED_MEMBER(void, OnDebugStopped, (const std::string& classId, const String& uri));

		DECLARE_EXPORTED_STATIC(void, OnFirstBreakpoint, (void));
		DECLARE_EXPORTED_STATIC(void, OnFirstBreakpointEnabled, (void));
		DECLARE_EXPORTED_STATIC(void, OnFirstBreakpointDisbled, (void));
		DECLARE_EXPORTED_STATIC(void, OnNoBreakpoints, (void));
		DECLARE_EXPORTED_STATIC(void, OnNoBreakpointsEnabled, (void));
		DECLARE_EXPORTED_STATIC(void, OnNoBreakpointsDisabled, (void));

		DECLARE_EXPORTED_FUNCTION(const Handle&, GetRootWorkingResource, (void));
		DECLARE_EXPORTED_FUNCTION(void, StartWorking, (const Handle& root, const String& task));

		DECLARE_EXPORTED_FUNCTION(void, OnResourceWorkCompleted, (const Handle& resource, const String& task, const UIntList& workId));

		DECLARE_EXPORTED_STATIC_(void, Go, (void), _("Continue debugging"));
		DECLARE_EXPORTED_STATIC_(void, Break, (void), _("Break execution"));
		DECLARE_EXPORTED_STATIC_(void, StopDebug, (void),_("Stop debugging"));
		DECLARE_EXPORTED_STATIC_(void, ShowNextStatement, (void),_("Show the next statement to be executed"));
		DECLARE_EXPORTED_STATIC_(void, StepIn, (void), _("Step into function"));
		DECLARE_EXPORTED_STATIC_(void, SelectiveStepIn, (void), _("Selectively step into function"));
		DECLARE_EXPORTED_STATIC_(void, StepOver, (void), _("Step over function"));
		DECLARE_EXPORTED_STATIC_(void, StepOut, (void), _("Step out of current function"));
		DECLARE_EXPORTED_STATIC_(void, RunToCursor, (void), _("Continue debugging until the cursor position"));

		DECLARE_EXPORTED_STATIC_(void, CancelWork, (void), _("Cancels the work in progress"));

		DECLARE_EXPORTED_STATIC_(void, ToggleBreakpoint, (void),
			_("Toggle a breakpoint to the cursor"));
		DECLARE_EXPORTED_STATIC_(void, ToggleBreakpointStatus, (void),
			_("Enable or disable a breakpoint to the cursor"));
		DECLARE_EXPORTED_STATIC_(void, NewBreakpoint, (void),
			_("Insert a new breakpoint"));
		DECLARE_EXPORTED_STATIC_(void, DeleteAllBreakpoints, (void),
			_("Delete all breakpoints"));
		DECLARE_EXPORTED_STATIC_(void, EnableAllBreakpoints, (void),
			_("Enable all breakpoints"));
		DECLARE_EXPORTED_STATIC_(void, DisableAllBreakpoints, (void),
			_("Disable all breakpoints"));

	protected:
		virtual bool IsTopLevelContainer(void) const { return true; }

		///--- protected API
		virtual void ComponentDestroyed(void);

	private:
		///--- private API
		void SetWorkspaceWorkCommandsStatus(bool enabled);
		void Debug(const String& msg, const std::string& debugFunc);

		bool SaveBreakpoints(void);
		static void FocusComponent(const std::string& classId);
		static const Handle GetActiveResource(void);
		static const Handle GetActiveEditor(void);
		static void OnWorkCompleted(const Handle& root, const String& task);

		///--- private members
		static std::string	debuggerId;
		static Handle		debuggerHandle;
		Handle				m_rootWorkingResource;
		String				m_task;
		bool				m_buildingEditorScript;
	};
}

#endif	//PROJECT_MANAGER_WORKSPACE
