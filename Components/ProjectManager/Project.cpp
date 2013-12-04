/**
 *	Project.cpp
 *
 *	-- IDE Profile Manager Component --
 *
 *	Project component is the component representing
 *	a collection of script files in the Project
 *	Manager's tree view.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 *  Added support for console debugger invocation, AS, August 2009. 
 */
#include "Project.h"
#include "IDEDialogs.h"

#include "ConsoleHost.h"
#include "StringUtils.h"
#include "ComponentEntry.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"

#include "BitmapRegistry.h"
#include "ComponentConfigurationDialog.h"
#include "ProjectManagerCommon.h"

#include <wx/filename.h>
#include <boost/foreach.hpp>

#include "Icons/run_script.xpm"
#include "Icons/debug_script.xpm"
#include "Icons/debug_script_console.xpm"
#include "Icons/build.xpm"
#include "Icons/delete.xpm"

namespace ide
{
	static const char *defaultChildren[] = { "Script" };

	//-------------------------------------------------------//
	//---- class Project ------------------------------------//

	COMPONENT_METADATA(
		Project,
		_("Project"),
		_("Project component is the component representing a collection of \
		  script files in the Project Manager's tree view"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("0.1")
	);
	IMPLEMENT_COMPONENT_(Project, Container);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(Project, table)
	{
		StringVec options;
		options.push_back(String(_T("Project")));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of project in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	Project::Project(void)
	{
		StringVec options;
		options.push_back(DELTA_APPLICATION);
		options.push_back(DELTA_LIBRARY);
		options.push_back(HOST_APPLICATION_ACTIVATOR);
		
		AddInstanceProperty(conf::GetDeploymentPropertyId(), new conf::EnumStringProperty(_("Deployment"), options, 0,
			_("Delta project deployment type"), _("General"))
		);

		AddInstanceProperty("host", new conf::FileProperty(_("Host Application (Release)"), String(),
			_("Host application binary for extension projects"), _("Execution"))
		);
		AddInstanceProperty("hostd", new conf::FileProperty(_("Host Application (Debug)"), String(),
			_("Host application debug binary for extension projects"), _("Execution"))
		);
		AddInstanceProperty("args", new conf::StringProperty(_("Host Application Arguments (Release)"), String(),
			_("Host application arguments for extension projects"), _("Execution"))
		);
		AddInstanceProperty("argsd", new conf::StringProperty(_("Host Application Arguments (Debug)"), String(),
			_("Host application debug arguments for extension projects"), _("Execution"))
		);

		conf::AddScriptExecutionProperties(this);
		conf::AddProjectBuildProperties(this);

		ResetPropertiesForProjectType();
	}

	//-----------------------------------------------------------------------

	Project::~Project(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Project, void, Initialize, (void))
	{
		Call<void (const std::string&, const std::string&)>
			Register(s_classId, "TreeItemComponent", "RegisterChildType");
		BOOST_FOREACH(const char* child, defaultChildren)
			Register(s_classId, child);

		extensions.insert(std::make_pair(s_classId, ".dproj"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Project, void, CleanUp, (void))
	{
		extensions.erase(s_classId);

		Call<void (const std::string&, const std::string&), SafeCall>
			Unregister(s_classId, "TreeItemComponent", "UnregisterChildType");
		BOOST_FOREACH(const char* child, defaultChildren)
			Unregister(s_classId, child);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Project, bool, Load, (const String& uri))
	{
		if (!Container::Load(uri))
			return false;

		ResetPropertiesForProjectType();
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Project, void, DefaultAction, (void))
	{
		PropertiesCtx();
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Project, "run_project", run_script_xpm);
	EXPORTED_CMD_FUNCTION(Project, Run, _T("/{5}--Run Project"), MT_NMAIN, "run_project")
	{
		using namespace conf;
		const String& type = get_prop_value<EnumStringProperty>(GetInstanceProperty(GetDeploymentPropertyId()));
		if (type == DELTA_APPLICATION) {
			const String startup = GetStartupResource();
			if (startup.empty())
				Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
					(_("Execution Error"), _("There is no start-up script for the project, please check project's properties!"));
			else if (const Handle resource = GetChildByURI(startup))
				Call<void (void), SafeCall>(this, resource, "Run")();
			else
				Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
					(_("Execution Error"), _("Start-up script of the project could not be found!"));
		}
		else if (type == DELTA_LIBRARY)
			Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
				(_("Execution Error"), _("Library projects can not be exectued, you should change the type property of the project to execute!"));
		else if (type == HOST_APPLICATION_ACTIVATOR) {
			const String host =	get_path_prop_value<FileProperty>(GetInstanceProperty("host"), String());
			if (host.empty())
				Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
					(_("Execution Error"), _("No host application has been specified for the activator project!"));
			else
			{
				wxFileName filename(host);
				if (filename.IsRelative())
					filename.MakeAbsolute(GetPath());
				const std::string chost = util::str2std(filename.GetFullPath());
				const std::string& args = util::str2std(get_prop_value<StringProperty>(GetInstanceProperty("args"), String()));
				util::ConsoleHost().Execute(chost + " " + args, util::str2std(GetWorkingDirectory()));
			}
		}
		else
			Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
				(_("Execution Error"), _("Unrecognized project type, can not execute project!"));
	}

	//-----------------------------------------------------------------------

	void Project::CommitDebugProject (const std::string& scriptMethod, const std::string& vmMethod) {

		using namespace conf;
		const String& type = get_prop_value<EnumStringProperty>(GetInstanceProperty(GetDeploymentPropertyId()));
		if (type == DELTA_APPLICATION) {
			const String startup = GetStartupResource();
			if (startup.empty())
				Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
					(_("Execution Error"), _("There is no start-up script for the project, please check project's properties!"));
			else if (const Handle resource = GetChildByURI(startup))
				Call<void (void), SafeCall>(this, resource, scriptMethod)();
			else
				Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
					(_("Execution Error"), _("Start-up script of the project could not be found!"));
		}
		else if (type == DELTA_LIBRARY)
			Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
				(_("Execution Error"), _("Library projects can not be exectued, you should change the type property of the project to execute!"));
		else if (type == HOST_APPLICATION_ACTIVATOR) {
			String hostd = get_path_prop_value<FileProperty>(GetInstanceProperty("hostd"), String());
			if (hostd.empty())
				Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
					(_("Execution Error"), _("No debug host application has been specified for the extension project!"));
			else {
				wxFileName filename(hostd);
				if (filename.IsRelative())
					filename.MakeAbsolute(GetPath());
				hostd = filename.GetFullPath();
				const String argsd = get_prop_value<StringProperty>(GetInstanceProperty("argsd"), String());

				Call<void (const String&, const String&, const String&)>(this, "DeltaVM", vmMethod)
					(hostd + _T(" ") + argsd, _T("host application"), GetWorkingDirectory());
			}
		}
		else
			Call<void (const String&, const String&)>(this, "Shell", "ShowMessage")
				(_("Execution Error"), _("Unrecognized project type, can not debug project!"));
	}

	/////////////////////////////////////////////////////////////////////////

	EXPORTED_IMAGE(Project, "debug_project", debug_script_xpm);
	EXPORTED_CMD_FUNCTION(Project, Debug, _T("/{6}Debug Project"), MT_NMAIN, "debug_project")
	{
		CommitDebugProject("Debug", "DebugLocalScript");
	}

	EXPORTED_IMAGE(Project, "debug_project_console", debug_script_console_xpm);
	EXPORTED_CMD_FUNCTION(Project, DebugWithConsoleDebugger, _T("/{7}Debug Project (Console)--"), MT_NMAIN, "debug_project_console")
	{
		CommitDebugProject("DebugWithConsoleDebugger", "DebugLocalScriptWithConsoleDebugger");
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(Project, UpdateScriptProperties, _("/{18}--Update Script Properties"), MT_CTX, "")
	{
		const String message = _("This action will overwrite all build and execution properties for scripts within this project.\nAre you sure you want to proceed?");
		if (gui::getConfirmation(0, _T("Project"), message)) {
			HandleList scripts = CollectChildren(_T("Script"));
			BOOST_FOREACH(const Handle& script, scripts)
				SetChildProperties(script);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(Project, SetStartupProjectCtx, _("/{19}Select"), MT_CTX, "")
	{
		Call<bool (const String&), SafeCall>(this, GetParent(), "SetStartupResource")(GetURI());
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Project, "delete", delete_xpm);
	EXPORTED_CMD_FUNCTION(Project, RemoveCtx, _("/{50}--Remove"), MT_CTX, "delete")
	{
		assert(treeview);
		const Handle& parent = Call<const Handle& (const Handle&)>(this, treeview, "GetParent")(this);
		Call<bool (const Handle&)>(this, treeview, "RemoveComponent")(this);
		Destroy();

		if (parent)
			Call<void (void)>(s_classId, parent, "Save")();
	}

	//-----------------------------------------------------------------------

	void Project::OnConfigurationDialogChange(ComponentConfigurationDialog& dialog, const std::string& name) {
		if (name == conf::GetDeploymentPropertyId()) {
			dialog.ApplyConfiguration();
			shouldReconfigure = true;
		}
	}

	EXPORTED_FUNCTION(Project, void, PropertiesCtx, (void))
	{
		//Changing project deployment changes the visible properties, but we can't change them during editing
		//so we need to be notified for changes, use them to get the updated UI and reconfigure.
		//Of course, in case of dialog cancellation we should be able to revert to the original properties
		//and undo any actions that previous saves may have caused during property editing.

		const conf::PropertyTable& table = GetInstancePropertyTable();
		conf::PropertyTable* old = static_cast<conf::PropertyTable*>(table.Clone());

		ComponentConfigurationDialog dialog;
		dialog.SetOnConfigurationDialogChange(
			boost::bind(&Project::OnConfigurationDialogChange, this, boost::ref(dialog), _1)
		);
		do {
			shouldReconfigure = false;
			if (!dialog.ConfigureInstance(this)) {	//cancelled so restore old properties and apply them
				assert(!shouldReconfigure);
				conf::PropertyTable* changed = static_cast<conf::PropertyTable*>(table.Clone());
				const_cast<conf::PropertyTable&>(table) = *old;
				ComponentAppliedChangedProperties(*changed, conf::PropertyIdVec(1, "*"));
				delete changed;
			}
		} while (shouldReconfigure);
		
		delete old;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Project, void, SetChildProperties, (const Handle& handle))
	{
		using namespace conf;
		if (Component* child = handle.Resolve()) {
			for (const char** p = conf::GetProjectBuildPropertyIdsForScripts(); *p; ++p)
				if (const Property* prop = GetInstanceProperty(*p))
					child->AddInstanceProperty(*p, prop->Clone());
			for (const char** p = conf::GetScriptExecutionPropertyIds(); *p; ++p)
				if (const Property* prop = GetInstanceProperty(*p))
					child->AddInstanceProperty(*p, prop->Clone());

			if (child->GetClassId() == "Script") {
				const conf::Property* deploymentProp = GetInstanceProperty(conf::GetDeploymentPropertyId());
				const String deployment = conf::get_prop_value<conf::EnumStringProperty>(deploymentProp);
				if (deployment != DELTA_APPLICATION) {
					conf::EnumStringProperty* prop = conf::GenerateScriptDeploymentProperty();
					prop->SetValue(_T(DELTA_SCRIPT_DEPLOYMENT_LIBRARY));
					prop->SetEnabled(false);
					child->AddInstanceProperty(conf::GetDeploymentPropertyId(), prop->Clone());
					delete prop;
				}

				conf::Property* output = const_cast<conf::Property*>(child->GetInstanceProperty("output"));
				if (output && conf::get_prop_value<conf::StringProperty>(output, _T("")).empty()) {
					String name = Call<const String& (void)>(this, child, "GetName")();
					name = name.substr(0, name.find_last_of(_T(".")));
					conf::set_prop_value<conf::StringProperty>(output, name);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Project, int, OnCompareItems, (const Handle& handle1, const Handle& handle2))
	{
		char *priorities[] = { "Filter", "Script", "TextFile", "GenericFile" };
		assert(handle1.Resolve() && handle2.Resolve());
		if (handle1->GetClassId() != handle2->GetClassId()) {
			char **pos1 = std::find(priorities, priorities + SIZEOF_ARRAY(priorities), handle1->GetClassId());
			char **pos2 = std::find(priorities, priorities + SIZEOF_ARRAY(priorities), handle2->GetClassId());
			if (pos1 < pos2)
				return -1;
			else if (pos2 < pos1)
				return 1;
		}
		return	wxStricmp(
					Call<const String& (void)>(this, handle1, "GetName")(),
					Call<const String& (void)>(this, handle2, "GetName")()
				);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Project, void, OnLibraryDefinitionsChanged,
		(const std::string& classId, const StringList newDefinitions), "LibraryDefinitionsChanged")
	{
		using namespace conf;
		if (const Property *property = GetInstanceProperty("libs")) {
			const PropertyTable old = GetInstancePropertyTable();
			MultiChoiceProperty* p = safe_prop_cast<MultiChoiceProperty>(const_cast<Property*>(property));
			BOOST_FOREACH(const String& prevChoice, p->GetAllChoices())	//Remove old choices
				if (std::find(newDefinitions.begin(), newDefinitions.end(), prevChoice) == newDefinitions.end())
					p->RemoveChoice(prevChoice);
			BOOST_FOREACH(const String& def, newDefinitions)			//Add new choices
				p->AddChoice(def);
			this->ComponentAppliedChangedProperties(old, PropertyIdVec(1, "libs"));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Project, void, OnAspectProjectAdded,
		(const std::string& classId, const Handle& project), "AspectProjectAdded")
	{
		if (Component* comp = project.Resolve()) {
			using namespace conf;
			if (const Property *property = GetInstanceProperty("aspects")) {
				MultiChoiceProperty* p = safe_prop_cast<MultiChoiceProperty>(const_cast<Property*>(property));
				p->AddChoice(Call<const String& (void)>(this, comp, "GetName")());
			}
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_SLOT_MEMBER(Project, void, OnAspectProjectRemoved,
		(const std::string& classId, const Handle& project), "AspectProjectRemoved")
	{
		if (Component* comp = project.Resolve()) {
			using namespace conf;
			if (const Property *property = GetInstanceProperty("aspects")) {
				MultiChoiceProperty* p = safe_prop_cast<MultiChoiceProperty>(const_cast<Property*>(property));
				p->RemoveChoice(Call<const String& (void)>(this, comp, "GetName")());
			}
		}
	}

	//-----------------------------------------------------------------------

	void Project::ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
	{
		bool allPropsChanged = std::find(changed.begin(), changed.end(), "*") != changed.end();
		using namespace conf;
		if (!changed.empty()) {
			if (allPropsChanged || std::find(changed.begin(), changed.end(), "startup") != changed.end()) {
				const String newStartup = get_prop_value<StringProperty>(GetInstanceProperty("startup"), String());
				set_prop_value<StringProperty>(			//first restore the original startup resource
					const_cast<Property*>(GetInstanceProperty("startup")),
					get_prop_value<StringProperty>(old.GetProperty("startup"), String())
				);
				if (const Handle resource = GetChildBySymbolicURI(newStartup))	//then if the new is valid apply it
					SetStartupResource(Call<const String& (void)>(this, resource, "GetURI")());
			}

			if (allPropsChanged || std::find(changed.begin(), changed.end(), GetDeploymentPropertyId()) != changed.end())
				ResetPropertiesForProjectType();
		}

		TreeItemComponent::ComponentAppliedChangedProperties(old, changed);
	}

	//-----------------------------------------------------------------------

	void Project::ResetPropertiesForProjectType(void)
	{
		using namespace conf;
		static const char *allProperties[] = { "host", "hostd", "args", "argsd", "startup" };
		static const char *applicationProperties[] = { "startup", 0 };
		static const char *libraryProperties[] = { 0 };
		static const char *hostActivatorProperties[] = { "host", "hostd", "args", "argsd", 0 };

		BOOST_FOREACH(const char * property, allProperties)
			const_cast<Property*>(GetInstanceProperty(property))->SetVisible(false);

		typedef std::map<String, const char **> PropertyMapping;
		static PropertyMapping propertyMap;
		if (propertyMap.empty()) {
			propertyMap[DELTA_APPLICATION]			= applicationProperties;
			propertyMap[DELTA_LIBRARY]				= libraryProperties;
			propertyMap[HOST_APPLICATION_ACTIVATOR] = hostActivatorProperties;
		}

		const Property* deploymentProp = GetInstanceProperty(GetDeploymentPropertyId());
		const String deployment = get_prop_value<EnumStringProperty>(deploymentProp);
		PropertyMapping::const_iterator iter = propertyMap.find(deployment);
		assert(iter != propertyMap.end());
		for(unsigned i = 0; iter->second[i]; ++i)
			const_cast<Property*>(GetInstanceProperty(iter->second[i]))->SetVisible(true);
		
		const HandleList scripts = CollectChildren(_("Script"));
		if (deployment == DELTA_APPLICATION) {
			BOOST_FOREACH(const Handle& script, scripts)
				if (Component* child = script.Resolve())
					const_cast<Property *>(child->GetInstanceProperty(GetDeploymentPropertyId()))->SetEnabled(true);
		}
		else {
			EnumStringProperty* prop = GenerateScriptDeploymentProperty();
			prop->SetValue(_T(DELTA_SCRIPT_DEPLOYMENT_LIBRARY));
			prop->SetEnabled(false);
			BOOST_FOREACH(const Handle& script, scripts)
				if (Component* child = script.Resolve())
					child->AddInstanceProperty(GetDeploymentPropertyId(), prop->Clone());
			delete prop;
		}
	}

	//-----------------------------------------------------------------------

	const String Project::GetOutputPath(void)
	{
		return GetDirectoryProperty("output_path");
	}

	//-----------------------------------------------------------------------

	const String Project::GetWorkingDirectory(void)
	{
		return GetDirectoryProperty("working_directory");
	}

	//-----------------------------------------------------------------------

	const String Project::GetDirectoryProperty(const std::string& property)
	{
		String directory = conf::get_path_prop_value<conf::DirectoryProperty>(GetInstanceProperty(property));		
		if (directory.empty() || wxFileName(directory).IsRelative())
			directory = GetPath() + directory;
		directory = util::normalizepath(directory);
		if (directory.GetChar(directory.length() - 1) != _T('/'))
			directory += _T('/');
		return directory;
	}

	//-----------------------------------------------------------------------

	const String Project::NewFilterName(void)
	{
		int count = 0;
		String name;
		while(true) {
			bool found = true;
			name = wxString::Format(_T("NewFilter%d"), ++count);
			BOOST_FOREACH(Component* child, GetChildren())
				if (child->GetClassId() == "Filter" && Call<const String& (void)>(this, child, "GetName")() == name) {
					found = false;
					break;
				}
			if (found)
				break;
		}
		return name;
	}
}
