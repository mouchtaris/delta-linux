/**
 *	Config.h
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	Config component is a configuration resource
 *	for the IDE's Adaptation Manager that allows
 *  the user to select it's invocation priority.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	January 2009
 */
#include "Config.h"

#include "ComponentFunctionCallerSafe.h"
#include "Call.h"

#include "Icons/move_up.xpm"
#include "Icons/move_down.xpm"

namespace ide
{
	//-------------------------------------------------------//
	//---- class Config ------------------------------//

	COMPONENT_METADATA(
		Config,
		_("Config"),
		_("Prioritized configuration resource of the Adaptation Manager"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_COMPONENT_(Config, TextFile);

	COMPONENT_SET_PROPERTIES_FUNCTION(Config, table)
	{
	}

	//-----------------------------------------------------------------------

	Config::Config(void)
	{
	}

	//-----------------------------------------------------------------------

	Config::~Config(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Config, "move_up", move_up_xpm);
	EXPORTED_CMD_FUNCTION(Config, MoveUp, _("/{1}Move up"), MT_TOOLBAR, "move_up")
	{
		Call<void (const Handle&), SafeCall>(this, GetParent(), "MoveChildUp")(this);
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Config, "move_down", move_down_xpm);
	EXPORTED_CMD_FUNCTION(Config, MoveDown, _("/{2}Move down"), MT_TOOLBAR, "move_down")
	{
		Call<void (const Handle&), SafeCall>(this, GetParent(), "MoveChildDown")(this);
	}

	//-----------------------------------------------------------------------
}
