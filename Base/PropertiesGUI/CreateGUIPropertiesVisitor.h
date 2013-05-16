/**
 *	CreateGUIPropertiesVisitor.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_CREATEGUIPROPERTIESVISITOR_H
#define CONF_CREATEGUIPROPERTIESVISITOR_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "PropertyVisitor.h"
#include "PropertyTable.h"
#include "PropertyGridPortability.h"

#include <boost/ref.hpp>
#include <vector>

class wxWindow;

namespace conf {

////////////////////////////////////////////////////////////////////////

class _BASE_API CreateGUIPropertiesVisitor : public PropertyVisitor {
public:
	CreateGUIPropertiesVisitor (void) : m_guiProp(0) {}
	~CreateGUIPropertiesVisitor (void) {}

	wxPGProperty* GetGeneratedProperty (void) { return m_guiProp; }

	void Visit (const std::string& id, IntProperty* prop);
	void Visit (const std::string& id, BoolProperty* prop);
	void Visit (const std::string& id, RealProperty* prop);
	void Visit (const std::string& id, StringProperty* prop);
	void Visit (const std::string& id, StdStringProperty* prop);
	void Visit (const std::string& id, IntRangeProperty* prop);
	void Visit (const std::string& id, EnumStringProperty* prop);
	void Visit (const std::string& id, StringListProperty* prop);
	void Visit (const std::string& id, AggregateProperty* prop);

	void Visit (const std::string& id, FontProperty* prop);
	void Visit (const std::string& id, ColorProperty* prop);
	void Visit (const std::string& id, FileProperty* prop);
	void Visit (const std::string& id, DirectoryProperty* prop);
	void Visit (const std::string& id, DateProperty* prop);
	void Visit (const std::string& id, FileListProperty* prop);
	void Visit (const std::string& id, DirectoryListProperty* prop);
	void Visit (const std::string& id, AggregateListProperty* prop);
	void Visit (const std::string& id, MultiChoiceProperty* prop);

private:
	void SetBaseProperties (Property* prop);

	wxPGProperty* m_guiProp;
};

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_CREATEGUIPROPERTIESVISITOR_H
