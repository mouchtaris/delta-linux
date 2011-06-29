/**
 *	Script.cpp
 *
 *	IDE Project Manager Component --
 *
 *	Script component is the component representing
 *	a script files in the Project Manager's tree view.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007.
 *  Added support for console debugger invocation, AS, August 2009. 
 */

#include "Script.h"

#include "StringUtils.h"
#include "Properties.h"
#include "ExtProperties.h"
#include "PropertyUtils.h"
#include "ProjectManagerCommon.h"

#include "ComponentRegistry.h"
#include "ComponentFactory.h"
#include "ComponentEntry.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "DelayedCaller.h"
#include "ConsoleHost.h"

#include "BitmapRegistry.h"
#include "ComponentConfigurationDialog.h"
#include "GenericDialogs.h"
#include "DynamicLoader.h"

#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

#include <wx/filename.h>

#include <fstream>
#include <ctype.h>
#include <assert.h>

#include "Icons/script.xpm"
#include "Icons/build.xpm"
#include "Icons/run_script.xpm"
#include "Icons/debug_script.xpm"
#include "Icons/debug_script_console.xpm"
#include "Icons/delete.xpm"
#include "Icons/properties.xpm"
#include <algorithm>

#define MAX_LIBRARY_FUNC_DESCRIPTION		1024

namespace ide
{
	//-------------------------------------------------------//
	//---- class Script -------------------------------------//

