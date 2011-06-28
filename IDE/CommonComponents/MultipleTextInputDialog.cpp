/**
 *	MultipleTextInputDialog.cpp
 *
 *	-- IDE Common Component --
 *
 *	MultipleTextInputDialog class allows the user to
 *	give multiple text input via a modal dialog.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	July 2008
 */
#include "MultipleTextInputDialog.h"

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>

#include <boost/foreach.hpp>

namespace ide
{
	//-------------------------------------------------------//
	//---- class MultipleTextInputDialog --------------------//

	MultipleTextInputDialog::MultipleTextInputDialog(const String& title, const StringList& labels, const StringList& defaultValues, wxWindow* parent)
		: wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
		  wxDEFAULT_DIALOG_STYLE)
	{
		//-- Buttons
		wxButton *btnOk = new wxButton(this, wxID_OK, _("Ok"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		btnOk->SetDefault();
		wxButton *btnCancel = new wxButton(this, wxID_CANCEL, _("Cancel"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

		//-- Layout
			//-- text controls
		wxGridSizer* txtSizer = new wxGridSizer((int) labels.size(), 2, 1, 1);
		for(StringList::const_iterator i = labels.begin(), j = defaultValues.begin(); i != labels.end(); ++i)
		{
			bool hasMoreDefaultValues = j != defaultValues.end();

			//-- Text Input Creation
			wxStaticText* lblName = new wxStaticText(this, wxID_ANY, *i);
			wxTextCtrl *txtInput = new wxTextCtrl(this, wxID_ANY, hasMoreDefaultValues ? *j : String());
			if(i == labels.begin())
				txtInput->SetFocus();
			txtInputList.push_back(txtInput);
			txtSizer->Add(lblName);
			txtSizer->Add(txtInput);

			if(hasMoreDefaultValues)
				++j;
		}

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

	MultipleTextInputDialog::~MultipleTextInputDialog(void)
	{
		txtInputList.clear();
	}

	//-----------------------------------------------------------------------

	StringList MultipleTextInputDialog::GetInput(void) const
	{
		StringList ret;
		BOOST_FOREACH(wxTextCtrl *txtInput, txtInputList)
			ret.push_back(txtInput->GetValue());
		return ret;
	}

	//-----------------------------------------------------------------------
}
