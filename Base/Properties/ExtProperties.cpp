/**
 *	ExtProperties.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ExtProperties.h"
#include "Adaptors.h"
#include "Algorithms.h"
#include "PropertyVisitor.h"
#include "StringUtils.h"

#include "Encoding.h"
#include "ExtSerialization.h"

#include <wx/filename.h>

////////////////////////////////////////////////////////////////////////

namespace conf {

////////////////////////////////////////////////////////////////////////

String ExpandEnvironmentVariables(String path) {
	wxFileName filename(path);
	filename.Normalize(wxPATH_NORM_ENV_VARS);
	return filename.GetFullPath();
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_GENERIC_FUNCTIONS(FontProperty);
IMPLEMENT_GENERIC_FUNCTIONS(ColorProperty);
IMPLEMENT_GENERIC_FUNCTIONS(FileProperty);
IMPLEMENT_GENERIC_FUNCTIONS(DirectoryProperty);
IMPLEMENT_GENERIC_FUNCTIONS(DateProperty);
IMPLEMENT_GENERIC_FUNCTIONS(MultiChoiceProperty);

IMPLEMENT_SERIALIZATION_FUNCTIONS(FileProperty);
IMPLEMENT_SERIALIZATION_FUNCTIONS(DirectoryProperty);

////////////////////////////////////////////////////////////////////////
// FontProperty class
//
const String FontProperty::Serialize (void) const { return m_value.GetNativeFontInfoUserDesc(); }

//**********************************************************************

bool FontProperty::Deserialize (const String& str) { return m_value.SetNativeFontInfoUserDesc(str); }

////////////////////////////////////////////////////////////////////////
// ColorProperty class
//
const String ColorProperty::Serialize (void) const { return m_value.GetAsString(wxC2S_CSS_SYNTAX); }

//**********************************************************************

bool ColorProperty::Deserialize (const String& str) { return m_value.Set(str); }

////////////////////////////////////////////////////////////////////////
// DateProperty class
//
const String DateProperty::Serialize (void) const { return m_value.FormatDate(); }

//**********************************************************************

bool DateProperty::Deserialize (const String& str) { return m_value.WX_DATE_TIME_PARSE_DATE(str) != NULL; }

////////////////////////////////////////////////////////////////////////
// FileListProperty & DirectoryListProperty classes
//
#define IMPLEMENT_PATH_LIST_GENERIC_FUNCTIONS(className)									\
	className::className (const className& p) : StringListProperty(p) {}					\
	className& className::operator= (const className& p) {									\
		StringListProperty::operator =(p);													\
		return *this;																		\
	}																						\
	void className::Accept (const std::string& propId, PropertyVisitor* visitor)			\
		{ visitor->Visit(propId, this); }													\
	const String className::GetExpandedValue (void) const {									\
		String result;																		\
		for (StringVec::const_iterator i = m_values.begin(); i != m_values.end(); ++i) {	\
			if (!result.empty())															\
				result.append(_T(";"));														\
			result.append(ExpandEnvironmentVariables(*i));									\
		}																					\
		return result;																		\
	}																						\
	const StringVec className::GetExpandedValues (void) const {								\
		StringVec result;																	\
		for (StringVec::const_iterator i = m_values.begin(); i != m_values.end(); ++i)		\
			result.push_back(ExpandEnvironmentVariables(*i));								\
		return result;																		\
	}																						\
	const String className::GetExpandedValue (uint pos) const								\
		{ return ExpandEnvironmentVariables(StringListProperty::GetValue(pos)); }

IMPLEMENT_PATH_LIST_GENERIC_FUNCTIONS(FileListProperty)
IMPLEMENT_PATH_LIST_GENERIC_FUNCTIONS(DirectoryListProperty)

////////////////////////////////////////////////////////////////////////
// AggregateListProperty class
//
AggregateListProperty::AggregateListProperty (const AggregateListProperty& p) :
	Property(p), m_type(static_cast<AggregateProperty*>(p.m_type->Clone()))
{
	AggregatePropertyList::const_iterator iter = p.m_properties.begin(), end = p.m_properties.end();
	for (; iter != end; ++iter)
		m_properties.push_back(static_cast<AggregateProperty*>((*iter)->Clone()));
}

//**********************************************************************

AggregateListProperty::~AggregateListProperty (void)
{
	this->Clear();
	delete m_type;
}

//**********************************************************************

void AggregateListProperty::Accept (const std::string& propId, PropertyVisitor* visitor)
	{ visitor->Visit(propId, this); }

//**********************************************************************

void AggregateListProperty::Encode (comm::encoder& enc)
{
	AggregatePropertyList::iterator iter = m_properties.begin(), end = m_properties.end();
	for (; iter != end; ++iter)
		(*iter)->Encode(enc);
}

//**********************************************************************

void AggregateListProperty::Decode (comm::decoder& dec)
{
	AggregatePropertyList::iterator iter = m_properties.begin(), end = m_properties.end();
	for (; iter != end; ++iter)
		(*iter)->Decode(dec);
}

//**********************************************************************

bool AggregateListProperty::Equal (const Property* prop) const {

	if (GetType() != prop->GetType())
		return false;

	if (!m_type->Equal(static_cast<const AggregateListProperty*>(prop)->m_type))
		return false;

	AggregatePropertyList otherProperties = static_cast<const AggregateListProperty*>(prop)->m_properties;
	if (otherProperties.size() != m_properties.size())
		return false;

	AggregatePropertyList::const_iterator i = m_properties.begin(), i_end = m_properties.end();
	AggregatePropertyList::const_iterator j = otherProperties.begin(), j_end = otherProperties.end();

	for (; i != i_end && j != j_end; ++i, ++j)
		if (!(*i)->Equal((*j)))
			return false;

	return true;
}

//**********************************************************************

AggregateProperty* AggregateListProperty::NewProperty (void)
{
	AggregateProperty* p = static_cast<AggregateProperty*>(m_type->Clone());
	m_properties.push_back(p);
	return p;
}

//**********************************************************************

void AggregateListProperty::Clear (void)
{
	util::for_all(m_properties, util::deleter<Property>());
	m_properties.clear();
}

////////////////////////////////////////////////////////////////////////
// MultiChoiceProperty class
//
MultiChoiceProperty::~MultiChoiceProperty() { Clear(); }

//**********************************************************************

void MultiChoiceProperty::SetValue (const String& value)
{
	for (ChoiceMap::iterator i = m_choices.begin(); i != m_choices.end(); ++i)
		i->second = false;
	m_extraSelections.clear();

	StringVec values;
	util::stringtokenizer(values, value, _T(";,"));
	for (StringVec::iterator i = values.begin(); i != values.end(); ++i)
		SelectChoice(*i);
}

//**********************************************************************

const String MultiChoiceProperty::GetValue(void) const {
	String value;
	for (ChoiceMap::const_iterator i = m_choices.begin(); i != m_choices.end(); ++i) {
		if (i->second) {
			if (!value.empty())
				value += _T(";");
			value += i->first;
		}
	}
	if (m_allowExtraSelections) {
		for (StringSet::const_iterator i = m_extraSelections.begin(); i != m_extraSelections.end(); ++i) {
			if (!value.empty())
				value += _T(";");
			value += *i;
		}
	}
	return value;
}

//**********************************************************************

const StringVec	MultiChoiceProperty::GetAllChoices (void) const
{
	StringVec values;
	for (ChoiceMap::const_iterator i = m_choices.begin(); i != m_choices.end(); ++i)
		values.push_back(i->first);
	return values;
}

//**********************************************************************

const StringVec	MultiChoiceProperty::GetSelectedChoices (void) const
{
	StringVec values;
	for (ChoiceMap::const_iterator i = m_choices.begin(); i != m_choices.end(); ++i)
		if (i->second)
			values.push_back(i->first);
	return values;
}

//**********************************************************************

bool MultiChoiceProperty::AddChoice (const String& value)
{
	ChoiceMap::iterator iter = m_choices.find(value);
	bool isNewChoice = iter == m_choices.end();
	if (isNewChoice)
		m_choices[value] = false;
	if (m_allowExtraSelections) {
		StringSet::iterator i = m_extraSelections.find(value);
		if (i != m_extraSelections.end()) {
			m_choices[value] = true;
			m_extraSelections.erase(i);
		}
	}
	return isNewChoice;
}

//**********************************************************************

void MultiChoiceProperty::RemoveChoice (const String& value)
{
	ChoiceMap::iterator i = m_choices.find(value);
	if (i != m_choices.end()) {
		if (m_allowExtraSelections)
			m_extraSelections.insert(value);
		m_choices.erase(i);
	}
}

//**********************************************************************

void MultiChoiceProperty::SelectChoice (const String& value)
{
	ChoiceMap::iterator iter = m_choices.find(value);
	if (iter != m_choices.end())
		iter->second = true;
	else if (m_allowExtraSelections)
		m_extraSelections.insert(value);
}

//**********************************************************************

void MultiChoiceProperty::DeselectChoice (const String& value)
{
	ChoiceMap::iterator iter = m_choices.find(value);
	if (iter != m_choices.end())
		iter->second = false;
	else if (m_allowExtraSelections)
		m_extraSelections.insert(value);
}

//**********************************************************************

void MultiChoiceProperty::Clear(bool keepExtraSelections) {
	m_choices.clear();
	if (!keepExtraSelections)
		m_extraSelections.clear();
}

//**********************************************************************

const String MultiChoiceProperty::Serialize (void) const { return GetValue(); }

//**********************************************************************

bool MultiChoiceProperty::Deserialize (const String& str) {
	SetValue(str);
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace conf
