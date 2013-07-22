#include "wx/wxprec.h"
#include "extraprops.h"

#define DELIMCHAR	wxT(';')

#if wxUSE_PROPGRID
#undef WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR
#define WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR(PROPNAME,DELIMCHAR,CUSTBUTTXT)										\
	wxPG_PROPCLASS(PROPNAME)::wxPG_PROPCLASS(PROPNAME)(const wxString& label, const wxString& name, const wxArrayString& value)	\
		: wxArrayStringProperty(label,name,value)																				\
	{																															\
		wxPG_PROPCLASS(PROPNAME)::GenerateValueAsString();																		\
		m_delimiter = DELIMCHAR;																								\
	}																															\
	wxPG_PROPCLASS(PROPNAME)::~wxPG_PROPCLASS(PROPNAME)() {}																	\
	bool wxPG_PROPCLASS(PROPNAME)::OnEvent(wxPropertyGrid* propgrid, wxWindow* primary, wxEvent& event)							\
	{																															\
		if (event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)																\
			return OnButtonClick(propgrid,primary,(const wxChar*) CUSTBUTTXT);													\
		return false;																											\
	}																															\
	wxClassInfo* wxPG_PROPCLASS(PROPNAME)::GetClassInfo(void) const { return NULL; }											\
	WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(PROPNAME, wxArrayString, TextCtrlAndButton)

#endif

// -----------------------------------------------------------------------
// FileList Property
// -----------------------------------------------------------------------
WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR(wxFileListProperty,DELIMCHAR,wxT("Browse"))

#if wxUSE_VALIDATORS
wxValidator* wxPG_PROPCLASS(wxFileListProperty)::DoGetValidator() const
	{ return wxPG_PROPCLASS(wxFileProperty)::GetClassValidator(); }
#endif

bool wxPG_PROPCLASS(wxFileListProperty)::OnCustomStringEdit (wxWindow* parent, wxString& value) {
    wxFileDialog dlg(parent, _("Select a file to be added to the list:"), wxString(), value);
    if (dlg.ShowModal() == wxID_OK) {
        value = dlg.GetPath();
        return true;
    }
	else
		return false;
}

// -----------------------------------------------------------------------
// DirectoryList Property
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR(wxDirectoryListProperty,DELIMCHAR,wxT("Browse"))

#if wxUSE_VALIDATORS
wxValidator* wxPG_PROPCLASS(wxDirectoryListProperty)::DoGetValidator() const { return NULL; }
#endif

bool wxPG_PROPCLASS(wxDirectoryListProperty)::OnCustomStringEdit (wxWindow* parent, wxString& value) {
    wxDirDialog dlg(parent, _("Select a directory to be added to the list:"), value);
    if (dlg.ShowModal() == wxID_OK) {
        value = dlg.GetPath();
        return true;
    }
	else
		return false;
}

// -----------------------------------------------------------------------
// GenericList Property
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
#if wxCHECK_VERSION(2, 9, 0)
			| wxPG_SPLITTER_AUTO_CENTER
#endif
		);
		grid->Append(pg);
		grid->ExpandAll();
#if !wxCHECK_VERSION(2, 9, 0)
		grid->SetSplitterLeft();
#endif
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

	void RemovePGProperty(wxPGProperty* pg) { grid->REMOVE_PROPERTY(pg); }
};



wxPG_PROPCLASS(wxGenericListProperty)::wxPG_PROPCLASS(wxGenericListProperty)(const wxString& label, const wxString& name, const wxArrayString& value, wxParentPropertyClass* valueCreator) : 
	wxPG_PROPCLASS(wxArrayStringProperty)(label,name,value), valueCreator(valueCreator)
{
	assert(valueCreator && valueCreator->GetCount());
	GenerateValueAsString();
}
wxPG_PROPCLASS(wxGenericListProperty)::~wxPG_PROPCLASS(wxGenericListProperty)() {
	assert(valueCreator);
	delete valueCreator;
}

#if wxUSE_PROPGRID
wxClassInfo* wxPG_PROPCLASS(wxGenericListProperty)::GetClassInfo(void) const { return NULL; }
#endif

