/**
 *	RegistrationWindow.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_REGISTRATIONWINDOW_H
#define TALKIE_REGISTRATIONWINDOW_H

#include "Common.h"
#include "FriendEntry.h"
#include "ImageSelectorCtrl.h"
#include "UndefineAllWin32Shit.h"

#include <boost/signal.hpp>
#include <wx/panel.h>

class wxStaticText;
class wxStaticBitmap;
class wxTextCtrl;
class wxAnimationCtrl;
class wxButton;

namespace talkie {

////////////////////////////////////////////////////////////////////////

class RegistrationWindow : public wxPanel {
public:
	typedef boost::signal<void (const FriendEntry&, const String& passwd)>	Sig1;
	typedef boost::signal<void (void)>										Sig2;

	RegistrationWindow (wxWindow* parent, const String& anim, const wxSize& size = wxDefaultSize);
	~RegistrationWindow (void);

	void SetUserInfo (const FriendEntry& userInfo);
	void SetPasswd (const String& passwd);

	void RegisterStart (void);
	void RegisterStop (void);

	void SetErrorMessage (const String& message);

	Sig1 sigRegister;
	Sig2 sigCancel;

private:
	DECLARE_EVENT_TABLE();

	void onImageChanged (wxCommandEvent& event);
	void onEnter (wxCommandEvent& event);
	void onRegisterPressed (wxCommandEvent& event);
	void onCancelPressed (wxCommandEvent& event);

	wxStaticBitmap*			m_logo;
	wxTextCtrl*				m_name;
	wxTextCtrl*				m_handle;
	wxTextCtrl*				m_passwd;
	wxTextCtrl*				m_verifyPasswd;
	wxButton*				m_registerButton;
	wxButton*				m_cancelButton;

	wxStaticText*			m_errorText;
	ide::ImageSelectorCtrl* m_imageSelector;
	wxAnimationCtrl*		m_animation;
};

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_REGISTRATIONWINDOW_H
