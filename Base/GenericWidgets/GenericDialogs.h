/**
 *	GenericDialogs.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef GUI_GENERICDIALOGS_H
#define GUI_GENERICDIALOGS_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "UndefineAllWin32Shit.h"

#include <wx/dialog.h>

class wxStaticText;
class wxStaticBitmap;
class wxTextCtrl;
class wxButton;
class wxSpinCtrl;

namespace gui {

////////////////////////////////////////////////////////////////////////
// DialogBase
//
class _BASE_API DialogBase : public wxDialog {
public:
	typedef std::pair<String, wxWindowID> ButtonInfo;
	typedef std::list<ButtonInfo> ButtonList;

	DialogBase (
		wxWindow*		parent,
		const String&	title,
		const wxSize&	size = wxDefaultSize
	);
	~DialogBase (void);

	static void SetImage (const String& image) { s_image = image; }
	static void SetFont (const wxFont& font) { s_font = font; }
	static void SetColor (const wxColour& color) { s_color = color; }

protected:
	void RealizeDialog (
		const String&		message,
		const ButtonList&	buttons,
		wxWindow*			contained = 0
	);

private:
	void OnButtonPressed(wxCommandEvent& event);

	static String	s_image;
	static wxFont	s_font;
	static wxColour	s_color;
};

////////////////////////////////////////////////////////////////////////
// MessageDialog
//
class _BASE_API MessageDialog : public DialogBase {
public:
	MessageDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message
	);
};

////////////////////////////////////////////////////////////////////////
// ConfirmationDialog
//
class _BASE_API ConfirmationDialog : public DialogBase {
public:
	ConfirmationDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message,
		bool			hasCancel = false
	);
};

////////////////////////////////////////////////////////////////////////
// InputDialog
//
class _BASE_API InputDialog : public DialogBase {
public:
	InputDialog (
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
// IntegerDialog
//
class _BASE_API IntegerDialog : public DialogBase {
public:
	IntegerDialog (
		wxWindow*		parent,
		const String&	title,
		const String&	message,
		int				min = 0,
		int				max = 100,
		int				value = 0
	);

	void	SetValue (int value);
	void	SetRange (int minVal, int maxVal);
	int		GetValue (void) const;

private:
	wxSpinCtrl* m_input;
};

////////////////////////////////////////////////////////////////////////
// Utility functions
//
_BASE_API void			displayMessage (wxWindow* parent, const String& title, const String& message);

// Yes/No confirmation
_BASE_API bool			getConfirmation (wxWindow* parent, const String& title, const String& message);
// Yes/No/Cancel options
_BASE_API int			getConfirmationOption (wxWindow* parent, const String& title, const String& message);

_BASE_API const String	getInput (wxWindow* parent, const String& title, const String& message);

_BASE_API int getInteger (
				wxWindow*		parent,
				const String&	title,
				const String&	message,
				int				min = 0,
				int				max = 100,
				int				value = 0
	);

////////////////////////////////////////////////////////////////////////

} // namespace gui

#endif // GUI_GENERICDIALOGS_H