bool wxPG_PROPCLASS(wxGenericListProperty)::OnEvent(wxPropertyGrid* propgrid, wxWindow* primary, wxEvent& event) {
	if (event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
		return OnButtonClick(propgrid,primary, wxT("Create new"));
	return false;
}

bool wxPG_PROPCLASS(wxGenericListProperty)::OnCustomStringEdit(wxWindow* parent, wxString& value) {
	bool retval;
	PropertiesDialog dialog(parent, _("Create new entry"), valueCreator);
	if (dialog.ShowModal() == wxID_OK) {
		value = valueCreator->GetValueAsString(0);
		retval = true;
	}
	else
		retval = false;
	dialog.RemovePGProperty(valueCreator);
	return retval;
}

void wxPG_PROPCLASS(wxGenericListProperty)::GenerateValueAsString() {
	wxChar delimChar = DELIMCHAR;

	m_display.Empty();

#if wxUSE_PROPGRID
	wxArrayString value = m_value.GetArrayString();
#else
	wxArrayString& value = m_value;
#endif

	int iMax = value.Count();
	int iMaxMinusOne = iMax-1;

	for (int i = 0; i < iMax; i++) {
		m_display += wxT("[") + value[i] + wxT("]");
		if (i < iMaxMinusOne) {
			m_display += wxT("; ");
		}
	}
}

bool wxPG_PROPCLASS(wxGenericListProperty)::SetValueFromString(const wxString& text, int WXUNUSED(argFlags)) {
#if wxUSE_PROPGRID
	wxArrayString value;
#else
	wxArrayString& value = m_value;
	value.Empty();
#endif
	
	unsigned int curChild = 0;

	wxString token;
	size_t pos = 0;

	const wxChar delimeter = DELIMCHAR;
	wxChar a;

	size_t lastPos = text.length();
	size_t tokenStart = 0xFFFFFF;

	while(pos < lastPos) {
		a = text[pos];

		if (tokenStart != 0xFFFFFF) {
			// Token is running
			if (a == delimeter || a == 0) {
				token = text.substr(tokenStart,pos-tokenStart);
				token.Trim(true);
				value.Add(token);
				if (++curChild >= valueCreator->GetCount())
					curChild = 0;	//wrap for next list item
				tokenStart = 0xFFFFFF;
			}
		}
		else {
			// Token is not running
			if (a != wxT(' ')) {
				// Is this a group of tokens?
				if (a == wxT('[')) {
					int depth = 1;
					pos++;
					size_t startPos = pos;

					// Group item - find end
					do {
						a = text[pos];
						pos++;
						if (a == wxT(']'))
							depth--;
						else if (a == wxT('['))
							depth++;
					} while (depth > 0 && a);

					token = text.substr(startPos,pos-startPos-1);
					value.Add(token);
					if (++curChild >= valueCreator->GetCount())
						curChild = 0;	//wrap for next list item
					tokenStart = 0xFFFFFF;
				}
				else {
					tokenStart = pos;
					if (a == delimeter)
						pos--;
				}
			}
		}
		pos++;
	}
#if wxUSE_PROPGRID
	m_value = value;
#endif

	GenerateValueAsString();
	return true;
}

bool wxPG_PROPCLASS(wxGenericListProperty)::StringToValue(wxVariant& variant, const wxString& text, int argFlags) const {
	if (const_cast<wxPG_PROPCLASS(wxGenericListProperty)*>(this)->SetValueFromString(text, argFlags)) {
		variant = m_value;
		return true;
	}
	else
		return false;
}

#if !wxUSE_PROPGRID
wxPGProperty* wxPG_CONSTFUNC(wxGenericListProperty)(const wxString& label, const wxString& name, const wxArrayString& value, wxParentPropertyClass *valueCreator)
	{ return new wxPG_PROPCLASS(wxGenericListProperty)(label,name,value,valueCreator); }
wxPGProperty* wxPG_CONSTFUNC(wxGenericListProperty)(const wxString& label, const wxString& name)
	{ return new wxPG_PROPCLASS(wxGenericListProperty)(label,name,wxArrayString(),0); }
#endif

WX_PG_IMPLEMENT_CLASSINFO(wxGenericListProperty,wxArrayStringPropertyClass)
WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxGenericListProperty,wxArrayString,TextCtrlAndButton)

#if wxUSE_VALIDATORS
WX_PG_IMPLEMENT_EMPTY_VALIDATOR(wxGenericListProperty)
#endif

// -----------------------------------------------------------------------
// ExpandedPath Property
// -----------------------------------------------------------------------

DECLARE_LOCAL_EVENT_TYPE(EVENT_UPDATE_CHILDREN, -1);
DEFINE_EVENT_TYPE(EVENT_UPDATE_CHILDREN);

BEGIN_EVENT_TABLE(wxPG_PROPCLASS(wxExpandedPathProperty)::CustomHandler, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, EVENT_UPDATE_CHILDREN, wxPG_PROPCLASS(wxExpandedPathProperty)::CustomHandler::OnExpandChildren)
END_EVENT_TABLE();

wxPG_PROPCLASS(wxExpandedPathProperty)::wxPG_PROPCLASS(wxExpandedPathProperty)(const wxString& label, const wxString& name, const wxString& basePath, wxPGProperty* p) : 
	wxParentPropertyClass(label,name), basePath(basePath), path(p), settingValue(false), handler(new CustomHandler())
{
	const wxString text = GetValueAsString();
	WX_PG_TOKENIZER1_BEGIN(text,DELIMCHAR)
		wxFileName filename(token);
		filename.Normalize(wxPATH_NORM_ALL, basePath);
		wxPGProperty* p = PG_CREATE_PROP(wxStringProperty)(_T("Full Path"), *((wxString *) 0), filename.GetFullPath());
#if wxUSE_PROPGRID
		p->Enable(false);
		AppendChild(p);
#else
		p->SetFlag(wxPG_PROP_DISABLED);
		AddChild(p);
#endif
	WX_PG_TOKENIZER1_END()

#if wxUSE_PROPGRID
		m_value = DoGetValue();
#endif
}
wxPG_PROPCLASS(wxExpandedPathProperty)::~wxPG_PROPCLASS(wxExpandedPathProperty)() {
	delete path;
	delete handler;
}

