/**
 *	AttachedScript.cpp
 *
 *	-- IDE AttachedScript Component --
 *
 *	AttachedScript component represents a script that can
 *  be attached to another script within the Workspace.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2013
 */
#include "AttachedScript.h"
#include "ProjectManagerCommon.h"
#include "Call.h"

#include <boost/foreach.hpp>

namespace ide
{
	//-------------------------------------------------------//
	//---- class AttachedScript -----------------------------//

	COMPONENT_METADATA(
		AttachedScript,
		_("Attached Script"),
		_("A component representing a script that can be attached to another script within the workspace"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
		);
	IMPLEMENT_COMPONENT_(AttachedScript, Script);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(AttachedScript, table)
	{
	}

	//-----------------------------------------------------------------------

	AttachedScript::AttachedScript(void)
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

	AttachedScript::~AttachedScript(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(AttachedScript, void, Initialize, (void))
	{
		extensions.insert(std::make_pair(s_classId, ".dsc"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(AttachedScript, void, CleanUp, (void))
	{
		extensions.erase(s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(AttachedScript, Open, _("/{0}Open"), MT_CTX, "")
	{
		Script::Open();
		if (const Handle& editor = Call<const Handle& (const String&)>(this, "EditorManager", "GetEditor")(GetURI()))
			Call<void (bool)>(this, editor, "SetReadOnly")(true);
	}

	//-----------------------------------------------------------------------

	void AttachedScript::ComponentAddedProperty(const std::string& id, conf::Property* prop)
	{
		prop->SetEnabled(false);
	}

	//-----------------------------------------------------------------------
}