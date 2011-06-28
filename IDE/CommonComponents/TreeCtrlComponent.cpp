/**
 *	TreeCtrlComponent.cpp
 *
 *	-- IDE Common Components --
 *
 *	Component callbacks that are exchanged among
 *	components.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "TreeCtrlComponent.h"

#include "ComponentRegistry.h"

#include "ImageList.h"
#include "IDEDialogs.h"
#include "PropertyUtils.h"

#include "Call.h"
#include "UndefineAllWin32Shit.h"

#include <wx/sizer.h>
#include <wx/imaglist.h>

namespace ide
{
	BEGIN_EVENT_TABLE(TreeCtrlComponent, wxPanel)
		EVT_TREE_SEL_CHANGED(wxID_ANY, TreeCtrlComponent::onSelectionChanged)
		EVT_TREE_ITEM_ACTIVATED(wxID_ANY, TreeCtrlComponent::onItemActivated)
		EVT_TREE_ITEM_COLLAPSED(wxID_ANY, TreeCtrlComponent::onItemCollapsed)
		EVT_TREE_ITEM_EXPANDED(wxID_ANY, TreeCtrlComponent::onItemExpanded)
	END_EVENT_TABLE();

	//-------------------------------------------------------//
	//---- class TreeCtrlComponent --------------------------//

	COMPONENT_METADATA(
		TreeCtrlComponent,
		_("Tree Control Component"),
		_("Common Component with a tree view of components"),
		_T("Themistoklis Bourdenas <themis@ics.forth.gr>"),
		_T("alpha")
	);
	IMPLEMENT_WX_COMPONENT_(TreeCtrlComponent, wxPanel, DockableComponent);

	//-----------------------------------------------------------------------

	COMPONENT_SET_PROPERTIES_FUNCTION(TreeCtrlComponent, table)
	{
	}

	//-----------------------------------------------------------------------

	TreeCtrlComponent::TreeCtrlComponent(void)
	{
		conf::BoolProperty *p = new conf::BoolProperty(_("Collapse"));
		p->SetVisible(false);
		AddInstanceProperty("collapse", p);
	}

	//-----------------------------------------------------------------------

	TreeCtrlComponent::~TreeCtrlComponent(void)
	{
	}

	//-----------------------------------------------------------------------

	wxWindow* TreeCtrlComponent::GenerateWindow(wxWindow* parent)
	{
		Create(parent);

		//-- Create Tree Control
		treeview = new ComponentTreeView(this, wxID_ANY, wxDefaultPosition);
		treeview->SetComparator(boost::bind(&TreeCtrlComponent::OnCompareItems, this, _1, _2));

		//-- Relize Toolbar
		toolbar = new ComponentToolBar(this);
		toolbar->SetToolBitmapSize(wxSize(16,16));
		toolbar->Realize();
		toolbar->sigToolSelected.connect(
			boost::bind(&TreeCtrlComponent::slotToolSelected, this, _1)
		);

		vSizer = new wxBoxSizer(wxVERTICAL);
		vSizer->Add(toolbar, 0, wxALL, 0);
		vSizer->AddSpacer(3);
		vSizer->Add(treeview, 1, wxEXPAND | wxALL, 0);

		SetSizer(vSizer);
		vSizer->SetSizeHints(this);		///< set size hints to honour minimum size

		return this;
	}

	//-----------------------------------------------------------------------

	wxWindow* TreeCtrlComponent::GetWindow(void)
	{
		return this;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, bool, AddComponent, (const Handle& parent, const Handle& handle))
	{
		if (!treeview->InsertItem(parent, handle))
			return false;
		treeview->EnsureVisible(treeview->GetNode(handle));
		Call<void (const Handle&)>(this, handle, "SetTreeView")(this);
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, bool, RemoveComponent, (const Handle& handle))
	{
		return treeview->RemoveItem(handle);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, bool, RenameComponent, (const Handle& handle, const String& name))
	{
		treeview->SetItemText(treeview->GetNode(handle), name);
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, bool, EditLabel, (const Handle& handle))
	{
		treeview->EditLabel(treeview->GetNode(handle));
		return true;
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, const Handle&, GetTreeRoot, (void))
	{
		return treeview->GetRoot();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, const Handle&, GetParent, (const Handle& handle))
	{
		return treeview->GetParent(treeview->GetNode(handle));
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, void, SortChildren, (const Handle& handle))
	{
		wxTreeItemId id = treeview->GetNode(handle);
		if (id.IsOk())
			treeview->SortChildren(id);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, void, Collapse, (const Handle& handle))
	{
		wxTreeItemId id = treeview->GetNode(handle);
		if (id.IsOk())
			treeview->Collapse(id);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, void, Expand, (const Handle& handle))
	{
		wxTreeItemId id = treeview->GetNode(handle);
		if (id.IsOk())
			treeview->Expand(id);
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, HandleList, GetResources, (const String& type))
	{
		return (treeview->GetRoot())
			? Call<HandleList (const String&)>(this, treeview->GetRoot(), "CollectChildren")(type)
			: HandleList();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, Handle, GetResource, (const String& name))
	{
		return (treeview->GetRoot())
			? Call<Handle (const String&)>(this, treeview->GetRoot(), "GetChild")(name)
			: Handle();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, Handle, GetResourceByURI, (const String& uri))
	{
		return (treeview->GetRoot())
			? Call<Handle (const String&)>(this, treeview->GetRoot(), "GetChildByURI")(uri)
			: Handle();
	}

	//-----------------------------------------------------------------------

	EXPORTED_FUNCTION(TreeCtrlComponent, Handle, GetResourceBySymbolicURI, (const String& uri))
	{
		return (treeview->GetRoot())
			? Call<Handle (const String&)>(this, treeview->GetRoot(), "GetChildBySymbolicURI")(uri)
			: Handle();
	}

	//-----------------------------------------------------------------------

	void TreeCtrlComponent::slotToolSelected(const Callback& action)
	{
		const Handle& handle = treeview->GetComponent(treeview->GetSelection());
		if (!handle.IsDangling()) {
			Callback selectedInstanceCall(action);
			selectedInstanceCall.SetInstance(handle);
			selectedInstanceCall();
		}
	}

	//-----------------------------------------------------------------------

	void TreeCtrlComponent::onSelectionChanged(wxTreeEvent& event)
	{
		const Handle& handle = treeview->GetComponent(event.GetItem());
		if (!handle.IsDangling()) {
			const UserCommand& root = handle->GetUserCommands();
			handle->Focus();

			toolbar->ClearTools();
			root.GenerateToolBar(toolbar)->Realize();
		}
	}

	//-----------------------------------------------------------------------

	void TreeCtrlComponent::onItemActivated(wxTreeEvent& event)
	{
		const Handle& handle = treeview->GetComponent(event.GetItem());
		if (!handle.IsDangling() &&
			ComponentRegistry::Instance().GetComponentEntry(handle.GetClassId()).GetFuncEntry("DefaultAction")
		)
			Call<void (void)>(this, handle, "DefaultAction")();
	}

	//-----------------------------------------------------------------------


	void TreeCtrlComponent::onItemCollapsed(wxTreeEvent& event)
	{
		const Handle& handle = treeview->GetComponent(event.GetItem());
		if (!handle.IsDangling())
			Call<void (bool)>(this, handle, "SetExpanded")(false);
	}

	//-----------------------------------------------------------------------

	void TreeCtrlComponent::onItemExpanded(wxTreeEvent& event)
	{
		const Handle& handle = treeview->GetComponent(event.GetItem());
		if (!handle.IsDangling())
			Call<void (bool)>(this, handle, "SetExpanded")(true);
	}

	//-----------------------------------------------------------------------

	int TreeCtrlComponent::OnCompareItems(const Handle& handle1, const Handle& handle2)
	{
		if (handle1.IsDangling() || handle2.IsDangling())
			return 0;
		const Handle& parent = GetParent(handle1);
		assert(parent == GetParent(handle2) && handle1.Resolve() && handle2.Resolve());
		try {
			return Call<int (const Handle&, const Handle&)>(this, parent, "OnCompareItems")(handle1, handle2);
		} catch (std::exception&) {
			return	wxStricmp(
						Call<const String& (void)>(this, handle1, "GetName")(),
						Call<const String& (void)>(this, handle2, "GetName")()
					);
		}
	}

	//-----------------------------------------------------------------------
}
