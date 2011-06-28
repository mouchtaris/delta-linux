/**
 *	Properties.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "Properties.h"
#include "Adaptors.h"
#include "Algorithms.h"
#include "PropertyVisitor.h"
#include "StringUtils.h"

#include "Encoding.h"

namespace conf {

////////////////////////////////////////////////////////////////////////

IMPLEMENT_GENERIC_FUNCTIONS(IntProperty);
IMPLEMENT_GENERIC_FUNCTIONS(BoolProperty);
IMPLEMENT_GENERIC_FUNCTIONS(RealProperty);
IMPLEMENT_GENERIC_FUNCTIONS(StringProperty);
IMPLEMENT_GENERIC_FUNCTIONS(StdStringProperty);
IMPLEMENT_GENERIC_FUNCTIONS(IntRangeProperty);
IMPLEMENT_GENERIC_FUNCTIONS(EnumStringProperty);
IMPLEMENT_GENERIC_FUNCTIONS(StringListProperty);

////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIALIZATION_FUNCTIONS(IntProperty);
IMPLEMENT_SERIALIZATION_FUNCTIONS(BoolProperty);
IMPLEMENT_SERIALIZATION_FUNCTIONS(RealProperty);
IMPLEMENT_SERIALIZATION_FUNCTIONS(StringProperty);
IMPLEMENT_SERIALIZATION_FUNCTIONS(StdStringProperty);
IMPLEMENT_SERIALIZATION_FUNCTIONS(IntRangeProperty);

////////////////////////////////////////////////////////////////////////
// EnumStringProperty class
//
void EnumStringProperty::SetOption (size_type index)
{
	m_index = util::clamp(index, 0U, m_options.size() - 1);
}

//**********************************************************************

void EnumStringProperty::SetValue (const String& value)
{
	StringVec::iterator iter = std::find(m_options.begin(), m_options.end(), value);
	if (iter != m_options.end())
		m_index = std::distance(m_options.begin(), iter);
}

//**********************************************************************

const String EnumStringProperty::Serialize (void) const { return serialize(m_index); }

//**********************************************************************

bool EnumStringProperty::Deserialize (const String& str) {
	bool retval = deserialize(str, m_index);
	SetOption(m_index);
	return retval;
}

////////////////////////////////////////////////////////////////////////
// StringListProperty class
//
StringListProperty::StringListProperty (const StringListProperty& p) :
	Property(p), m_values(p.m_values), m_value(p.m_value)
{
}

//**********************************************************************

StringListProperty::~StringListProperty (void)
{
	this->Clear();
}

//**********************************************************************

StringListProperty& StringListProperty::operator= (const StringListProperty& p)
{
	this->Clear();
	this->Add(p);
	return *this;
}

//**********************************************************************

void StringListProperty::SetValue (const String& value)
{
	m_value = value;

	m_values.clear();
	util::stringtokenizer(m_values, value, _T(";,"));
}

//**********************************************************************

void StringListProperty::SetValues (const StringVec& values)
{
	m_value.clear();
	m_values.clear();
	for (StringVec::const_iterator i = values.begin(); i != values.end(); ++i)
		AddValue(*i);
}

//**********************************************************************

const String& StringListProperty::GetValue (uint pos) const
{
	static const String nullString;
	return pos < (uint) m_values.size() ? m_values[pos] : nullString;
}

//**********************************************************************

void StringListProperty::AddValue (const String& value)
{
	m_values.push_back(value);
	if (!m_value.empty())
		m_value += _T(";");
	m_value += value;
}

//**********************************************************************

void StringListProperty::RemoveValue (const String& value)
{
	StringVec::iterator iter = std::find(m_values.begin(), m_values.end(), value);
	if (iter != m_values.end())
		m_values.erase(iter);
	m_value = util::stringconcat<StringVec, String>(m_values, _T(";"));
}

//**********************************************************************

void StringListProperty::Add (const StringListProperty& prop)
{
	m_values.insert(m_values.end(), prop.m_values.begin(), prop.m_values.end());
	m_value = prop.m_value;
}

//**********************************************************************

void StringListProperty::Clear (void)
{
	m_values.clear();
	m_value.clear();
}

//**********************************************************************

const String StringListProperty::Serialize (void) const { return GetValue(); }

//**********************************************************************

bool StringListProperty::Deserialize (const String& str) {
	SetValue(str);
	return true;
}

////////////////////////////////////////////////////////////////////////
// AggregateProperty class
//
void AggregateProperty::Accept (const std::string& propId, PropertyVisitor* visitor)
	{ visitor->Visit(propId, this); }

void AggregateProperty::Encode (comm::encoder& enc)
{
	PropertyMap::iterator iter = m_properties.begin(), end = m_properties.end();
	for (; iter != end; ++iter)
		iter->second->Encode(enc);
}

//**********************************************************************

void AggregateProperty::Decode (comm::decoder& dec)
{
	PropertyMap::iterator iter = m_properties.begin(), end = m_properties.end();
	for (; iter != end; ++iter)
		iter->second->Decode(dec);
}

bool AggregateProperty::Equal (const Property* prop) const {

	if (GetType() != prop->GetType())
		return false;

	PropertyMap otherProperties = static_cast<const AggregateProperty*>(prop)->m_properties;
	if (otherProperties.size() != m_properties.size())
		return false;

	PropertyMap::const_iterator i = m_properties.begin(), end = m_properties.end();
	for (; i != end; ++i) {
		PropertyMap::const_iterator j = otherProperties.find(i->first);
		if (j == otherProperties.end())
			return false;
		if (!i->second->Equal(j->second))
			return false;
	}

	return true;
}

//**********************************************************************

AggregateProperty::AggregateProperty (const AggregateProperty& p) :
	Property(p)
{
	this->Merge(p);
}

//**********************************************************************

AggregateProperty::~AggregateProperty (void)
{
	this->Clear();
}

//**********************************************************************

AggregateProperty& AggregateProperty::operator= (const AggregateProperty& p)
{
	this->Clear();
	this->Merge(p);
	return *this;
}

//**********************************************************************

void AggregateProperty::AddProperty (const std::string& id, Property* prop)
{
	Property*& currProp = m_properties[id];
	if (currProp)
		delete currProp;
	currProp = prop;
}

//**********************************************************************

void AggregateProperty::RemoveProperty (const std::string& id)
{
	PropertyMap::iterator iter = m_properties.find(id);
	if (iter != m_properties.end())
		m_properties.erase(iter);
}

//**********************************************************************

Property* AggregateProperty::GetProperty (const std::string& id) const
{
	PropertyMap::const_iterator iter = m_properties.find(id);
	if (iter != m_properties.end())
		return iter->second;
	return 0;
}

//**********************************************************************

void AggregateProperty::Merge (const AggregateProperty& prop)
{
	PropertyMap::const_iterator iter = prop.m_properties.begin();
	for (; iter != prop.m_properties.end(); ++iter)
		this->AddProperty(iter->first, iter->second->Clone());
}

//**********************************************************************

void AggregateProperty::Clear (void)
{
	util::for_all(m_properties,
		util::project2nd<PropertyMap::value_type>(util::deleter<Property>())
	);
	m_properties.clear();
}

////////////////////////////////////////////////////////////////////////

} // namespace conf
