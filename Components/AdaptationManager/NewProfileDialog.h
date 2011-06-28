/**
 *	NewProfileDialog.h
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	NewProfileDialog class is allows the user to
 *	name a new profile that added in the IDE.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#ifndef PROFILE_MANAGER_NEW_PROFILE_DIALOG_H_
#define PROFILE_MANAGER_NEW_PROFILE_DIALOG_H_

#include "Common.h"

#include "AdaptationManager.h"
#include "UndefineAllWin32Shit.h"

#include <wx/dialog.h>

class wxListView;
class wxTextCtrl;
class wxButton;

namespace ide
{
	//----------------------------
	//-- class NewProfileDialog

	class NewProfileDialog :
		public wxDialog
	{
	public:
		///--- type definitions

		///--- constructors / destructor
		NewProfileDialog(wxWindow* parent=0);
		~NewProfileDialog(void);

		///--- public API
		String GetName(void) const	{ return txtName->GetValue(); }

	private:
		///--- private members
		wxTextCtrl*		txtName;
		wxButton*		btnOk;
		wxButton*		btnCancel;
	};
}

#endif	//PROFILE_MANAGER_NEW_PROFILE_DIALOG_H_
