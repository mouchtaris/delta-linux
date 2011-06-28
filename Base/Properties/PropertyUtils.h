/**
 *	PropertyUtils.h
 *
 * Commonly used functionality for properties
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_PROPERTYUTILS_H
#define CONF_PROPERTYUTILS_H

#include "Common.h"
#include "Properties.h"

namespace conf {

////////////////////////////////////////////////////////////////////////

template <class TPropDerived, class TPropBase>
inline TPropDerived* safe_prop_cast (TPropBase* prop)
{
	assert(prop && prop->GetType() == TPropDerived::Type);
	return static_cast<TPropDerived*>(prop);
}

//**********************************************************************

template <class TProp>
inline PARAM(typename TProp::ValueType) get_prop_value (const Property* prop)
{
	return safe_prop_cast<const TProp>(prop)->GetValue();
}

//**********************************************************************

template <class TProp>
inline PARAM(typename TProp::ValueType) get_prop_value (
		const Property*						prop,
		PARAM(typename TProp::ValueType)	defaultValue
	)
{
	if (prop && prop->GetType() == TProp::Type)
		return static_cast<const TProp*>(prop)->GetValue();
	else
		return defaultValue;
}

//**********************************************************************

template <class TProp>
inline void set_prop_value (Property* prop, PARAM(typename TProp::ValueType) value)
{
	safe_prop_cast<TProp>(prop)->SetValue(value);
}

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_PROPERTYUTILS_H