	COMPONENT_METADATA(
		Script,
		_("Script"),
		_("Script component is the component representing \
		  script files in the Project Manager's tree view"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("0.1")
		);
	IMPLEMENT_COMPONENT_(Script, TextFile);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(Script, table)
	{
		StringVec options;
		options.push_back(_T("Script"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of script in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SIGNAL(Script, ResourceWorkStarted, (const String& task, const UIntList& workId));
	EXPORTED_SIGNAL(Script, ResourceWorkCompleted, (const String& task, const UIntList& workId));

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Script, void, Initialize, (void))
	{
		extensions.insert(std::make_pair(s_classId, ".dsc"));
		s_allScripts = new ScriptPtrList;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Script, void, CleanUp, (void))
	{
		extensions.erase(s_classId);
		delete s_allScripts;
		s_allScripts = (ScriptPtrList*) 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Script, const std::string&, GetDebuggerId, (void))
	{
		static std::string debuggerId("DeltaVM");
		return debuggerId;
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Script, "run_script", run_script_xpm);

	EXPORTED_CMD_FUNCTION(Script, Run, _("/{10}--Run"), MT_NMAIN, "run_script")
		{ RunImpl("RunConsole"); }

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Script, "debug_script", debug_script_xpm);
	
	EXPORTED_CMD_FUNCTION(Script, Debug, _("/{20}Debug"), MT_NMAIN, "debug_script")
		{ RunImpl("DebugConsole"); }

	EXPORTED_IMAGE(Script, "debug_script_console", debug_script_console_xpm);

	EXPORTED_CMD_FUNCTION(Script, DebugWithConsoleDebugger, _("/{30}Debug (Console)"), MT_NMAIN, "debug_script_console")
		{ RunImpl("DebugConsoleWithConsoleDebugger"); }

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Script, "build", build_xpm);
	EXPORTED_CMD_FUNCTION(Script, BuildCtx, _("/{50}--Build"), MT_NMAIN, "build")
	{
		const Handle& workspace = Call<const Handle& (void)>(this, treeview, "GetWorkspace")();
		if (!Call<const Handle& (void)>(this, workspace, "GetRootWorkingResource")()) {
			Call<void (const Handle&, const String&)>(this, workspace, "StartWorking")(this, _T("Build"));
			Build(UIntList(1, 1));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(Script, CleanCtx, _("/{60}Clean"), MT_CTX, "")
	{
		const Handle& workspace = Call<const Handle& (void)>(this, treeview, "GetWorkspace")();
		if (!Call<const Handle& (void)>(this, workspace, "GetRootWorkingResource")()) {
			Call<void (const Handle&, const String&)>(this, workspace, "StartWorking")(this, _T("Clean"));
			Clean(UIntList(1, 1));
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Script, void, EnableWorkCommands, (void))
	{
		EnableCommand(_("/Build"));
		EnableCommand(_("/Clean"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Script, void, DisableWorkCommands, (void))
	{
		DisableCommand(_("/Build"));
		DisableCommand(_("/Clean"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(Script, SetStartupScript, _("/{100}--Select"), MT_CTX, "")
	{
		Call<bool (const String&), SafeCall>(this, GetParent(), "SetStartupResource")(GetURI());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Script, bool, Rename, (const String& newName))
	{
		const String oldUri = GetURI();
		if (GenericFile::Rename(newName)) {
			if (wxFileName(oldUri).SameAs(Call<const String(void)>(this, GetParent(), "GetStartupResource")()))
				SetStartupScript();
			return true;
		}
		else
			return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Script, const String, GetWorkingDirectory, (void))
	{
		return GetDirectoryProperty("working_directory");
	}

	EXPORTED_FUNCTION(Script, const String, GetOutputDirectory, (void))
	{
		return GetDirectoryProperty("output_path");
	}

	//-----------------------------------------------------------------------
	// Called also when the file is opened!

	EXPORTED_SLOT_MEMBER(Script, void, OnFileNameChanged, (const Handle& editor, const String& uri), "FileNameChanged") {
		if (editor.GetClassId() == "Editor" && uri == GetURI()) {
			Call<void (const String&)>(this, editor, "SetByteCodeLoadingPath")(GetByteCodeLoadingPath());

			StringList libfuncs = GetFileLibraryFunctions();
			if (!libfuncs.empty())
				Call<void (const StringList&)>(this, editor, "SetExtraLibraryDefinitions")(libfuncs);
		}
	}

	//-----------------------------------------------------------------------

	void Script::ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
	{
		HandleNewProperties(changed);
		GenericFile::ComponentAppliedChangedProperties(old, changed);
	}

	//-----------------------------------------------------------------------

	void Script::ComponentAddedProperty(const std::string& id, conf::Property* prop)
	{
		HandleNewProperties(conf::PropertyIdVec(1, id));
		GenericFile::ComponentAddedProperty(id, prop);
	}

	//-----------------------------------------------------------------------

	void Script::HandleNewProperties (const conf::PropertyIdVec& changed) {

		using namespace conf;

		CheckDeploymentPropertyChanged(changed);

		if (ComponentRegistry::Instance().GetFocusedInstance("EditorManager")) {

			PropertyIdVec::const_iterator ib = std::find(changed.begin(), changed.end(), GetByteCodePathPropertyId());
			PropertyIdVec::const_iterator il = std::find(changed.begin(), changed.end(), "libs");

			if (ib != changed.end() || il != changed.end()) 
				if (const Handle& editor = Call<const Handle& (const String&)>(this, "EditorManager", "GetEditor")(GetURI())) {
					if (ib != changed.end())
						Call<void (const String&)>(this, editor, "SetByteCodeLoadingPath")(GetByteCodeLoadingPath());
					if (il != changed.end())
						Call<void (const StringList&)>(this, editor, "SetExtraLibraryDefinitions")(GetFileLibraryFunctions());
				}
		}
	}

	//-----------------------------------------------------------------------

	const String Script::ExtractFuncFilesStr(void)
	{
		String funcFilesStr;
		if (const conf::Property* property = GetInstanceProperty("libs")) {
			const conf::MultiChoiceProperty *p = conf::safe_prop_cast<const conf::MultiChoiceProperty>(property);
			BOOST_FOREACH(const String& lib, p->GetSelectedChoices())
				funcFilesStr += _T(" --funcfile=") + util::quotepath(GetLibraryPath(lib));
		}
		return funcFilesStr;
	}

	//-----------------------------------------------------------------------

	const String Script::GetDirectoryProperty(const std::string& property) const
	{
		String directory = conf::get_prop_value<conf::DirectoryProperty>(GetInstanceProperty(property));		
		if (directory.empty() || wxFileName(directory).IsRelative()) {
			Script *self = const_cast<Script*>(this);
			directory = const_cast<Script*>(this)->GetPath() + directory;
		}
		directory = util::normalizepath(directory);
		if (directory.GetChar(directory.length() - 1) != _T('/'))
			directory += _T('/');
		return directory;
	}
	
	//-----------------------------------------------------------------------

	const StringList Script::GetFileLibraryFunctions(void) const {

		const String directory(const_cast<Script*>(this)->GetPath());
		StringList functions;
		if (const conf::Property* property = GetInstanceProperty("libs")) {
			const conf::MultiChoiceProperty *p = conf::safe_prop_cast<const conf::MultiChoiceProperty>(property);
			BOOST_FOREACH(const String& lib, p->GetSelectedChoices()) {

				const String path = GetLibraryPath(lib);
				functions.push_back(_T("#libname(") + path + _T(")"));
				std::ifstream in(util::str2std(path).c_str());

				if(in) {
					char str[MAX_LIBRARY_FUNC_DESCRIPTION];
					while(!in.eof()) {
						in.getline(str, MAX_LIBRARY_FUNC_DESCRIPTION - 1);
						const std::string value = util::strip<std::string>(str, UTIL_WHITESPACE);
						if(!value.empty())
							functions.push_back(util::std2str(str));
					}
				}
			}
		}
		return functions;
	}

	//-----------------------------------------------------------------------

	const conf::Property* Script::GetDeltaVMProperty(const std::string& id) const
	{
		using namespace conf;
		ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry("DeltaVM");
		if (const Property* vmProps = compEntry.GetProperty("vm"))
			return safe_prop_cast<const AggregateProperty>(vmProps)->GetProperty(id);
		else
			return (conf::Property*) 0;
	}

	//-----------------------------------------------------------------------

	const String Script::GetLibraryPath(const String& lib) const
	{
		if (const conf::Property* property = GetDeltaVMProperty("libdefs")) {
			const conf::AggregateListProperty::AggregatePropertyList& l = 
				conf::safe_prop_cast<const conf::AggregateListProperty>(property)->GetPropertyList();
			BOOST_FOREACH(const conf::AggregateProperty* p, l)
				if (conf::get_prop_value<conf::StringProperty>(p->GetProperty("name"), String()) == lib)
					return conf::get_prop_value<conf::FileProperty>(p->GetProperty("path"), String());
		}
		return String();
	}

	//-----------------------------------------------------------------------

	void Script::OnResourceWorkStarted(Component *component, const String& task, const UIntList& workId)
	{
		sigResourceWorkStarted(component, task, workId);
	}

	//-----------------------------------------------------------------------

	void Script::OnResourceWorkCompleted(Component *component, const String& task, const UIntList& workId)
	{
		sigResourceWorkCompleted(component, task, workId);
	}

	//-----------------------------------------------------------------------
}
