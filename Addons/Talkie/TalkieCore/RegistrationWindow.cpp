/**
 *	RegistrationWindow.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "RegistrationWindow.h"
#include "ContactsImageList.h"
#include "StringUtils.h"

#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/animate.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/log.h>

namespace talkie {

////////////////////////////////////////////////////////////////////////
// RegistrationWindow class
//
enum ButtonCommands {
	BUTTON_REGISTER,
	BUTTON_CANCEL,
};

//**********************************************************************

BEGIN_EVENT_TABLE(RegistrationWindow, wxPanel)
	EVT_COMBOBOX(wxID_ANY, RegistrationWindow::onImageChanged)
	EVT_TEXT_ENTER(wxID_ANY, RegistrationWindow::onEnter)
	EVT_BUTTON(BUTTON_REGISTER, RegistrationWindow::onRegisterPressed)
	EVT_BUTTON(BUTTON_CANCEL, RegistrationWindow::onCancelPressed)
END_EVENT_TABLE();

//**********************************************************************

RegistrationWindow::RegistrationWindow (wxWindow* parent, const String& anim, const wxSize& size) :
	wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxSTATIC_BORDER)
{
	this->SetBackgroundColour(*wxWHITE);
	wxFont font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

#ifdef NDEBUG
	wxLogNull shutDownWarnings;
#endif

	////////////////////////////////////////////////////////////////////
	// Initialize text entries
	//
	wxStaticText* nameText = new wxStaticText(this, wxID_ANY, _("Full Name:"));
	m_name = new wxTextCtrl(this, wxID_ANY, String(), wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER);

	wxStaticText* handleText = new wxStaticText(this, wxID_ANY, _("Username:"));
	m_handle = new wxTextCtrl(this, wxID_ANY, String(), wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER);

	wxStaticText* passwdText = new wxStaticText(this, wxID_ANY, _("Password:"));
	m_passwd = new wxTextCtrl(this, wxID_ANY, String(), wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER | wxTE_PASSWORD);

	wxStaticText* verifyPasswdText = new wxStaticText(this, wxID_ANY, _("Verify password:"));
	m_verifyPasswd = new wxTextCtrl(this, wxID_ANY, String(), wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER | wxTE_PASSWORD);

	////////////////////////////////////////////////////////////////////
	// Initialize other stuff
	//
	m_errorText = new wxStaticText(this, wxID_ANY, String());
	m_errorText->SetForegroundColour(*wxRED);
	m_errorText->SetFont(font);

	m_imageSelector = new ide::ImageSelectorCtrl(this);
	m_imageSelector->ShowImages(
		ContactsImageList::GetImageList(), ContactsImageList::GetCaptionList()
	);

	m_animation = 0;
	wxAnimation loginAnim;
	if (loginAnim.LoadFile(anim, wxANIMATION_TYPE_ANY))
		m_animation = new wxAnimationCtrl(this, wxID_ANY, loginAnim);

	m_registerButton = new wxButton(this, BUTTON_REGISTER, _("Register"),
		wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

	m_cancelButton = new wxButton(this, BUTTON_CANCEL, _("Cancel"),
		wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

	m_logo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(_T("logo_text")));

	////////////////////////////////////////////////////////////////////
	// Layout stuff
	//
	wxFlexGridSizer* sizer = new wxFlexGridSizer(4, 2, 5, 5);
	sizer->Add(nameText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizer->Add(m_name);
	sizer->Add(handleText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizer->Add(m_handle);
	sizer->Add(passwdText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizer->Add(m_passwd);
	sizer->Add(verifyPasswdText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizer->Add(m_verifyPasswd);

	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->AddSpacer(5);
	buttonSizer->Add(m_registerButton);
	buttonSizer->AddSpacer(5);
	buttonSizer->Add(m_cancelButton);

	wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
	vsizer->AddSpacer(5);
	vsizer->Add(m_logo, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(10);
	vsizer->Add(m_imageSelector, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(10);
	vsizer->Add(sizer, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(5);
	vsizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(5);
	vsizer->Add(m_errorText, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(5);

	if (m_animation) {
		vsizer->Add(m_animation, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(5);
	}
	this->SetSizer(vsizer);
}

//**********************************************************************

RegistrationWindow::~RegistrationWindow (void)
{

}

//**********************************************************************

void RegistrationWindow::SetUserInfo (const FriendEntry& userInfo)
{
	m_name->SetValue(userInfo.GetName());
	m_handle->SetValue(userInfo.GetHandle());
	m_imageSelector->SetSelection(userInfo.GetImage());
}

//**********************************************************************

void RegistrationWindow::SetPasswd (const String& passwd)
{
	m_passwd->SetValue(passwd);
	m_verifyPasswd->SetValue(passwd);
}

//**********************************************************************

void RegistrationWindow::RegisterStart (void)
{
	m_name->Disable();
	m_handle->Disable();
	m_passwd->Disable();
	m_verifyPasswd->Disable();
	m_registerButton->Disable();
	m_cancelButton->Disable();
	m_imageSelector->Disable();

	if (m_animation)
		m_animation->Play();
}

//**********************************************************************

void RegistrationWindow::RegisterStop (void)
{
	m_name->Enable();
	m_handle->Enable();
	m_passwd->Enable();
	m_verifyPasswd->Enable();
	m_registerButton->Enable();
	m_cancelButton->Enable();
	m_imageSelector->Enable();

	if (m_animation)
		m_animation->Stop();
}

//**********************************************************************

void RegistrationWindow::SetErrorMessage (const String& message)
{
	m_errorText->SetLabel(message);
	this->Layout();
}

//**********************************************************************

void RegistrationWindow::onImageChanged (wxCommandEvent& event)
{
	//m_nameImg->SetBitmap(ContactsImageList::GetImage(event.GetSelection()));
}

//**********************************************************************

void RegistrationWindow::onEnter (wxCommandEvent& event)
{
	if (event.GetEventObject() == m_name)
		m_handle->SetFocus();
	else if (event.GetEventObject() == m_handle)
		m_passwd->SetFocus();
	else if (event.GetEventObject() == m_passwd)
		m_verifyPasswd->SetFocus();
	else
		this->onRegisterPressed(event);
}

//**********************************************************************

void RegistrationWindow::onRegisterPressed (wxCommandEvent& event)
{
	const String name = util::strip(m_name->GetValue(), _T(" "));
	const String handle = util::strip(m_handle->GetValue(), _T(" "));
	const String passwd = m_passwd->GetValue();

	if (name.empty()) {
		this->SetErrorMessage(_("Please write your full name"));
		m_name->SetFocus();
	}
	else if (handle.empty()) {
		this->SetErrorMessage(_("Please provide a valid username"));
		m_handle->SetFocus();
	}
	else if (handle.length() < 2) {
		this->SetErrorMessage(_("Your handle must be at least two characters"));
		m_handle->SetFocus();
	}
	else if (passwd.empty()) {
		this->SetErrorMessage(_("Please provide a password"));
		m_passwd->SetFocus();
	}
	else if (passwd != m_verifyPasswd->GetValue()) {
		this->SetErrorMessage(_("Passwords don't match"));
		m_passwd->SetFocus();
	}
	else {
		FriendEntry entry;
		entry.SetName(name);
		entry.SetHandle(handle);
		entry.SetImage(m_imageSelector->GetSelection());

		this->SetErrorMessage(String());
		sigRegister(entry, passwd);
	}
}

//**********************************************************************

void RegistrationWindow::onCancelPressed (wxCommandEvent& event)
{
	sigCancel();
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
