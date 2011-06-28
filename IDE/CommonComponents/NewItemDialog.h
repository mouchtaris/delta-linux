/**
 *	NewItemDialog.h
 *
 *	-- IDE Common Component --
 *
 *	NewItemDialog class is component selection dialog
 *	to insert in a tree-view. Listed items are extracted
 *	from the basic API that a TreeItemComponent provides
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef COMMON_COMPONENTS_NEW_ITEM_DIALOG_H_
#define COMMON_COMPONENTS_NEW_ITEM_DIALOG_H_

#include "Common.h"
#include "IDECommon.h"
#include "UndefineAllWin32Shit.h"

#include <wx/dialog.h>
#include <wx/textctrl.h>

class wxListView;
class wxTextCtrl;
class wxButton;

namespace ide
{
	//----------------------------
	//-- class NewItemDialog

	class _IDE_API NewItemDialog :
		public wxDialog
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		NewItemDialog(wxWindow* parent, const StdStringList& items, const String& path);
		~NewItemDialog(void);

		///--- public API
		bool IsOk(void)	{ return isOk; }

		String GetItemType(void) const;
		String GetItemName(void) const	{ return txtName->GetValue(); }
		String GetItemPath(void) const	{ return txtLocation->GetValue(); }
		String GetItemURI(void) const	{ return txtLocation->GetValue() + txtName->GetValue(); }

	private:
		///--- event table
		DECLARE_EVENT_TABLE();

		///--- private API
		long GetSelectedItem(void) const;

		void onOk(wxCommandEvent& event);
		void onCancel(wxCommandEvent& event);
		void onBrowse(wxCommandEvent& event);

		///--- private members
		StdStringList items;
		bool isOk;

		wxListView*		lstItems;
		wxTextCtrl*		txtName;
		wxTextCtrl*		txtLocation;
		wxButton*		btnBrowse;
		wxButton*		btnOk;
		wxButton*		btnCancel;
	};
}

#endif	//COMMON_COMPONENTS_NEW_ITEM_DIALOG_H_
