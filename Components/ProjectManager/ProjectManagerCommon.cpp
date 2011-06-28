/*
 *	ProjectManagerCommon.cpp
 *	Common stuff across project manager files.
 *	Anthony Savidis, November 2010
 *  Functionality moved here from opriginal code from Yannis Lilis.
 */
#include "ProjectManagerCommon.h"
#include "ComponentHandle.h"
#include "ComponentEntry.h"
#include "ComponentRegistry.h"
#include "Call.h"
#include <boost/foreach.hpp>

using namespace ide;

#define	BYTECODE_PATH_PROPERTY_ID			"bytecode_path"

///////////////////////////////////////////////////////////////////////

namespace conf {
	_PROJECT_MANAGER_API Property* GenerateLibraryProperty (void) {
		ComponentEntry& compEntry = ComponentRegistry::Instance().GetComponentEntry("DeltaVM");
		const Property* vmProps = compEntry.GetProperty("vm");
		const Property* libdefs = safe_prop_cast<const AggregateProperty>(vmProps)->GetProperty("libdefs");

		MultiChoiceProperty::ChoiceMap choices;
		BOOST_FOREACH(const AggregateProperty* p, safe_prop_cast<const AggregateListProperty>(libdefs)->GetPropertyList())
			choices[get_prop_value<StringProperty>(p->GetProperty("name"))] = false;
		MultiChoiceProperty* p = new MultiChoiceProperty(_("Libraries"), choices, _("External libraries used"), _("Compilation"));
		p->SetAllowExtraSelections(true);
		return p;
	}

	///////////////////////////////////////////////////////////////////////

	_PROJECT_MANAGER_API EnumStringProperty* GenerateScriptDeploymentProperty (void) {
		StringVec options;
		options.push_back(_T(DELTA_SCRIPT_DEPLOYMENT_APPLICATION));
		options.push_back(_T(DELTA_SCRIPT_DEPLOYMENT_LIBRARY));
		return new EnumStringProperty(
						_("Deployment"), 
						options, 
						DELTA_SCRIPT_DEPLOYMENT_DEFAULT_LIBRARY, 
						_("Delta script deployment type"), 
						_("General")
					);
	}

	///////////////////////////////////////////////////////////////////////

	_PROJECT_MANAGER_API void AddProjectBuildProperties (ide::Component* comp) {

		comp->AddInstanceProperty("libs", GenerateLibraryProperty());

		comp->AddInstanceProperty("output_path", new DirectoryProperty(_("Output Path"), _T("./"),
			_("Target directory for the output files"), _("Compilation"))
		);

		comp->AddInstanceProperty("debug", new BoolProperty(_("Debug"), true,
			_("produce debug symbols"), _("Compilation"))
		);

		comp->AddInstanceProperty("icode", new BoolProperty(_("Icode"), false,
			_("produce intermediate code quads file"), _("Compilation"))
		);

		comp->AddInstanceProperty("text", new BoolProperty(_("Text"), false,
			_("produce target code text file"), _("Compilation"))
		);
	}

	_PROJECT_MANAGER_API void AddScriptBuildProperties (ide::Component* comp) {
		AddProjectBuildProperties(comp);
		comp->AddInstanceProperty(conf::GetDeploymentPropertyId(), conf::GenerateScriptDeploymentProperty());
	}

	///////////////////////////////////////////////////////////////////////

	_PROJECT_MANAGER_API void AddScriptLastBuildProperties (PropertyTable* props) {

		props->AddProperty("libs", GenerateLibraryProperty());
		props->AddProperty("output_path", new DirectoryProperty(_("Output Path"), _T("./"),
			_("Target directory for the output files"), _("Compilation"))
		);
		props->AddProperty("debug", new BoolProperty(_("Debug"), true,
			_("produce debug symbols"), _("Compilation"))
		);
		props->AddProperty("icode", new BoolProperty(_("Icode"), false,
			_("produce intermediate code quads file"), _("Compilation"))
		);
		props->AddProperty("text", new BoolProperty(_("Text"), false,
			_("produce target code text file"), _("Compilation"))
		);
		props->AddProperty(conf::GetDeploymentPropertyId(), conf::GenerateScriptDeploymentProperty());
		props->AddProperty(
			conf::GetByteCodePathPropertyId(), 
			new conf::StringProperty(
				util::std2str(conf::GetByteCodePathPropertyId()), 
				_(""),
				_("produce debug symbols"), 
				_("Compilation")
			)
		);
	}

	_PROJECT_MANAGER_API void AddScriptExecutionProperties (ide::Component* comp) {
		comp->AddInstanceProperty("working_directory", new DirectoryProperty(_("Working Directory"), _T("./"),
			_("The application's working directory"), _("Execution"))
		);

		comp->AddInstanceProperty(BYTECODE_PATH_PROPERTY_ID, new DirectoryListProperty(_("Additional Bytecode Paths"),
			_("Additional bytecode loading paths"), _("Execution"))
		);
	}

	_PROJECT_MANAGER_API const char ** GetProjectBuildPropertyIdsForScripts (void) {
		static const char *buildPropertyIds[] = { "output_path", "libs", "debug", "icode", "text", (char*) 0 };
		return buildPropertyIds;
	}
	_PROJECT_MANAGER_API const char ** GetScriptBuildPropertyIds (void) {
		static const char *buildPropertyIds[] = { "output_path", "libs", "debug", "icode", "text", "deployment", (char*) 0 };
		return buildPropertyIds;
	}

	_PROJECT_MANAGER_API const char ** GetScriptExecutionPropertyIds (void) {
		static const char *buildPropertyIds[] = { "working_directory", BYTECODE_PATH_PROPERTY_ID, (char*) 0 };
		return buildPropertyIds;
	}

	_PROJECT_MANAGER_API const char* GetByteCodePathPropertyId (void)
		{ return BYTECODE_PATH_PROPERTY_ID; }

	_PROJECT_MANAGER_API const char* GetDeploymentPropertyId (void)
		{ return "deployment"; }
}

///////////////////////////////////////////////////////////////////////
