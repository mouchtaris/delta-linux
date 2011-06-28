/**
 *	DefaultGUIGenerator.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "DefaultGUIGenerator.h"
#include "GenericDialogs.h"
#include "PropertyUtils.h"

#include "CreateGUIPropertiesVisitor.h"
#include "ChangeGUIPropertiesVisitor.h"

#include "Adaptors.h"
#include "Algorithms.h"
#include "StringUtils.h"

#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/toolbar.h>
#include <wx/stattext.h>


#if _MSC_VER == 1600 && defined (NO_THIRD_PARTY_BASE)
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/extras.h>
#include <wx/propgrid/manager.h>

typedef wxMultiChoiceProperty		wxMultiChoicePropertyClass;
#define WX_ARRAY_INT_FROM_VARIANT	wxArrayIntFromVariant
#define WX_PG_ID_IS_OK(ID)			((ID) != NULL)
#define WX_PG_ID_GET_NAME(ID)		(ID)->GetName()
#define wxPGIdToPtr(ID)				(ID)
#define GET_NEXT_SIBLING_PROPERTY	GetNextSiblingProperty

class wxParentPropertyClass: public wxPGProperty {
public:
	wxParentPropertyClass (String const& label) throw():
		wxPGProperty(label, String::FromAscii(""))
		{ }
};

#define WX_PARENT_PROPERTY__ADD_CHILD(PARENT, CHILD)	(PARENT)->AddPrivateChild(CHILD);

#else
#include "propgrid.h"
#include "propdev.h"
#include "advprops.h"
#include "manager.h"

#define WX_ARRAY_INT_FROM_VARIANT	wxPGVariantToArrayInt
#define WX_PG_ID_IS_OK(ID)			ID.IsOk()
#define WX_PG_ID_GET_NAME(ID)		ID.GetName()
#define wxPGIdToPtr(ID)				ID.GetPropertyPtr()
#define GET_NEXT_SIBLING_PROPERTY	GetNextSibling

#define WX_PARENT_PROPERTY__ADD_CHILD(PARENT, CHILD)	(PARENT)->AddChild(CHILD);
#endif
#define wxPGIdGen(PTR)									wxPGId(PTR)

#include "Streams.h"

////////////////////////////////////////////////////////////////////////
// Properties dialog class
//
class PropertiesDialog : public gui::DialogBase {
public:
	typedef conf::DefaultGUIGenerator::ChangeCallback ChangeCallback;

	PropertiesDialog (
		wxWindow*		parent,
		const String&	title,
		const wxSize&	size = wxDefaultSize
	) : gui::DialogBase(parent, title, size) {}

	void SetGUI (wxWindow* gui)
	{
		ButtonList buttons;
		buttons.push_back(ButtonInfo(_("Apply"), wxID_OK));
		buttons.push_back(ButtonInfo(_("Cancel"), wxID_CANCEL));
		this->RealizeDialog(String(), buttons, gui);
	}
	
	void SetChangeCallback(ChangeCallback cb) { m_changeCallback = cb; }

	void OnPropertyGridChange(wxPropertyGridEvent& event)
	{
		if (m_changeCallback)
			m_changeCallback(util::str2std(event.GetPropertyName()));
	}
private:
	ChangeCallback m_changeCallback;
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(PropertiesDialog, wxDialog)
EVT_PG_CHANGED(wxID_ANY, PropertiesDialog::OnPropertyGridChange)
END_EVENT_TABLE()

namespace conf {

////////////////////////////////////////////////////////////////////////

wxWindow* DefaultGUIGenerator::CreateGUIFromProperties (
		wxWindow*				parent,
		const PropertyTable&	propTable
	)
{
	m_propGrid = new wxPropertyGrid(
		parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxNO_BORDER | wxPG_BOLD_MODIFIED | wxTAB_TRAVERSAL | wxPG_TOOLTIPS
	);
	m_propGrid->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);

	this->generateGUI(propTable);
	m_propGrid->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, (long) 1);
	
	int count = 0;
	wxPGId id = m_propGrid->GetFirst();
	while(WX_PG_ID_IS_OK(id)) {
		if (m_propGrid->IsPropertyShown(id)) {
			m_propGrid->Expand(id);
			count += (int) m_propGrid->GetChildrenCount(id) + 1;
		}
		id = m_propGrid->wxPropertyContainerMethods::GET_NEXT_SIBLING_PROPERTY(id);
	}

	// The propertyGrid doesn't calculate the size correctly
	wxSize size(std::min(640, count * 100 + 100), std::min(480, count * 20 + 20));
	m_propGrid->SetMinSize(size);
	m_propGrid->SetSize(size);

	return m_propGrid;
}

//**********************************************************************

wxWindow* DefaultGUIGenerator::CreateGUIFromProperties (
		wxWindow*				parent,
		const PropTableVec&		propTables
	)
{
	m_propGrid = 0;

	wxPropertyGridManager* mngr = new wxPropertyGridManager(
		parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxNO_BORDER | wxPG_BOLD_MODIFIED | wxTAB_TRAVERSAL |
				wxPG_TOOLTIPS | wxPG_TOOLBAR
	);
	mngr->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);

	PropTableVec::const_iterator iter = propTables.begin();
	for (; iter != propTables.end(); ++iter) {
		m_propGridPage = new wxPropertyGridPage;
		mngr->AddPage(iter->get().GetLabel(), wxNullBitmap, m_propGridPage);
		this->generateGUI(*iter);
	}
	mngr->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, (long) 1);

	int count = 0;
	wxPGId id = m_propGrid->GetFirst();
	while(WX_PG_ID_IS_OK(id)) {
		if (m_propGrid->IsPropertyShown(id)) {
			m_propGrid->Expand(id);
			count += (int) m_propGrid->GetChildrenCount(id) + 1;
		}
		id = m_propGrid->wxPropertyContainerMethods::GET_NEXT_SIBLING_PROPERTY(id);
	}

	// The propertyGrid doesn't calculate the size correctly
	count = std::min(10, count);
	wxSize size(640, std::min(480, count * 20 + 20));
	m_propGrid->SetMinSize(size);
	m_propGrid->SetSize(size);

	return mngr;
}

//**********************************************************************

void DefaultGUIGenerator::SetPropertiesFromGUI (
		wxWindow*				gui,
		const PropertyTable&	propTable,
		PropertyIdVec&			changed
	)
{
	m_propGrid = static_cast<wxPropertyGrid*>(gui);
	this->getValuesFromGUI(propTable, changed);
}

//**********************************************************************

void DefaultGUIGenerator::SetPropertiesFromGUI (
		wxWindow*			gui,
		const PropTableVec&	propTables,
		PropertyIdVecVec&	changed
	)
{
	m_propGrid = 0;

	wxPropertyGridManager* pgm = static_cast<wxPropertyGridManager*>(gui);
	changed.resize(propTables.size());

	for (uint i = 0; i < propTables.size(); ++i) {
		m_propGridPage = pgm->GetPage(i);
		this->getValuesFromGUI(propTables[i], changed[i]);
	}
}

//**********************************************************************

void DefaultGUIGenerator::ApplyGUIProperties (void) {
	if (m_activeDialog)
		m_activeDialog->EndModal(wxID_OK);
}

//**********************************************************************

bool DefaultGUIGenerator::ShowDialogFromProperties (
		wxWindow*				parent,
		const PropertyTable&	propTable,
		PropertyIdVec&			changed,
		const String&			title
	)
{
	PropertiesDialog dialog(parent, title);
	wxWindow* gui = this->CreateGUIFromProperties(&dialog, propTable);
	dialog.SetGUI(gui);
	dialog.SetChangeCallback(m_changeCallback);

	m_activeDialog = &dialog;
	bool retval = false;
	if (dialog.ShowModal() == wxID_OK) {
		this->SetPropertiesFromGUI(gui, propTable, changed);
		retval = true;
	}
	m_activeDialog = (gui::DialogBase*) 0;
	return retval;
}

//**********************************************************************

bool DefaultGUIGenerator::ShowDialogFromProperties (
		wxWindow*				parent,
		const PropTableVec&		propTables,
		PropertyIdVecVec&		changed,
		const String&			title
	)
{
	PropertiesDialog dialog(parent, title);
	wxWindow* gui = this->CreateGUIFromProperties(&dialog, propTables);
	dialog.SetGUI(gui);
	dialog.SetChangeCallback(m_changeCallback);

	m_activeDialog = &dialog;
	bool retval = false;
	if (dialog.ShowModal() == wxID_OK) {
		this->SetPropertiesFromGUI(gui, propTables, changed);
		retval = true;
	}
	m_activeDialog = (gui::DialogBase*) 0;
	return retval;
}

//**********************************************************************

wxParentPropertyClass * DefaultGUIGenerator::CreatePGProperty(const AggregateProperty& property, const String& name)
{
	wxParentPropertyClass *pg = new wxParentPropertyClass(name.empty() ? property.GetLabel() : name);
	pg->SetHelpString(property.GetDescription());
	createPGPropertyHelper(property, pg);
	return pg;
}

//**********************************************************************

void DefaultGUIGenerator::createPGPropertyHelper(const AggregateProperty& property, wxParentPropertyClass* parent)
{
	CreateGUIPropertiesVisitor visitor;
	const PropertyMap& props = property.GetPropertyMap();
	for (PropertyMap::const_iterator iter = props.begin(); iter != props.end(); ++iter) {
		wxPGProperty *prop;
		if (iter->second->GetType() == AggregateProperty::Type)
			prop = CreatePGProperty(
				*conf::safe_prop_cast<AggregateProperty>(iter->second),
				util::std2str(iter->first)
			);
		else {
			iter->second->Accept(iter->first, &visitor);
			prop = visitor.GetGeneratedProperty();
		}
		WX_PARENT_PROPERTY__ADD_CHILD(parent, prop);
	}
}

//**********************************************************************

void DefaultGUIGenerator::generateGUI (const AggregateProperty& table, wxPGProperty* propParent)
{
	CreateGUIPropertiesVisitor visitor;
	const PropertyMap& props = table.GetPropertyMap();
	for (PropertyMap::const_iterator iter = props.begin(); iter != props.end(); ++iter) {
		iter->second->Accept(iter->first, &visitor);
		wxPGProperty* prop = this->append(visitor.GetGeneratedProperty(), propParent, iter->second->GetCategory());
		prop->Hide(!iter->second->IsVisible());	//Can only be performed after the property is in a grid
		m_propGrid->EnableProperty(prop, iter->second->IsEnabled());

		if (iter->second->GetType() == AggregateProperty::Type) {
			this->generateGUI(*conf::safe_prop_cast<AggregateProperty>(iter->second), prop);
			this->collapse(prop);
		}
	}
	setSplitterLeft();
}

//**********************************************************************

bool DefaultGUIGenerator::getValuesFromGUI (
		const AggregateProperty&	table,
		PropertyIdVec&				changed,
		const String&				prefix
	)
{
	static ChangeGUIPropertiesVisitor visitor;
	bool changedValue = false;

	const PropertyMap& props = table.GetPropertyMap();
	PropertyMap::const_iterator iter = props.begin();
	for (; iter != props.end(); ++iter) {

		const String name = util::std2str(iter->first);
		bool currentlyChangedValue = false;

		if (iter->second->GetType() != AggregateProperty::Type) {

			visitor.Reset(this->getProperty(name, prefix));
			iter->second->Accept(iter->first, &visitor);
			currentlyChangedValue = visitor.GetChangedValue();
		}
		else {

			currentlyChangedValue = this->getValuesFromGUI(
					*conf::safe_prop_cast<AggregateProperty>(iter->second),
					changed,
					prefix + name + _T(".")
				);
		}

		if (currentlyChangedValue) {
			changedValue = true;
			changed.push_back(util::str2std(prefix) + iter->first);
		}
	}
	return changedValue;
}

//**********************************************************************

static bool PropertyGridHasCategory(wxPropertyGrid* pg, const String& category)
{
	wxPGId id = pg->GetFirstCategory();
	while (WX_PG_ID_IS_OK(id)) {
		if (WX_PG_ID_GET_NAME(id) == category)
			return true;
		id = pg->GetNextCategory(id);
	}
	return false;
}

//**********************************************************************

wxPGProperty* DefaultGUIGenerator::appendToGrid (wxPropertyGrid *pg, bool readOnly, wxPGProperty* pgProp, wxPGProperty* parent, const String& category)
{
	wxPGId id;
	if (!category.empty()) {
		if (!PropertyGridHasCategory(pg, category))
			pg->AppendCategory(category);
		pg->SetCurrentCategory(category);
	}
	if (!parent) {
		id = pg->Append(pgProp);
		pg->EnableProperty(id, !readOnly);
	}
	else {
		id = pg->AppendIn(wxPGIdGen(parent), pgProp);
		pg->EnableProperty(id, !readOnly);
	}
	return wxPGIdToPtr(id);
}

wxPGProperty* DefaultGUIGenerator::appendToGridPage (wxPropertyGridPage *pgPage, bool readOnly, wxPGProperty* pgProp, wxPGProperty* parent, const String& PORT_UNUSED_PARAM(category))
{
	wxPGId id;
	if (!parent) {
		id = pgPage->Append(pgProp);
		pgPage->EnableProperty(id, !readOnly);
	}
	else {
		id = pgPage->AppendIn(wxPGIdGen(parent), pgProp);
		pgPage->EnableProperty(id, !readOnly);
	}
	return wxPGIdToPtr(id);
}

wxPGProperty* DefaultGUIGenerator::append (wxPGProperty* pgProp, wxPGProperty* parent, const String& category)
{
	if (m_propGrid)
		return appendToGrid(m_propGrid, m_readOnly, pgProp, parent, category);
	else {
		assert(m_propGridPage);
		return appendToGridPage(m_propGridPage, m_readOnly, pgProp, parent, category);
	}
}

//**********************************************************************

wxPGProperty* DefaultGUIGenerator::getProperty (const String& name, const String& prefix)
{
	wxPGId id;

	if (prefix.empty()) {

		if (m_propGrid)
			id = m_propGrid->GetPropertyByName(name);
		else if (m_propGridPage)
			id = m_propGridPage->GetPropertyByName(name);
	}
	else {

		if (m_propGrid)
			id = m_propGrid->GetPropertyByName(prefix + name);
		else if (m_propGridPage)
			id = m_propGridPage->GetPropertyByName(prefix + name);
	}
	return wxPGIdToPtr(id);
}

//**********************************************************************

bool DefaultGUIGenerator::collapse (wxPGProperty* pgProp)
{
	if (m_propGrid)
		return m_propGrid->Collapse(wxPGIdGen(pgProp));
	else if (m_propGridPage)
		return m_propGridPage->Collapse(pgProp);
	return false;
}

//**********************************************************************

void DefaultGUIGenerator::setSplitterLeft (void)
{
	if (m_propGrid)
		m_propGrid->SetSplitterLeft();
// else: TODO: m_propGridPage does not have a SetSplitterLeft method...
}

////////////////////////////////////////////////////////////////////////

} // namespace conf
