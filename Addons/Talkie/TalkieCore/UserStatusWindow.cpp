/**
 *	UserStatusWindow.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "UserStatusWindow.h"
#include "ContactsImageList.h"

#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/bmpcbox.h>
#include <wx/sizer.h>

namespace talkie {

////////////////////////////////////////////////////////////////////////
// UserStatusWindow class
//
UserStatusWindow::UserStatusWindow (wxWindow* parent, const wxSize& size) :
	wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxSTATIC_BORDER)
{
	wxFont font(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	m_name = new wxStaticText(this, wxID_ANY, String());
	m_name->SetFont(font);
	m_nameImg = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);

	wxStaticText* statusText = new wxStaticText(this, wxID_ANY, _("Status:"));
	font.SetPointSize(8);
	statusText->SetFont(font);

	m_status = new wxBitmapComboBox(this, wxID_ANY, String(), wxDefaultPosition,
		wxDefaultSize, 0, 0, wxCB_READONLY);
	m_status->EnablePopupAnimation(true);
	m_status->Append(_("Online"), wxBitmap(_T("ind_online")));
	m_status->Append(_("Busy"), wxBitmap(_T("ind_busy")));
	m_status->Append(_("Idle"), wxBitmap(_T("ind_idle")));
	m_status->Append(_("Offline"), wxBitmap(_T("ind_disc")));

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->AddSpacer(5);
	sizer->Add(m_nameImg, 0, wxALIGN_CENTER_VERTICAL);
	sizer->AddSpacer(5);
	sizer->Add(m_name, 0, wxALIGN_CENTER_VERTICAL);

	wxBoxSizer* statusSizer = new wxBoxSizer(wxHORIZONTAL);
	statusSizer->AddSpacer(5);
	statusSizer->Add(statusText, 0, wxALIGN_CENTER_VERTICAL);
	statusSizer->AddSpacer(5);
	statusSizer->Add(m_status, 1, wxEXPAND);
	statusSizer->AddSpacer(5);

	wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
	vsizer->AddSpacer(5);
	vsizer->Add(sizer, 0, wxEXPAND);
	vsizer->AddSpacer(5);
	vsizer->Add(statusSizer, 0, wxEXPAND);
	vsizer->AddSpacer(5);

	this->SetSizer(vsizer);
}

//**********************************************************************

UserStatusWindow::~UserStatusWindow (void)
{

}

//**********************************************************************

void UserStatusWindow::SetEntry (const FriendEntry& entry)
{
	m_name->SetLabel(entry.GetName());
	m_nameImg->SetBitmap(ContactsImageList::GetImage(entry.GetImage()));
	m_status->Select(FriendEntry::ONLINE);
}

////////////////////////////////////////////////////////////////////////

} // namespace talkie
