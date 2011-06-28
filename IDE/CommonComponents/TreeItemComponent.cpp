/**
 *	TreeItemComponent.cpp
 *
 *	-- IDE Common Components --
 *
 *	Common component base class of components that can
 *	be inserted to a tree control common component.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "TreeItemComponent.h"

#include "BitmapRegistry.h"

#include "ComponentEntry.h"
#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "GenericDialogs.h"
#include "Call.h"
#include "Algorithms.h"
#include "Adaptors.h"
#include "StringUtils.h"
#include "PropertyUtils.h"

#include <wx/filename.h>
#include <boost/foreach.hpp>

namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//

	TreeItemComponent::StrStrMMap TreeItemComponent::childrenTypes;
	TreeItemComponent::StrStrMMap TreeItemComponent::extensions;


	//-------------------------------------------------------//
	//---- class TreeItemComponent --------------------------//

	COMPONENT_METADATA(
		TreeItemComponent,
		_("Tree Item Component"),
		_("Common component base class of components that can be inserted to a tree control common component"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_COMPONENT_(TreeItemComponent, IDEComponent);

	COMPONENT_SET_PROPERTIES_FUNCTION(TreeItemComponent, table)
	{
		StringVec options;
		options.push_back(_T("default"));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of component in the tree view"))
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_SIGNAL(TreeItemComponent, TreeItemURIChanged, (const String& oldUri, const String& newUri));
	EXPORTED_SIGNAL(TreeItemComponent, TreeItemSymbolicURIChanged, (const String& oldUri, const String& newUri));

	//-----------------------------------------------------------------------

	TreeItemComponent::TreeItemComponent(void) : hasDefaultSymbolic(true)
	{
		conf::BoolProperty *collapsed = new conf::BoolProperty(_("Collapsed"), false);
		collapsed->SetVisible(false);
		AddInstanceProperty("collapsed", collapsed);
		AddInstanceProperty(
			"symbolic",
			new conf::StringProperty(_T("Symbolic Uri"), String(), _("The tree item's symbolic uri"))
		);
	}

	//-----------------------------------------------------------------------

	TreeItemComponent::~TreeItemComponent(void)
	{
	}

	//-----------------------------------------------------------------------

	wxWindow* TreeItemComponent::GenerateWindow(wxWindow* parent)
	{
		return 0;
	}

	//-----------------------------------------------------------------------

	wxWindow* TreeItemComponent::GetWindow(void)
	{
		return 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(TreeItemComponent, void, RegisterChildType,
		(const std::string& parentId, const std::string& classId))
	{
		if (std::find_if(
				childrenTypes.lower_bound(parentId),
				childrenTypes.upper_bound(parentId),
				util::project2nd<StrStrMMap::value_type>(
					boost::bind(std::equal_to<std::string>(), classId, _1)
				)
			) == childrenTypes.upper_bound(parentId)
		)
			childrenTypes.insert(std::make_pair(parentId, classId));
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(TreeItemComponent, void, UnregisterChildType,
		(const std::string& parentId, const std::string& classId))
	{
		childrenTypes.erase(
			std::find_if(
				childrenTypes.lower_bound(parentId),
				childrenTypes.upper_bound(parentId),
				util::project2nd<StrStrMMap::value_type>(
					boost::bind(std::equal_to<std::string>(), classId, _1)
				)
			)
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(TreeItemComponent, const StdStringList, GetChildrenTypesForClass, (const std::string& parentId))
	{
		StdStringList list;
		std::transform(
			childrenTypes.lower_bound(parentId),
			childrenTypes.upper_bound(parentId),
			std::back_inserter(list),
			util::project2nd<StrStrMMap::value_type>(
				util::identity<std::string>()
			)
		);

		return list;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(TreeItemComponent, const StringList, GetSupportedExtensions, (const std::string& classId))
	{
		StringList list;
		std::transform(
			extensions.lower_bound(classId),
			extensions.upper_bound(classId),
			std::back_inserter(list),
			util::project2nd<StrStrMMap::value_type>(boost::bind(util::std2str, _1))
				///< boost::bind only used to transform util::std2str to a functor
		);

		return list;
	}

	//-----------------------------------------------------------------------

	EXPORTED_STATIC(TreeItemComponent, const StdStringList, GetExtensionTypes, (const std::string& extension))
	{
		StdStringList list;
		BOOST_FOREACH(StrStrMMap::value_type& entry, extensions)
			if (entry.second == extension)
				list.push_back(entry.first);

		return list;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, const StdStringList, GetChildrenTypes, (void))
	{
		return GetChildrenTypesForClass(GetClassId());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, const String&, GetName, (void))
	{
		return name;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, const String&, GetURI, (void))
	{
		return uri;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, const String&, GetSymbolicURI, (void))
	{
		return conf::get_prop_value<conf::StringProperty>(GetInstanceProperty("symbolic"));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, bool, HasDefaultSymbolicURI, (void))
	{
		return hasDefaultSymbolic;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, const String, GetPath, (void))
	{
		size_t end = uri.find_last_of(_T("/"));
		return (end != String::npos) ? uri.substr(0, end +1) : String();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, const String, GetLeafURI, (void))
	{
		size_t end = uri.find_last_of(_T("/"));
		return (end != String::npos) ? uri.substr(end +1) : uri;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, void, SetName, (const String& name))
	{
		this->name = name;

		if (treeview.Resolve())
			Call<bool (const Handle&, const String&)>(this, treeview, "RenameComponent")(this, name);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, void, SetURI, (const String& uri))
	{
		const String oldUri = this->uri;
		this->uri = util::normalizepath(uri);
		if (!oldUri.empty())	//oldUri is empty only upon construction/initialization
			sigTreeItemURIChanged(this, oldUri, this->uri);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, void, SetSymbolicURI, (const String& uri))
	{
		const String oldUri = GetSymbolicURI();
		conf::set_prop_value<conf::StringProperty>(
			const_cast<conf::Property*>(GetInstanceProperty("symbolic")), uri
		);
		hasDefaultSymbolic = false;
		if (!oldUri.empty())	//oldUri is empty only upon construction/initialization
			sigTreeItemSymbolicURIChanged(this, oldUri, uri);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, void, SetTreeView, (const Handle& handle))
	{
		treeview = handle;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, void, SetExpanded, (bool value))
	{
		conf::set_prop_value<conf::BoolProperty>(
			const_cast<conf::Property*>(GetProperty("collapsed")), 
			!value
		);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, const String, GetDefaultSymbolicURIFromURI, (const String& uri))
	{
		String symbolicUri;
		if (!(symbolicUri = uri).empty())
		{
			if (treeview.Resolve())
			{
				wxFileName filename(symbolicUri);
				if (filename.IsAbsolute())
				{
					const Handle& root = Call<const Handle& (void)>(this, treeview, "GetTreeRoot")();
					filename.MakeRelativeTo(Call<const String (void)>(this, root, "GetPath")());
				}
				symbolicUri = util::normalizeslashes(filename.GetFullPath());
			}
		}
		return symbolicUri;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, void, CreateDefaultSymbolicURI, (void))
	{
		SetSymbolicURI(GetDefaultSymbolicURIFromURI(uri));
		hasDefaultSymbolic = true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, Handle, GetParent, (void))
	{
		Component *parent = Component::GetParent();
		return parent ? Handle(parent) : Handle();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, HandleList, CollectChildren, (const String& type))
	{
		HandleList handles;
		BOOST_FOREACH(Component* child, GetChildren()) {
			const HandleList& children = Call<HandleList (const String&)>
				(this, child, "CollectChildren")(type);

			if (type.empty() || child->GetClassId() == util::str2std(type))
				handles.push_back(child);
			handles.insert(handles.end(), children.begin(), children.end());
		}

		return handles;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, Handle, GetChild, (const String& name))
	{
		if (name == GetName())
			return this;

		Handle handle;
		BOOST_FOREACH(Component* child, GetChildren()) {
			handle = Call<Handle (const String&)>(this, child, "GetChild")(name);
			if (handle)
				break;
		}

		return handle;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, Handle, GetChildByURI, (const String& uri))
	{
		if (wxFileName(uri).SameAs(GetURI()))
			return this;

		Handle handle;
		BOOST_FOREACH(Component* child, GetChildren()) {
			handle = Call<Handle (const String&)>(this, child, "GetChildByURI")(uri);
			if (handle)
				break;
		}

		return handle;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, Handle, GetChildBySymbolicURI, (const String& uri))
	{
		if (util::filepathsequal(uri, GetSymbolicURI()))
			return this;

		Handle handle;
		BOOST_FOREACH(Component* child, GetChildren()) {
			handle = Call<Handle (const String&)>(this, child, "GetChildBySymbolicURI")(uri);
			if (handle)
				break;
		}

		return handle;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, Handle, GetDirectChild, (const String& name))
	{
		BOOST_FOREACH(Component* child, GetChildren())
			if (Call<const String& (void)>(this, child, "GetName")() == name)
				return child;
		return Handle();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, Handle, GetDirectChildByURI, (const String& uri))
	{
		wxFileName filename(uri);
		if (!filename.IsAbsolute())
			filename = wxFileName(GetPath() + uri);
		BOOST_FOREACH(Component* child, GetChildren())
			if (filename.SameAs(Call<const String& (void)>(this, child, "GetURI")()))
				return child;
		return Handle();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, Handle, GetDirectChildBySymbolicURI, (const String& uri))
	{
		BOOST_FOREACH(Component* child, GetChildren())
			if (util::filepathsequal(Call<const String& (void)>(this, child, "GetSymbolicURI")(), uri))
				return child;
		return Handle();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeItemComponent, bool, HasNamingConflict, (const String& name, const String& uri))
	{
		return !name.empty() && GetName() == name || !uri.empty() && wxFileName(uri).SameAs(GetURI());
	}

	//-----------------------------------------------------------------------

	void TreeItemComponent::ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
	{
		BOOST_FOREACH(const std::string& property, changed)
			if (property == "symbolic") {
				const String& uri = GetSymbolicURI();
				const String& oldUri = conf::get_prop_value<conf::StringProperty>(old.GetProperty("symbolic"));
				if (uri.empty()) {
					gui::displayMessage(GetWindow(), _("Error"), _("Symbolic uri cannot be empty!"));
					conf::set_prop_value<conf::StringProperty>(
						const_cast<conf::Property*>(GetInstanceProperty("symbolic")), oldUri
					);
					break;
				}
				else {
					GetInstancePropertyTable().SetDirty(true);
					hasDefaultSymbolic = false;
					sigTreeItemSymbolicURIChanged(this, oldUri, uri);
				}
			}
		IDEComponent::ComponentAppliedChangedProperties(old, changed);
	}

	//-----------------------------------------------------------------------

	void TreeItemComponent::ComponentDestroyed(void)
	{
		if (treeview.Resolve())
			Call<bool (const Handle&)>(this, treeview, "RemoveComponent")(this);
		IDEComponent::ComponentDestroyed();
	}

	//-----------------------------------------------------------------------
}
