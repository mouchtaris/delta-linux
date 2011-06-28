#ifndef	TEXTUAL_FRAME_H
#define	TEXTUAL_FRAME_H



#include <wx/frame.h>
#include <wx/string.h>
#include <wx/window.h>
#include <wx/textctrl.h>

namespace iviews {

class TextualFrame : public wxFrame {
public:
	TextualFrame(wxWindow * parent, const wxString & text = _(""));

	void AppendText(const wxString & text);

private:
	wxTextCtrl * textctrl;
	DECLARE_EVENT_TABLE()
};

}	//namespace iviews 


#endif	//TEXTUAL_FRAME_H