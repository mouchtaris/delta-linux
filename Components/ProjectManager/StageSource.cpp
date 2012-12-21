/**
 *	StageSource.cpp
 *
 *	-- IDE StageSource Component --
 *
 *	StageSource component is the component
 *  representing a staged compilation output.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2011
 */
#include "StageSource.h"
#include "ProjectManagerCommon.h"
#include "Call.h"

#include <boost/foreach.hpp>

namespace ide
{
	//-------------------------------------------------------//
	//---- class StageSource --------------------------------//

	COMPONENT_METADATA(
		StageSource,
		_("Stage Source"),
		_("A component representing a staged compilation output in the workspace"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
		);
	IMPLEMENT_COMPONENT_(StageSource, Script);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(StageSource, table)
	{
		StringVec options;
		options.push_back(_T("stage"));
		options.push_back(_T("result"));
		options.push_back(_T("aspect"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of script in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	StageSource::StageSource(void)
	{
		//Make properties non-editable
		conf::PropertyTable& table = const_cast<conf::PropertyTable&>(GetInstancePropertyTable());
		conf::PropertyMap& props = table.GetPropertyMap();
		for(conf::PropertyMap::iterator i = props.begin(); i != props.end(); ++i)
			i->second->SetEnabled(false);

		//Remove script based commands
		const char* unavailableScriptCommands[] = {
			"Run", "Debug", "Debug (Console)", "Build", "Debug Build", "Clean", "Remove", "Rename", "Select"
		};
		BOOST_FOREACH(const char* command, unavailableScriptCommands)
			RemoveCommand(_T("/") + util::std2str(command));
	}

	//-----------------------------------------------------------------------

	StageSource::~StageSource(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(StageSource, void, Initialize, (void))
	{
		extensions.insert(std::make_pair(s_classId, ".dsc"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(StageSource, void, CleanUp, (void))
	{
		extensions.erase(s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(StageSource, Open, _("/{0}Open"), MT_CTX, "")
	{
		Script::Open();
		if (const Handle& editor = Call<const Handle& (const String&)>(this, "EditorManager", "GetEditor")(GetURI()))
			Call<void (bool)>(this, editor, "SetReadOnly")(true);
	}

	//-----------------------------------------------------------------------
	
	EXPORTED_FUNCTION(StageSource, const std::string, GetType, (void))
	{
		return util::str2std(static_cast<const conf::EnumStringProperty*>(GetInstanceProperty("treeCtrl_icon"))->GetValue());
	}

	//-----------------------------------------------------------------------

	void StageSource::ComponentAddedProperty(const std::string& id, conf::Property* prop)
	{
		prop->SetEnabled(false);
	}

	//-----------------------------------------------------------------------
}