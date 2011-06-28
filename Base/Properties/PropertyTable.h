/**
 *	PropertyTable.h
 *
 * Commonly used functionality for properties
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_PROPERTYTABLE_H
#define CONF_PROPERTYTABLE_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "Properties.h"

namespace conf {

////////////////////////////////////////////////////////////////////////

class _BASE_API PropertyTable : public AggregateProperty {
public:
	PropertyTable (const String& label = String(), const String& desc = String()) :
		AggregateProperty(label, desc), m_dirty(false) {}
	PropertyTable (const PropertyTable& p) :
		AggregateProperty(p), m_dirty(false) {}
	~PropertyTable (void) {}

	void SetDirty (bool value) const { m_dirty = value; }
	bool GetDirty (void) const { return m_dirty; }

	bool LoadValues (const String& uri);
	bool SaveValues (const String& uri);

	virtual Property* Clone (void) const { return new PropertyTable(*this); }

private:
	mutable bool m_dirty;
};

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_PROPERTYTABLE_H
