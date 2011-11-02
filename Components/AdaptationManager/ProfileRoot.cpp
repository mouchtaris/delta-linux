/**
 *	ProfileRoot.cpp
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	ProfileRoot component is the root component of
 *	the Adaptation Manager's tree view.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#include "ProfileRoot.h"
#include "NewProfileDialog.h"

#include "Algorithms.h"
#include "PropertyUtils.h"
#include "BitmapRegistry.h"
#include "ComponentConfigurationDialog.h"

#include "StringUtils.h"
#include "Call.h"
#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "IDEDialogs.h"

#include <wx/filename.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/foreach.hpp>

#include "Icons/profile_root.xpm"
#include "Icons/edit_dmsl.xpm"
#include "Icons/profile_add.xpm"
#include "Icons/refresh.xpm"
#include "Icons/properties.xpm"

#include "xml.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class ProfileRoot --------------------------------//

	COMPONENT_METADATA(
		ProfileRoot,
		_("ProfileRoot"),
		_("Adaptation Manager's tree view root"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_COMPONENT_(ProfileRoot, TreeItemComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(ProfileRoot, table)
	{
		StringVec options;
		options.push_back(_T("profile_root"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of profile root in Adaptation Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	ProfileRoot::ProfileRoot(void)
	{
		const String sparrowDir = util::std2str(IDECore::GetInstallationDir());
		AddInstanceProperty("path", new conf::StringProperty(_("Directory Path"), sparrowDir + _T("profiles"),
			_("Directory path of profiles")));
	}

	//-----------------------------------------------------------------------

	ProfileRoot::~ProfileRoot(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ProfileRoot, void, ScanProfiles, (void))
	{
		///--- Functor ProfileLoader
		struct ProfileLoader :
			std::unary_function<const boost::filesystem::path&, void>
		{
			ProfileLoader(const Handle& treeview, const Handle& parent)
				: treeview(treeview), parent(parent) {}

			result_type operator ()(argument_type path) {
				if (boost::filesystem::is_directory(path))
					///< iterate through the directory
				{
					boost::filesystem::directory_iterator begin(path);
					boost::filesystem::directory_iterator end; ///< default construction yields past-the-end
					std::for_each(begin, end, ProfileLoader(treeview, parent));
				}
				else if (boost::filesystem::extension(path) == ".pfl")
					///< add profile in the profile manager
				{
					//-- parse & add profile
					Component* profile = ComponentFactory::Instance().CreateComponent("Profile");
					if (!profile)
						return;
					profile->SetParent(parent.Resolve());

					if (treeview.Resolve())
						Call<bool (const Handle&, const Handle&)>(parent.GetClassId(), treeview, "AddComponent")(parent, profile);
					Call<void (const String&)>(parent.GetClassId(), profile, "Load")(util::std2str(path.string()));
				}
			}

		private:
			const Handle& treeview;
			const Handle& parent;
		};
		///------------------------------------

		const String sparrowDir = util::std2str(IDECore::GetInstallationDir());
		String path = conf::get_prop_value<conf::StringProperty>(GetProperty("path"), sparrowDir + _T("profiles"));

		boost::filesystem::path dir_path(util::str2std(path));
		if (!boost::filesystem::exists(dir_path))
			return;

		std::for_each(
			boost::filesystem::directory_iterator(dir_path),
			boost::filesystem::directory_iterator(),
			ProfileLoader(treeview, Handle(this))
		);
		Call<void (const Handle&)>(this, treeview, "SortChildren")(this);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(ProfileRoot, void, ClearProfiles, (void))
	{
		util::for_each_safe(
			GetChildren().begin(),
			GetChildren().end(),
			boost::bind(&Component::Destroy, _1)
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(ProfileRoot, "edit_dmsl", edit_dmsl_xpm);
	EXPORTED_CMD_FUNCTION(ProfileRoot, EditDMSL, _("/{-10}Edit DMSL rules"), MT_NMAIN, "edit_dmsl")
	{
		DockableComponent::EnsureVisibility("EditorManager");
		const String sparrowDir = util::std2str(IDECore::GetInstallationDir());
		const String file = sparrowDir + _T("sparrow.dmsl");
		if (const Handle editor = Call<const Handle (const String&)>(this, "EditorManager", "OpenDocument")(file))
			Call<void (const Handle&)>(this, editor, "SetAssociatedComponent")(this);
		else
			gui::displayMessage(0, _("Error"), _("Unable to open DMSL rule file '") + file + _("'\n"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(ProfileRoot, "profile_add", profile_add_xpm);
	EXPORTED_CMD_FUNCTION(ProfileRoot, AddProfile, _("/{0}--Add Profile"), MT_NMAIN, "profile_add")
	{
		NewProfileDialog dlg;
		if(dlg.ShowModal() != wxID_OK)
			return;
		const String& profileName = dlg.GetName();

		BOOST_FOREACH(Component* child, GetChildren())
		{
			if (wxFileName(Call<const String& (void)>(this, child, "GetName")()).SameAs(profileName))
			{
				gui::displayMessage(0, _("Illegal Operation"),
					_("There is already an item with the same Name in the profile root!"));
				return;
			}
		}

		//-- ensure an seperator is appended at the end of the profile path
		const String sparrowDir = util::std2str(IDECore::GetInstallationDir());
		String profiles_dir =
			conf::get_prop_value<conf::StringProperty>(GetProperty("path"), sparrowDir + _T("profiles"));
		if (!boost::algorithm::ends_with(profiles_dir, _T("/"))	&&
			!boost::algorithm::ends_with(profiles_dir, _T("\\")))
			profiles_dir += _T("/");

		Component* profile = ComponentFactory::Instance().CreateComponent("Profile");
		if (!profile)
			return;
		profile->SetParent(this);

		//-- create profile directory
		if (!boost::filesystem::exists(util::str2std(profiles_dir + profileName)))
			boost::filesystem::create_directory(util::str2std(profiles_dir + profileName));

		assert(treeview);
		Call<bool (const Handle&, const Handle&)>(this, treeview, "AddComponent")(this, profile);
		Call<void (const Handle&)>(this, profile, "SetTreeView")(treeview);
		Call<void (const String&)>(this, profile, "SetName")(profileName);
		Call<void (const String&)>(this, profile, "SetURI")
			(profiles_dir + profileName + _T("/") + profileName + _T(".pfl"));
		Call<void (void)>(this, profile, "CreateDefaultSymbolicURI")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(ProfileRoot, "refresh", refresh_xpm);
	EXPORTED_CMD_FUNCTION(ProfileRoot, Refresh, _("/{50}--Refresh"), MT_NMAIN, "refresh")
	{
		ClearProfiles();
		ScanProfiles();
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(ProfileRoot, "properties", properties_xpm);
	EXPORTED_CMD_FUNCTION(ProfileRoot, Properties, _("/{5000}--Properties"), MT_NMAIN, "properties")
	{
		ComponentConfigurationDialog().ConfigureInstance(this);
	}

	//-----------------------------------------------------------------------
}