void wxPG_PROPCLASS(wxExpandedPathProperty)::UpdateChildren (wxPropertyGrid* pg) {
	const wxString text = GetValueAsString();

#if wxUSE_PROPGRID
	DeleteChildren();
#else
	for (unsigned i = 0; i < m_children.GetCount(); ++i)
		pg->Delete(wxPGIdGen((wxPGProperty*)m_children.Item(i)));
	m_children.Clear();
#endif		
	WX_PG_TOKENIZER1_BEGIN(text,DELIMCHAR)
		wxFileName filename(token);
		filename.Normalize(wxPATH_NORM_ALL, basePath);
		wxPGProperty* p = PG_CREATE_PROP(wxStringProperty)(_T("Full Path"), *((wxString *) 0), filename.GetFullPath());
#if wxUSE_PROPGRID
		p->Enable(false);
#else
		p->SetFlag(wxPG_PROP_DISABLED);
#endif
		pg->AppendIn(wxPGIdGen(this), p);
	WX_PG_TOKENIZER1_END()
	pg->Refresh();
}

#if wxUSE_PROPGRID
wxVariant wxPG_PROPCLASS(wxExpandedPathProperty)::DoGetValue(void) const {
	const wxString text = GetValueAsString();
	wxArrayString strs;
	WX_PG_TOKENIZER1_BEGIN(text,DELIMCHAR)
		strs.Add(token);
	WX_PG_TOKENIZER1_END()
	return strs;
}

void wxPG_PROPCLASS(wxExpandedPathProperty)::OnSetValue (void) {
	wxString text;
	const wxString type = m_value.GetType();
	if (type == "string")
		text = m_value.GetString();
	else {
		assert(type == "arrstring");
		const wxArrayString strs = m_value.GetArrayString();
	
		for (unsigned i = 0; i < strs.size(); ++i) {
			if (!text.empty())
				text += DELIMCHAR;
			text += strs.Item(i);
		}
	}
	SetValueFromString(text, 0);
}
#endif

wxString wxPG_PROPCLASS(wxExpandedPathProperty)::GetValueAsString(int argFlags) const
	{ return path->GetValueAsString(argFlags); }

bool wxPG_PROPCLASS(wxExpandedPathProperty)::SetValueFromString(const wxString& text, int argFlags) {
	if (settingValue)
		return true;
	settingValue = true;
	path->SetValueFromString(text, argFlags);
	handler->SetData(this);
	handler->AddPendingEvent(wxCommandEvent(EVENT_UPDATE_CHILDREN));
	settingValue = false;
	return true;
}

bool wxPG_PROPCLASS(wxExpandedPathProperty)::StringToValue(wxVariant& variant, const wxString& text, int argFlags) const {
	const_cast<wxPG_PROPCLASS(wxExpandedPathProperty)*>(this)->SetValueFromString(text, argFlags);
	variant = path->GetValueAsString();
	return true;
}

bool wxPG_PROPCLASS(wxExpandedPathProperty)::OnEvent(wxPropertyGrid* propgrid, wxWindow* wnd_primary, wxEvent& event) {
#if wxUSE_PROPGRID
	AppendChild(path);
#endif
	bool result = path->OnEvent(propgrid, wnd_primary, event);
#if wxUSE_PROPGRID
	RemoveChild(path);
#endif

	if (result) {
		UpdateChildren(propgrid);
		return true;
	}
	else if (event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER) {
		handler->SetData(this);
		handler->AddPendingEvent(wxCommandEvent(EVENT_UPDATE_CHILDREN));
	}
	return false;
}

#if !wxUSE_PROPGRID
wxPGProperty* wxPG_CONSTFUNC(wxExpandedPathProperty)(const wxString& label, const wxString& name, const wxString& basePath, wxPGProperty *p)
	{ return new wxPG_PROPCLASS(wxExpandedPathProperty)(label,name,basePath,p); }
#endif

#if wxUSE_PROPGRID
IMPLEMENT_DYNAMIC_CLASS(wxExpandedPathProperty, wxParentPropertyClass)
#else
const wxPGPropertyClassInfo* wxPG_PROPCLASS(wxExpandedPathProperty)::GetClassInfo(void) const { return NULL; }
#endif
WX_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN(wxExpandedPathProperty,wxString,TextCtrlAndButton)

#if wxUSE_VALIDATORS
WX_PG_IMPLEMENT_EMPTY_VALIDATOR(wxExpandedPathProperty)
#endif
