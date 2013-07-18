#ifndef _WX_PROPGRID_EXTRAPROPS_H_
#define _WX_PROPGRID_EXTRAPROPS_H_

#include "PropertyGridPortability.h"

#ifdef THIRDPARTYBASELIBRARY_EXPORTS
#	define THIRD_PARTY_API __declspec(dllexport)
#else
#	define THIRD_PARTY_API __declspec(dllimport)
#endif

WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_DECL(wxFileListProperty, EXTRA_PROP_DECL)
WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_DECL(wxDirectoryListProperty, EXTRA_PROP_DECL)

class THIRD_PARTY_API wxPG_PROPCLASS(wxGenericListProperty) : public wxPG_PROPCLASS(wxArrayStringProperty) {
	wxParentPropertyClass *valueCreator;
#ifdef THIRD_PARTY_PROPGRID
	WX_PG_DECLARE_PROPERTY_CLASS()
#else
    WX_PG_DECLARE_PROPERTY_CLASS(wxGenericListProperty)
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

class THIRD_PARTY_API wxPG_PROPCLASS(wxExpandedPathProperty) : public wxParentPropertyClass {
	wxString basePath;
	wxPGProperty *path;
	wxPGProperty *expandedPath;
#ifdef THIRD_PARTY_PROPGRID
	WX_PG_DECLARE_PROPERTY_CLASS()
#else
    WX_PG_DECLARE_PROPERTY_CLASS(wxExpandedPathProperty)
#endif
public:
	wxPG_PROPCLASS(wxExpandedPathProperty)(const wxString& label, const wxString& name, const wxString& basePath, wxPGProperty *p);
	~wxPG_PROPCLASS(wxExpandedPathProperty)();

	void SetExpandedPathValue (const wxString& text);
	virtual wxString GetValueAsString(int argFlags = 0) const;
	virtual bool SetValueFromString(const wxString& text, int argFlags);
	virtual bool StringToValue(wxVariant& variant, const wxString& text, int argFlags) const;

	virtual bool OnEvent(wxPropertyGrid* propgrid, wxWindow* wnd_primary, wxEvent& event);
	WX_PG_DECLARE_VALIDATOR_METHODS()
};

#ifdef THIRD_PARTY_PROPGRID
extern WXDLLIMPEXP_PG wxPGProperty* wxGenericListProperty(
	const wxString& label, const wxString& name, const wxArrayString& value, wxParentPropertyClass *valueCreator);
extern WXDLLIMPEXP_PG wxPGProperty* wxExpandedPathProperty(
	const wxString& label, const wxString& name, const wxString& basePath, wxPGProperty *p);
#endif

#endif // _WX_PROPGRID_EXTRAPROPS_H_