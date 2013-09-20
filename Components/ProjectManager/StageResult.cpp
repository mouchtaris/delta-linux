/**
 *	StageResult.cpp
 *
 *	-- IDE StageResult Component --
 *
 *	StageResult component represents the
 *  result produced by a compilation stage.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2013
 */
#include "StageResult.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class StageResult -------------------------------//

	COMPONENT_METADATA(
		StageResult,
		_("Aspect Result"),
		_("A component representing the result produced by a compilation stage"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
		);
	IMPLEMENT_COMPONENT_(StageResult, AttachedScript);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(StageResult, table)
	{
		StringVec options;
		options.push_back(_T("StageResult"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of script in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	StageResult::StageResult(void)
	{
	}

	//-----------------------------------------------------------------------

	StageResult::~StageResult(void)
	{
	}

	//-----------------------------------------------------------------------
}
