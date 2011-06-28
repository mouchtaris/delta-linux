/**
 *	UserStatusWindow.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_USERSTATUSWINDOW_H
#define TALKIE_USERSTATUSWINDOW_H

#include "Common.h"
#include "FriendEntry.h"

#include "UndefineAllWin32Shit.h"

#include <wx/panel.h>
class wxStaticText;
class wxStaticBitmap;
class wxBitmapComboBox;

namespace talkie {

////////////////////////////////////////////////////////////////////////

class UserStatusWindow : public wxPanel {
public:
	UserStatusWindow (wxWindow* parent, const wxSize& size = wxDefaultSize);
	~UserStatusWindow (void);

	void SetEntry (const FriendEntry& entry);

private:
	wxStaticText*		m_name;
	wxStaticBitmap*		m_nameImg;
	wxBitmapComboBox*	m_status;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_USERSTATUSWINDOW_H
