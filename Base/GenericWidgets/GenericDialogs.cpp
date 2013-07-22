/**
 *	GenericDialogs.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "GenericDialogs.h"
#include "StringUtils.h"

#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/animate.h>
#include <wx/button.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/log.h>

namespace gui {

////////////////////////////////////////////////////////////////////////
// DialogBase class
//
DialogBase::DialogBase (
		wxWindow*		parent,
		const String&	title,
		const wxSize&	size
	) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, size,
			wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	this->SetBackgroundColour(s_color);
}

//**********************************************************************

DialogBase::~DialogBase (void)
{

}

//**********************************************************************

void DialogBase::RealizeDialog (
		const String&		message,
		const ButtonList&	buttons,
		wxWindow*			contained
	)
{
#ifdef NDEBUG
	wxLogNull shutDownWarnings;
#endif

	////////////////////////////////////////////////////////////////////
	// Initialize dialog
	//
	wxStaticBitmap* logo = s_image.empty() ? 0 : new wxStaticBitmap(this, wxID_ANY, wxBitmap(s_image));

	wxStaticText* messageText = 0;
	if (!message.empty()) {
		messageText = new wxStaticText(this, wxID_ANY, message);
		messageText->SetFont(s_font);
		messageText->Wrap(350);
	}

	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->AddSpacer(25);

	assert(!buttons.empty());
	for(ButtonList::const_iterator i = buttons.begin(); i != buttons.end(); ++i) {
		wxButton* button = new wxButton(this, i->second, i->first,
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		if (i == buttons.begin())
			button->SetDefault();
		else
			buttonSizer->AddSpacer(5);
		Connect(i->second, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DialogBase::OnButtonPressed));

		buttonSizer->Add(button);
	}

	buttonSizer->AddSpacer(25);

	////////////////////////////////////////////////////////////////////
	// Layout remaining stuff
	//
	wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
	vsizer->AddSpacer(5);

	if (logo) {
		vsizer->Add(logo, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(10);
	}

	if (messageText) {
		wxBoxSizer* messageSizer = new wxBoxSizer(wxHORIZONTAL);
		messageSizer->AddSpacer(10);
		messageSizer->Add(messageText);
		messageSizer->AddSpacer(10);
		vsizer->Add(messageSizer, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(10);
	}
	else
		vsizer->AddSpacer(5);

	if (contained) {
		vsizer->Add(contained, 1, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxEXPAND, 10);
		vsizer->AddSpacer(10);
	}
	vsizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	vsizer->AddSpacer(10);

	vsizer->Fit(this);
	this->SetSizer(vsizer);
	this->Centre();
}

//**********************************************************************

void DialogBase::OnButtonPressed(wxCommandEvent& event)
{
	EndModal(event.GetId());
}

//**********************************************************************

String		DialogBase::s_image;
wxFont		DialogBase::s_font(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
wxColour	DialogBase::s_color(*wxWHITE);

////////////////////////////////////////////////////////////////////////
// MessageDialog class
//
MessageDialog::MessageDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message
	) : DialogBase(parent, title)
{
	this->RealizeDialog(message, ButtonList(1, ButtonInfo(_("Ok"), wxID_OK)));
}

////////////////////////////////////////////////////////////////////////
// ConfirmationDialog class
//
ConfirmationDialog::ConfirmationDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message,
		bool			hasCancel
	) : DialogBase(parent, title)
{
	ButtonList buttons;
	buttons.push_back(ButtonInfo(_("Yes"), wxID_YES));
	buttons.push_back(ButtonInfo(_("No"), wxID_NO));
	if (hasCancel)
		buttons.push_back(ButtonInfo(_("Cancel"), wxID_CANCEL));
	this->RealizeDialog(message, buttons);
}

////////////////////////////////////////////////////////////////////////
// InputDialog class
//
InputDialog::InputDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message
	) : DialogBase(parent, title)
{
	ButtonList buttons;
	buttons.push_back(ButtonInfo(_("Ok"), wxID_OK));
	buttons.push_back(ButtonInfo(_("Cancel"), wxID_CANCEL));
	m_input = new wxTextCtrl(this, wxID_ANY);
	this->RealizeDialog(message, buttons, m_input);
}

//**********************************************************************

void InputDialog::SetInput (const String& input) { m_input->SetValue(input); }
void InputDialog::ClearInput (void) { m_input->Clear(); }
const String InputDialog::GetInput (void) const { return m_input->GetValue(); }

////////////////////////////////////////////////////////////////////////
// IntegerDialog class
//
IntegerDialog::IntegerDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message,
		int				min,
		int				max,
		int				value
	) : DialogBase(parent, title)
{
	ButtonList buttons;
	buttons.push_back(ButtonInfo(_("Ok"), wxID_OK));
	buttons.push_back(ButtonInfo(_("Cancel"), wxID_CANCEL));
	m_input = new wxSpinCtrl(this, wxID_ANY, String(), wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS, min, max, value);
	m_input->SetSelection(-1, -1);
	this->RealizeDialog(message, buttons, m_input);
}

//**********************************************************************

void IntegerDialog::SetValue (int value)
{
	m_input->SetValue(value);
	m_input->SetSelection(-1, -1);
}

void IntegerDialog::SetRange (int minVal, int maxVal) { m_input->SetRange(minVal, maxVal); }
int	IntegerDialog::GetValue (void) const { return m_input->GetValue(); }


////////////////////////////////////////////////////////////////////////
// Utility functions
//
void displayMessage (wxWindow* parent, const String& title, const String& message)
{
	MessageDialog dialog(parent, title, message);
	dialog.ShowModal();	
}

//**********************************************************************

bool getConfirmation (wxWindow* parent, const String& title, const String& message)
{
	ConfirmationDialog dialog(parent, title, message);
	return dialog.ShowModal() == wxID_YES;
}

//**********************************************************************

int getConfirmationOption (wxWindow* parent, const String& title, const String& message)
{
	ConfirmationDialog dialog(parent, title, message, true);
	return dialog.ShowModal();
}

//**********************************************************************

const String getInput (wxWindow* parent, const String& title, const String& message)
{
	InputDialog dialog(parent, title, message);
	return dialog.ShowModal() == wxID_OK ? dialog.GetInput() : String();
}

//**********************************************************************

int	getInteger (
		wxWindow*		parent,
		const String&	title,
		const String&	message,
		int				min,
		int				max,
		int				value
	)
{
	IntegerDialog dialog(parent, title, message, min, max, value);
	return dialog.ShowModal() == wxID_OK ? dialog.GetValue() : max + 1;
}


////////////////////////////////////////////////////////////////////////

} // namespace gui
