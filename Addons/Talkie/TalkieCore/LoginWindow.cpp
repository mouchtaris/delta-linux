/**
 *	LoginWindow.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "LoginWindow.h"
#include "StringUtils.h"

#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/animate.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/log.h>

namespace talkie {

////////////////////////////////////////////////////////////////////////
// LoginWindow class
//
enum ButtonCommands {
	BUTTON_LOGIN,
	BUTTON_REGISTER,
};

//**********************************************************************

BEGIN_EVENT_TABLE(LoginWindow, wxPanel)
	EVT_BUTTON(BUTTON_LOGIN, LoginWindow::onLoginPressed)
	EVT_BUTTON(BUTTON_REGISTER, LoginWindow::onRegisterPressed)
	EVT_TEXT_ENTER(wxID_ANY, LoginWindow::onEnter)
END_EVENT_TABLE();

////////////////////////////////////////////////////////////////////////

LoginWindow::LoginWindow (wxWindow* parent, const String& anim, const wxSize& size) :
	wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxSTATIC_BORDER)
{
	this->SetBackgroundColour(*wxWHITE);
	wxFont font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

#ifdef NDEBUG
	wxLogNull shutDownWarnings;
#endif

	////////////////////////////////////////////////////////////////////
	// Initialize stuff
	//
	m_logo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(_T("logo_text")));

	wxStaticText* usernameText = new wxStaticText(this, wxID_ANY, _("Username:"));
	m_username = new wxTextCtrl(this, wxID_ANY, String(), wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER);

	wxStaticText* passwdText = new wxStaticText(this, wxID_ANY, _("Password:"));
	m_passwd = new wxTextCtrl(this, wxID_ANY, String(), wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER | wxTE_PASSWORD);

	m_logButton = new wxButton(this, BUTTON_LOGIN, _("Login"), wxDefaultPosition, wxDefaultSize,
		wxNO_BORDER);
	m_regButton = new wxButton(this, BUTTON_REGISTER, _("Register"), wxDefaultPosition,
		wxDefaultSize, wxNO_BORDER);

	m_message = new wxStaticText(this, wxID_ANY, String());
	m_message->SetFont(font);

	m_animation = 0;
	wxAnimation loginAnim;
	if (loginAnim.LoadFile(anim, wxANIMATION_TYPE_ANY))
		m_animation = new wxAnimationCtrl(this, wxID_ANY, loginAnim);

	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->AddSpacer(5);
	buttonSizer->Add(m_logButton, 0);
	buttonSizer->AddSpacer(5);
	buttonSizer->Add(m_regButton, 0);

	wxFlexGridSizer* infoSizer = new wxFlexGridSizer(2, 2, 5, 5);
	infoSizer->Add(usernameText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	infoSizer->Add(m_username, 1, wxEXPAND);
	infoSizer->Add(passwdText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	infoSizer->Add(m_passwd, 1, wxEXPAND);

	wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
	vsizer->AddSpacer(5);
	vsizer->Add(m_logo, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(10);
	vsizer->Add(infoSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(5);
	vsizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(15);
	vsizer->Add(m_message, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(10);

	if (m_animation) {
		vsizer->Add(m_animation, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(5);
	}
	this->SetSizer(vsizer);
}

//**********************************************************************

LoginWindow::~LoginWindow (void)
{

}

//**********************************************************************

void LoginWindow::LoginStart (void)
{
	m_username->Disable();
	m_passwd->Disable();
	m_logButton->Disable();
	m_regButton->Disable();

	if (m_animation)
		m_animation->Play();
}

//**********************************************************************

void LoginWindow::LoginStop (void)
{
	m_username->Enable();
	m_passwd->Enable();
	m_logButton->Enable();
	m_regButton->Enable();

	if (m_animation)
		m_animation->Stop();
}

//**********************************************************************

void LoginWindow::SetInfoMessage (const String& message)
{
	m_message->SetForegroundColour(*wxGREEN);
	m_message->SetLabel(message);
	this->Layout();
}

//**********************************************************************

void LoginWindow::SetErrorMessage (const String& message)
{
	m_message->SetForegroundColour(*wxRED);
	m_message->SetLabel(message);
	this->Layout();
}

//**********************************************************************

void LoginWindow::SetUsername (const String& username)
{
	m_username->SetValue(username);
	m_passwd->SetFocus();
}

//**********************************************************************

const String LoginWindow::GetUsername (void) const
	{ return util::strip(m_username->GetValue(), _T(" ")); }

const String LoginWindow::GetPasswd (void) const
	{ return util::strip(m_passwd->GetValue(), _T(" ")); }

//**********************************************************************

void LoginWindow::onEnter (wxCommandEvent& event)
{
	if (event.GetEventObject() == m_username)
		m_passwd->SetFocus();
	else
		this->onLoginPressed(event);
}

//**********************************************************************

void LoginWindow::onRegisterPressed (wxCommandEvent& event)
{
	sigRegister();
}

//**********************************************************************

void LoginWindow::onLoginPressed (wxCommandEvent& event)
{
	if (this->GetUsername().empty()) {
		this->SetErrorMessage(_("Please type your username"));
		m_username->SetFocus();
	}
	else if (this->GetPasswd().empty()) {
		this->SetErrorMessage(_("Please type your password"));
		m_passwd->SetFocus();
	}
	else {

		this->SetErrorMessage(String());
		sigLogin();
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
