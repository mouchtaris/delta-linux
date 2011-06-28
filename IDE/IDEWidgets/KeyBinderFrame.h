/**
 *	KeyBinderFrame.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_KEYBINDERFRAME_H
#define IDE_KEYBINDERFRAME_H

#include "Common.h"
#include "IDECommon.h"

#include <wx/frame.h>
#include "keybinder.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

#define IDE_KEYBINDERFRAME_FLAGS \
	wxDEFAULT_FRAME_STYLE

////////////////////////////////////////////////////////////////////////

class _IDE_API KeyBinderFrame : public wxFrame {
public:
	KeyBinderFrame (void) {}
	KeyBinderFrame (
		wxWindow*		parent,
		const String&	title,
		long			style = IDE_KEYBINDERFRAME_FLAGS,
		const wxSize&	size = wxDefaultSize
	);
	~KeyBinderFrame (void);

	bool Create (
		wxWindow*		parent,
		const String&	title,
		long			style = IDE_KEYBINDERFRAME_FLAGS,
		const wxSize&	size = wxDefaultSize
	);

	//******************************************************************
	// wxFrame API
	//
	void SetMenuBar (wxMenuBar* menuBar);

protected:
	wxADD_KEYBINDER_SUPPORT();

	//******************************************************************
	// Key binding API
	//
	void KBAttachWindow (wxWindow* window);
	void KBDetachWindow (wxWindow* window);

	void KBConfigureKeys (void);

private:
	void registerMenuBar (wxMenuBar* menuBar);

	wxKeyProfileArray m_profiles;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_KEYBINDERFRAME_H
