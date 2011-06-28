/**
 *	ListBasicView.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef LISTBASICVIEW_H
#define LISTBASICVIEW_H

#include "Common.h"
#include "ListMVC.h"
#include "ItemListCtrl.h"
class wxImageList;

namespace ide {

////////////////////////////////////////////////////////////////////////

class ListBasicView : public ItemListCtrl, public ListView {
public:
	ListBasicView (void);
	ListBasicView (wxWindow* parent, const wxSize& size = wxDefaultSize);
	~ListBasicView (void);

	bool Create (wxWindow* parent, const wxSize& size = wxDefaultSize);

	virtual Window* GetWindow (void) { return this; }
	virtual void Update (void);

	// Query API
	//
	virtual void GetSelectedItems (IntList& result) const;
	virtual void SelectItem (int id, bool select);
	virtual void SelectAllItems (bool select);

	// Update API
	//
	virtual void AddedNewItem (const Entry& entry);
	virtual void ModifiedItem (int id, const Entry& entry);
	virtual void ModifiedLabel (int id, const String& label);
	virtual void ModifiedDescription (int id, const String& desc);
	virtual void ModifiedImageIndex (int id, int imageIndex);
	virtual void ModifiedId (int oldId, int newId);
	virtual void ModifiedColor (int id, Color color);
	virtual void ModifiedState (int id, StateType type);

	virtual void RemovedItem (int id);
	virtual void ClearedAllItems (void);

	// Appearance
	//
	void SetImages (wxImageList* imageList, bool own = false);
	void SetImages (const StringList& images);

protected:
	 virtual void OnResetModel (void);

	 virtual void OnSetModel (ListModel* model);
	 virtual void OnSetController (ListController* controller, bool own);

private:
	DECLARE_EVENT_TABLE();

	void onItemSelected (wxListEvent& event);
	void onItemDeselected (wxListEvent& event);
	void onItemActivated (wxListEvent& event);
	void onItemFocused (wxListEvent& event);
	void onItemDragged (wxListEvent& event);
	void onItemEditStart (wxListEvent& event);
	void onItemEditEnd (wxListEvent& event);

	void onContextMenu (wxContextMenuEvent& event);

	long		findItem (int id);
	wxListItem	convert (const Entry& entry);
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // LISTBASICVIEW_H
