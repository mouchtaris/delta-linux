/**
 *	NewProfileDialog.cpp
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	NewProfileDialog class is allows the user to
 *	name a new profile that added in the IDE.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	July 2007
 */
#include "NewProfileDialog.h"

#include "ImageList.h"
#include "GenericDialogs.h"
#include "IDEDialogs.h"

#include "BitmapRegistry.h"
#include "ComponentRegistry.h"
#include "Call.h"

#include "Algorithms.h"

#include <wx/stattext.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dirdlg.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <exception>

namespace ide
{
	//-------------------------------------------------------//
	//---- class NewProfileDialog ------------------------------//

	NewProfileDialog::NewProfileDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, _("New Profile"), wxDefaultPosition, wxDefaultSize,
		  wxDEFAULT_DIALOG_STYLE)
	{
		//-- Text Input
		wxStaticText* lblName = new wxStaticText(this, wxID_ANY, _("Profile Name:"));
		txtName = new wxTextCtrl(this, wxID_ANY);

		//-- Buttons
		btnOk = new wxButton(this, wxID_OK, _("Ok"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		btnOk->SetDefault();
		btnCancel = new wxButton(this, wxID_CANCEL, _("Cancel"),
			wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

		//-- Layout
			//-- text controls
		wxBoxSizer* txtSizer = new wxBoxSizer(wxVERTICAL);
		txtSizer->Add(lblName,
			wxSizerFlags().Border(wxUP | wxDOWN, 2));
		txtSizer->Add(txtName,
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

	NewProfileDialog::~NewProfileDialog(void)
	{
	}

	//-----------------------------------------------------------------------
}
