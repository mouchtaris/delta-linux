/**
 *	LoginWindow.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_LOGINWINDOW_H
#define TALKIE_LOGINWINDOW_H

#include "Common.h"

#include <wx/panel.h>
#include <boost/signal.hpp>

class wxTextCtrl;
class wxStaticBitmap;
class wxStaticText;
class wxAnimationCtrl;
class wxButton;

namespace talkie {

////////////////////////////////////////////////////////////////////////

class LoginWindow : public wxPanel {
public:
	LoginWindow (wxWindow* parent, const String& anim, const wxSize& size = wxDefaultSize);
	~LoginWindow (void);

	void LoginStart (void);
	void LoginStop (void);

	void SetInfoMessage (const String& message);
	void SetErrorMessage (const String& message);

	void			SetUsername (const String& username);
	const String	GetUsername (void) const;
	const String	GetPasswd (void) const;

	//******************************************************************
	// Signals
	//
	boost::signal<void ()> sigLogin;
	boost::signal<void ()> sigRegister;

private:
	DECLARE_EVENT_TABLE();

	void onEnter (wxCommandEvent& event);
	void onLoginPressed (wxCommandEvent& event);
	void onRegisterPressed (wxCommandEvent& event);

	wxStaticBitmap*		m_logo;
	wxTextCtrl*			m_username;
	wxTextCtrl*			m_passwd;
	wxButton*			m_logButton;
	wxButton*			m_regButton;
	wxAnimationCtrl*	m_animation;
	wxStaticText*		m_message;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_LOGINWINDOW_H
