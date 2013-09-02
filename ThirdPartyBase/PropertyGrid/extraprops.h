#ifndef _WX_PROPGRID_EXTRAPROPS_H_
#define _WX_PROPGRID_EXTRAPROPS_H_

#include "PropertyGridPortability.h"

/////////////////////////////////////////////////////////////////////////

WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_DECL(wxFileListProperty, EXTRA_PROP_DECL)
WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_DECL(wxDirectoryListProperty, EXTRA_PROP_DECL)

/////////////////////////////////////////////////////////////////////////

class THIRD_PARTY_API wxPG_PROPCLASS(wxGenericListProperty) : public wxPG_PROPCLASS(wxArrayStringProperty) {
	wxParentPropertyClass *valueCreator;
#if wxUSE_PROPGRID
	WX_PG_DECLARE_PROPERTY_CLASS(wxGenericListProperty)
#else
	WX_PG_DECLARE_PROPERTY_CLASS()    
#endif
public:
	wxPG_PROPCLASS(wxGenericListProperty)(const wxString& label, const wxString& name, const wxArrayString& value, wxParentPropertyClass* valueCreator);
	~wxPG_PROPCLASS(wxGenericListProperty)();

	virtual void GenerateValueAsString();
	virtual bool SetValueFromString(const wxString& text, int argFlags);
	virtual bool StringToValue(wxVariant& variant, const wxString& text, int argFlags) const;

	virtual bool OnEvent(wxPropertyGrid* propgrid, wxWindow* primary, wxEvent& event);
	virtual bool OnCustomStringEdit(wxWindow* parent, wxString& value);
    WX_PG_DECLARE_VALIDATOR_METHODS()
};

/////////////////////////////////////////////////////////////////////////

class THIRD_PARTY_API wxPG_PROPCLASS(wxExpandedPathProperty) : public wxParentPropertyClass {
	wxString basePath;
	wxPGProperty *path;
	wxPGProperty *expandedPath;
#if wxUSE_PROPGRID
    WX_PG_DECLARE_PROPERTY_CLASS(wxExpandedPathProperty)
#else
	WX_PG_DECLARE_PROPERTY_CLASS()
#endif

public:
	wxPG_PROPCLASS(wxExpandedPathProperty)(const wxString& label, const wxString& name, const wxString& basePath, wxPGProperty *p);
	wxPG_PROPCLASS(wxExpandedPathProperty)(void) : wxParentPropertyClass(wxPG_LABEL) {}
	~wxPG_PROPCLASS(wxExpandedPathProperty)();

	void SetExpandedPathValue (const wxString& text);

	virtual PG_VARIANT_TYPE DoGetValue (void) const;
#if wxUSE_PROPGRID
	virtual void OnSetValue (void);
#endif

	virtual wxString ValueToString(wxVariant& value, int argFlags) const { return GetValueAsString(argFlags); }
	virtual wxString GetValueAsString(int argFlags = 0) const;
	virtual bool SetValueFromString(const wxString& text, int argFlags);
	virtual bool StringToValue(wxVariant& variant, const wxString& text, int argFlags) const;

	virtual bool OnEvent(wxPropertyGrid* propgrid, wxWindow* wnd_primary, wxEvent& event);
	WX_PG_DECLARE_VALIDATOR_METHODS()
};

/////////////////////////////////////////////////////////////////////////

class THIRD_PARTY_API wxPG_PROPCLASS(wxExpandedPathListProperty) : public wxParentPropertyClass {
	wxString basePath;
	wxPGProperty *path;
#if wxUSE_PROPGRID
	WX_PG_DECLARE_PROPERTY_CLASS(wxExpandedPathListProperty)
#else
	mutable wxArrayString m_strs;
	WX_PG_DECLARE_PROPERTY_CLASS()    
#endif

	struct CustomHandler : public wxEvtHandler {
	private:
		wxPG_PROPCLASS(wxExpandedPathListProperty)* p;
		DECLARE_EVENT_TABLE();
	public:
		CustomHandler(void) { SetData(); }
		void SetData(wxPG_PROPCLASS(wxExpandedPathListProperty)* p = 0) { this->p = p; }
		void OnExpandChildren(wxCommandEvent& event) { if(p) { p->UpdateChildren(p->GetGrid()); SetData(); } }
	};
	CustomHandler* handler;
public:
	wxPG_PROPCLASS(wxExpandedPathListProperty)(const wxString& label, const wxString& name, const wxString& basePath, wxPGProperty *p);
	wxPG_PROPCLASS(wxExpandedPathListProperty)(void) : wxParentPropertyClass(wxPG_LABEL) {}
	~wxPG_PROPCLASS(wxExpandedPathListProperty)();

	void UpdateChildren (wxPropertyGrid* pg);

	virtual PG_VARIANT_TYPE DoGetValue (void) const;
#if wxUSE_PROPGRID
	virtual void OnSetValue (void);
#endif

	virtual wxString ValueToString(wxVariant& value, int argFlags) const { return GetValueAsString(argFlags); }
	virtual wxString GetValueAsString(int argFlags = 0) const;
	virtual bool SetValueFromString(const wxString& text, int argFlags);
	virtual bool StringToValue(wxVariant& variant, const wxString& text, int argFlags) const;

	virtual bool OnEvent(wxPropertyGrid* propgrid, wxWindow* wnd_primary, wxEvent& event);
	WX_PG_DECLARE_VALIDATOR_METHODS()
};

#if !wxUSE_PROPGRID
extern WXDLLIMPEXP_PG wxPGProperty* wxGenericListProperty(
	const wxString& label, const wxString& name, const wxArrayString& value, wxParentPropertyClass *valueCreator);
extern WXDLLIMPEXP_PG wxPGProperty* wxExpandedPathProperty(
	const wxString& label, const wxString& name, const wxString& basePath, wxPGProperty *p);
extern WXDLLIMPEXP_PG wxPGProperty* wxExpandedPathListProperty(
	const wxString& label, const wxString& name, const wxString& basePath, wxPGProperty *p);
#endif

#endif // _WX_PROPGRID_EXTRAPROPS_H_