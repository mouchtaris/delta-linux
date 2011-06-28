#include "TextualFrame.h"

namespace iviews {


BEGIN_EVENT_TABLE(TextualFrame, wxFrame)
END_EVENT_TABLE()




TextualFrame::TextualFrame(wxWindow * parent, const wxString & text)
	       : wxFrame(parent, wxID_ANY, _T("i-views (Slot Information)"))
{
    // create the log text window
    textctrl = new wxTextCtrl(	this, 
								wxID_ANY, 
								wxEmptyString,
                                wxDefaultPosition, 
								wxDefaultSize,
                                wxTE_MULTILINE
							);
    textctrl->SetEditable(false);
	textctrl->AppendText(text);
}

void TextualFrame::AppendText(const wxString & text) 
	{	textctrl->AppendText(text);	}


}	//namespace iviews