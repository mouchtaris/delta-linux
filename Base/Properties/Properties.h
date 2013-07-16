/**
 *	Properties.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_PROPERTY_H
#define CONF_PROPERTY_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "PropertyCreator.h"

#include <map>
#include <vector>

#include <boost/mpl/vector.hpp>

namespace comm {
	class encoder;
	class decoder;
}

namespace conf {

////////////////////////////////////////////////////////////////////////

class PropertyVisitor;

////////////////////////////////////////////////////////////////////////

class _BASE_API Property {
public:
	CLASS_TYPE_ID(0);

	Property (
		const String& label, const String& desc, const String& category = String(),
		bool visible = true, bool enabled = true
	) :	m_label(label), m_desc(desc), m_category(category), m_visible(visible), m_enabled(enabled) {}
	virtual ~Property (void) {}

	virtual Property*	Clone (void) const = 0;
	virtual void		Accept (const std::string& propId, PropertyVisitor* visitor) = 0;
	virtual	void		Encode (comm::encoder& enc) = 0;
	virtual void		Decode (comm::decoder& dec) = 0;
	virtual bool		Equal (const Property* prop) const = 0;

	virtual const String	Serialize (void) const { return String(); }
	virtual bool			Deserialize (const String& str) { return false; }

	void			SetLabel (const String& label) { m_label = label; }
	const String&	GetLabel (void) const { return m_label; }

	void			SetDescription (const String& desc) { m_desc = desc; }
	const String&	GetDescription (void) const { return m_desc; }

	void			SetCategory (const String& category) { m_category = category; }
	const String&	GetCategory (void) const { return m_category; }

	void			SetVisible (bool val = true) { m_visible = val; }
	bool			IsVisible (void) const { return m_visible; }

	void			SetEnabled (bool val = true) { m_enabled = val; }
	bool			IsEnabled (void) const { return m_enabled; }

private:
	String	m_label;
	String	m_desc;
	String	m_category;
	bool	m_visible;
	bool	m_enabled;
};

////////////////////////////////////////////////////////////////////////

class _BASE_API PropertyMap : public std::map<const std::string, Property*> {};
class _BASE_API PropertyVec : public std::vector<Property*> {};
class _BASE_API PropertyList : public std::list<Property*> {};

// The following produces a linker symbol redefinition error when
// std::vector<std::string> is used in another dll
//
// class _BASE_API PropertyIdVec : public std::vector<std::string> {};
//
typedef std::vector<std::string> PropertyIdVec;

////////////////////////////////////////////////////////////////////////

#pragma warning (push)
#pragma warning (disable:4251)

IMPLEMENT_PRIMITIVE_PROPERTY(IntProperty, int, 1);
IMPLEMENT_PRIMITIVE_PROPERTY(BoolProperty, bool, 2);
IMPLEMENT_PRIMITIVE_PROPERTY(RealProperty, double, 3);
IMPLEMENT_PRIMITIVE_PROPERTY(StringProperty, String, 4);
IMPLEMENT_PRIMITIVE_PROPERTY(StdStringProperty, std::string, 5);

#pragma warning (pop)

////////////////////////////////////////////////////////////////////////

class _BASE_API IntRangeProperty : public Property {
public:
	CLASS_TYPE_ID(6);

	typedef int ValueType;

	IntRangeProperty (
		const String& label, int min, int max, int value, const String& desc = String(), const String& category = String()
	) : Property(label, desc, category), m_min(min), m_max(max), m_value(value) {}
	~IntRangeProperty (void) {}

	//******************************************************************

	virtual Property*	Clone (void) const { return new IntRangeProperty(*this); }
	virtual void		Accept (const std::string& propId, PropertyVisitor* visitor);
	virtual	void		Encode (comm::encoder& enc);
	virtual void		Decode (comm::decoder& dec);
	virtual bool		Equal (const Property* prop) const {
							return	GetType() == prop->GetType() && 
									m_min == static_cast<const IntRangeProperty*>(prop)->m_min &&
									m_max == static_cast<const IntRangeProperty*>(prop)->m_max &&
									m_value == static_cast<const IntRangeProperty*>(prop)->m_value;
						}

	virtual const String	Serialize (void) const;
	virtual bool			Deserialize (const String& str);

	//******************************************************************

	void	SetMin (int min) { m_min = min; }
	int		GetMin (void) const { return m_min; }

	void	SetMax (int max) { m_max = max; }
	int		GetMax (void) const { return m_max; }

	void	SetValue (int value) { m_value = util::clamp(value, m_min, m_max); }
	int		GetValue (void) const { return m_value; }

private:
	int m_min;
	int m_max;
	int m_value;
};

////////////////////////////////////////////////////////////////////////

class _BASE_API EnumStringProperty : public Property {
public:
	CLASS_TYPE_ID(7);

	typedef String::size_type	size_type;
	typedef String				ValueType;

	EnumStringProperty (
		const String& label, const StringVec& options, uint option, const String& desc = String(), const String& category = String()
	) : Property(label, desc, category), m_options(options), m_index(option) {}
	~EnumStringProperty (void) {}

	//******************************************************************

	virtual Property*	Clone (void) const { return new EnumStringProperty(*this); }
	virtual void		Accept (const std::string& propId, PropertyVisitor* visitor);
	virtual	void		Encode (comm::encoder& enc);
	virtual void		Decode (comm::decoder& dec);
	virtual bool		Equal (const Property* prop) const {
							return	GetType() == prop->GetType() && 
									m_options == static_cast<const EnumStringProperty*>(prop)->m_options &&
									m_index == static_cast<const EnumStringProperty*>(prop)->m_index;
						}

	virtual const String	Serialize (void) const;
	virtual bool			Deserialize (const String& str);

	//******************************************************************

	void				SetOption (size_type index);
	size_type			GetOption (void) const { return m_index; }
	const StringVec&	GetOptions (void) const { return m_options; }

	void			SetValue (const String& value);
	const String&	GetValue (void) const { return m_options[m_index]; }

private:
	StringVec	m_options;
	size_type	m_index;
};

////////////////////////////////////////////////////////////////////////

class _BASE_API StringListProperty : public Property {
public:
	CLASS_TYPE_ID(8);

	typedef String ValueType;

	StringListProperty (const String& label, const String& desc = String(), const String& category = String()) :
		Property(label, desc, category) {}
	StringListProperty (const StringListProperty& p);
	virtual ~StringListProperty (void);

	StringListProperty& operator= (const StringListProperty& p);

	//******************************************************************

	virtual Property*	Clone (void) const { return new StringListProperty(*this); }
	virtual void		Accept (const std::string& propId, PropertyVisitor* visitor);
	virtual	void		Encode (comm::encoder& enc);
	virtual void		Decode (comm::decoder& dec);
	virtual bool		Equal (const Property* prop) const {
							return	GetType() == prop->GetType() && 
									m_values == static_cast<const StringListProperty*>(prop)->m_values &&
									m_value == static_cast<const StringListProperty*>(prop)->m_value;
						}

	virtual const String	Serialize (void) const;
	virtual bool			Deserialize (const String& str);

	//******************************************************************

	void				SetValue (const String& value);
	const String&		GetValue (void) const { return m_value; }

	void				SetValues (const StringVec& values);
	const StringVec&	GetValues (void) const { return m_values; }

	const String&		GetValue (uint pos) const;
	uint				GetValuesCount (void) const { return (uint) m_values.size(); }

	void AddValue (const String& value);
	void RemoveValue (const String& value);

	//******************************************************************

	void Add (const StringListProperty& prop);
	void Clear (void);

protected:
	String		m_value;
	StringVec	m_values;
};

////////////////////////////////////////////////////////////////////////

class _BASE_API AggregateProperty : public Property {
public:
	CLASS_TYPE_ID(9);

	AggregateProperty (const String& label, const String& desc = String(), const String& category = String()) :
		Property(label, desc, category) {}
	AggregateProperty (const AggregateProperty& p);
	~AggregateProperty (void);

	AggregateProperty& operator= (const AggregateProperty& p);

	//******************************************************************

	virtual Property*	Clone (void) const { return new AggregateProperty(*this); }
	virtual void		Accept (const std::string& propId, PropertyVisitor* visitor);
	virtual	void		Encode (comm::encoder& enc);
	virtual void		Decode (comm::decoder& dec);
	virtual bool		Equal (const Property* prop) const;

	void AddProperty (const std::string& id, Property* prop); // Takes ownership !!!
	void RemoveProperty (const std::string& id);

	//******************************************************************

	Property*			GetProperty (const std::string& id) const; // Do not delete !!!
	PropertyMap&		GetPropertyMap (void) { return m_properties; }
	const PropertyMap&	GetPropertyMap (void) const { return m_properties; }

	//******************************************************************

	void Merge (const AggregateProperty& prop);
	void Clear (void);

private:
	PropertyMap m_properties;
};

////////////////////////////////////////////////////////////////////////
// Type vector with all the properties. Mind the ORDER !!!
//
typedef boost::mpl::vector<
	Property,
	IntProperty,
	BoolProperty,
	RealProperty,
	StringProperty,
	IntRangeProperty,
	EnumStringProperty,
	StringListProperty,
	AggregateProperty
> PropertyTypeVec;

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_PROPERTY_H
