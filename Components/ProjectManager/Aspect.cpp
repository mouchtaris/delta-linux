/**
 *	Aspect.cpp
 *
 *	-- IDE Aspect Component --
 *
 *	Aspect component is the component
 *  representing a script aspect transformation.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	August 2012
 */
#include "Aspect.h"
#include "ProjectManagerCommon.h"
#include "Call.h"
#include "ComponentFunctionCallerSafe.h"

#include <boost/foreach.hpp>

#include "Icons/move_up.xpm"
#include "Icons/move_down.xpm"

namespace ide
{
	//-------------------------------------------------------//
	//---- class Aspect --------------------------------//

	COMPONENT_METADATA(
		Aspect,
		_("Stage Source"),
		_("A component representing a script aspect transformation"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
		);
	IMPLEMENT_COMPONENT_(Aspect, Script);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(Aspect, table)
	{
		StringVec options;
		options.push_back(_T("Aspect"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of script in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	Aspect::Aspect(void)
	{
		const_cast<conf::Property*>(GetInstanceProperty("deployment"))->SetEnabled(false);

		//Remove some script based commands
		const char* unavailableScriptCommands[] = { "Run", "Debug", "Debug (Console)", "Select" };
		BOOST_FOREACH(const char* command, unavailableScriptCommands)
			RemoveCommand(_T("/") + util::std2str(command));

		conf::MultiChoiceProperty::ChoiceMap choiceMap;
		const char* choices[] = { "Pre", "Interim", "Post" };
		BOOST_FOREACH(const char* choice, choices)
			choiceMap[util::std2str(choice)] = false;
		AddInstanceProperty("type", new conf::MultiChoiceProperty(_("Transformation"), choiceMap, 
			_("Transformation type"), _("General")));
	}

	//-----------------------------------------------------------------------

	Aspect::~Aspect(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Aspect, void, Initialize, (void))
	{
		extensions.insert(std::make_pair(s_classId, ".dsc"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Aspect, void, CleanUp, (void))
	{
		extensions.erase(s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Aspect, "move_up", move_up_xpm);
	EXPORTED_CMD_FUNCTION(Aspect, MoveUp, _("/{1}Move up"), MT_TOOLBAR, "move_up")
	{
		Call<void (const Handle&), SafeCall>(this, GetParent(), "MoveChildUp")(this);
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Aspect, "move_down", move_down_xpm);
	EXPORTED_CMD_FUNCTION(Aspect, MoveDown, _("/{2}Move down"), MT_TOOLBAR, "move_down")
	{
		Call<void (const Handle&), SafeCall>(this, GetParent(), "MoveChildDown")(this);
	}

	//-----------------------------------------------------------------------
}