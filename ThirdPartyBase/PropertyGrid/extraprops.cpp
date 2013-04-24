#include "wx/wxprec.h"
#include "extraprops.h"

#define DELIMCHAR	wxT(';')

// -----------------------------------------------------------------------
// FileList Property
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR(wxFileListProperty,DELIMCHAR,wxT("Browse"))

#if wxUSE_VALIDATORS
wxValidator* wxPG_PROPCLASS(wxFileListProperty)::DoGetValidator() const
	{ return wxFilePropertyClass::GetClassValidator(); }
#endif

bool wxFileListPropertyClass::OnCustomStringEdit (wxWindow* parent, wxString& value) {
    wxFileDialog dlg(parent, _("Select a file to be added to the list:"), wxString(), value);
    if ( dlg.ShowModal() == wxID_OK ) {
        value = dlg.GetPath();
        return TRUE;
    }
	else
		return FALSE;
}

// -----------------------------------------------------------------------
// DirectoryList Property
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR(wxDirectoryListProperty,DELIMCHAR,wxT("Browse"))

#if wxUSE_VALIDATORS
wxValidator* wxPG_PROPCLASS(wxDirectoryListProperty)::DoGetValidator() const
	{ return wxFilePropertyClass::GetClassValidator(); }
#endif

bool wxDirectoryListPropertyClass::OnCustomStringEdit (wxWindow* parent, wxString& value) {
    wxDirDialog dlg(parent, _("Select a directory to be added to the list:"), value);
    if ( dlg.ShowModal() == wxID_OK ) {
        value = dlg.GetPath();
        return TRUE;
    }
	else
		return FALSE;
}

// -----------------------------------------------------------------------
// wxGenericListProperty Property
// -----------------------------------------------------------------------

class PropertiesDialog : public wxDialog {
private:
	wxPropertyGrid *grid;
	void OnButtonPressed(wxCommandEvent& event) { EndModal(event.GetId()); }

public:
	PropertiesDialog (wxWindow* parent, const wxString& title, wxPGProperty* pg) :
		wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
	{
		wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer->AddSpacer(25);

		wxButton* ok = new wxButton(this, wxID_OK, _("Apply"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		ok->SetDefault();
		buttonSizer->Add(ok);
		buttonSizer->AddSpacer(5);
		wxButton* cancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
		buttonSizer->Add(cancel);
		buttonSizer->AddSpacer(30);
		Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesDialog::OnButtonPressed));
		Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesDialog::OnButtonPressed));

		grid = new wxPropertyGrid(
			this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxNO_BORDER | wxPG_BOLD_MODIFIED | wxTAB_TRAVERSAL | wxPG_TOOLTIPS
		);
		grid->Append(pg);
		grid->ExpandAll();
		grid->SetSplitterLeft();
		grid->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);
		int count = (int) pg->GetChildCount() + 1;
		wxSize size(std::min(640, count * 100 + 100), std::min(480, count * 20 + 20));
		grid->SetMinSize(size);
		grid->SetSize(size);

		////////////////////////////////////////////////////////////////////
		// Layout remaining stuff
		//
		wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
		vsizer->AddSpacer(10);
		vsizer->Add(grid, 1, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxEXPAND, 10);
		vsizer->AddSpacer(10);
		vsizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL);
		vsizer->AddSpacer(10);

		vsizer->Fit(this);
		this->SetSizer(vsizer);
		this->Centre();
	}

	void RemovePGProperty(wxPGProperty* pg) { grid->Remove(wxPGId(pg)); }
};

class wxPG_PROPCLASS(wxGenericListProperty) : public wxPG_PROPCLASS(wxArrayStringProperty)
{
	wxParentPropertyClass *valueCreator;
    WX_PG_DECLARE_PROPERTY_CLASS()
public:
	wxPG_PROPCLASS(wxGenericListProperty)( const wxString& label, const wxString& name, const wxArrayString& value, wxParentPropertyClass* valueCreator) : 
		wxPG_PROPCLASS(wxArrayStringProperty)(label,name,value), valueCreator(valueCreator)
	{
		assert(valueCreator && valueCreator->GetCount());
		wxPG_PROPCLASS(wxGenericListProperty)::GenerateValueAsString();
	}
	~wxPG_PROPCLASS(wxGenericListProperty)() {
		assert(valueCreator);
		delete valueCreator;
	}

