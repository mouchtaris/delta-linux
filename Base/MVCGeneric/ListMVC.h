/**
 *	ListMVC.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_LISTMVC_H
#define IDE_LISTMVC_H

#include "Common.h"
#include "MVC.h"
#include <boost/tuple/tuple.hpp>

#include <wx/colour.h>
typedef wxColour Color;

namespace ide {

////////////////////////////////////////////////////////////////////////

class ListView;
class ListController;

////////////////////////////////////////////////////////////////////////
// ListModel
//
class ListModel : public Model<ListModel, ListView> {
public:
	enum StateType {
		STATE_INACTIVE,
		STATE_ACTIVE,
	};

	typedef boost::tuple<
			String,		// Label
			String,		// Tooltip
			int,		// Image index
			int,		// Id
			Color,		// Guess
			StateType	// State
	> Entry;
	typedef std::list<Entry> EntryList;

	ListModel (void);
	~ListModel (void);

	virtual void GetAllItems (EntryList& result) const = 0;
	virtual bool GetItem (Entry& result, int id) const = 0;

	virtual void ClearAllItems (void) = 0;
	virtual void RemoveItem (int id) = 0;

	virtual bool Empty (void) const = 0;

	virtual bool GetItems (std::string& encodedStr, const IntList& items) const = 0;
};

////////////////////////////////////////////////////////////////////////
// ListView
//
class ListView : public View<ListModel, ListView, ListController> {
public:
	typedef ListModel::Entry		Entry;
	typedef ListModel::EntryList	EntryList;
	typedef ListModel::StateType	StateType;

	ListView (void) {}
	virtual ~ListView (void) {}

	// Query API
	//
	virtual void GetSelectedItems (IntList& result) const = 0;

	// Command API
	//
	virtual void SelectItem (int id, bool select = true) = 0;
	virtual void SelectAllItems (bool select = true) = 0;

	// Update API
	//
	virtual void AddedNewItem (const Entry& entry) = 0;
	virtual void ModifiedItem (int oldId, const Entry& entry) = 0;
	virtual void ModifiedLabel (int id, const String& label) = 0;
	virtual void ModifiedDescription (int id, const String& desc) = 0;
	virtual void ModifiedImageIndex (int id, int imageIndex) = 0;
	virtual void ModifiedId (int oldId, int newId) = 0;
	virtual void ModifiedColor (int id, Color color) = 0;
	virtual void ModifiedState (int id, StateType type) = 0;
	virtual void RemovedItem (int id) = 0;
	virtual void ClearedAllItems (void) = 0;
};

////////////////////////////////////////////////////////////////////////
// ListController
//
class ListController : public Controller<ListModel, ListView, ListController> {
public:
	typedef ListModel::Entry		Entry;
	typedef ListModel::EntryList	EntryList;

	virtual void AddNewItem (void) = 0;
	virtual void ModifyItem (int id) = 0;
	virtual void RemoveItem (void) = 0;

	virtual void SelectItem (int id, bool select = true) = 0;
	virtual void FocusItem (int id) = 0;
	virtual void ActivateItem (int id) = 0;

	virtual void DragItems (void) = 0;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_LISTMVC_H
