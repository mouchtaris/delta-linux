/**
 *	ExtProperties.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_EXTPROPERTIES_H
#define CONF_EXTPROPERTIES_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include "Properties.h"
#include "PropertyCreator.h"

#include <set>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/datetime.h>

namespace conf {

////////////////////////////////////////////////////////////////////////

class PropertyVisitor;

////////////////////////////////////////////////////////////////////////

#pragma warning (push)
#pragma warning (disable:4251)

IMPLEMENT_PRIMITIVE_PROPERTY(FontProperty, wxFont, 10);
IMPLEMENT_PRIMITIVE_PROPERTY(ColorProperty, wxColour, 11);
IMPLEMENT_PRIMITIVE_PROPERTY(FileProperty, String, 12);
IMPLEMENT_PRIMITIVE_PROPERTY(DirectoryProperty, String, 13);
IMPLEMENT_PRIMITIVE_PROPERTY_WITH_DEFAULT_VALUE(DateProperty, wxDateTime, wxDateTime::Now(), 14);

////////////////////////////////////////////////////////////////////////

class _BASE_API FileListProperty : public StringListProperty {
public:
	CLASS_TYPE_ID(15);

	FileListProperty (const String& label, const String& desc = String(), const String& category = String()) :
		StringListProperty(label, desc, category) {}
	FileListProperty (const FileListProperty& p);
	~FileListProperty (void) {}

	FileListProperty& operator= (const FileListProperty& p);

	//******************************************************************

	virtual void		Accept (const std::string& propId, PropertyVisitor* visitor);
	virtual Property*	Clone (void) const { return new FileListProperty(*this); }
};

////////////////////////////////////////////////////////////////////////

class _BASE_API DirectoryListProperty : public StringListProperty {
public:
	CLASS_TYPE_ID(16);

	DirectoryListProperty (const String& label, const String& desc = String(), const String& category = String()) :
		StringListProperty(label, desc, category) {}
	DirectoryListProperty (const DirectoryListProperty& p);
	~DirectoryListProperty (void) {}

	DirectoryListProperty& operator= (const DirectoryListProperty& p);

	//******************************************************************

	virtual void		Accept (const std::string& propId, PropertyVisitor* visitor);
	virtual Property*	Clone (void) const { return new DirectoryListProperty(*this); }
};

////////////////////////////////////////////////////////////////////////

class _BASE_API AggregateListProperty : public Property {
public:
	CLASS_TYPE_ID(17);

	typedef std::list<AggregateProperty*> AggregatePropertyList;
	
	AggregateListProperty (const String& label, AggregateProperty* type, const String& desc = String(), const String& category = String()) :
		Property(label, desc, category), m_type(type) {}
	AggregateListProperty (const AggregateListProperty& p);
	~AggregateListProperty (void);

	AggregateListProperty& operator= (const AggregateListProperty& p);

	//******************************************************************

	virtual Property*	Clone (void) const { return new AggregateListProperty(*this); }
	virtual void		Accept (const std::string& propId, PropertyVisitor* visitor);
	virtual	void		Encode (comm::encoder& enc);
	virtual void		Decode (comm::decoder& dec);
	virtual bool		Equal (const Property* prop) const;

	//******************************************************************

	const StringVec					GetValues (void) const;
	const AggregateProperty*		GetListType(void) const { return m_type; }

	AggregatePropertyList&			GetPropertyList(void) { return m_properties; }
	const AggregatePropertyList&	GetPropertyList(void) const { return m_properties; }

	//******************************************************************

	AggregateProperty*	NewProperty(void);
	void Clear (void);

private:
	AggregateProperty*		m_type;
	AggregatePropertyList	m_properties;
};

////////////////////////////////////////////////////////////////////////

class _BASE_API MultiChoiceProperty : public Property {
public:
	CLASS_TYPE_ID(18);

	typedef std::map<String, bool>	ChoiceMap;
	typedef String					ValueType;
	typedef std::set<String>		StringSet;

	MultiChoiceProperty (
		const String& label, const ChoiceMap& choices = ChoiceMap(), const String& desc = String(), const String& category = String()
	) : Property(label, desc, category), m_choices(choices), m_allowExtraSelections(false) {}
	~MultiChoiceProperty (void);

	//******************************************************************

	virtual Property*	Clone (void) const { return new MultiChoiceProperty(*this); }
	virtual void		Accept (const std::string& propId, PropertyVisitor* visitor);
	virtual	void		Encode (comm::encoder& enc);
	virtual void		Decode (comm::decoder& dec);
	virtual bool		Equal (const Property* prop) const {
							const MultiChoiceProperty* p;
							return	GetType() == prop->GetType()												&&
									m_choices == (p = static_cast<const MultiChoiceProperty*>(prop))->m_choices &&
									m_allowExtraSelections == p->m_allowExtraSelections							&&
									m_extraSelections == p->m_extraSelections									;
						}

	virtual const String	Serialize (void) const;
	virtual bool			Deserialize (const String& str);

	//******************************************************************
	
	void				SetValue (const String& value);
	const String		GetValue (void) const;

	const StringVec		GetAllChoices (void) const;
	const StringVec		GetSelectedChoices (void) const;

	void				SetAllowExtraSelections (bool val) { m_allowExtraSelections = val; }
	bool				AllowExtraSelections (void) const { return m_allowExtraSelections; }
	const StringSet&	GetExtraSelections (void) const { return m_extraSelections; }

	ChoiceMap&			GetChoiceMap (void) { return m_choices; }
	const ChoiceMap&	GetChoiceMap (void) const { return m_choices; }

	//******************************************************************

	bool AddChoice (const String& value);
	void RemoveChoice (const String& value);

	void SelectChoice (const String& value);
	void DeselectChoice (const String& value);

	void Clear (bool keepExtraSelections = false);

private:
	ChoiceMap	m_choices;
	bool		m_allowExtraSelections;
	StringSet	m_extraSelections;
};

////////////////////////////////////////////////////////////////////////

#pragma warning (pop)

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_EXTPROPERTIES_H
