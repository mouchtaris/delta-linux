/**
 *	Project.h
 *
 *	-- IDE Profile Manager Component --
 *
 *	Project component is the component representing
 *	a collection of script files in the Project
 *	Manager's tree view.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#ifndef PROJECT_MANAGER_PROJECT_H_
#define PROJECT_MANAGER_PROJECT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentHandle.h"

#include "Container.h"

namespace ide
{
	class ComponentConfigurationDialog;

	class Project :
		public Container
	{
		DECLARE_IDE_COMPONENT(Project);

	public:
		///--- type definitions

		///--- constructors / destructor
		Project(void);
		~Project(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(bool, Load, (const String& uri), _("Load project from disk"));
		DECLARE_EXPORTED_FUNCTION_(void, DefaultAction, (void), _("Execute project's default action (configure)"));

		DECLARE_EXPORTED_FUNCTION(void, Run, (void));
		DECLARE_EXPORTED_FUNCTION(void, Debug, (void));
		DECLARE_EXPORTED_FUNCTION(void, DebugWithConsoleDebugger, (void));

		DECLARE_EXPORTED_FUNCTION(void, UpdateScriptProperties, (void));
		DECLARE_EXPORTED_FUNCTION(void, SetStartupProjectCtx, (void));
		DECLARE_EXPORTED_FUNCTION(void, RemoveCtx, (void));
		DECLARE_EXPORTED_FUNCTION(void, PropertiesCtx, (void));

		DECLARE_EXPORTED_FUNCTION(void, SetChildProperties, (const Handle& handle));
		DECLARE_EXPORTED_FUNCTION(int, OnCompareItems, (const Handle& handle1, const Handle& handle2));
		DECLARE_EXPORTED_FUNCTION(void, OnLibraryDefinitionsChanged, (const std::string& classId, const StringList newDefinitions));

	protected:
		///--- protected API
		virtual void ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed);

	private:
		///--- private API
		void OnChildBuilt(const Handle& child);
		void ResetPropertiesForProjectType (void);

		const String GetOutputPath(void);
		const String GetWorkingDirectory(void);
		const String GetDirectoryProperty(const std::string& property);
		const String NewFilterName(void);

		void CommitDebugProject (const std::string& scriptMethod, const std::string& vmMethod);
		void OnConfigurationDialogChange (ComponentConfigurationDialog& dialog, const std::string& name);

		bool shouldReconfigure;
	};
}

#endif	//PROJECT_MANAGER_PROJECT