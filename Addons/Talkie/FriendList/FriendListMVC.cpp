/**
 *	FriendListMVC.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "FriendListMVC.h"
#include "Algorithms.h"
#include "StringUtils.h"
#include "Serialization.h"
#include "ContactsImageList.h"
#include "InternalDataObject.h"
#include "InternalDropTarget.h"

#include <wx/dnd.h>
#include <sstream>

namespace talkie {

////////////////////////////////////////////////////////////////////////

using namespace ide;

////////////////////////////////////////////////////////////////////////
// FriendListModel class
//
FriendListModel::FriendListModel (void)
{
}

//**********************************************************************

FriendListModel::FriendListModel (const FriendEntryVec& friends) :
	m_friends(friends)
{

}

//**********************************************************************

FriendListModel::~FriendListModel (void)
{

}

//**********************************************************************

void FriendListModel::GetAllItems (EntryList& result) const
{
	for (uint i = 0; i < m_friends.size(); ++i)
		result.push_back(convert(m_friends[i], i));
}

//**********************************************************************

bool FriendListModel::GetItem (Entry& result, int id) const
{
	assert(uint(id) < m_friends.size());
	result = convert(m_friends[id], id);
	return true;
}

//**********************************************************************

void FriendListModel::ClearAllItems (void)
{
	m_friends.clear();
	this->ApplyToAllViews(std::mem_fun(&ListView::ClearedAllItems));
}

//**********************************************************************

void FriendListModel::RemoveItem (int id)
{
	FriendEntryVec::iterator iter = m_friends.begin();
	std::advance(iter, id);
	iter = m_friends.erase(iter);
	this->ApplyToAllViews(std::bind2nd(std::mem_fun(&ListView::RemovedItem), id));

	for (int oldId = id + 1; iter != m_friends.end(); ++iter, ++oldId)
		this->ApplyToAllViews(boost::bind(&ListView::ModifiedId, _1, oldId, oldId - 1));
}

//**********************************************************************

bool FriendListModel::GetItems (std::string& encodedStr, const IntList& items) const
{
	FriendEntryVec entries;
	std::transform(items.begin(), items.end(), std::back_inserter(entries),
		boost::bind(&FriendListModel::GetFriend, this, _1)); 

	const FriendEntryVec& constEntries = entries;

	std::ostringstream ostream(encodedStr);
	util::default_oarchive archive(ostream);
	archive << constEntries;
	encodedStr = ostream.str();
	return true;
}

//**********************************************************************

void FriendListModel::AddFriend (const FriendEntry& entry)
{
	int index = int(m_friends.size());
	m_friends.push_back(entry);
	Entry pEntry(convert(entry, index));
	this->ApplyToAllViews(boost::bind(&ListView::AddedNewItem, _1, pEntry));
}

//**********************************************************************

void FriendListModel::AddFriends (const FriendEntryVec& entries)
{
	util::for_all(entries, boost::bind(&FriendListModel::AddFriend, this, _1));
}

//**********************************************************************

void FriendListModel::RemoveFriend (const String& username)
{
	FriendEntryVec::iterator iter =
		std::find(m_friends.begin(), m_friends.end(), username);

	if (iter != m_friends.end())
		this->RemoveItem(std::distance(m_friends.begin(), iter));
}

//**********************************************************************

const FriendEntry& FriendListModel::FindFriend (const String& handle) const
{
	static const FriendEntry nullEntry;

	FriendEntryVec::const_iterator iter = std::find(m_friends.begin(), m_friends.end(), handle);
	return iter != m_friends.end() ? *iter : nullEntry;
}

//**********************************************************************

const FriendEntry& FriendListModel::GetFriend (int id) const
{
	return m_friends[id];
}

//**********************************************************************

void FriendListModel::ChangeEndpoint (const String& handle, net::Endpoint endpoint)
{
	FriendEntryVec::iterator iter = std::find(m_friends.begin(), m_friends.end(), handle);
	if (iter != m_friends.end())
		iter->SetEndpoint(endpoint);
}

//**********************************************************************

void FriendListModel::ChangeStatus (const String& handle, FriendEntry::Status status)
{
	FriendEntryVec::iterator iter = std::find(m_friends.begin(), m_friends.end(), handle);
	if (iter != m_friends.end()) {

		int id = (int) std::distance(m_friends.begin(), iter);
		if (status != iter->GetStatus()) {

			iter->SetStatus(status);

			Color color = getColor(*iter);
			StateType state = getState(*iter);
			this->ApplyToAllViews(boost::bind(&ListView::ModifiedColor, _1, id, color));
			this->ApplyToAllViews(boost::bind(&ListView::ModifiedState, _1, id, state));
		}
	}
}

//**********************************************************************

const FriendListModel::Entry FriendListModel::convert (const FriendEntry& entry, int index)
{
	return Entry(
		entry.GetName(),
		entry.GetHandle(),
		entry.GetImage(),
		index,
		getColor(entry),
		getState(entry)
	);
}

//**********************************************************************

Color FriendListModel::getColor (const FriendEntry& entry)
{
	static const Color colors[] = {
		Color(0, 128, 0),
		Color(170, 0, 0),
		Color(255, 140, 0),
		Color(220, 220, 220),
		Color(190, 190, 190),
	};
	assert(entry.GetStatus() < SIZEOF_ARRAY(colors));
	return colors[entry.GetStatus()];
}

//**********************************************************************

FriendListModel::StateType FriendListModel::getState (const FriendEntry& entry)
{
	static const StateType states[] = {
		STATE_ACTIVE,
		STATE_ACTIVE,
		STATE_ACTIVE,
		STATE_INACTIVE,
		STATE_INACTIVE
	};
	assert(entry.GetStatus() < SIZEOF_ARRAY(states));
	return states[entry.GetStatus()];
}

////////////////////////////////////////////////////////////////////////
// FriendListView class
//
FriendListView::FriendListView (wxWindow* parent, const wxSize& size) :
	ide::ListBasicView(parent, size)
{
	this->SetImages(ContactsImageList::GetImageList());
}

////////////////////////////////////////////////////////////////////////
// FriendDropTarget class
//
class FriendDropTarget : public gui::InternalDropTarget {
public:
	FriendDropTarget (FriendListController* controller) :
		InternalDropTarget(_T("FriendEntryVec")), m_controller(controller) {}

	virtual bool DataDropped (wxCoord x, wxCoord y, const std::string& data)
	{
		FriendEntryVec entries;

		std::istringstream istream(data);
		util::default_iarchive archive(istream);
		archive >> entries;

		return m_controller->DropItems(entries);
	}

private:
	FriendListController* m_controller;
};

////////////////////////////////////////////////////////////////////////
// FriendListController class
//
FriendListController::FriendListController (void)
{
}

//**********************************************************************

FriendListController::~FriendListController (void)
{
}

//**********************************************************************

DropTarget*	FriendListController::CreateDropTarget (void)
	{ return new FriendDropTarget(this); }

void FriendListController::DragItems (void)
{
	IntList items;
	this->GetMonitoredView()->GetSelectedItems(items);

	std::string dataStr;
	this->GetMonitoredModel()->GetItems(dataStr, items);

	gui::InternalDataObject data(_T("FriendEntryVec"), dataStr);
	wxDropSource dropSource(this->GetMonitoredView()->GetWindow());
	dropSource.SetData(data);
	wxDragResult result = dropSource.DoDragDrop(wxDrag_DefaultMove);
}

//**********************************************************************

bool FriendListController::DropItems (const FriendEntryVec& entries)
{
	return false;
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
