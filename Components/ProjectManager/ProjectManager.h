/**
 *	ProjectManager.h
 *
 *	-- IDE Project Manager Component --
 *
 *	Project manager component is a visual component
 *	that allows selection / manipulation and organization
 *	of IDE projects and scripts.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#ifndef IDE_PROJECT_MANAGER_COMPONENT_H_
#define IDE_PROJECT_MANAGER_COMPONENT_H_

#include "ProjectManagerCommon.h"
#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"
#include "ComponentUserCommand.h"

#include "TreeCtrlComponent.h"
#include "UndefineAllWin32Shit.h"

namespace ide
{
	class ProjectManager :
		public TreeCtrlComponent
	{
		DECLARE_IDE_COMPONENT(ProjectManager);

	public:
		///--- type definitions

		///--- constructors / destructor
		ProjectManager(void);
		~ProjectManager(void);

		///--- public overridden API
		virtual wxWindow* GenerateWindow(wxWindow* parent);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_FUNCTION(bool, CanDestroy, (void));

		DECLARE_EXPORTED_FUNCTION_(bool, OpenWorkspace, (const String& uri),
			_("Load a workspace from file"));
		DECLARE_EXPORTED_FUNCTION_(const Handle&, GetWorkspace, (void),
			_("Returns a handle to the current workspace"));

		DECLARE_EXPORTED_FUNCTION_(void, NewWorkspace, (const String& name, const String& folder),
			_("Create a new workspace"));
		DECLARE_EXPORTED_FUNCTION_(void, NewWorkspaceMenu, (void),
			_("Create a new workspace using an interactive dialog"));
		DECLARE_EXPORTED_FUNCTION_(void, NewWorkspaceToolbar, (void),
			_("Create a new workspace using an interactive dialog"));
		DECLARE_EXPORTED_FUNCTION_(void, NewWorkspaceDialog, (void),
			_("Create a new workspace using an interactive dialog"));

		DECLARE_EXPORTED_FUNCTION_(void, OpenWorkspaceMenu, (void),
			_("Choose an existing workspace to open"));
		DECLARE_EXPORTED_FUNCTION_(void, OpenWorkspaceToolbar, (void),
			_("Choose an existing workspace to open"));
		DECLARE_EXPORTED_FUNCTION_(void, OpenWorkspaceDialog, (void),
			_("Choose an existing workspace to open"));

		DECLARE_EXPORTED_FUNCTION_(void, SaveWorkspaceMenu, (void),
			_("Save the currently open workspace"));
		DECLARE_EXPORTED_FUNCTION_(void, SaveWorkspaceToolbar, (void),
			_("Save the currently open workspace"));
		DECLARE_EXPORTED_FUNCTION_(void, SaveWorkspace, (void),
			_("Save the currently open workspace"));

		DECLARE_EXPORTED_FUNCTION_(void, SaveAllMenu, (void),
			_("Save all files in the currently open workspace"));
		DECLARE_EXPORTED_FUNCTION_(void, SaveAllToolbar, (void),
			_("Save all files in the currently open workspace"));
		DECLARE_EXPORTED_FUNCTION_(void, SaveAll, (void),
			_("Save all files in the currently open workspace"));

		DECLARE_EXPORTED_FUNCTION_(void, CloseWorkspace, (void),
			_("Close the currently open workspace"));
		DECLARE_EXPORTED_FUNCTION_(void, SetStartupResource, (const Handle& comp, bool enable),
			_("Set the startup resource state"));

		DECLARE_EXPORTED_STATIC_(void, View, (void),
			_("Show the Project Manager Window"));

	protected:
		///--- protected API

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		Component*	CreateWorkspace(void);
		void		onBeginDrag	(wxTreeEvent& event);
		void		onEndDrag	(wxTreeEvent& event);

		///--- private members
	};
}

#endif	//IDE_PROJECT_MANAGER_COMPONENT_H_
