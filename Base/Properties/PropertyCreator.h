/**
 *	PropertyCreator.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_PROPERTYCREATOR_H
#define CONF_PROPERTYCREATOR_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include <sstream>

namespace conf {

////////////////////////////////////////////////////////////////////////

class PropertyVisitor;

template<class T>
const String serialize (T value) {
	std::ostringstream stream;
	stream << value;
	return util::std2str(stream.str());
}

////////////////////////////////////////////////////////////////////////

template<class T>
bool deserialize (const String& str, T& value) {
	std::istringstream stream(util::str2std(str));
	stream >> std::noskipws >> value;
	return true;
}
template<>
inline bool deserialize<String> (const String& str, String& value) { value = str; return true; }
template<>
inline bool deserialize<std::string> (const String& str, std::string& value) { value = util::str2std(str); return true; }

////////////////////////////////////////////////////////////////////////
// We don't make it a template, because we want to export the type
//
#define IMPLEMENT_PRIMITIVE_PROPERTY_WITH_DEFAULT_VALUE(className, type, defaultVal, id)			\
	class _BASE_API className : public Property {													\
	public:																							\
		CLASS_TYPE_ID(id);																			\
																									\
		typedef type ValueType;																		\
																									\
		className (																					\
			const String&	label,																	\
			PARAM_T(type)	value = defaultVal,														\
			const String&	desc = String(),														\
			const String&	category = String()														\
		) : Property(label, desc, category), m_value(value) {}										\
																									\
		virtual Property*	Clone (void) const { return new className(*this); }						\
		virtual void		Accept (const std::string& propId, PropertyVisitor* visitor);			\
		virtual	void		Encode (comm::encoder& enc);											\
		virtual void		Decode (comm::decoder& dec);											\
		virtual bool		Equal (const Property* prop) const {									\
								return	GetType() == prop->GetType() &&								\
										m_value == static_cast<const className*>(prop)->m_value;	\
							}																		\
		virtual const String	Serialize (void) const;												\
		virtual bool			Deserialize (const String& str);									\
																									\
		void			SetValue (PARAM_T(type) value) { m_value = value; }							\
		PARAM_T(type)	GetValue (void) const { return m_value; }									\
																									\
		static Property* CreateProperty (const String& label, const String& desc)					\
			{ return new className(label, type(), desc); }											\
	private:																						\
		type m_value;																				\
	}

#define IMPLEMENT_PRIMITIVE_PROPERTY(className, type, id) \
	IMPLEMENT_PRIMITIVE_PROPERTY_WITH_DEFAULT_VALUE(className, type, type(), id)

////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_GENERIC_FUNCTIONS(className)										\
	void className::Accept (const std::string& propId, PropertyVisitor* visitor)	\
		{ visitor->Visit(propId, this); }											\
	void className::Encode (comm::encoder& enc)										\
		{ enc << this->GetValue(); }												\
	void className::Decode (comm::decoder& dec)	{									\
		ValueType value;															\
		dec >> value;																\
		this->SetValue(value);														\
	}

#define IMPLEMENT_SERIALIZATION_FUNCTIONS(className)										\
	const String className::Serialize (void) const { return serialize(m_value); }			\
	bool className::Deserialize (const String& str) { return deserialize(str, m_value); }

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_PROPERTYCREATOR_H
