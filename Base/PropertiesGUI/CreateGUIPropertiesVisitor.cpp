/**
 *	CreateGUIPropertiesVisitor.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "CreateGUIPropertiesVisitor.h"
#include "GenericDialogs.h"
#include "DefaultGUIGenerator.h"
#include "Adaptors.h"
#include "Algorithms.h"

#include "extraprops.h"

namespace conf {

////////////////////////////////////////////////////////////////////////

void CreateGUIPropertiesVisitor::Visit (const std::string& id, IntProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxIntProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, BoolProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxBoolProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, RealProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxFloatProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, StringProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxStringProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, StdStringProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxStringProperty)(prop->GetLabel(), util::std2str(id),
		util::std2str(prop->GetValue()));
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, IntRangeProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxIntProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, EnumStringProperty* prop)
{
	wxArrayString options;
	StringVec::const_iterator iter = prop->GetOptions().begin();
	for (; iter != prop->GetOptions().end(); ++iter)
		options.Add(*iter);
#ifdef THIRD_PARTY_PROPGRID
	m_guiProp = PG_CREATE_PROP(wxEnumProperty)(prop->GetLabel(), util::std2str(id), options, (int) prop->GetOption());
#else
	m_guiProp = PG_CREATE_PROP(wxEnumProperty)(prop->GetLabel(), util::std2str(id), options, wxArrayInt(), (int) prop->GetOption());
#endif
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, StringListProperty* prop)
{
	wxArrayString strings;
	const StringVec values = prop->GetValues();
	for (StringVec::const_iterator iter = values.begin(); iter != values.end(); ++iter)
		strings.Add(*iter);
	m_guiProp = PG_CREATE_PROP(wxArrayStringProperty)(prop->GetLabel(), util::std2str(id), strings);
#if wxCHECK_VERSION(2, 9, 0)
	m_guiProp->SetAttribute(_T("Delimiter"), _T(";"));
#endif
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, AggregateProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxPropertyCategory)(prop->GetLabel(), util::std2str(id));
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, FontProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxFontProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, ColorProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxColourProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, FileProperty* prop)
{
	m_guiProp = AdaptPathProperty(PG_CREATE_PROP(wxFileProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue()));
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, DirectoryProperty* prop)
{
	m_guiProp = AdaptPathProperty(PG_CREATE_PROP(wxDirProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue()));
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, DateProperty* prop)
{
	m_guiProp = PG_CREATE_PROP(wxDateProperty)(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, FileListProperty* prop)
{
	wxArrayString strings;
	const StringVec values = prop->GetValues();
	for (StringVec::const_iterator iter = values.begin(); iter != values.end(); ++iter)
		strings.Add(*iter);
	m_guiProp = AdaptPathProperty(PG_CREATE_PROP(wxFileListProperty)(prop->GetLabel(), util::std2str(id), strings));
#if wxCHECK_VERSION(2, 9, 0)
	m_guiProp->SetAttribute(_T("Delimiter"), _T(";"));
#endif
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, DirectoryListProperty* prop)
{
	wxArrayString strings;
	const StringVec values = prop->GetValues();
	for (StringVec::const_iterator iter = values.begin(); iter != values.end(); ++iter)
		strings.Add(*iter);
	m_guiProp = AdaptPathProperty(PG_CREATE_PROP(wxDirectoryListProperty)(prop->GetLabel(), util::std2str(id), strings));
#if wxCHECK_VERSION(2, 9, 0)
	m_guiProp->SetAttribute(_T("Delimiter"), _T(";"));
#endif
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, AggregateListProperty* prop)
{
	wxArrayString strings;
	const AggregateListProperty::AggregatePropertyList& properties = prop->GetPropertyList();
	AggregateListProperty::AggregatePropertyList::const_iterator iter = properties.begin(), end = properties.end();
	for(; iter != end; ++iter) {
		wxParentPropertyClass* p = DefaultGUIGenerator::CreatePGProperty(**iter);
		strings.Add(PG_GET_VALUE_AS_STRING(p));
		delete p;
	}

	m_guiProp = PG_CREATE_PROP(wxGenericListProperty)(
		prop->GetLabel(),
		util::std2str(id),
		strings,
		DefaultGUIGenerator::CreatePGProperty(*prop->GetListType(), m_basePath)
	);
#if wxCHECK_VERSION(2, 9, 0)
	m_guiProp->SetAttribute(_T("Delimiter"), _T(";"));
#endif
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, MultiChoiceProperty* prop)
{
	wxArrayString choices;
#ifdef THIRD_PARTY_PROPGRID
	wxArrayInt values;
#else
	wxArrayString values;
#endif
	const MultiChoiceProperty::ChoiceMap& choiceMap = prop->GetChoiceMap();
	MultiChoiceProperty::ChoiceMap::const_iterator iter = choiceMap.begin(), end = choiceMap.end();
	for (int count = 0; iter != end; ++iter, ++count) {
		choices.Add(iter->first);
		if (iter->second)
			values.Add(
#ifdef THIRD_PARTY_PROPGRID
				count
#else
				iter->first
#endif
			);
	}
	m_guiProp = PG_CREATE_PROP(wxMultiChoiceProperty)(prop->GetLabel(), util::std2str(id), choices, values);
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::SetBaseProperties (Property* prop)
{
	m_guiProp->SetHelpString(prop->GetDescription());
}

//**********************************************************************


wxPGProperty* CreateGUIPropertiesVisitor::AdaptPathProperty (wxPGProperty* prop)
{
	return PG_CREATE_PROP(wxExpandedPathProperty)(prop->GetLabel(), prop->GetName(), m_basePath, prop); 
}

////////////////////////////////////////////////////////////////////////

} // namespace conf
