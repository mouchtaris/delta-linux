/**
 *	StageSource.cpp
 *
 *	-- IDE StageSource Component --
 *
 *	StageSource component represents
 *  the source of a compilation stage.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2011
 */
#include "StageSource.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class StageSource --------------------------------//

	COMPONENT_METADATA(
		StageSource,
		_("Stage Source"),
		_("A component representing the source of a compilation stage"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
		);
	IMPLEMENT_COMPONENT_(StageSource, AttachedScript);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(StageSource, table)
	{
		StringVec options;
		options.push_back(_T("StageSource"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of script in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	StageSource::StageSource(void)
	{
	}

	//-----------------------------------------------------------------------

	StageSource::~StageSource(void)
	{
	}

	//-----------------------------------------------------------------------
}