	virtual void GenerateValueAsString();
	virtual bool SetValueFromString(const wxString& text, int);

	virtual bool OnEvent(wxPropertyGrid* propgrid, wxWindow* primary, wxEvent& event) {
		if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
			return OnButtonClick(propgrid,primary, wxT("Create new"));
		return false;
	}

	virtual bool OnCustomStringEdit(wxWindow* parent, wxString& value) {
		bool retval;
		PropertiesDialog dialog(parent, _("Create new entry"), valueCreator);
		if ( dialog.ShowModal() == wxID_OK ) {
			value = valueCreator->GetValueAsString(0);
			retval = TRUE;
		}
		else
			retval = FALSE;
		dialog.RemovePGProperty(valueCreator);
		return retval;
	}

    WX_PG_DECLARE_VALIDATOR_METHODS()
};

void wxPG_PROPCLASS(wxGenericListProperty)::GenerateValueAsString() {
	wxChar delimChar = DELIMCHAR;

	m_display.Empty();

	int iMax = m_value.Count();
	int iMaxMinusOne = iMax-1;

	for (int i = 0; i < iMax; i++) {
		m_display += wxT("[") + m_value[i] + wxT("]");
		if (i < iMaxMinusOne) {
			m_display += wxT("; ");
		}
	}
}

bool wxPG_PROPCLASS(wxGenericListProperty)::SetValueFromString(const wxString& text, int) {
	m_value.Empty();

	unsigned int curChild = 0;

	wxString token;
	size_t pos = 0;

	const wxChar delimeter = DELIMCHAR;
	wxChar a;

	size_t lastPos = text.length();
	size_t tokenStart = 0xFFFFFF;

	do {
		a = text[pos];

		if ( tokenStart != 0xFFFFFF ) {
			// Token is running
			if ( a == delimeter || a == 0 ) {
				token = text.substr(tokenStart,pos-tokenStart);
				token.Trim(true);
				m_value.Add(token);
				if ( ++curChild >= valueCreator->GetCount() )
					curChild = 0;	//wrap for next list item
				tokenStart = 0xFFFFFF;
			}
		}
		else {
			// Token is not running
			if ( a != wxT(' ') ) {
				// Is this a group of tokens?
				if ( a == wxT('[') ) {
					int depth = 1;
					pos++;
					size_t startPos = pos;

					// Group item - find end
					do {
						a = text[pos];
						pos++;
						if ( a == wxT(']') )
							depth--;
						else if ( a == wxT('[') )
							depth++;
					} while ( depth > 0 && a );

					token = text.substr(startPos,pos-startPos-1);
					m_value.Add(token);
					if ( ++curChild >= valueCreator->GetCount() )
						curChild = 0;	//wrap for next list item
					tokenStart = 0xFFFFFF;
				}
				else {
					tokenStart = pos;
					if ( a == delimeter )
						pos--;
				}
			}
		}
		pos++;
	} while ( pos <= lastPos );
	GenerateValueAsString();
	return true;
}

wxPGProperty* wxPG_CONSTFUNC(wxGenericListProperty)( const wxString& label, const wxString& name, const wxArrayString& value, wxParentPropertyClass *valueCreator)
	{ return new wxPG_PROPCLASS(wxGenericListProperty)(label,name,value,valueCreator); }
wxPGProperty* wxPG_CONSTFUNC(wxGenericListProperty)( const wxString& label, const wxString& name)
	{ return new wxPG_PROPCLASS(wxGenericListProperty)(label,name,wxArrayString(),0); }

WX_PG_IMPLEMENT_CLASSINFO(wxGenericListProperty,wxArrayStringPropertyClass)
WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxGenericListProperty,wxArrayString,TextCtrlAndButton)

#if wxUSE_VALIDATORS
wxValidator* wxPG_PROPCLASS(wxGenericListProperty)::DoGetValidator() const { return (wxValidator*) NULL; }
#endif
