/*
 *	ProjectManagerCommon.cpp
 *	Common stuff across project manager files.
 *	Anthony Savidis, November 2010
 *  Functionality moved here from original code from Yannis Lilis.
 */
#include "ProjectManagerCommon.h"
#include "ComponentHandle.h"
#include "ComponentEntry.h"
#include "ComponentRegistry.h"
#include "Call.h"
#include <boost/foreach.hpp>

using namespace ide;

#define	BYTECODE_PATH_PROPERTY_ID			"bytecode_path"
#define	DLLIMPORT_PATH_PROPERTY_ID			"dllimport_path"

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

	_PROJECT_MANAGER_API Property* GenerateLineMappingsProperty (void) {
		AggregateProperty *listType = new AggregateProperty(_("Stage Source Options"), _("Single stage source option"));
		listType->AddProperty("original", new IntProperty(_("Original Line")));
		listType->AddProperty("mapped", new StringListProperty(_("Mapped Lines")));
		return new AggregateListProperty(_T("Line Mappings"), listType);
	}

	///////////////////////////////////////////////////////////////////////

	_PROJECT_MANAGER_API Property* GenerateStageSourcesProperty (void) {
		AggregateProperty *listType = new AggregateProperty(_("Stage Source Options"), _("Single stage source option"));
		listType->AddProperty("stage", new IntProperty(_("Stage")));
		listType->AddProperty(BYTECODE_PATH_PROPERTY_ID, new DirectoryListProperty(_("Additional Bytecode Paths"),
			_("Additional bytecode loading paths"), _("Execution")));
		listType->AddProperty(DLLIMPORT_PATH_PROPERTY_ID, new DirectoryListProperty(_("Additional Dllimport Paths"),
			_("Additional dll loading paths"), _("Execution")));
		listType->AddProperty("libs", GenerateLibraryProperty());
		return new AggregateListProperty(_T("Stage Source Options"), listType, _T("Compilation options for stage metaprograms"));
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

		comp->AddInstanceProperty("stage_output_path", new DirectoryProperty(_("Stage Output Path"), _T("./"),
			_("Target directory for the stage output files"), _("Compilation"))
		);
	}

	_PROJECT_MANAGER_API void AddScriptBuildProperties (ide::Component* comp) {
		AddProjectBuildProperties(comp);
		comp->AddInstanceProperty("output", new StringProperty(_("Target name"), _T(""),
			_("Target name for the output files"), _("Compilation"))
		);
		comp->AddInstanceProperty(GetDeploymentPropertyId(), GenerateScriptDeploymentProperty());
		comp->AddInstanceProperty("stage_sources_options", GenerateStageSourcesProperty());
		comp->AddInstanceProperty("aspects", new FileListProperty(_("Aspect Transformations"),
			_("Aspect transformation script binaries"), _("Compilation"))
		);
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
		props->AddProperty("stage_output_path", new DirectoryProperty(_("Stage Output Path"), _T("./"),
			_("Target directory for the stage output files"), _("Compilation"))
		);
		props->AddProperty(GetDeploymentPropertyId(), GenerateScriptDeploymentProperty());
		props->AddProperty(
			GetByteCodePathPropertyId(), 
			new DirectoryListProperty(
				util::std2str(GetByteCodePathPropertyId()), 
				_("produce debug symbols"), 
				_("Compilation")
			)
		);
		props->AddProperty("aspects", new FileListProperty(_("Aspect Transformations"),
			_("Aspect transformation script binaries"), _("Compilation"))
		);
	}

	_PROJECT_MANAGER_API void AddScriptStageSourceProperties (PropertyTable* props) {
		props->AddProperty("name", new StringProperty(_("name")));
		props->AddProperty("index", new IntProperty(_("index")));
		props->AddProperty("type", new StringProperty(_("type")));
		props->AddProperty("final", new BoolProperty(_("final")));
		props->AddProperty("lineMappings", GenerateLineMappingsProperty());
	}

	_PROJECT_MANAGER_API void AddScriptExecutionProperties (ide::Component* comp) {
		comp->AddInstanceProperty("working_directory", new DirectoryProperty(_("Working Directory"), _T("./"),
			_("The application's working directory"), _("Execution"))
		);

		comp->AddInstanceProperty(BYTECODE_PATH_PROPERTY_ID, new DirectoryListProperty(_("Additional Bytecode Paths"),
			_("Additional bytecode loading paths"), _("Execution"))
		);
		comp->AddInstanceProperty(DLLIMPORT_PATH_PROPERTY_ID, new DirectoryListProperty(_("Additional Dllimport Paths"),
			_("Additional dll loading paths"), _("Execution"))
		);
	}

	_PROJECT_MANAGER_API const char ** GetScriptPropertyIdsForStageSources (void) {
		static const char *buildPropertyIds[] = { BYTECODE_PATH_PROPERTY_ID, DLLIMPORT_PATH_PROPERTY_ID, "libs", (char*) 0 };
		return buildPropertyIds;
	}

	_PROJECT_MANAGER_API const char ** GetProjectBuildPropertyIdsForScripts (void) {
		static const char *buildPropertyIds[] = { "output_path", "libs", "debug", "icode", "text", "stage_output_path", (char*) 0 };
		return buildPropertyIds;
	}
	_PROJECT_MANAGER_API const char ** GetScriptBuildPropertyIds (void) {
		static const char *buildPropertyIds[] = { "output_path", "libs", "debug", "icode", "text", "stage_output_path", "deployment", "aspects", (char*) 0 };
		return buildPropertyIds;
	}

	_PROJECT_MANAGER_API const char ** GetScriptLastBuildPropertyIds (void) {
		static const char *lastBuildPropertyIds[] = { "output_path", "libs", "debug", "icode", "text", "stage_output_path", "deployment", "aspects", BYTECODE_PATH_PROPERTY_ID, (char*) 0 };
		return lastBuildPropertyIds;
	}

	_PROJECT_MANAGER_API const char ** GetScriptExecutionPropertyIds (void) {
		static const char *buildPropertyIds[] = { "working_directory", BYTECODE_PATH_PROPERTY_ID, DLLIMPORT_PATH_PROPERTY_ID, (char*) 0 };
		return buildPropertyIds;
	}

	_PROJECT_MANAGER_API const char* GetByteCodePathPropertyId (void)
		{ return BYTECODE_PATH_PROPERTY_ID; }

	_PROJECT_MANAGER_API const char* GetDeploymentPropertyId (void)
		{ return "deployment"; }
}

///////////////////////////////////////////////////////////////////////
