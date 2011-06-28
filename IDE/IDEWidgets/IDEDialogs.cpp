/**
 *	IDEDialogs.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "IDEDialogs.h"

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

HostPortDialog::HostPortDialog(
		wxWindow*		parent,
		const String&	title,
		const String&	hostValue,
		int				portValue
	) : gui::DialogBase(parent, title)
{
	wxPanel* panel = new wxPanel(this);

	host = new wxTextCtrl(panel, wxID_ANY, hostValue, wxDefaultPosition, wxDefaultSize);
	port = new wxSpinCtrl(panel, wxID_ANY, String(), wxDefaultPosition, wxDefaultSize,
		wxSP_ARROW_KEYS, -1, 65535, portValue);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(2, 2, 5, 5);
	sizer->SetFlexibleDirection(wxHORIZONTAL);
	sizer->Add(new wxStaticText(panel, wxID_ANY, _("Host:")));
	sizer->Add(host);
	sizer->Add(new wxStaticText(panel, wxID_ANY, _("Port:")));
	sizer->Add(port);

	panel->SetSizer(sizer);

	ButtonList buttons;
	buttons.push_back(ButtonInfo(_("Ok"), wxID_OK));
	buttons.push_back(ButtonInfo(_("Cancel"), wxID_CANCEL));
	this->RealizeDialog(String(), buttons, panel);
}

//**********************************************************************

const String HostPortDialog::GetHost (void) const
{
	const String hostStr = host->GetValue();
	return (hostStr.empty()) ? _T("localhost") : hostStr;
}

//**********************************************************************

int	HostPortDialog::GetPort (void) const
{
	return port->GetValue();
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
