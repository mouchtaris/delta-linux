/**
 *	KeyBinderFrame.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "KeyBinderFrame.h"
#include "CommonGUIUtils.h"
#include "Algorithms.h"

#include "menuutils.h"
#include <wx/sizer.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

class KeyBinderDialog : public wxDialog {
public:
	KeyBinderDialog (wxKeyProfileArray& prof, wxWindow* parent, const wxString& title, int mode);
	~KeyBinderDialog (void) {}

	void				OnApply (wxCommandEvent& event) { this->EndModal(wxID_OK); }
	wxKeyConfigPanel*	GetPanel (void) { return m_p; }

private:
	DECLARE_EVENT_TABLE();

	wxKeyConfigPanel* m_p;
};

//**********************************************************************

BEGIN_EVENT_TABLE(KeyBinderDialog, wxDialog)
	EVT_BUTTON(wxID_APPLY, KeyBinderDialog::OnApply)
END_EVENT_TABLE();

//**********************************************************************

KeyBinderDialog::KeyBinderDialog (
		wxKeyProfileArray&	prof,
		wxWindow*			parent,
		const String&		title,
		int					mode
	) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, 
			wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)	
{
	// we can do our task in two ways:
	// 1) we can use wxKeyConfigPanel::ImportMenuBarCmd which gives
	//    better appearances (for me, at least, :-))
	// 2) we can use wxKeyConfigPanel::ImportKeyBinderCmd

	// STEP #1: create a simple wxKeyConfigPanel
	m_p = new wxKeyConfigPanel(this, wxID_ANY, wxDefaultPosition, 
		wxDefaultSize, wxTAB_TRAVERSAL | mode);

	// STEP #2: add a profile array to the wxKeyConfigPanel
	m_p->AddProfiles(prof);

	// STEP #3: populate the wxTreeCtrl widget of the panel
	m_p->ImportMenuBarCmd(static_cast<wxFrame*>(parent)->GetMenuBar());

	// and embed it in a little sizer
	wxBoxSizer* main = new wxBoxSizer(wxVERTICAL);
	main->Add(m_p, 1, wxGROW);
	SetSizer(main);
	main->SetSizeHints(this);

	// this is a little modification to make dlg look nicer
	wxSize sz(GetSizer()->GetMinSize());
	this->SetSize(-1, -1, (int)(sz.GetWidth()*1.3), (int)(sz.GetHeight()*1.1));
	this->Center();
}

////////////////////////////////////////////////////////////////////////

KeyBinderFrame::KeyBinderFrame (
		wxWindow*		parent,
		const String&	title,
		long			style,
		const wxSize&	size
	) : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, size, style)
{
	this->KBAttachWindow(this);
}

//**********************************************************************

KeyBinderFrame::~KeyBinderFrame (void)
{
	this->KBDetachWindow(this);
}

//**********************************************************************

bool KeyBinderFrame::Create (
		wxWindow*		parent,
		const String&	title,
		long			style,
		const wxSize&	size
	)
{
	if (!this->wxFrame::Create(parent, wxID_ANY, title, wxDefaultPosition, size, style)) {
		return false;
	}
	this->KBAttachWindow(this);
	return true;
}

//**********************************************************************

void KeyBinderFrame::SetMenuBar (wxMenuBar* menuBar)
{
	this->wxFrame::SetMenuBar(menuBar);
	this->registerMenuBar(menuBar);
}

//**********************************************************************

void KeyBinderFrame::KBAttachWindow (wxWindow* window) { m_profiles.AttachAllTo(window); }
void KeyBinderFrame::KBDetachWindow (wxWindow* window) { m_profiles.DetachAllFrom(window); }

//**********************************************************************

void KeyBinderFrame::KBConfigureKeys (void)
{
	int retVal;
	{
		KeyBinderDialog dlg(m_profiles, this, _("Configure keys"),
			wxKEYBINDER_USE_TREECTRL | wxKEYBINDER_SHOW_ADDREMOVE_PROFILE |
				wxKEYBINDER_ENABLE_PROFILE_EDITING | wxKEYBINDER_SHOW_APPLYBUTTON);

		dlg.GetPanel()->EnableKeyProfiles(true);
		if ((retVal = dlg.ShowModal()) == wxID_OK)
			m_profiles = dlg.GetPanel()->GetProfiles();
	}
	if (retVal == wxID_OK) {
		m_profiles.GetSelProfile()->Enable(TRUE);
	}
}

//**********************************************************************

void KeyBinderFrame::registerMenuBar (wxMenuBar* menuBar)
{
	// Cleanup the profiles
	m_profiles.Cleanup();

	wxKeyProfile* pPrimary = new wxKeyProfile(_("Default"), _("Default profile"));

	if (menuBar)
		pPrimary->ImportMenuBarCmd(menuBar);

	// Put keyprofiles into our array
	m_profiles.Add(pPrimary);

	// By now, attach to this window the primary keybinder
	m_profiles.SetSelProfile(0);
	m_profiles.GetSelProfile()->Enable(TRUE);
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
