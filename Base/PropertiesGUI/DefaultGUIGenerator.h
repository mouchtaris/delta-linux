/**
 *	DefaultGUIGenerator.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_DEFAULTGUIGENERATOR_H
#define CONF_DEFAULTGUIGENERATOR_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "PropertyVisitor.h"
#include "PropertyTable.h"

#include <boost/ref.hpp>
#include <boost/function.hpp>
#include <vector>

class wxWindow;

#if _MSC_VER == 1600 && defined (NO_THIRD_PARTY_BASE)
#include "ChangeGUIPropertiesVisitor.h" // for wxPropertyGrid types
#include <wx/propgrid/propgrid.h>
#else
class wxPGProperty;
class wxPropertyGrid;
class wxParentPropertyClass;
#endif

class wxPropertyGridPage;
class wxPropertyGridEvent;

namespace gui {
	class DialogBase;
}

namespace conf {

////////////////////////////////////////////////////////////////////////

class _BASE_API DefaultGUIGenerator {
public:
	typedef boost::reference_wrapper<const PropertyTable>	PropertyTableRef;
	typedef std::vector<PropertyTableRef>					PropTableVec;
	typedef std::vector<PropertyIdVec>						PropertyIdVecVec;
	typedef boost::function<void (const std::string&)>		ChangeCallback;

	DefaultGUIGenerator (void) : m_propGrid(0), m_propGridPage(0), m_readOnly(false) {}
	~DefaultGUIGenerator (void) {}

	//******************************************************************

	void SetReadOnly (bool value = true) { m_readOnly = value; }
	bool GetReadOnly (void) const { return m_readOnly; }

	//******************************************************************

	// The first group 'o' functions creates a gui from a property table, while
	// the second updates the properties according to the changes.
	//
	// To the "changed" parameters, all the ids of the changed properties are stored.
	// If changes.empty(), then no changes were performed
	//
	wxWindow* CreateGUIFromProperties (wxWindow* parent, const PropertyTable& propTable);
	wxWindow* CreateGUIFromProperties (wxWindow* parent, const PropTableVec& propTables);

	void SetPropertiesFromGUI (
			wxWindow*				gui,
			const PropertyTable&	propTable,
			PropertyIdVec&			changed
		);

	void SetPropertiesFromGUI (
			wxWindow*				gui,
			const PropTableVec&		propTables,
			PropertyIdVecVec&		changed
		);

	void SetOnConfigurationDialogChange(ChangeCallback cb) { m_changeCallback = cb; }
	void ApplyGUIProperties(void);
	
	// Popups a dialog for changing the given property table.
	//
	bool ShowDialogFromProperties (
			wxWindow*				parent,
			const PropertyTable&	propTable,
			PropertyIdVec&			changed,
			const String&			title = String()
		);

	bool ShowDialogFromProperties (
			wxWindow*				parent,
			const PropTableVec&		propTables,
			PropertyIdVecVec&		changed,
			const String&			title = String()
		);

	static wxParentPropertyClass *	CreatePGProperty(const AggregateProperty& property, const String& name = String());
private:
	static void createPGPropertyHelper(const AggregateProperty& property, wxParentPropertyClass* propParent);

	void generateGUI (const AggregateProperty& table, wxPGProperty* propParent = 0);

	bool getValuesFromGUI (
			const AggregateProperty&	table,
			PropertyIdVec&				changed,
			const String&				prefix = String()
		);

	static wxPGProperty* appendToGrid		(wxPropertyGrid* pg, bool readOnly, wxPGProperty* pgProp, wxPGProperty* parent, const String& category);
	static wxPGProperty* appendToGridPage	(wxPropertyGridPage* pgPage, bool readOnly, wxPGProperty* pgProp, wxPGProperty* parent, const String& category);

	wxPGProperty*	append (wxPGProperty* pgProp, wxPGProperty* parent, const String& category);
	wxPGProperty*	getProperty (const String& name, const String& prefix);
	bool			collapse (wxPGProperty* pgProp);
	void			setSplitterLeft (void);

	wxPropertyGrid*		m_propGrid;
	wxPropertyGridPage*	m_propGridPage;
	bool				m_readOnly;
	gui::DialogBase*	m_activeDialog;
	ChangeCallback		m_changeCallback;
};

////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_DEFAULTGUIGENERATOR_H
