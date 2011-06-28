/**
 *	Profile.cpp
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	Profile component is a collection of configuration
 *	resources for the IDE's Adaptation Manager.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 *		Original implementation
 *	November 2007
 *		Management of profile sources has been separated from the project
 *		manager as it was a source of many nuisances on user interaction with
 *		the environment and unintuitive
 */
#include "Profile.h"

#include "Algorithms.h"
#include "StringUtils.h"
#include "Properties.h"
#include "XMLPropertyVisitor.h"
#include "IDEDialogs.h"
#include "NewItemDialog.h"
#include "xml.h"

#include "BitmapRegistry.h"
#include "ComponentConfigurationDialog.h"

#include "ComponentRegistry.h"
#include "ComponentFactory.h"
#include "Call.h"
#include "ComponentFunctionCallerSafe.h"

#include <wx/filename.h>

#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "Icons/select.xpm"
#include "Icons/save_all.xpm"
#include "Icons/add_new_item.xpm"
#include "Icons/add_existing_item.xpm"
#include "Icons/properties.xpm"

namespace ide
{
	static const char *defaultChildren[] = { "DeltaScriptConfig", "ProfileConfig" };

	//-------------------------------------------------------//
	//---- class Profile ------------------------------------//

	COMPONENT_METADATA(
		Profile,
		_("Profile"),
		_("IDE Configuration Profile"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("0.1")
	);
	IMPLEMENT_COMPONENT_(Profile, TreeItemComponent);

	COMPONENT_SET_PROPERTIES_FUNCTION(Profile, table)
	{
		StringVec options;
		options.push_back(_T("profile"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of profile in Adaptation Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	Profile::Profile(void)
	{
	}

	//-----------------------------------------------------------------------

	Profile::~Profile(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Profile, void, Initialize, (void))
	{
		Call<void (const std::string&, const std::string&)>
			Register(s_classId, "TreeItemComponent", "RegisterChildType");
		BOOST_FOREACH(const char* child, defaultChildren)
			Register(s_classId, child);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(Profile, void, CleanUp, (void))
	{
		Call<void (const std::string&, const std::string&), SafeCall>
			Unregister(s_classId, "TreeItemComponent", "UnregisterChildType");
		BOOST_FOREACH(const char* child, defaultChildren)
			Unregister(s_classId, child);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Profile, bool, Load, (const String& uri))
	{
		wxXmlDocument doc;
		if (!doc.Load(uri))
			return false;

		//-- Start processing the XML file
		if (doc.GetRoot()->GetName() != _T("Profile"))
			return false;

		xml::Node root(doc.GetRoot());
		SetName(root[_T("name")]);
		SetURI(uri);
		CreateDefaultSymbolicURI();

		//-- Load project properties
		conf::XMLValueLoader propertyLoader;
		propertyLoader.SetParent(root);
		const_cast<conf::PropertyTable&>(GetInstancePropertyTable()).Accept("ProfileProperties", &propertyLoader);

		//-- Load Children
		BOOST_FOREACH(const xml::Node& node,
			boost::iterator_range<xml::Node::iterator>(
				root.ChildBegin(_T("Child")), root.ChildEnd(_T("Child"))
			)
		)
		{
			Component* item =
				ComponentFactory::Instance().CreateComponent(util::str2std(node[_T("type")]));
			if (!item)
				return false;
			item->SetParent(this);
			orderedChildren.push_back(item);

			if (treeview.Resolve())
				Call<bool (const Handle&, const Handle&)>(this, treeview, "AddComponent")(this, item);
			Call<void (const String&)>(this, item, "Load")(GetPath() + node.GetContent());
		}
		if (treeview.Resolve())
			Call<void (const Handle&)>(this, treeview, "SortChildren")(this);

		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Profile, bool, Save, (void))
	{
		return SaveAs(GetURI());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Profile, bool, SaveAs, (const String& uri))
	{
		//-- Check path existance
		std::string path = util::str2std(uri.substr(0, uri.find_last_of('/')));
		if (!path.empty() && !boost::filesystem::exists(path))
			boost::filesystem::create_directory(path);

		//-- Create XML document
		xml::Node root;
		root.Create(_T("Profile"));

		root.SetProperty(_T("name"), GetName());
		root.SetProperty(_T("version"), _T("alpha"));

		//-- Save project properties
		conf::XMLValueSaver propertySaver(conf::MAKE_PATHS_RELATIVE, GetPath());
		propertySaver.SetParent(root);
		const_cast<conf::PropertyTable&>(GetInstancePropertyTable()).Accept("ProfileProperties", &propertySaver);

		//-- encode children
		BOOST_FOREACH(Component* child, orderedChildren)
		{
			xml::Node node;
			node.Create(_T("Child"));
			node.SetProperty(_T("type"), util::std2str(child->GetClassId()));
			wxFileName filename(Call<const String& (void)>(this, child, "GetURI")());
			if (filename.IsAbsolute())
				filename.MakeRelativeTo(GetPath());	
			node.SetContent(filename.GetFullPath());
			root.InsertChild(node);
		}

		wxXmlDocument doc;
		doc.SetFileEncoding(_T("utf-8"));
		doc.SetRoot(root.NativeType());
		doc.Save(uri, 2);

		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Profile, void, DefaultAction, (void))
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Profile, bool, LoadComponent, (const String& classId))
	{
		BOOST_FOREACH(Component* child, GetChildren()) {
			try {
				if (Call<bool (const String&)>(this, child, "LoadComponent")(classId))
					return true;
			}
			catch(...) {}
		}
		return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Profile, void, MoveChildUp, (const Handle& child))
	{
		List::iterator i = std::find(orderedChildren.begin(), orderedChildren.end(), child.Resolve());
		assert(i != orderedChildren.end());
		if (i != orderedChildren.begin())
		{
			List::iterator pos = i;
			orderedChildren.splice(--pos, orderedChildren, i);
			Call<void (const Handle&)>(this, treeview, "SortChildren")(this);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Profile, void, MoveChildDown, (const Handle& child))
	{
		List::iterator i = std::find(orderedChildren.begin(), orderedChildren.end(), child.Resolve());
		assert(i != orderedChildren.end());
		if (i != --orderedChildren.end())
		{
			List::iterator pos = ++i;
			orderedChildren.splice(--pos, orderedChildren, i);
			Call<void (const Handle&)>(this, treeview, "SortChildren")(this);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Profile, "select", select_xpm);
	EXPORTED_CMD_FUNCTION(Profile, Select, _("/{0}Select"), MT_NMAIN, "select")
	{
		const String message = _("This action will reconstruct all sparrow components.\nAre you sure you want to proceed?");
		if (gui::getConfirmation(0, _("Profile"), message))
			Call<void (const String&, const String&)>(this, "Shell", "Construct")(GetURI(), String());
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(Profile, SetStartupProfile, _("/{2}--Set Startup Profile"), MT_CTX, "")
	{
		Call<void (const String&)>(this, "Shell", "SetStartupProfile")(GetURI());
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(Profile, Edit, _("/{5}Edit"), MT_CTX, "")
	{
		Call<void (const std::string&)>(this, "DockableComponent", "EnsureVisibility")("EditorManager");
		const Handle editor = Call<const Handle (const String&)>(this, "EditorManager", "OpenDocument")(GetURI());
		Call<void (const Handle&)>(this, editor, "SetAssociatedComponent")(this);
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Profile, "save_all", save_all_xpm);
	EXPORTED_CMD_FUNCTION(Profile, SaveAll, _("/{9}--Save All"), MT_NMAIN, "save_all")
	{
		///--- Functor DescendingSaver
		struct DescendingSaver :
			std::unary_function<Component*, bool>
		{
			DescendingSaver(Component* caller)
				: caller(caller) {}

			result_type operator ()(argument_type comp) {
				const ComponentEntry& entry =
					ComponentRegistry::Instance().GetComponentEntry(comp->GetClassId());
				assert(entry);

				if (entry.HasFunction("SaveAll"))
					return Call<bool (void)>(caller, comp, "SaveAll")();
				else if (entry.HasFunction("Save"))
					return Call<bool (void)>(caller, comp, "Save")();
				return false;
			}

		private:
			Component* caller;
		};
		///------------------------------------

		//-- Save Profile
		if (!Save())
			return;

		//-- Save children recursively
		util::reduce(
			GetChildren().begin(),
			GetChildren().end(),
			std::logical_and<bool>(),
			DescendingSaver(this)
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Project, "add_new_item", add_new_item_xpm);
	EXPORTED_CMD_FUNCTION(Profile, NewItem, _("/{20}--Add/{0}New Profile Item..."), MT_NMAIN, "add_new_item")
	{
		NewItemDialog dlg(0, GetChildrenTypes(), GetPath());
		dlg.ShowModal();

		if (!dlg.IsOk())
			return;

		String itemName = dlg.GetItemName();
		String itemURI  = dlg.GetItemURI();

		BOOST_FOREACH(Component* child, GetChildren())
		{
			if (Call<const String& (void)>(this, child, "GetName")() == itemName	||
				wxFileName(Call<const String& (void)>(this, child, "GetURI")()).SameAs(itemURI))
			{
				gui::displayMessage(0, _("Illegal Operation"),
					_("There is already an item with the same Name/URI in the project!"));
				return;
			}
		}

		Component* item = ComponentFactory::Instance().CreateComponent(util::str2std(dlg.GetItemType()));
		if (!item)
			return;
		item->SetParent(this);
		orderedChildren.push_back(item);

		assert(treeview);
		Call<bool (const Handle&, const Handle&)>(this, treeview, "AddComponent")(this, item);
		Call<void (const Handle&)>(this, item, "SetTreeView")(treeview);
		Call<void (const String&)>(this, item, "SetName")(itemName);
		Call<void (const String&)>(this, item, "SetURI")(itemURI);
		Call<void (void)>(this, item, "CreateDefaultSymbolicURI")();

		//-- Save new changes
		Save();
		SaveComponent(item);
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Project, "add_existing_item", add_existing_item_xpm);
	EXPORTED_CMD_FUNCTION(Profile, AddExistingItem, _("/{20}--Add/{10}Existing Profile Item..."), MT_NMAIN, "add_existing_item")
	{
		String wildcard;
		std::vector<std::string> types;

		const StdStringList& items = GetChildrenTypes();
		BOOST_FOREACH(const std::string& classId, items)
		{
			StringList extensions =
				Call<StringList (const std::string&)>(this, classId, "GetSupportedExtensions")(classId);
			BOOST_FOREACH(const String& ext, extensions)
			{
				wildcard += util::std2str(classId) + _T(" ") + _T("(*") + ext + _T(")")
					+ _T("|*") + ext + _T("|");
				types.push_back(classId);
			}
		}
		wildcard = wildcard.erase(wildcard.length() -1);

		FileDialog fileDlg(0, _("Add Existing Item"), String(), String(), wildcard,
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);
		if (fileDlg.ShowModal() != wxID_OK)
			return;

		//-- Create & Load existing item
		Component* item =
			ComponentFactory::Instance().CreateComponent(types[fileDlg.GetFilterIndex()]);
		if (!item)
			return;
		item->SetParent(this);
		orderedChildren.push_back(item);
		Call<void (const String&)>(this, item, "Load")(fileDlg.GetPath());

		//-- The profile is considered a domain specific project, as such it should have a local copy
		//	 of the added configuration script, not relying on the remote instance of it, in order for
		//	 a profile to be considered as a whole package without external references
		const String& profile_path = GetPath();
		const String& filename = Call<const String& (void)>(this, item, "GetLeafURI")();
		const String& original_uri = Call<const String& (void)>(this, item, "GetURI")();
		const String& local_uri = profile_path + filename;

		if (!boost::filesystem::equivalent(util::str2std(original_uri), util::str2std(local_uri)))
		{	///< in this case we need a local copy
			boost::filesystem::copy_file(util::str2std(original_uri), util::str2std(local_uri));
			Call<void (const String&)>(this, item, "SetURI")(local_uri);
			Call<void (void)>(this, item, "CreateDefaultSymbolicURI")();
		}

		//-- Add the new component in the tree view
		if (treeview.Resolve())
			Call<bool (const Handle&, const Handle&)>(this, treeview, "AddComponent")(this, item);

		//-- Save new changes
		Save();
		SaveComponent(item);
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(Profile, Remove, _("/{50}--Remove"), MT_CTX, "")
	{
		StdStringList uris;
		BOOST_FOREACH(Component* child, GetChildren())
			uris.push_back(util::str2std(Call<const String& (void)>(this, child, "GetURI")()));
		uris.push_back(util::str2std(GetURI()));

		BOOST_FOREACH(const std::string& uri, uris)
			if (boost::filesystem::exists(uri))
				try { boost::filesystem::remove(uri); }
				catch(...) {}

		try { boost::filesystem::remove(util::str2std(GetPath())); }
		catch(...) {}
		Call<bool (const Handle&)>(this, treeview, "RemoveComponent")(this);
		Destroy();
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Profile, "properties", properties_xpm);
	EXPORTED_CMD_FUNCTION(Profile, Properties, _("/{5000}--Properties"), MT_NMAIN, "properties")
	{
		ComponentConfigurationDialog().ConfigureInstance(this);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Profile, int, OnCompareItems, (const Handle& handle1, const Handle& handle2))
	{
		assert(handle1.Resolve() && handle2.Resolve());
		List::const_iterator begin = orderedChildren.begin(), end = orderedChildren.end();
		List::const_iterator i1 = std::find(begin, end, handle1.Resolve());
		List::const_iterator i2 = std::find(begin, end, handle2.Resolve());
		assert(i1 != end && i2 != end);
		return std::distance(begin, i1) - std::distance(begin, i2);
	}

	//-----------------------------------------------------------------------

	void Profile::ChildRemoved (Component* component)
	{
		orderedChildren.remove(component);
		Save();
		TreeItemComponent::ChildRemoved(component);
	}

	//-----------------------------------------------------------------------

	void Profile::ChildDestroyed (Component* component)
	{
		orderedChildren.remove(component);
		TreeItemComponent::ChildDestroyed(component);
	}

	//-----------------------------------------------------------------------

	bool Profile::SaveComponent(Component* comp)
	{
		const ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(comp->GetClassId());
		assert(entry);
		if (entry.HasFunction("Save"))
			return Call<bool (void)>(this, comp, "Save")();
		else
			return false;
	}

	//-----------------------------------------------------------------------
}
