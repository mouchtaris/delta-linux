/**
 *	ChangeGUIPropertiesVisitor.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ChangeGUIPropertiesVisitor.h"
#include "GenericDialogs.h"
#include "DefaultGUIGenerator.h"

#include "Adaptors.h"
#include "Algorithms.h"
#include "PropertyUtils.h"

namespace conf {

////////////////////////////////////////////////////////////////////////

#define EXTRACT_VALUE(type, extractionExpr)				\
	type value = extractionExpr;						\
	m_changedValue = (value != prop->GetValue());		\
	if (m_changedValue)									\
		prop->SetValue(value)

#define EXTRACT_VALUE_AS_VARIANT(type, method) \
	EXTRACT_VALUE(type, m_guiProp->GET_VALUE_AS_VARIANT().method())

#define EXTRACT_VALUE_AS_VARIANT_CAST(type)									\
	type* __v = wxGetVariantCast(m_guiProp->GET_VALUE_AS_VARIANT(), type);	\
	if (!__v) return;														\
	EXTRACT_VALUE(type, *__v)

////////////////////////////////////////////////////////////////////////

ChangeGUIPropertiesVisitor::ChangeGUIPropertiesVisitor (void) :
	m_changedValue(false), m_guiProp(0)
{

}

//**********************************************************************

ChangeGUIPropertiesVisitor::~ChangeGUIPropertiesVisitor (void)
{

}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Reset (wxPGProperty* guiProp)
{
	m_changedValue = false;
	m_guiProp = guiProp;
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), IntProperty* prop)
{
	// wxIntProperty
	EXTRACT_VALUE_AS_VARIANT(int, GetLong);
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), BoolProperty* prop)
{
	// wxBoolProperty
	EXTRACT_VALUE_AS_VARIANT(bool, GetBool);
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), RealProperty* prop)
{
	// wxFloatProperty
	EXTRACT_VALUE_AS_VARIANT(double, GetDouble);
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), StringProperty* prop)
{
	// wxStringProperty
	EXTRACT_VALUE(String, PG_GET_VALUE_AS_STRING(m_guiProp));
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), StdStringProperty* prop)
{
	// wxStringProperty
	EXTRACT_VALUE(std::string, util::str2std(PG_GET_VALUE_AS_STRING(m_guiProp)));
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), IntRangeProperty* prop)
{
	// wxIntProperty
	EXTRACT_VALUE_AS_VARIANT(int, GetLong);
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), EnumStringProperty* prop)
{
	// wxEnumProperty
	EXTRACT_VALUE(String, PG_GET_VALUE_AS_STRING(m_guiProp));
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), StringListProperty* prop)
	{ VisitStringListProperty(prop); }

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), AggregateProperty* PORT_UNUSED_PARAM(prop))
{
	// Nothing to see here, move along
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), FontProperty* prop)
{
	// wxFontProperty
	EXTRACT_VALUE_AS_VARIANT_CAST(wxFont);
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), ColorProperty* prop)
{
	// wxColourProperty
	EXTRACT_VALUE_AS_VARIANT_CAST(wxColour);
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), FileProperty* prop)
{
	// wxFileProperty
	EXTRACT_VALUE(String, PG_GET_VALUE_AS_STRING(m_guiProp));
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), DirectoryProperty* prop)
{
	// wxDirProperty
	EXTRACT_VALUE(String, PG_GET_VALUE_AS_STRING(m_guiProp));
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), DateProperty* prop)
{
	// wxDateProperty
	EXTRACT_VALUE_AS_VARIANT(wxDateTime, GetDateTime);
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), FileListProperty* prop)
{ 
	VisitStringListProperty(prop);
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), DirectoryListProperty* prop)
{ 
	VisitStringListProperty(prop);
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), AggregateListProperty* prop)
{ 
	// wxArrayStringProperty
	wxArrayString const& value = m_guiProp->GET_VALUE_AS_VARIANT().GetArrayString();
	AggregateListProperty::AggregatePropertyList& l = prop->GetPropertyList();

	m_changedValue = value.GetCount() != l.size();
	if (!m_changedValue) {
		AggregateListProperty::AggregatePropertyList::iterator iter = l.begin(), end = l.end();
		for(uint i = 0; iter != end; ++iter, ++i) {
			wxParentPropertyClass* p = DefaultGUIGenerator::CreatePGProperty(**iter);
			m_changedValue = value[i] != PG_GET_VALUE_AS_STRING(p);
			delete p;
			if (m_changedValue)
				break;
		}
	}

	if (m_changedValue) {
		wxParentPropertyClass* pg = DefaultGUIGenerator::CreatePGProperty(*prop->GetListType());
		prop->Clear();
		for (uint i = 0; i < value.GetCount(); ++i) {
			pg->SetValueFromString(value[i], 0);
			AggregateProperty* p = static_cast<AggregateProperty*>(prop->GetListType()->Clone());
			p->SetLabel(pg->GetLabel());
			p->SetDescription(pg->GetHelpString());
			ExtractValuesFromPGProperty(*p, pg);
			l.push_back(p);
		}
		delete pg;
	}
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), MultiChoiceProperty* prop)
{
	// wxMultiChoiceProperty
	wxPG_PROPCLASS(wxMultiChoiceProperty)* pg = static_cast<wxPG_PROPCLASS(wxMultiChoiceProperty)*>(m_guiProp);
#if wxUSE_PROPGRID
	const wxArrayString choices = pg->GetChoices().GetLabels();	
	const wxArrayString selections = pg->GetValue().GetArrayString();
#else
	wxPGChoiceInfo choiceInfo;
	pg->GetChoiceInfo(&choiceInfo);
	const wxArrayString& choices = choiceInfo.m_choices->GetLabels();
	const wxArrayInt& indices = wxPGVariantToArrayInt(pg->DoGetValue());
	wxArrayString selections;
	for (uint i = 0; i < indices.GetCount(); ++i)
		selections.Add(choices[indices[i]]);
#endif

	const StringVec oldChoices = prop->GetAllChoices();
	const StringVec oldSelections = prop->GetSelectedChoices();
	
	m_changedValue =	choices.GetCount()		!= oldChoices.size()	||
						selections.GetCount()	!= oldSelections.size()	;

	if (!m_changedValue)
		for (uint i = 0; i < choices.GetCount(); ++i)
			if (choices[i] != oldChoices[i]) {
				m_changedValue = true;
				break;
			}
	if (!m_changedValue)
		for (uint i = 0; i < selections.GetCount(); ++i)
			if (selections[i] != oldSelections[i]) {
				m_changedValue = true;
				break;
			}

	if (m_changedValue) {
		prop->Clear(true);
		for (uint i = 0; i < choices.GetCount(); ++i)
			prop->AddChoice(choices[i]);
		for (uint i = 0; i < selections.GetCount(); ++i)
			prop->SelectChoice(selections[i]);
	}
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::VisitStringListProperty (StringListProperty* prop)
{
	// wxArrayStringProperty
	const wxArrayString& value = m_guiProp->GET_VALUE_AS_VARIANT().GetArrayString();

	m_changedValue = value.GetCount() != prop->GetValuesCount();
	if (!m_changedValue) {

		for (uint i = 0; i < prop->GetValuesCount(); ++i) {
			if (value[i] != prop->GetValue(i)) {
				m_changedValue = true;
				break;
			}
		}		
	}

	if (m_changedValue) {
		prop->Clear();
		for (uint i = 0; i < value.GetCount(); ++i)
			prop->AddValue(value[i]);
	}
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::ExtractValuesFromPGProperty(
		const AggregateProperty&	prop,
		wxParentPropertyClass*		pg,
		const String&				prefix
	)
{
	ChangeGUIPropertiesVisitor visitor;
	const PropertyMap& props = prop.GetPropertyMap();
	PropertyMap::const_iterator iter = props.begin();
	for (; iter != props.end(); ++iter) {
		const String name = util::std2str(iter->first);
		if (iter->second->GetType() == AggregateProperty::Type)
			ExtractValuesFromPGProperty(
				*safe_prop_cast<AggregateProperty>(iter->second),
				pg,
				prefix + name + _T(".")
			);
		else {
			visitor.Reset(pg->GetPropertyByName(prefix + name));
			iter->second->Accept(iter->first, &visitor);
		}
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace conf
