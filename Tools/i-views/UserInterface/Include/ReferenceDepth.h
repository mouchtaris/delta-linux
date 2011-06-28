#ifndef	REFERENCE_DEPTH_H 
#define REFERENCE_DEPTH_H

#include <wx/dialog.h>
#include <wx/checkbox.h>


namespace iviews {
class ReferenceDepth : public wxDialog {
public:
	ReferenceDepth (
		wxWindow *parent,
		const wxString& message,
		const wxString& prompt,
		const wxString& caption,
		long			value, 
		long			min, 
		long			max,
		const wxPoint&	pos = wxDefaultPosition
	);

	long GetValue(void) const;
	bool IsUnlimited(void) const;
	
	// implementation only
	void OnOK(wxCommandEvent & event);
	void OnCancel(wxCommandEvent & event);
	void OnCheckBox(wxCommandEvent & event);

protected:
	#if wxUSE_SPINCTRL
		wxSpinCtrl *spinCtrl;
	#else
		wxTextCtrl *spinCtrl;
	#endif // wxUSE_SPINCTRL
	wxCheckBox * checkBox;
	long value, min, max;

private:
	DECLARE_EVENT_TABLE()
};



long GetReferenceDepth(
        long value = 0,
        long min = 0,
        long max = 100,
        wxWindow *parent = (wxWindow *)NULL,
        const wxPoint& pos = wxDefaultPosition
	);

}	//namespace iviews 

#endif	REFERENCE_DEPTH_H