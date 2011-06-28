/**
 *	TalkieDialogs.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "TalkieDialogs.h"
#include "ContactsImageList.h"
#include "StringUtils.h"
#include "UserInfoWindow.h"

#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/animate.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/log.h>

namespace talkie {

////////////////////////////////////////////////////////////////////////
// TalkieDialogBase class
//
TalkieDialogBase::TalkieDialogBase (
		wxWindow*		parent,
		const String&	title,
		const wxSize&	size
	) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE)
{
	this->SetBackgroundColour(*wxWHITE);
}

//**********************************************************************

TalkieDialogBase::~TalkieDialogBase (void)
{

}

//**********************************************************************

void TalkieDialogBase::RealizeDialog (
		const String&	message,
		const String&	okButtonLabel,
		const String&	cancelButtonLabel,
		wxWindow*		contained
	)
{
	wxFont font(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

#ifdef NDEBUG
	wxLogNull shutDownWarnings;
#endif

	////////////////////////////////////////////////////////////////////
	// Initialize dialog
	//
	wxStaticBitmap* logo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(_T("logo_text")));

	wxStaticText* messageText = new wxStaticText(this, wxID_ANY, message);
	messageText->SetFont(font);
	messageText->Wrap(350);

	wxButton* okButton = new wxButton(this, wxID_OK, okButtonLabel,
		wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

	wxButton* cancelButton = 0;
	if (!cancelButtonLabel.empty()) {
		cancelButton = new wxButton(this, wxID_CANCEL, cancelButtonLabel,
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	}

	////////////////////////////////////////////////////////////////////
	// Layout stuff
	//
	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->AddSpacer(25);
	buttonSizer->Add(okButton);
	if (cancelButton) {
		buttonSizer->AddSpacer(5);
		buttonSizer->Add(cancelButton);
	}
	buttonSizer->AddSpacer(25);

	wxBoxSizer* messageSizer = new wxBoxSizer(wxHORIZONTAL);
	messageSizer->AddSpacer(10);
	messageSizer->Add(messageText);
	messageSizer->AddSpacer(10);

	wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
	vsizer->AddSpacer(5);
	vsizer->Add(logo, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(10);
	vsizer->Add(messageSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(10);

	if (contained) {
		vsizer->Add(contained, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(10);
	}
	vsizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(10);

	vsizer->Fit(this);
	this->SetSizer(vsizer);
	this->Centre();
}

////////////////////////////////////////////////////////////////////////
// TalkieMessageDialog class
//
TalkieMessageDialog::TalkieMessageDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message
	) : TalkieDialogBase(parent, title)
{
	this->RealizeDialog(message, _("Ok"));
}

////////////////////////////////////////////////////////////////////////
// TalkieConfirmationDialog class
//
TalkieConfirmationDialog::TalkieConfirmationDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message
	) : TalkieDialogBase(parent, title)
{
	this->RealizeDialog(message, _("Yes"), _("No"));
}

////////////////////////////////////////////////////////////////////////
// TalkieInputDialog class
//
TalkieInputDialog::TalkieInputDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message
	) : TalkieDialogBase(parent, title)
{
	m_input = new wxTextCtrl(this, wxID_ANY, String(), wxDefaultPosition, wxDefaultSize);
	this->RealizeDialog(message, _("Ok"), _("Cancel"), m_input);
}

//**********************************************************************

void TalkieInputDialog::SetInput (const String& input) { m_input->SetValue(input); }
void TalkieInputDialog::ClearInput (void) { m_input->Clear(); }
const String TalkieInputDialog::GetInput (void) const { return m_input->GetValue(); }

////////////////////////////////////////////////////////////////////////
// TalkieUserInfoDialog
//
TalkieUserInfoDialog::TalkieUserInfoDialog (
		wxWindow*			parent,
		const String&		title,
		const FriendEntry&	entry
	) : TalkieDialogBase(parent, title)
{
	m_userInfoWin = new UserInfoWindow(this);
	m_userInfoWin->SetEntry(entry);
	this->RealizeDialog(_("Recognize your main man:"), _("Ok"), String(), m_userInfoWin);
}

//**********************************************************************

void TalkieUserInfoDialog::SetFriendEntry (const FriendEntry& entry)
{
	m_userInfoWin->SetEntry(entry);
}

////////////////////////////////////////////////////////////////////////
// Utility functions
//
void displayMessage (wxWindow* parent, const String& title, const String& message)
{
	TalkieMessageDialog dialog(parent, title, message);
	dialog.ShowModal();	
}

//**********************************************************************

bool getConfirmation (wxWindow* parent, const String& title, const String& message)
{
	TalkieConfirmationDialog dialog(parent, title, message);
	return dialog.ShowModal() == wxID_OK;
}

//**********************************************************************

const String getInput (wxWindow* parent, const String& title, const String& message)
{
	TalkieInputDialog dialog(parent, title, message);
	return dialog.ShowModal() == wxID_OK ? dialog.GetInput() : String();
}

//**********************************************************************

void displayUserInfo (wxWindow* parent, const String& title, const FriendEntry& entry)
{
	TalkieUserInfoDialog dialog(parent, title, entry);
	dialog.ShowModal();	
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
