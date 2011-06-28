/**
 *	Filter.cpp
 *
 *	-- IDE Profile Manager Component --
 *
 *	Filter component is the component representing a virtual
 *	folder able to contain projects, scripts and files.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2009
 */
#include "Filter.h"
#include "ComponentFunctionCallerSafe.h"
#include "Call.h"
#include "GenericDialogs.h"

#include <boost/foreach.hpp>

#include "Icons/delete.xpm"
#include "Icons/properties.xpm"

namespace ide
{
	//-------------------------------------------------------//
	//---- class Filter ------------------------------------//

	COMPONENT_METADATA(
		Filter,
		_("Filter"),
		_("Filter component is the component representing a virtual\
		  folder able to contain projects, scripts and files"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("0.1")
	);
	IMPLEMENT_COMPONENT_(Filter, VirtualContainer);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(Filter, table)
	{
		StringVec options;
		options.push_back(String(_T("Filter")));

		table.AddProperty("treeCtrl_icon", new conf::EnumStringProperty(_("Icon"), options, 0,
			_("Icon representation of a filter in Project Manager's tree-view"))
		);
	}

	//-----------------------------------------------------------------------

	Filter::Filter(void)
	{
	}

	//-----------------------------------------------------------------------

	Filter::~Filter(void)
	{
	}

	//-----------------------------------------------------------------------

	EXPORTED_IMAGE(Filter, "delete", delete_xpm);
	EXPORTED_CMD_FUNCTION(Filter, RemoveCtx, _("/{50}--Remove"), MT_CTX, "delete")
	{
		assert(treeview);
		const Handle& parent = Call<const Handle& (const Handle&)>(this, treeview, "GetParent")(this);
		Call<bool (const Handle&)>(this, treeview, "RemoveComponent")(this);
		Destroy();

		if (parent)
			Call<void (void)>(s_classId, parent, "Save")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_CMD_FUNCTION(Filter, RenameDialog, _("/{60}Rename"), MT_CTX, "")
	{
		StringBoolPair result = 
			Call<StringBoolPair (const String&, const String&, const String&)>(this, "Shell", "PromptInput")
				(_("Rename"), _("New name:"), GetName());
		if (result.second)
			Rename(result.first);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Filter, bool, Rename, (const String& newName))
	{
		assert(treeview);
		const String name = GetName();
		if (newName.empty() || newName == name)
			return false;

		BOOST_FOREACH(Component* sibling, GetParent()->GetChildren())
			if (sibling->GetClassId() == "Filter" && Call<const String& (void)>(this, sibling, "GetName")() == newName)
			{
				gui::displayMessage(0, _("Rename"),
					_("There is already a filter with the same name in the project!"));
				return false;
			}
		SetName(newName);
		Call<void (const Handle&)>(this, treeview, "SortChildren")(GetParent());
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Filter, const StdStringList, GetChildrenTypes, (void))
	{
		//-- Propage call to parent
		return Call<const StdStringList (void)>(this, GetParent(), "GetChildrenTypes")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Filter, Handle, GetChild, (const String& name))
	{
		//Filters cannot be obtained by name so refine this method skipping the self name check
		Handle handle;
		BOOST_FOREACH(Component* child, GetChildren()) {
			handle = Call<Handle (const String&)>(this, child, "GetChild")(name);
			if (handle)
				break;
		}
		return handle;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Filter, bool, HasNamingConflict, (const String& name, const String& uri))
	{
		BOOST_FOREACH(Component* child, GetChildren())
			if (Call<bool (const String&, const String&)>(this, child, "HasNamingConflict")(String(), uri))	//just check uri conflicts
				return true;
		return TreeItemComponent::HasNamingConflict(name, uri);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Filter, bool, Save, (void))
	{
		//-- Propage call to parent
		return Call<bool (void)>(this, GetParent(), "Save")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Filter, const String, GetPath, (void))
	{
		//-- Propage call to parent
		return Call<const String (void)>(this, GetParent(), "GetPath")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Filter, bool, SetStartupResource, (const String& uri))
	{
		//-- Propage call to parent
		return Call<bool (const String&)>(this, GetParent(), "SetStartupResource")(uri);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Filter, const String, GetStartupResource, (void))
	{
		//-- Propage call to parent
		return Call<const String (void)>(this, GetParent(), "GetStartupResource")();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(Filter, void, SetChildProperties, (const Handle& child))
	{
		//-- Propage call to parent
		Call<void (const Handle&), SafeCall>(this, GetParent(), "SetChildProperties")(child);
	}

	//-----------------------------------------------------------------------

	bool Filter::HasChildrenNamingConflict(const String& name, const String& uri) {
		Component* parent = Component::GetParent();
		while (parent->GetClassId() == s_classId)
			parent = parent->Component::GetParent();
		BOOST_FOREACH(Component* child, parent->GetChildren())
			if (Call<bool (const String&, const String&)>(this, child, "HasNamingConflict")(name, uri))
				return true;
		return VirtualContainer::HasChildrenNamingConflict(name, uri);
	}

	//-----------------------------------------------------------------------

	void Filter::ComponentAddedProperty(const std::string& id, const conf::Property* prop)
	{
		BOOST_FOREACH(Component* child, GetChildren())
			child->AddInstanceProperty(id, prop->Clone());
	}

	//-----------------------------------------------------------------------
}
