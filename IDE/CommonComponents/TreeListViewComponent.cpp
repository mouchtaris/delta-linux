/**
 *	TreeListViewComponent.cpp
 *
 *	-- Sparrow Core Components --
 *
 *	TreeListView component is a base class component
 *	providing a tree list view interface to the derived
 *	components.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2008
 */
#include "TreeListViewComponent.h"

#include "StringUtils.h"
#include "ImageList.h"
#include "BitmapRegistry.h"

#include "ComponentMenu.h"

#include "Call.h"
#include "DelayedCaller.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>
#include <queue>

#define CHECK_VALID_TREE_ITEM(event)	if (!event.GetItem().IsOk()) return

namespace ide
{
	//-------------------------------------------------------//
	//---- Custom Events ------------------------------------//

	DECLARE_LOCAL_EVENT_TYPE(EVENT_PRINT_TREE_LIST_VIEW, -1);
	DEFINE_EVENT_TYPE(EVENT_PRINT_TREE_LIST_VIEW);

	BEGIN_EVENT_TABLE(TreeListViewComponent, wxTreeListCtrl)
		EVT_COMMAND(wxID_ANY, EVENT_PRINT_TREE_LIST_VIEW, TreeListViewComponent::onDisplay)
		EVT_MENU(wxID_ANY, TreeListViewComponent::onMenuCommand)
		EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY, TreeListViewComponent::onContextMenu)
		EVT_TREE_ITEM_ACTIVATED(wxID_ANY, TreeListViewComponent::onItemActivated)
		EVT_TREE_ITEM_EXPANDING(wxID_ANY, TreeListViewComponent::onItemExpanding)
		EVT_TREE_ITEM_COLLAPSING(wxID_ANY, TreeListViewComponent::onItemCollapsing)
		EVT_TREE_SEL_CHANGED(wxID_ANY, TreeListViewComponent::onItemSelected)
		EVT_TREE_KEY_DOWN(wxID_ANY, TreeListViewComponent::onKeyDown)
		EVT_LIST_COL_END_DRAG(wxID_ANY, TreeListViewComponent::onColEndDrag)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class TreeListViewComponent ----------------------//

	COMPONENT_METADATA(
		TreeListViewComponent,
		_("Tree List View"),
		_("TreeListView is a base class component providing a tree list view interface"),
		_T("Ioannis Lilis <lilis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(TreeListViewComponent, wxTreeListCtrl, DockableComponent);

	//-----------------------------------------------------------------------

	EXPORTED_SIGNAL(TreeListViewComponent, TreeListItemActivated, (uint serial));
	EXPORTED_SIGNAL(TreeListViewComponent, TreeListItemExpanding, (uint serial));
	EXPORTED_SIGNAL(TreeListViewComponent, TreeListItemCollapsing, (uint serial));
	EXPORTED_SIGNAL(TreeListViewComponent, TreeListItemSelected, (uint serial));
	EXPORTED_SIGNAL(TreeListViewComponent, TreeListWidthChanged, (uint width));
	EXPORTED_SIGNAL(TreeListViewComponent, DeleteTreeListItem, (uint serial));

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(TreeListViewComponent, table)
	{
	}

	//-----------------------------------------------------------------------

	TreeListViewComponent::TreeListViewComponent(void)
		: imageList(0), treeItemIdCounter(0), popupMenu(UserCommand()), menu(0), ignoreCollapseAndExpandChanges(0)
	{
	}

	//-----------------------------------------------------------------------

	TreeListViewComponent::~TreeListViewComponent(void)
	{
		if (menu) {
			delete menu;
			menu = 0;
		}
	}

	//-----------------------------------------------------------------------

	wxWindow* TreeListViewComponent::GenerateWindow(wxWindow* parent)
	{
		wxTreeListCtrl::Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
			wxLC_REPORT | wxTR_HAS_BUTTONS | wxTR_COLUMN_LINES | wxTR_ROW_LINES |
			wxTR_FULL_ROW_HIGHLIGHT | wxTR_MULTIPLE);
		menu = popupMenu.GenerateMenu(new ComponentMenu);
		return this;
	}

	//-----------------------------------------------------------------------

	wxWindow* TreeListViewComponent::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetTitle, (const String& title))
	{
		this->title = title;
		Call<bool (const Handle&, const String&)>(this, "Shell", "RenameWindow")(this, title);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, const String&, GetTitle, (void))
	{
		return title;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetColumns, (const StringList& columns))
	{
		if (columns.size()) {
			BOOST_FOREACH(const String& title, columns)
			{
				std::vector<String> tokens;
				util::stringtokenizer(tokens, title, _T(":"));
				AddColumn(tokens[0], (tokens.size() == 2 ? boost::lexical_cast<int>(tokens[1]) : 150), wxALIGN_LEFT);
			}
			SetMainColumn(0);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetImages, (const StringList& images))
	{
		imageList = new ImageList(16,16);

		BOOST_FOREACH(const String& image, images)
			imageList->Add(image, BitmapRegistry::Instance().Get(image));

		AssignImageList(imageList);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, uint, Append, (uint parentSerial, const StringList& fields))
	{
		wxTreeItemId parent = GetNode(parentSerial);
		uint serial = ++treeItemIdCounter;
		wxTreeItemId id;
		if (!parent)
			id = AddRoot(fields.front(), -1, -1, new TreeItemData(serial));
		else
			id = AppendItem(parent, fields.front(), -1, -1, new TreeItemData(serial));

		int i=0;
		BOOST_FOREACH(const String& text, fields)
			SetItemText(id, i++, text);

		return serial;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, uint, Insert, (uint parentSerial, uint index, const StringList& fields))
	{
		wxTreeItemId parent = GetNode(parentSerial);
		uint serial = ++treeItemIdCounter;
		wxTreeItemId id;
		if (!parent)
			id = AddRoot(fields.front(), -1, -1, new TreeItemData(serial));
		else
			id = InsertItem(parent, index, fields.front(), -1, -1, new TreeItemData(serial));

		int i=0;
		BOOST_FOREACH(const String& text, fields)
			SetItemText(id, i++, text);

		return serial;
	}

	//-----------------------------------------------------------------------
	
	EXPORTED_FUNCTION(TreeListViewComponent, bool, Remove, (uint serial))
	{
		if (serial == GetRoot()) {
			DeleteRoot();
			return true;
		}
		else {
			wxTreeItemId node = GetNode(serial);
			if (!node)
				return false;

			Delete(node);
			return true;
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, int, GetRelativePosition, (uint serial))
	{
		wxTreeItemId target = GetNode(serial);
		if (!target)
			return -1;
		wxTreeItemId parent = GetItemParent(target);
		if (!parent)	// In case of the root return an error value as well
			return -1;

		wxTreeItemIdValue cookie;
		wxTreeItemId node = GetFirstChild(parent, cookie);
		for (unsigned i = 0; node.IsOk(); ++i, node = GetNextChild(parent, cookie))
			if (node == target)
				return i;
		
		assert(false);	// When passing a valid item with a valid parent it should always be found
		return -1;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, uint, GetTotalChildren, (uint parentSerial, bool recursively))
	{
		wxTreeItemId node = GetNode(parentSerial);
		return node ? GetChildrenCount(node, recursively) : 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, UIntList, GetChildren, (uint parentSerial, bool recursively))
	{
		UIntList children;
		wxTreeItemId parent = GetNode(parentSerial);
		if (!parent)
			return children;

		wxTreeItemIdValue cookie;
		wxTreeItemId node = GetFirstChild(parent, cookie);
		while (node.IsOk())
		{
			uint serial = static_cast<TreeItemData *>(GetItemData(node))->GetSerial();
			children.push_back(serial);
			if (recursively) {
				const UIntList decendants = GetChildren(serial, true);
				children.insert(children.end(), decendants.begin(), decendants.end());
			}
			node = GetNextChild(parent, cookie);
		}

		return children;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, uint, GetParent, (uint serial))
	{
		wxTreeItemId parent = GetItemParent(GetNode(serial));
		return parent ? static_cast<TreeItemData *>(GetItemData(parent))->GetSerial() : 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, uint, GetRoot, (void))
	{
		wxTreeItemId root = GetRootItem();
		return root ? static_cast<TreeItemData *>(GetItemData(root))->GetSerial() : 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetRootVisibility, (bool val))
	{
		long style = GetWindowStyle();
		if (val)
			style &= ~wxTR_HIDE_ROOT;
		else
			style |= wxTR_HIDE_ROOT;
		SetWindowStyle(style);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, const String, GetText, (uint serial, uint col))
	{
		wxTreeItemId node = GetNode(serial);
		return node ? GetItemText(node, col) : String();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetText, (uint serial, uint col, const String& text))
	{
		if (wxTreeItemId node = GetNode(serial))
			SetItemText(node, col, text);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, const wxColour, GetTextColor, (uint serial))
	{
		wxTreeItemId node = GetNode(serial);
		return node ? GetItemTextColour(node) : wxColour();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetTextColor, (uint serial, const wxColour& color))
	{
		if (wxTreeItemId node = GetNode(serial))
			SetItemTextColour(node, color);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, const wxFont, GetFont, (uint serial))
	{
		wxTreeItemId node = GetNode(serial);
		return node ? GetItemFont(node) : wxFont();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetFont, (uint serial, const wxFont& font))
	{
		if (wxTreeItemId node = GetNode(serial)) {
			SetItemFont(node, font);
			// manually refresh by collapsing and reexpanding the node
			++ignoreCollapseAndExpandChanges;
			bool hadChildren = HasChildren(node);
			bool wasExpanded = IsExpanded(node);
			wxTreeListCtrl::SetItemHasChildren(node);
			wxTreeListCtrl::Collapse(node);
			wxTreeListCtrl::Expand(node);
			if (!wasExpanded)
				wxTreeListCtrl::Collapse(node);
			wxTreeListCtrl::SetItemHasChildren(node, hadChildren);
			--ignoreCollapseAndExpandChanges;
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, String, GetImage, (uint serial, uint col))
	{
		wxTreeItemId node = GetNode(serial);
		return node ? imageList->GetImageKeyByIndex(GetItemImage(node, col)) : String();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetImage, (uint serial, uint col, const String& image))
	{
		if (wxTreeItemId node = GetNode(serial))
			SetItemImage(node, col, imageList->ResolveImageIndex(image));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, uint, GetColumnWidth, (uint col))
	{
		return col < (uint) GetColumnCount() ? GetColumn(col).GetWidth() : 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetColumnWidth, (uint col, uint width))
	{
		if (col < (uint) GetColumnCount())
		{
			wxTreeListColumnInfo info = GetColumn(col);
			info.SetWidth(width);
			SetColumn(col, info);
		}
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, bool, HasSingleSelection, (void))
	{
		return !(GetWindowStyle() & wxTR_MULTIPLE);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, bool, SetSingleSelectionMode, (bool value))
	{
		if (GetRootItem()) {
			long style = GetWindowStyle();
			if (value)
				style &= ~wxTR_MULTIPLE;
			else
				style |= wxTR_MULTIPLE;
			SetWindowStyle(style);
			return true;
		}
		else
			return false;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, int, GetSingleSelection, (void))
	{
		if (!HasSingleSelection())
		{
			UIntList multiple = GetMultipleSelection();
			return multiple.empty() ? 0 : multiple.front();
		}
		wxTreeItemId node = GetSelection();
		return node ? static_cast<TreeItemData *>(GetItemData(node))->GetSerial() : 0;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, UIntList, GetMultipleSelection, (void))
	{
		UIntList selected;
		if (HasSingleSelection())
		{
			if (int serial = GetSingleSelection())
				selected.push_back(serial);
		}
		else
		{
			wxArrayTreeItemIds ids;
			GetSelections(ids);
			for(unsigned i = 0; i < ids.size(); ++i)
				selected.push_back(static_cast<TreeItemData *>(GetItemData(ids[i]))->GetSerial());
		}
		return selected;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetSingleSelection, (uint serial))
	{
		if (wxTreeItemId node = GetNode(serial))
			SelectItem(node);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetMultipleSelection, (UIntList serials))
	{
		for(UIntList::const_iterator i = serials.begin(); i != serials.end(); ++i)
			if (wxTreeItemId node = GetNode(*i))
				SelectItem(node, NULL, i == serials.begin());
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, SetItemHasChildren, (uint serial, bool val))
	{
		if (wxTreeItemId node = GetNode(serial))
			wxTreeListCtrl::SetItemHasChildren(node, val);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, Collapse, (uint serial))
	{
		if (wxTreeItemId node = GetNode(serial))
			wxTreeListCtrl::Collapse(node);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, Expand, (uint serial))
	{
		if (wxTreeItemId node = GetNode(serial))
			wxTreeListCtrl::Expand(node);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, EnsureVisible, (uint serial))
	{
		if (wxTreeItemId node = GetNode(serial))
			wxTreeListCtrl::EnsureVisible(node);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, ScrollTo, (uint serial))
	{
		if (wxTreeItemId node = GetNode(serial))
			wxTreeListCtrl::ScrollTo(node);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeListViewComponent, void, Clear, (void))
	{
		DeleteRoot();
	}

	//-----------------------------------------------------------------------

	wxTreeItemId TreeListViewComponent::GetNode(uint serial) const
	{
		if (!serial)
			return wxTreeItemId();

		std::queue<wxTreeItemId> nodes;
		wxTreeItemId root = GetRootItem();
		if (root)
		{
			if (static_cast<TreeItemData *>(GetItemData(root))->GetSerial() == serial)
				return root;
			nodes.push(root);
		}

		while (!nodes.empty())
		{
			wxTreeItemId parent = nodes.front();
			nodes.pop();

			wxTreeItemIdValue cookie;
			wxTreeItemId node = GetFirstChild(parent, cookie);
			if (node)
				nodes.push(node);

			while (node.IsOk())
			{
				if (static_cast<TreeItemData *>(GetItemData(node))->GetSerial() == serial)
					return node;
				node = GetNextChild(parent, cookie);
				if (node)
					nodes.push(node);
			}
		}

		return wxTreeItemId();
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::ComponentAddedUserCommand(const String& path, const UserCommand& cmd)
	{
		ComponentMenu *oldMenu = menu;
		popupMenu.InsertUserCommand(path, cmd);
		menu = popupMenu.GenerateMenu(new ComponentMenu);
		delete oldMenu;
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::ComponentRemovedUserCommand(const String& path)
	{
		ComponentMenu *oldMenu = menu;
		popupMenu.RemoveUserCommand(path);
		menu = popupMenu.GenerateMenu(new ComponentMenu);
		delete oldMenu;
	}

	//-----------------------------------------------------------------------

	static void _callComponentCallback(const UserCommand::Callback& action)
	{
		action();
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::onDisplay(wxCommandEvent& event)
	{
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::onMenuCommand(wxCommandEvent& event)
	{
		timer::DelayedCaller::Instance().PostDelayedCall(
			boost::bind(_callComponentCallback, menu->GetAction(event.GetId()))
		);
	}

	//-----------------------------------------------------------------------
	
	void TreeListViewComponent::onContextMenu (wxTreeEvent& event)
	{
		CHECK_VALID_TREE_ITEM(event);
		SelectItem(event.GetItem());
		PopupMenu(menu);
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::onItemActivated(wxTreeEvent& event)
	{
		CHECK_VALID_TREE_ITEM(event);
		uint serial = static_cast<TreeItemData *>(GetItemData(event.GetItem()))->GetSerial();
		sigTreeListItemActivated(this, serial);
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::onItemExpanding(wxTreeEvent& event)
	{
		CHECK_VALID_TREE_ITEM(event);
		if (!ignoreCollapseAndExpandChanges) {
			uint serial = static_cast<TreeItemData *>(GetItemData(event.GetItem()))->GetSerial();
			sigTreeListItemExpanding(this, serial);
		}
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::onItemCollapsing(wxTreeEvent& event)
	{
		CHECK_VALID_TREE_ITEM(event);
		if (!ignoreCollapseAndExpandChanges) {
			uint serial = static_cast<TreeItemData *>(GetItemData(event.GetItem()))->GetSerial();
			sigTreeListItemCollapsing(this, serial);
		}
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::onItemSelected(wxTreeEvent& event)
	{
		CHECK_VALID_TREE_ITEM(event);
		uint serial = static_cast<TreeItemData *>(GetItemData(event.GetItem()))->GetSerial();
		sigTreeListItemSelected(this, serial);
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::onKeyDown(wxTreeEvent& event)
	{
		int key = event.GetKeyEvent().GetKeyCode();
		if (key == WXK_DELETE || key == WXK_NUMPAD_DELETE)
			BOOST_FOREACH(uint serial, GetMultipleSelection())
				sigDeleteTreeListItem(this, serial);
		//Ctrl+A gives a key with value 1 (ascii value of Ctrl+A)
		else if (!HasSingleSelection() && key == 1 && event.GetKeyEvent().ControlDown())
			SetMultipleSelection(GetChildren(GetRoot(), true));
		else
			event.Skip();
	}

	//-----------------------------------------------------------------------

	void TreeListViewComponent::onColEndDrag(wxListEvent& event)
	{
		sigTreeListWidthChanged(this, GetSize().GetWidth());
	}

	//-----------------------------------------------------------------------
}
