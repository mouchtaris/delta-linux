/**
 *	TextInputDialog.cpp
 *
 *	-- IDE Common Component --
 *
 *	TextInputDialog class allows the user to
 *	write give text input via a modal dialog.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	August 2007
 */
#include "TextInputDialog.h"

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>

namespace ide
{
	//-------------------------------------------------------//
	//---- class TextInputDialog ----------------------------//

	TextInputDialog::TextInputDialog(const String& title, const String& label, const String& defaultValue, wxWindow* parent)
		: wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
		  wxDEFAULT_DIALOG_STYLE)
	{
		//-- Text Input
		wxStaticText* lblName = new wxStaticText(this, wxID_ANY, label);
		txtInput = new wxTextCtrl(this, wxID_ANY, defaultValue);

		//-- Buttons
		wxButton *btnOk = new wxButton(this, wxID_OK, _("Ok"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		btnOk->SetDefault();
		wxButton *btnCancel = new wxButton(this, wxID_CANCEL, _("Cancel"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

		//-- Layout
			//-- text controls
		wxBoxSizer* txtSizer = new wxBoxSizer(wxVERTICAL);
		txtSizer->Add(lblName,
			wxSizerFlags().Border(wxUP | wxDOWN, 2));
		txtSizer->Add(txtInput,
			wxSizerFlags().Proportion(2).Expand().Align(wxALIGN_CENTER_VERTICAL));

			//-- ok / cancel buttons
		wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer->Add(btnOk, 0, wxALIGN_BOTTOM | wxALL, 8);
		buttonSizer->Add(btnCancel, 0, wxALIGN_BOTTOM | wxALL, 8);

			//-- external sizer
		wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
		vsizer->AddSpacer(5);
		vsizer->Add(txtSizer,
			wxSizerFlags(2).Expand().Border(wxLEFT | wxRIGHT, 10));
		vsizer->AddSpacer(5);
		vsizer->Add(buttonSizer,
			wxSizerFlags().Align(wxALIGN_RIGHT).Border(wxLEFT | wxRIGHT, 10));
		vsizer->AddSpacer(5);

		this->SetSizer(vsizer);
		this->Fit();
	}

	//-----------------------------------------------------------------------

	TextInputDialog::~TextInputDialog(void)
	{
	}

	//-----------------------------------------------------------------------
}
