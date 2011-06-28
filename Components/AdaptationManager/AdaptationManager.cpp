/**
 *	AdaptationManager.cpp
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	Adapatation manager component is a visual component
 *	that allows selection / manipulation and organization
 *	of IDE configuration profiles.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 *	August 2007 (rename from ProfileManager --> AdaptationManager)
 */
#include "AdaptationManager.h"

#include "PropertyUtils.h"
#include "BitmapRegistry.h"

#include "Call.h"
#include "ComponentFactory.h"
#include "UndefineAllWin32Shit.h"

#ifndef NO_VLD
#include <vld.h>
#endif

#include <boost/foreach.hpp>

#include "Icons/profile_root.xpm"
#include "Icons/profile.xpm"
#include "Icons/profile_conf.xpm"
#include "Icons/script_conf.xpm"

#include "Icons/profile_add.xpm"
#include "Icons/refresh.xpm"

//-------------------------------------------------------//
//---- free Init/Clean functions  -----------------------//

_ADAPTATION_MANAGER_C_API void Initialize(void)
{
	ide::BitmapRegistry::Instance().Insert(_T("profile_root"), wxImage(profile_root_xpm));
	ide::BitmapRegistry::Instance().Insert(_T("profile"), wxImage(profile_xpm));
	ide::BitmapRegistry::Instance().Insert(_T("profile_conf"), wxImage(profile_conf_xpm));
	ide::BitmapRegistry::Instance().Insert(_T("script_conf"), wxImage(script_conf_xpm));
}

//-----------------------------------------------------------------------

_ADAPTATION_MANAGER_C_API void CleanUp(void)
{
	ide::BitmapRegistry::Instance().Remove(_T("script_conf"));
	ide::BitmapRegistry::Instance().Remove(_T("profile_conf"));
	ide::BitmapRegistry::Instance().Remove(_T("profile"));
	ide::BitmapRegistry::Instance().Remove(_T("profile_root"));
}

//-----------------------------------------------------------------------


namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//

	//BEGIN_EVENT_TABLE(AdaptationManager, TreeCtrlComponent)
	//END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class AdaptationManager -----------------------------//

	COMPONENT_METADATA(
		AdaptationManager,
		_("Adaptations"),
		_("Manager of development profiles and loaded extension scripts"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(AdaptationManager, wxPanel, TreeCtrlComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(AdaptationManager, table)
	{
		conf::EnumStringProperty* docking = const_cast<conf::EnumStringProperty*>(
			conf::safe_prop_cast<const conf::EnumStringProperty>(
				table.GetProperty("docking")
			)
		);
		if (docking)
			docking->SetOption(2);
	}

	//-----------------------------------------------------------------------

	AdaptationManager::AdaptationManager(void)
	{
	}

	//-----------------------------------------------------------------------

	AdaptationManager::~AdaptationManager(void)
	{
	}

	//-----------------------------------------------------------------------

	wxWindow* AdaptationManager::GenerateWindow(wxWindow* parent)
	{
		TreeCtrlComponent::GenerateWindow(parent);

		//-- Create root of Tree Control
		Component* root = ComponentFactory::Instance().CreateComponent("ProfileRoot");
		if (!root)
			return this;
		root->SetParent(this);

		AddComponent(Handle(), Handle(root));
		Call<void (const String&)>(this, root, "SetName")(_T("Profiles"));
		Call<void (void)>(this, root, "ScanProfiles")();

		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(AdaptationManager, const StringList, GetProfiles, (void))
	{
		const HandleList& profiles = GetResources(_T("Profile"));

		StringList names;
		BOOST_FOREACH(const Handle& handle, profiles)
			names.push_back(Call<const String& (void)>(this, handle, "GetName")());

		return names;
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(AdaptationManager, const Handle, GetProfile, (const String& name))
	{
		return GetResource(name);
	}

	//-----------------------------------------------------------------------

	EXPORTED_MEMBER(AdaptationManager, bool, ApplyProfile, (const String& name))
	{
		const Handle& profile = GetResource(name);
		if (!profile)
			return false;

		Call<void (const String&, const String&)>(this, "Shell", "Construct")(
			Call<const String& (void)>(this, profile, "GetURI")(), String()
		);

		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_STATIC(AdaptationManager, View, _("/{10}View/{100}Adaptation Manager\tCtrl+Shift+P"), MT_MAIN, "")
	{
		EnsureVisibility("AdaptationManager");
	}

	//-----------------------------------------------------------------------
}
