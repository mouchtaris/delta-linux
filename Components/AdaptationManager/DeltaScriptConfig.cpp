/**
 *	DeltaScriptConfig.cpp
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	DeltaScriptConfig component is a Delta script that manipulates the
 *	IDE for configuration of a profile.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#include "DeltaScriptConfig.h"

#include "Properties.h"
#include "StringUtils.h"
#include "ComponentRegistry.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "DockableComponent.h"
#include "IDEDialogs.h"

#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "Icons/script_conf.xpm"

#define MAX_LIBRARY_FUNC_DESCRIPTION	1024

namespace ide
{
	//-------------------------------------------------------//
	//---- class DeltaScriptConfig --------------------------//

	COMPONENT_METADATA(
		DeltaScriptConfig,
		_("Delta Script Config"),
		_("Delta configuration script of the Adaptation Manager"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_COMPONENT_(DeltaScriptConfig, Config);

	COMPONENT_SET_PROPERTIES_FUNCTION(DeltaScriptConfig, table)
	{
		StringVec options;
		options.push_back(_T("script_conf"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of profile configuration in Adaptation Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	DeltaScriptConfig::DeltaScriptConfig(void)
	{
	}

	//-----------------------------------------------------------------------

	DeltaScriptConfig::~DeltaScriptConfig(void)
	{
		if (!vmId.empty()) {
			Call<bool (const String&)>(this, "DeltaVM", "DeleteVM")(vmId);
			vmId.clear();
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaScriptConfig, void, Initialize, (void))
	{
		extensions.insert(std::make_pair(s_classId, ".dsc"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(DeltaScriptConfig, void, CleanUp, (void))
	{
		extensions.erase(s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaScriptConfig, bool, Run, (void)) {

		const String		output_path = Call<const String (void)>(this, "DeltaVM", "GetConfigScriptDirectory")();
		const std::string	source		= util::str2std(GetURI());
		const std::string	binary		= util::str2std(output_path + GetName().substr(0, GetName().find_last_of(_T("."))) + _T(".dbc"));

		if (!boost::filesystem::exists(binary)) {
			IDE_ERROR_DELTA_BYTE_CODE_FILE_NOT_FOUND("profile", binary, util::str2std(GetName()));
			//return false;
		}
		else if (boost::filesystem::last_write_time(binary) < boost::filesystem::last_write_time(source))
			IDE_WARNING_DELTA_BYTE_CODE_FILE_NOT_UP_TO_DATE("profile", binary, util::str2std(GetName()));

		vmId = Call<const String (const String&, const String&, const String&)>(this, "DeltaVM", "LoadAndRun")
			(util::std2str(binary), util::std2str(source), String());
		return !vmId.empty();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaScriptConfig, void, Open, (void))
	{
		const String uri = GetURI();
		const String message = _("Config scripts can only be edited in the Sparrow workspace.\nDo you want to open the Sparrow workspace to edit script '") + uri + _T("'?");
		if (Call<bool (const String&)>(this, "Shell", "LoadSparrowWorkspaceDialog")(message)) {
			DockableComponent::EnsureVisibility("EditorManager");
			if (const Handle script = Call<Handle (const String&)>(this, "ProjectManager", "GetResourceByURI")(uri))
				Call<void (void)>(this, script, "Open")();
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(DeltaScriptConfig, bool, LoadComponent, (const String& classId))
	{
		if (!vmId.empty()) {
			bool retval = Call<bool (const String&, const String&)>(this, "DeltaVM", "CallGlobalFunc")(vmId, classId);
			const StringList errors = Call<const StringList (void)>(this, "DeltaVM", "GetFailedConfigAndExtensionScriptErrors")();
			if (!errors.empty())
				Call<bool (void)>(s_classId, "Shell", "HandleInternalSparrowError")();
			return retval;
		}
		else
			return false;	
	}

	//-----------------------------------------------------------------------
}
