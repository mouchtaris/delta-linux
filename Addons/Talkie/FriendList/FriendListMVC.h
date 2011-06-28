/**
 *	FriendListMVC.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef FRIENDLISTMVC_H
#define FRIENDLISTMVC_H

#include "Common.h"
#include "ListMVC.h"
#include "ListBasicView.h"
#include "FriendEntry.h"
#include "Protect.h"

namespace talkie {

////////////////////////////////////////////////////////////////////////

class FriendListModel : public ide::ListModel {
public:
	FriendListModel (void);
	FriendListModel (const FriendEntryVec& friends);
	~FriendListModel (void);

	virtual void GetAllItems (EntryList& result) const;
	virtual bool GetItem (Entry& result, int id) const;

	virtual void ClearAllItems (void);
	virtual void RemoveItem (int id);

	virtual bool Empty (void) const { return m_friends.empty(); }

	virtual bool GetItems (std::string& encodedStr, const IntList& items) const;

	virtual void	AddFriend (const FriendEntry& entry);
	void			AddFriends (const FriendEntryVec& entries);
	virtual void	RemoveFriend (const String& username);

	FriendEntryVec&			GetFriends (void) { return m_friends; }
	const FriendEntryVec&	GetFriends (void) const { return m_friends; }

	const FriendEntry& FindFriend (const String& handle) const;
	const FriendEntry& GetFriend (int id) const;

	void ChangeEndpoint (const String& handle, net::Endpoint endpoint);
	void ChangeStatus (const String& handle, FriendEntry::Status status);

private:
	typedef ListModel::StateType StateType;

	friend class boost::serialization::access;

	template<class Archive>
	void serialize (Archive& ar, const unsigned int version) { ar & m_friends; }

	static const Entry	convert (const FriendEntry& entry, int index);
	static Color		getColor (const FriendEntry& entry);
	static StateType	getState (const FriendEntry& entry);

	FriendEntryVec m_friends;
};

////////////////////////////////////////////////////////////////////////

class FriendListView : public ide::ListBasicView {
public:
	FriendListView (wxWindow* parent, const wxSize& size = wxDefaultSize);
};

////////////////////////////////////////////////////////////////////////

class FriendListController : public ide::ListController {
public:
	FriendListController (void);
	~FriendListController (void);

	virtual void PopupContextMenu (void) {}

	virtual void AddNewItem (void) {}
	virtual void ModifyItem (int id) {}
	virtual void RemoveItem (void) {}

	virtual void SelectItem (int id, bool select) {}
	virtual void FocusItem (int id) {}
	virtual void ActivateItem (int id) {}

	// DnD support
	//
	virtual DropTarget*	CreateDropTarget (void);
	virtual void		DragItems (void);
	virtual bool		DropItems (const FriendEntryVec& items);

protected:
	 virtual void OnResetMonitoredModel (void) {}

	 virtual void OnSetMonitoredModel (ide::ListModel* model) {}
	 virtual void OnSetMonitoredView (ide::ListView* view) {}
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // FRIENDLISTMVC_H
