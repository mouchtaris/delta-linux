/**
 *	PropertyVisitor.h
 *
 * Property visitor that resolves the correct type of a
 * property and calls the appropriate "Visit" function
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_PROPERTYVISITOR_H
#define CONF_PROPERTYVISITOR_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include "Properties.h"
#include "ExtProperties.h"

namespace conf {

////////////////////////////////////////////////////////////////////////

class _BASE_API PropertyVisitor {
public:
	PropertyVisitor (void) {}
	virtual ~PropertyVisitor (void) {}

	virtual void Visit (const std::string& id, IntProperty* prop) = 0;
	virtual void Visit (const std::string& id, BoolProperty* prop) = 0;
	virtual void Visit (const std::string& id, RealProperty* prop) = 0;
	virtual void Visit (const std::string& id, StringProperty* prop) = 0;
	virtual void Visit (const std::string& id, StdStringProperty* prop) = 0;
	virtual void Visit (const std::string& id, IntRangeProperty* prop) = 0;
	virtual void Visit (const std::string& id, EnumStringProperty* prop) = 0;
	virtual void Visit (const std::string& id, StringListProperty* prop) = 0;
	virtual void Visit (const std::string& id, AggregateProperty* prop) = 0;

	virtual void Visit (const std::string& id, FontProperty* prop) = 0;
	virtual void Visit (const std::string& id, ColorProperty* prop) = 0;
	virtual void Visit (const std::string& id, FileProperty* prop) = 0;
	virtual void Visit (const std::string& id, DirectoryProperty* prop) = 0;
	virtual void Visit (const std::string& id, DateProperty* prop) = 0;
	virtual void Visit (const std::string& id, FileListProperty* prop) = 0;
	virtual void Visit (const std::string& id, DirectoryListProperty* prop) = 0;
	virtual void Visit (const std::string& id, AggregateListProperty* prop) = 0;
	virtual void Visit (const std::string& id, MultiChoiceProperty* prop) = 0;
};

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_PROPERTYVISITOR_H
