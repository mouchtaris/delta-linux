/**
 *	TreeItemDropTarget.cpp
 *
 *	-- IDE Drop target implementation for the Project Manager tree --
 *
 *	Support for dropping items in the Project Manager treeview.
 *	Objects accepted correspond to TreeItemDataObjects.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	April 2009
 */
#include "TreeItemDropTarget.h"
#include "Call.h"
#include "ComponentRegistry.h"
#include "ComponentTreeView.h"
#include "DefaultGUIGenerator.h"
#include "IDEDialogs.h"
#include "PropertyUtils.h"

#include "TreeItemDataObject.h"

#include <wx/filename.h>

#include <boost/foreach.hpp>

namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//

	const std::string TreeItemDropTarget::s_classId = "TreeItemDropTarget";

	//-------------------------------------------------------//
	//---- class TreeItemDropTarget -------------------------//
	
	TreeItemDropTarget::TreeItemDropTarget(ComponentTreeView* treeview) :
		wxDropTarget(new TreeItemDataObject), treeview(treeview)
	{
	}

	//-----------------------------------------------------------------------

	TreeItemDropTarget::~TreeItemDropTarget()
	{
	}

	//-----------------------------------------------------------------------

	void TreeItemDropTarget::UnHighlightCurrent(void)
	{
		if (highlighted.IsOk()) {
			treeview->SetItemDropHighlight(highlighted, false);
			highlighted = wxTreeItemId();
		}
	}

	//-----------------------------------------------------------------------

	bool TreeItemDropTarget::AddItemFromURI(const Handle& parent, const StdStringList& parentTypes, const String& uri)
	{
		using namespace conf;
		wxFileName filename(uri);
		const StdStringList extensionTypes = Call<const StdStringList (const std::string&)>
			(s_classId, "TreeItemComponent", "GetExtensionTypes")(util::str2std(_T(".") + filename.GetExt()));
		StdStringList validTypes;
		BOOST_FOREACH(const std::string& extensionType, extensionTypes)
			if (std::find(parentTypes.begin(), parentTypes.end(), extensionType) != parentTypes.end())
				validTypes.push_back(extensionType);

		std::string type;
		String finalUri = uri;
		if (validTypes.size() == 1)
			type = validTypes.front();
		else {
			PropertyTable table;
			table.AddProperty("file", new FileProperty(_("File"), uri, _("Path for imported file")));

			StringVec options;
			BOOST_FOREACH(const std::string& str, validTypes)
				options.push_back(util::std2str(str));
			options.push_back(_T("GenericFile"));
			options.push_back(_T("TextFile"));
			table.AddProperty("type", new EnumStringProperty(_("Type"), options, 0, _("Type for imported file")));

			if (DefaultGUIGenerator().ShowDialogFromProperties(0, table, PropertyIdVec(), _("Import file as..."))) {
				type = util::str2std(safe_prop_cast<EnumStringProperty>(table.GetProperty("type"))->GetValue());
				finalUri = get_path_prop_value<FileProperty>(table.GetProperty("file"));
			}
		}
		return type.empty() ? false:
			Call<bool (const std::string&, const String&)>(s_classId, parent, "AddExistingItem")
				(type, finalUri);
	}

	//-----------------------------------------------------------------------

	wxDragResult TreeItemDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult def)
	{
		return OnDragOver(x, y, def);
	}

	//-----------------------------------------------------------------------

	wxDragResult TreeItemDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
	{
		int flags;
		wxTreeItemId id = treeview->HitTest(wxPoint(x, y) - treeview->GetPosition(), flags);
		if (id.IsOk()) {
			if (id != highlighted) {
				UnHighlightCurrent();
				treeview->SetItemDropHighlight(highlighted = id);
			}
			return def;
		}
		else {
			UnHighlightCurrent();
			return wxDragNone;
		}
	}

	//-----------------------------------------------------------------------

	void TreeItemDropTarget::OnLeave(void)
	{
		UnHighlightCurrent();
	}

	//-----------------------------------------------------------------------
	
	bool TreeItemDropTarget::OnDrop(wxCoord x, wxCoord y)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	wxDragResult TreeItemDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
	{
		if (!highlighted.IsOk() || !GetData())
			return wxDragNone;
		Handle target = treeview->GetComponent(highlighted);
		UnHighlightCurrent();

		const StdStringList targetTypes = Call<const StdStringList (void)>(s_classId, target, "GetChildrenTypes")();
		Handle parent;
		StdStringList parentTypes;
		if (targetTypes.empty()) {
			parent = Handle(target->GetParent());
			parentTypes = Call<const StdStringList (void)>(s_classId, parent, "GetChildrenTypes")();
		}
		else {
			parent = target;
			parentTypes = targetTypes;
		}

		TreeItemDataObject *object = static_cast<TreeItemDataObject*>(GetDataObject());
		if (object->HasHandle()) {
			const Handle editor = object->GetHandle();
			bool tempEditor = Call<bool (void)>(s_classId, editor, "IsTemp")();
			bool isDynamicSource = false;				
			String uri = Call<const String& (void)>(s_classId, editor, "GetURI")();
			if (tempEditor) {	//if not, added as if the file was dragged into the workspace, so no option dialog
				// FIXME: make this a func of the delta vm as "IsDynamicSource".
				isDynamicSource =
					!Call<const String (const Handle&)>(s_classId, "DeltaVM", "GetSymbolicURIFromEditor")(editor).empty();
				const String parentPath = Call<const String (void)>(s_classId, parent, "GetPath")();
				conf::PropertyTable table;
				conf::StringProperty* nameProp = new conf::StringProperty(_T("Name"), uri,
					_("Name for the file"));
				conf::DirectoryProperty* dirProp = new conf::DirectoryProperty(_T("Directory"), parentPath,
					_("Directory for the file"));

				table.AddProperty(util::str2std(nameProp->GetLabel()), nameProp);
				table.AddProperty(util::str2std(dirProp->GetLabel()), dirProp);

				conf::PropertyIdVec changed;
				conf::DefaultGUIGenerator dgg;
				if (dgg.ShowDialogFromProperties(0, table, changed, _("Add item to Workspace..."))) {
					const String name = nameProp->GetValue();
					if (name.find_first_of(wxFileName::GetForbiddenChars()) != String::npos) {
						gui::displayMessage(0, _("Error"), _("Given name contains forbidden characters!"));
						return wxDragNone;
					}
					uri = dirProp->GetExpandedValue() + name;
				}
				else
					return wxDragNone;
			}
				
			if (Call<Handle (const String&)>(s_classId, parent, "GetChildByURI")(uri))
				return wxDragNone;
			else if (AddItemFromURI(parent, parentTypes, uri) && tempEditor) {
				if (isDynamicSource)
					Call<bool (const String&)>(s_classId, editor, "SaveToDisk")(uri);
				else
					Call<bool (const String&)>(s_classId, editor, "SaveAs")(uri);
			}
		}
		else
			BOOST_FOREACH(const String& file, object->GetFilenames())
				AddItemFromURI(parent, parentTypes, file);
		return def;
	}

	//-----------------------------------------------------------------------
}