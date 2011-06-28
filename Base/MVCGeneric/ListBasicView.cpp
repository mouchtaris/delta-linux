/**
 *	ListBasicView.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ListBasicView.h"
#include "Streams.h"
#include "Algorithms.h"

#include <wx/imaglist.h>

namespace ide {

////////////////////////////////////////////////////////////////////////
// ListBasicView class
//
BEGIN_EVENT_TABLE(ListBasicView, wxListCtrl)
	EVT_LIST_BEGIN_DRAG(wxID_ANY, ListBasicView::onItemDragged)
	EVT_LIST_BEGIN_LABEL_EDIT(wxID_ANY, ListBasicView::onItemEditStart)
	EVT_LIST_END_LABEL_EDIT(wxID_ANY, ListBasicView::onItemEditEnd)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, ListBasicView::onItemSelected)
	EVT_LIST_ITEM_FOCUSED(wxID_ANY, ListBasicView::onItemFocused)
	EVT_LIST_ITEM_DESELECTED(wxID_ANY, ListBasicView::onItemDeselected)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, ListBasicView::onItemActivated)
	EVT_CONTEXT_MENU(ListBasicView::onContextMenu)
END_EVENT_TABLE();

//**********************************************************************

ListBasicView::ListBasicView (void)
{
}

//**********************************************************************

ListBasicView::ListBasicView (wxWindow* parent, const wxSize& size) :
	ItemListCtrl(parent, IDE_ITEMLISTCTRL_FLAGS, size)
{
}

//**********************************************************************

ListBasicView::~ListBasicView (void)
{

}

//**********************************************************************

bool ListBasicView::Create (wxWindow* parent, const wxSize& size)
{
	if (!this->ItemListCtrl::Create(parent, IDE_ITEMLISTCTRL_FLAGS, size))
		return false;
	return true;
}

//**********************************************************************

void ListBasicView::Update (void)
{
	this->ClearAll();
	this->OnSetModel(this->GetModel());
}

//**********************************************************************

void ListBasicView::GetSelectedItems (IntList& result) const
{
	long onItem = -1;
	while (true) {
		onItem = this->GetNextItem(onItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		if (onItem == -1)
			break;
		result.push_back(this->GetItemData(onItem));
	}
}

//**********************************************************************

void ListBasicView::SelectItem (int id, bool select)
{
	long state = select ? wxLIST_STATE_SELECTED : ~wxLIST_STATE_SELECTED;
	long onItem = this->FindItem(-1, id);
	assert(onItem != -1);
	this->SetItemState(onItem, state, wxLIST_MASK_STATE);
}

void ListBasicView::SelectAllItems (bool select)
{
	long state = select ? wxLIST_STATE_SELECTED : ~wxLIST_STATE_SELECTED;

	long onItem = -1;
	while (true) {
		onItem = this->GetNextItem(onItem, wxLIST_NEXT_ALL, wxLIST_STATE_DONTCARE);
		if (onItem == -1)
			break;
		this->SetItemState(onItem, state, wxLIST_MASK_STATE);
	}
}

//**********************************************************************

void ListBasicView::AddedNewItem (const Entry& entry)
{
	this->InsertItem(this->convert(entry));
}

//**********************************************************************

void ListBasicView::ModifiedItem (int id, const Entry& entry)
{
	this->RemovedItem(id);
	this->AddedNewItem(entry);
}

//**********************************************************************

void ListBasicView::ModifiedLabel (int id, const String& label)
	{ this->SetItemText(this->findItem(id), label); }

void ListBasicView::ModifiedDescription (int id, const String& desc)
	{}

void ListBasicView::ModifiedImageIndex (int id, int imageIndex)
	{ this->SetItemImage(this->findItem(id), imageIndex); }

//**********************************************************************

void ListBasicView::ModifiedId (int oldId, int newId)
{
	DBGOUT << "New id for " << oldId << " set to: " << newId << std::endl;
	this->SetItemData(this->findItem(oldId), newId);
}

//**********************************************************************

void ListBasicView::ModifiedColor (int id, Color color)
	{ this->SetItemTextColour(this->findItem(id), color); }

//**********************************************************************

void ListBasicView::ModifiedState (int id, StateType type)
{
	long state = (type == ListModel::STATE_INACTIVE ? wxLIST_STATE_CUT : ~wxLIST_STATE_CUT);
	this->SetItemState(this->findItem(id), state, wxLIST_STATE_CUT);
}

//**********************************************************************

void ListBasicView::RemovedItem (int id)
	{ this->DeleteItem(this->findItem(id)); }

//**********************************************************************

void ListBasicView::ClearedAllItems (void) { this->ClearAll(); }

//**********************************************************************

void ListBasicView::SetImages (wxImageList* imageList, bool own)
{
	if (own)
		this->AssignImageList(imageList, wxIMAGE_LIST_NORMAL);
	else
		this->SetImageList(imageList, wxIMAGE_LIST_NORMAL);
}

//**********************************************************************

void ListBasicView::SetImages (const StringList& images)
{
	static wxBitmap defaultImage(_T("default"));

	wxImageList* imageList = new wxImageList(32, 32);
	for (StringList::const_iterator i = images.begin(); i != images.end(); ++i) {
		wxBitmap image(*i);
		imageList->Add((image.Ok() ? image : defaultImage));
	}
	imageList->Add(defaultImage);
	this->AssignImageList(imageList, wxIMAGE_LIST_NORMAL);
}

//**********************************************************************

void ListBasicView::OnResetModel (void)
{
	this->ClearAll();
}

//**********************************************************************

void ListBasicView::OnSetModel (ListModel* model)
{
	EntryList onItems;
	model->GetAllItems(onItems);
	util::for_all(onItems, boost::bind(&ListBasicView::AddedNewItem, this, _1));
}

//**********************************************************************

void ListBasicView::OnSetController (ListController* controller, bool own)
{

}

//**********************************************************************

void ListBasicView::onItemSelected (wxListEvent& event)
	{ this->GetController()->SelectItem(event.GetData(), true); }

void ListBasicView::onItemDeselected (wxListEvent& event)
	{ this->GetController()->SelectItem(event.GetData(), false); }

//**********************************************************************

void ListBasicView::onItemActivated (wxListEvent& event)
	{ this->GetController()->ActivateItem(event.GetData()); }

void ListBasicView::onItemFocused (wxListEvent& event)
	{ this->GetController()->FocusItem(event.GetData()); }

//**********************************************************************

void ListBasicView::onItemDragged (wxListEvent& event)
	{ this->GetController()->DragItems(); }

//**********************************************************************

void ListBasicView::onItemEditStart (wxListEvent& event)
{

}

void ListBasicView::onItemEditEnd (wxListEvent& event)
{

}

//**********************************************************************

void ListBasicView::onContextMenu (wxContextMenuEvent& event)
{
	DBGOUT << "CONTEXT" << std::endl;
	this->GetController()->PopupContextMenu();
}

//**********************************************************************

long ListBasicView::findItem (int id)
{
	long onItem = this->FindItem(-1, id);
	assert(onItem != -1);
	return onItem;
}

//**********************************************************************

wxListItem ListBasicView::convert (const Entry& entry)
{

	wxListItem onItem;
	onItem.SetText(entry.get<0>());

	// Tooltip : entry.get<1>() ?

	if (wxImageList* imageList = this->GetImageList(wxIMAGE_LIST_NORMAL)) {
		if (imageList->GetImageCount())
			onItem.SetImage(std::min<int>(entry.get<2>(), imageList->GetImageCount() - 1));
	}
	onItem.SetData(entry.get<3>());
	onItem.SetTextColour(entry.get<4>());

	if (entry.get<5>() == ListModel::STATE_INACTIVE)
		onItem.SetState(wxLIST_STATE_CUT);

	return onItem;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
