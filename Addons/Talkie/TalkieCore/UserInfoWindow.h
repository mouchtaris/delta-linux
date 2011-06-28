/**
 *	UserInfoWindow.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_USERINFOWINDOW_H
#define TALKIE_USERINFOWINDOW_H

#include "Common.h"
#include "FriendEntry.h"

#include "UndefineAllWin32Shit.h"

#include <wx/panel.h>
class wxStaticText;
class wxStaticBitmap;

namespace talkie {

////////////////////////////////////////////////////////////////////////

class UserInfoWindow : public wxPanel {
public:
	UserInfoWindow (wxWindow* parent, const wxSize& size = wxDefaultSize);
	~UserInfoWindow (void);

	void SetEntry (const FriendEntry& entry);

private:
	wxStaticBitmap*		m_image;
	wxStaticText*		m_name;
	wxStaticText*		m_handle;
	wxStaticText*		m_status;
	wxStaticText*		m_endpoint;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_USERINFOWINDOW_H
