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

#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/toolbar.h>
#include <wx/stattext.h>

#if _MSC_VER == 1600 && defined (NO_THIRD_PARTY_BASE)
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/extras.h>
#include <wx/propgrid/manager.h>

typedef wxMultiChoiceProperty wxMultiChoicePropertyClass;
#define WX_ARRAY_INT_FROM_VARIANT wxArrayIntFromVariant

class wxParentPropertyClass: public wxPGProperty {};

#else
#include "propgrid.h"
#include "propdev.h"
#include "advprops.h"
#include "extraprops.h"
#include "manager.h"

#define WX_ARRAY_INT_FROM_VARIANT wxPGVariantToArrayInt
#endif

namespace conf {

////////////////////////////////////////////////////////////////////////

#define EXTRACT_VALUE(type, extractionExpr)				\
	type value = extractionExpr;						\
	m_changedValue = (value != prop->GetValue());		\
	if (m_changedValue)									\
		prop->SetValue(value)

#define EXTRACT_VALUE_AS_VARIANT(type, method) \
	EXTRACT_VALUE(type, m_guiProp->GetValueAsVariant().method())

#define EXTRACT_VALUE_AS_VARIANT_CAST(type) \
	EXTRACT_VALUE(type, (*wxGetVariantCast(m_guiProp->GetValueAsVariant(), type)))

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
	EXTRACT_VALUE(String, m_guiProp->GetValueAsString());
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), StdStringProperty* prop)
{
	// wxStringProperty
	EXTRACT_VALUE(std::string, util::str2std(m_guiProp->GetValueAsString()));
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
	EXTRACT_VALUE(String, m_guiProp->GetValueAsString());
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
	EXTRACT_VALUE(String, m_guiProp->GetValueAsString());
}

//**********************************************************************

void ChangeGUIPropertiesVisitor::Visit (const std::string& PORT_UNUSED_PARAM(id), DirectoryProperty* prop)
{
	// wxDirProperty
	EXTRACT_VALUE(String, m_guiProp->GetValueAsString());
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
	wxArrayString const& value = m_guiProp->GetValueAsVariant().GetArrayString();
	AggregateListProperty::AggregatePropertyList& l = prop->GetPropertyList();

	m_changedValue = value.GetCount() != l.size();
	if (!m_changedValue) {
		AggregateListProperty::AggregatePropertyList::iterator iter = l.begin(), end = l.end();
		for(uint i = 0; iter != end; ++iter, ++i) {
			wxParentPropertyClass* p = DefaultGUIGenerator::CreatePGProperty(**iter);
			m_changedValue = value[i] != p->GetValueAsString(0);
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
	wxMultiChoicePropertyClass* pg = static_cast<wxMultiChoicePropertyClass*>(m_guiProp);
	wxPGChoiceInfo info;
	pg->GetChoiceInfo(&info);
	const wxArrayString& choices = info.m_choices->GetLabels();
	const wxArrayInt& indices = WX_ARRAY_INT_FROM_VARIANT(pg->DoGetValue());
	wxArrayString selections;
	for (uint i = 0; i < indices.GetCount(); ++i)
		selections.Add(choices[indices[i]]);

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
	wxArrayString const& value = m_guiProp->GetValueAsVariant().GetArrayString();

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
