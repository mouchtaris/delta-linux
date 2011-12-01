/**
 *	ComponentTreeView.cpp
 *
 *	-- IDEWidgets --
 *
 *	ComponentTreeView is a wrapper of a TreeCtrl that
 *	holds components. Every node in the tree is a
 *	component and has to comply to a standard API in
 *	order to be presented accordingly.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#include "ComponentTreeView.h"
#include "Streams.h"

#include "ComponentRegistry.h"
#include "Properties.h"
#include "PropertyUtils.h"

#include "Call.h"
#include "ImageList.h"
#include "BitmapRegistry.h"

#include <wx/menu.h>

#include <boost/tuple/tuple.hpp>

#include <queue>

namespace ide
{
	IMPLEMENT_DYNAMIC_CLASS(ComponentTreeView, wxTreeCtrl)
	BEGIN_EVENT_TABLE(ComponentTreeView, wxTreeCtrl)
		EVT_TREE_BEGIN_LABEL_EDIT(wxID_ANY, ComponentTreeView::onBeginLabelEdit)
		EVT_TREE_END_LABEL_EDIT(wxID_ANY, ComponentTreeView::onEndLabelEdit)
		EVT_TREE_ITEM_MENU(wxID_ANY, ComponentTreeView::onContextSelect)
		EVT_TREE_KEY_DOWN(wxID_ANY, ComponentTreeView::onKeyDown)
		EVT_MENU(wxID_ANY, ComponentTreeView::onCtxMenuSelect)
		EVT_LEFT_DOWN(ComponentTreeView::onMouseButtonDown)
		EVT_RIGHT_DOWN(ComponentTreeView::onMouseButtonDown)
	END_EVENT_TABLE();

	//----------------------------
	//-- class ComponentData

	class ComponentData :
		public wxTreeItemData,
		public Handle
	{
	public:
		///--- constructors / destructor
		ComponentData(void) {}
		ComponentData(Component* comp)
			: Handle(comp) {}
		ComponentData(const Handle& handle)
			: Handle(handle) {}
		virtual ~ComponentData(void) {}
	};


	//-------------------------------------------------------//
	//---- class ComponentTreeView --------------------------//

	ComponentTreeView::ComponentTreeView(wxWindow* parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size, long style)
		: wxTreeCtrl(parent, id, pos, size, style), popup(0), root((Handle*) 0)
	{
		//-- Create image list for Tree Control
		images = new ImageList(16, 16);
		wxTreeCtrl::AssignImageList(images);
	}

	//-----------------------------------------------------------------------

	bool ComponentTreeView::InsertItem(const Handle& parent, const Handle& item)
	{
		const String& name  = Call<const String& (void)>(parent.GetClassId(), item, "GetName")();
		const String& image =
			conf::get_prop_value<conf::EnumStringProperty>(item->GetProperty("treeCtrl_icon"), String());

		//-- Insert image is in the TreeCtrl image list
		if (images->ResolveImageIndex(image))
			AddImage(image);

		if (wxTreeItemId node = GetNode(item))
			return false;
		else {
			if (wxTreeItemId pnode = GetNode(parent)) {
				AppendItem(pnode, name, images->ResolveImageIndex(image), -1, new ComponentData(item));
				SortChildren(pnode);
			}
			else {
				ComponentData* data = new ComponentData(item);
				AddRoot(name, images->ResolveImageIndex(image), -1, data);
				root = data;
			}
			return true;
		}
	}

	//-----------------------------------------------------------------------

	bool ComponentTreeView::RemoveItem(const Handle& item)
	{
		if (wxTreeItemId node = GetNode(item)) {
			if (static_cast<ComponentData*>(GetItemData(node)) == root)
				root = (Handle*) 0;
			Delete(node);
			return true;
		}
		else
			return false;
	}

	//-----------------------------------------------------------------------

	const Handle& ComponentTreeView::GetRoot(void) const
	{
		static Handle null;
		return root ? *root : null;
	}

	//-----------------------------------------------------------------------

	const Handle& ComponentTreeView::GetComponent(wxTreeItemId item) const
	{
		static Handle null;
		if (item)
			return *(static_cast<ComponentData*>(GetItemData(item)));
		return null;
	}

	//-----------------------------------------------------------------------

	const Handle& ComponentTreeView::GetParent(wxTreeItemId item) const
	{
		static Handle null;
		if (item && (item = GetItemParent(item)))
			return *(static_cast<ComponentData*>(GetItemData(item)));
		return null;
	}

	//-----------------------------------------------------------------------

	wxTreeItemId ComponentTreeView::GetNode(const Handle& comp) const
	{
		if (!comp)
			return wxTreeItemId();

		std::queue<wxTreeItemId> nodes;
		if (GetRootItem())
		{
			if (GetRoot() == comp)
				return GetRootItem();
			nodes.push(GetRootItem());
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
				if (*static_cast<ComponentData*>(GetItemData(node)) == comp)
					return node;
				node = GetNextChild(parent, cookie);
				if (node)
					nodes.push(node);
			}
		}

		return wxTreeItemId();
	}

	//-----------------------------------------------------------------------

	bool ComponentTreeView::AddImage(const String& id)
	{
		return (BitmapRegistry::Instance().Exists(id))
			? (images->Add(id, BitmapRegistry::Instance().Get(id)) >= 0)
			: false;
	}

	//-----------------------------------------------------------------------

	void ComponentTreeView::SetComparator(const Comparator& comparator)
	{
		this->comparator = comparator;
	}

	//-----------------------------------------------------------------------

	int ComponentTreeView::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
	{
		return comparator
			?	comparator(
					*static_cast<ComponentData*>(GetItemData(item1)), 
					*static_cast<ComponentData*>(GetItemData(item2))
				)
			:	wxTreeCtrl::OnCompareItems(item1, item2);
	}

	//-----------------------------------------------------------------------

	void ComponentTreeView::onBeginLabelEdit(wxTreeEvent& event)
	{
		const Handle& handle = *static_cast<ComponentData*>(GetItemData(event.GetItem()));
		assert(handle);
		if (!ComponentRegistry::Instance().GetComponentEntry(handle.GetClassId()).HasFunction("Rename"))
			event.Veto();
	}

	//-----------------------------------------------------------------------

	void ComponentTreeView::onEndLabelEdit(wxTreeEvent& event)
	{
		if (!event.IsEditCancelled()) {
			const Handle& handle = *static_cast<ComponentData*>(GetItemData(event.GetItem()));
			assert(handle);
			assert(ComponentRegistry::Instance().GetComponentEntry(handle.GetClassId()).HasFunction("Rename"));
			if (!Call<bool (const String&)>("ComponentTreeView", handle, "Rename")(event.GetLabel()))
				event.Veto();
		}
	}

	//-----------------------------------------------------------------------

	void ComponentTreeView::onContextSelect(wxTreeEvent& event)
	{
		const wxTreeItemId id = event.GetItem();
		if (id.IsOk()) {
			SelectItem(id);

			const Handle& handle = *static_cast<ComponentData*>(GetItemData(id));
			popup = handle->GetUserCommands().GenerateMenu(new ComponentMenu);
			PopupMenu(popup);

			delete popup;
			popup=0;
		}
	}

	//-----------------------------------------------------------------------

	void ComponentTreeView::onKeyDown(wxTreeEvent& event)
	{
		const wxTreeItemId id = GetSelection();
		if (id.IsOk() && event.GetKeyEvent().GetKeyCode() == WXK_F2)
			EditLabel(id);
		else
			event.Skip();
	}

	//-----------------------------------------------------------------------

	void ComponentTreeView::onCtxMenuSelect(wxCommandEvent& event)
	{
		assert(popup);
		popup->GetAction(event.GetId())();
	}

	//-----------------------------------------------------------------------

	void ComponentTreeView::onMouseButtonDown(wxMouseEvent& event)
	{
		SetFocus();
		event.Skip();
	}

	//-----------------------------------------------------------------------
}
