/**
 *	UserInfoWindow.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "UserInfoWindow.h"
#include "ContactsImageList.h"

#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/sizer.h>

namespace talkie {

////////////////////////////////////////////////////////////////////////
// UserInfoWindow class
//
UserInfoWindow::UserInfoWindow (wxWindow* parent, const wxSize& size) :
	wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
{
	this->SetBackgroundColour(*wxWHITE);
	wxFont font(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

	m_image = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);
	m_name = new wxStaticText(this, wxID_ANY, String());
	m_name->SetFont(font);

	m_handle = new wxStaticText(this, wxID_ANY, String());
	m_status = new wxStaticText(this, wxID_ANY, String());
	m_endpoint = new wxStaticText(this, wxID_ANY, String());

	font.SetPointSize(8);

	wxStaticText* handleLabel = new wxStaticText(this, wxID_ANY, _("Username:"));
	handleLabel->SetFont(font);

	wxStaticText* statusLabel = new wxStaticText(this, wxID_ANY, _("Status:"));
	statusLabel->SetFont(font);

	wxStaticText* endpointLabel = new wxStaticText(this, wxID_ANY, _("Endpoint:"));
	endpointLabel->SetFont(font);

	wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
	nameSizer->AddSpacer(25);
	nameSizer->Add(m_image, 0, wxALIGN_CENTER_VERTICAL);
	nameSizer->AddSpacer(5);
	nameSizer->Add(m_name, 0, wxALIGN_CENTER_VERTICAL);
	nameSizer->AddSpacer(25);

	wxFlexGridSizer* infoSizer = new wxFlexGridSizer(3, 2, 5, 10);
	infoSizer->Add(handleLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	infoSizer->Add(m_handle, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	infoSizer->Add(statusLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	infoSizer->Add(m_status, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	infoSizer->Add(endpointLabel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	infoSizer->Add(m_endpoint, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);

	wxBoxSizer* spaceSizer = new wxBoxSizer(wxHORIZONTAL);
	spaceSizer->AddSpacer(25);
	spaceSizer->Add(infoSizer);
	spaceSizer->AddSpacer(25);

	wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
	vsizer->AddSpacer(5);
	vsizer->Add(nameSizer, 0, wxALIGN_CENTER);
	vsizer->AddSpacer(5);
	vsizer->Add(spaceSizer, 0, wxALIGN_CENTER);
	vsizer->AddSpacer(10);

	this->SetSizer(vsizer);
}

//**********************************************************************

UserInfoWindow::~UserInfoWindow (void)
{

}

//**********************************************************************

void UserInfoWindow::SetEntry (const FriendEntry& entry)
{
	m_image->SetBitmap(ContactsImageList::GetImage(entry.GetImage()));
	m_name->SetLabel(entry.GetName());
	m_handle->SetLabel(entry.GetHandle());

	String value;

	switch (entry.GetStatus()) {
		case FriendEntry::ONLINE:
			value = _("User is online");
			break;
		case FriendEntry::BUSY:
			value = _("User is busy");
			break;
		case FriendEntry::IDLE:
			value = _("User is idle ...");
			break;
		case FriendEntry::OFFLINE:
			value = _("User is offline");
			break;
		case FriendEntry::UNDEFINED:
			value = _("Cannot determine user's state");
			break;
		default:
			assert(false);
	}
	m_status->SetLabel(value);

	value = entry.GetEndpoint().GetHost().empty() ? _("Unknown for now") :
		entry.GetEndpoint().GetHost() + _T(" : ") + entry.GetEndpoint().GetPort();
	m_endpoint->SetLabel(value);
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
