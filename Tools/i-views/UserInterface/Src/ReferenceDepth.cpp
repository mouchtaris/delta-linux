/**
 *	ReferenceDepth.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	August 2010
 */

#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/spinCtrl.h>
#include <wx/stattext.h>

#include "ReferenceDepth.h"


namespace iviews {

BEGIN_EVENT_TABLE(ReferenceDepth, wxDialog)
    EVT_BUTTON		(wxID_OK, ReferenceDepth::OnOK)
    EVT_BUTTON		(wxID_CANCEL, ReferenceDepth::OnCancel)
	EVT_CHECKBOX	(wxID_ANY, ReferenceDepth::OnCheckBox)
END_EVENT_TABLE()


//-----------------------------------------------------------------------

long ReferenceDepth::GetValue(void) const 
	{ return value; }

//-----------------------------------------------------------------------

bool ReferenceDepth::IsUnlimited(void) const 
	{ return checkBox->IsChecked(); }

//-----------------------------------------------------------------------

ReferenceDepth::ReferenceDepth(
	wxWindow *parent,
    const wxString& message,
    const wxString& prompt,
    const wxString& caption,
    long value_,
    long min_,
    long max_,
    const wxPoint& pos
) : wxDialog(
		parent, 
		wxID_ANY, 
		caption, 
		pos, 
		wxDefaultSize
	) {
    value = value_;
    max = max_;
    min = min_;

    wxBeginBusyCursor();

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
#if wxUSE_STATTEXT
    // 1) text message
    topSizer->Add( CreateTextSizer( message ), 0, wxALL, 10 );
#endif
    // 2) prompt and text ctrl
    wxBoxSizer *inputSizer = new wxBoxSizer( wxHORIZONTAL );

#if wxUSE_STATTEXT
    // prompt if any
    if (!prompt.empty())
        inputSizer->Add( 
			new wxStaticText( 
					this, 
					wxID_ANY, 
					prompt 
				), 
			0, wxCENTER | wxLEFT, 10 
		);
#endif

    // spin ctrl
    wxString valStr;
    valStr.Printf(wxT("%ld"), value);
    spinCtrl = new wxSpinCtrl(
						this, 
						wxID_ANY, 
						valStr, 
						wxDefaultPosition, 
						wxSize( 140, wxDefaultCoord ), 
						wxSP_ARROW_KEYS, 
						(int)min, 
						(int)max, 
						(int)value
					);

    inputSizer->Add( spinCtrl, 1, wxCENTER | wxLEFT | wxRIGHT, 10 );

	// 3) prompt and text ctrl
    wxBoxSizer *unlimitedSizer = new wxBoxSizer( wxHORIZONTAL );
	
	unlimitedSizer->Add(
		checkBox = new wxCheckBox(
				this, 
				wxID_ANY, 
				_("Unlimited (entire graph)")
			), 
			0, wxCENTER | wxLEFT, 10 
		);

	// add both
    topSizer->Add( inputSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 5 );
    topSizer->Add( unlimitedSizer, 0, wxEXPAND | wxALL, 5 );

    // 4) buttons if any
    wxSizer *buttonSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
    if ( buttonSizer )
        topSizer->Add(buttonSizer, wxSizerFlags().Expand().DoubleBorder());

    SetSizer( topSizer );
    SetAutoLayout( true );

    topSizer->SetSizeHints( this );
    topSizer->Fit( this );

    Centre( wxBOTH );

    spinCtrl->SetSelection(-1, -1);
    spinCtrl->SetFocus();

    wxEndBusyCursor();
}

//-----------------------------------------------------------------------

void ReferenceDepth::OnOK(wxCommandEvent& WXUNUSED(event))
{
#if !wxUSE_SPINCTRL
    wxString tmp = spinCtrl->GetValue();
    if ( wxSscanf(tmp, _T("%ld"), &value) != 1 )
        EndModal(wxID_CANCEL);
    else
#else
	value = spinCtrl->GetValue();
#endif
    if ( value < min || value > max ) {
        // not a number or out of range
        value = -1;
        EndModal(wxID_CANCEL);
    }

    EndModal(wxID_OK);
}

//-----------------------------------------------------------------------

void ReferenceDepth::OnCancel(wxCommandEvent& WXUNUSED(event))
	{ EndModal(wxID_CANCEL); }

//-----------------------------------------------------------------------

void ReferenceDepth::OnCheckBox(wxCommandEvent & event) 
	{ checkBox->IsChecked() ? spinCtrl->Disable() : spinCtrl->Enable(); }




//************************************************************
// global functions
//************************************************************

//-----------------------------------------------------------------------

long GetReferenceDepth(
        long value,
        long min,
        long max,
        wxWindow *parent,
        const wxPoint& pos)
{
    ReferenceDepth dialog(
						parent, 
						_T(""), 
						_T("Depth"), 
						_T("Refrence depth (undirected)"), 
						value, 
						min, 
						max, 
						pos
					);
    if (dialog.ShowModal() == wxID_OK)
		return dialog.IsUnlimited() ? 0 : dialog.GetValue();
    return -1;
}

}	//namespace iviews 