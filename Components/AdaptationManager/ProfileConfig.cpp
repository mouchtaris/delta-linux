/**
 *	ProfileConfig.cpp
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	Profile config component is a configuration resource
 *	for the IDE's Adaptation Manager.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#include "ProfileConfig.h"

#include "Properties.h"
#include "ComponentRegistry.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"

#include "Icons/profile_conf.xpm"

namespace ide
{
	//-------------------------------------------------------//
	//---- class ProfileConfig ------------------------------//

	COMPONENT_METADATA(
		ProfileConfig,
		_("Profile Config"),
		_("Configuration resource of the Adaptation Manager"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_COMPONENT_(ProfileConfig, Config);

	COMPONENT_SET_PROPERTIES_FUNCTION(ProfileConfig, table)
	{
		StringVec options;
		options.push_back(_T("profile_conf"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of profile configuration in Adaptation Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	ProfileConfig::ProfileConfig(void)
	{
	}

	//-----------------------------------------------------------------------

	ProfileConfig::~ProfileConfig(void)
	{
		ComponentRegistry::Instance().sigCreatedInstance.disconnect(
			boost::bind(&ProfileConfig::ApplyConfig, this, _1)
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(ProfileConfig, void, Initialize, (void))
	{
		extensions.insert(std::make_pair(s_classId, ".conf"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(ProfileConfig, void, CleanUp, (void))
	{
		extensions.erase(s_classId);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ProfileConfig, bool, Run, (void))
	{
		//-- Apply configuration to all already existed components
		ComponentRegistry::InstanceList instances;
		ComponentRegistry::Instance().GetAllInstances(instances);
		std::for_each(
			instances.begin(),
			instances.end(),
			boost::bind(&ProfileConfig::ApplyConfig, this, _1)
		);

		//-- Apply configuration to every newly created component
		ComponentRegistry::Instance().sigCreatedInstance.connect(
			boost::bind(&ProfileConfig::ApplyConfig, this, _1)
		);

		return true;
	}

	//-----------------------------------------------------------------------

	void ProfileConfig::ApplyConfig(Component* comp)
	{
		Call<void (const String&), SafeCall>(this, comp, "RunConfiguration")(GetURI());
	}

	//-----------------------------------------------------------------------
}
