/**
 *	TalkieDialogs.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef TALKIE_TALKIEDIALOGS_H
#define TALKIE_TALKIEDIALOGS_H

#include "Common.h"
#include "UndefineAllWin32Shit.h"

#include <wx/dialog.h>

class wxStaticText;
class wxStaticBitmap;
class wxTextCtrl;
class wxButton;

namespace talkie {

////////////////////////////////////////////////////////////////////////
// TalkieDialogBase
//
class TalkieDialogBase : public wxDialog {
public:
	TalkieDialogBase (
		wxWindow*		parent,
		const String&	title,
		const wxSize&	size = wxDefaultSize
	);
	~TalkieDialogBase (void);

protected:
	void RealizeDialog (	
		const String&	message,
		const String&	okButtonLabel,
		const String&	cancelButtonLabel = String(),
		wxWindow*		contained = 0
	);
};

////////////////////////////////////////////////////////////////////////
// TalkieMessageDialog
//
class TalkieMessageDialog : public TalkieDialogBase {
public:
	TalkieMessageDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message
	);
};

////////////////////////////////////////////////////////////////////////
// TalkieConfirmationDialog
//
class TalkieConfirmationDialog : public TalkieDialogBase {
public:
	TalkieConfirmationDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message
	);
};

////////////////////////////////////////////////////////////////////////
// TalkieInputDialog
//
class TalkieInputDialog : public TalkieDialogBase {
public:
	TalkieInputDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message
	);

	void			SetInput (const String& input);
	void			ClearInput (void);
	const String	GetInput (void) const;

private:
	wxTextCtrl* m_input;
};

////////////////////////////////////////////////////////////////////////
// TalkieUserInfoDialog
//
class FriendEntry;
class UserInfoWindow;

class TalkieUserInfoDialog : public TalkieDialogBase {
public:
	TalkieUserInfoDialog (
		wxWindow*			parent,
		const String&		title,
		const FriendEntry&	entry
	);

	void SetFriendEntry (const FriendEntry& entry);

private:
	UserInfoWindow* m_userInfoWin;
};

////////////////////////////////////////////////////////////////////////
// Utility functions
//
void			displayMessage (wxWindow* parent, const String& title, const String& message);
bool			getConfirmation (wxWindow* parent, const String& title, const String& message);
const String	getInput (wxWindow* parent, const String& title, const String& message);
void			displayUserInfo (wxWindow* parent, const String& title, const FriendEntry& entry);

////////////////////////////////////////////////////////////////////////

} // namespace talkie

#endif // TALKIE_TALKIEDIALOGS_H
