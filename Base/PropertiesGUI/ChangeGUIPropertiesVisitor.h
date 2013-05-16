/**
 *	ChangeGUIPropertiesVisitor.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_CHANGEGUIPROPERTIESVISITOR_H
#define CONF_CHANGEGUIPROPERTIESVISITOR_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "PropertyVisitor.h"
#include "PropertyTable.h"
#include "PropertyGridPortability.h"

#include <boost/ref.hpp>
#include <vector>

namespace conf {

////////////////////////////////////////////////////////////////////////

class _BASE_API ChangeGUIPropertiesVisitor : public PropertyVisitor {
public:
	ChangeGUIPropertiesVisitor (void);
	~ChangeGUIPropertiesVisitor (void);

	void Reset (wxPGProperty* guiProp);
	bool GetChangedValue (void) const { return m_changedValue; }

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
	void ExtractValuesFromPGProperty (
		const AggregateProperty&	prop,
		wxParentPropertyClass*		pg,
		const String&				prefix = String()
	);

	void VisitStringListProperty (StringListProperty* prop);

	bool			m_changedValue;
	wxPGProperty*	m_guiProp;
};

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_CHANGEGUIPROPERTIESVISITOR_H
