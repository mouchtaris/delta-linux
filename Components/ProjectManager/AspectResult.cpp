/**
 *	AspectResult.cpp
 *
 *	-- IDE AspectResult Component --
 *
 *	AspectResult component represents 
 *  the result of an aspect transformation.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2013
 */
#include "AspectResult.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class AspectResult -------------------------------//

	COMPONENT_METADATA(
		AspectResult,
		_("Aspect Result"),
		_("A component representing the result of an aspect transformation"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
		);
	IMPLEMENT_COMPONENT_(AspectResult, AttachedScript);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(AspectResult, table)
	{
		StringVec options;
		options.push_back(_T("AspectResult"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of script in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	AspectResult::AspectResult(void)
	{
	}

	//-----------------------------------------------------------------------

	AspectResult::~AspectResult(void)
	{
	}

	//-----------------------------------------------------------------------
}