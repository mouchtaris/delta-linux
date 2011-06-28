/**
 *	CreateGUIPropertiesVisitor.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "CreateGUIPropertiesVisitor.h"
#include "GenericDialogs.h"
#include "DefaultGUIGenerator.h"
#include "Adaptors.h"
#include "Algorithms.h"

#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/toolbar.h>
#include <wx/stattext.h>

#include "propgrid.h"
#include "propdev.h"
#include "advprops.h"
#include "extraprops.h"
#include "manager.h"

namespace conf {

////////////////////////////////////////////////////////////////////////

void CreateGUIPropertiesVisitor::Visit (const std::string& id, IntProperty* prop)
{
	m_guiProp = wxIntProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, BoolProperty* prop)
{
	m_guiProp = wxBoolProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, RealProperty* prop)
{
	m_guiProp = wxFloatProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, StringProperty* prop)
{
	m_guiProp = wxStringProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, StdStringProperty* prop)
{
	m_guiProp = wxStringProperty(prop->GetLabel(), util::std2str(id),
		util::std2str(prop->GetValue()));
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, IntRangeProperty* prop)
{
	m_guiProp = wxIntProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, EnumStringProperty* prop)
{
	wxArrayString options;
	StringVec::const_iterator iter = prop->GetOptions().begin();
	for (; iter != prop->GetOptions().end(); ++iter)
		options.Add(*iter);
	m_guiProp = wxEnumProperty(prop->GetLabel(), util::std2str(id), options,
		(int) prop->GetOption());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, StringListProperty* prop)
{
	wxArrayString strings;
	StringVec::const_iterator iter = prop->GetValues().begin();
	for (; iter != prop->GetValues().end(); ++iter)
		strings.Add(*iter);
	m_guiProp = wxArrayStringProperty(prop->GetLabel(), util::std2str(id), strings);
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, AggregateProperty* prop)
{
	m_guiProp = wxPropertyCategory(prop->GetLabel(), util::std2str(id));
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, FontProperty* prop)
{
	m_guiProp = wxFontProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, ColorProperty* prop)
{
	m_guiProp = wxColourProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, FileProperty* prop)
{
	m_guiProp = wxFileProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, DirectoryProperty* prop)
{
	m_guiProp = wxDirProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, DateProperty* prop)
{
	m_guiProp = wxDateProperty(prop->GetLabel(), util::std2str(id), prop->GetValue());
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, FileListProperty* prop)
{
	wxArrayString strings;
	StringVec::const_iterator iter = prop->GetValues().begin();
	for (; iter != prop->GetValues().end(); ++iter)
		strings.Add(*iter);
	m_guiProp = wxFileListProperty(prop->GetLabel(), util::std2str(id), strings);
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, DirectoryListProperty* prop)
{
	wxArrayString strings;
	StringVec::const_iterator iter = prop->GetValues().begin();
	for (; iter != prop->GetValues().end(); ++iter)
		strings.Add(*iter);
	m_guiProp = wxDirectoryListProperty(prop->GetLabel(), util::std2str(id), strings);
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, AggregateListProperty* prop)
{
	wxArrayString strings;
	const AggregateListProperty::AggregatePropertyList& properties = prop->GetPropertyList();
	AggregateListProperty::AggregatePropertyList::const_iterator iter = properties.begin(), end = properties.end();
	for(; iter != end; ++iter) {
		wxParentPropertyClass* p = DefaultGUIGenerator::CreatePGProperty(**iter);
		strings.Add(p->GetValueAsString(0));
		delete p;
	}

	m_guiProp = wxGenericListProperty(
		prop->GetLabel(),
		util::std2str(id),
		strings,
		DefaultGUIGenerator::CreatePGProperty(*prop->GetListType())
	);
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::Visit (const std::string& id, MultiChoiceProperty* prop)
{
	wxArrayString choices;
	wxArrayInt values;
	const MultiChoiceProperty::ChoiceMap& choiceMap = prop->GetChoiceMap();
	MultiChoiceProperty::ChoiceMap::const_iterator iter = choiceMap.begin(), end = choiceMap.end();
	for (int count = 0; iter != end; ++iter, ++count) {
		choices.Add(iter->first);
		if (iter->second)
			values.Add(count);
	}
	m_guiProp = wxMultiChoiceProperty(prop->GetLabel(), util::std2str(id), choices, values);
	SetBaseProperties(prop);
}

//**********************************************************************

void CreateGUIPropertiesVisitor::SetBaseProperties (Property* prop)
{
	m_guiProp->SetHelpString(prop->GetDescription());
}

////////////////////////////////////////////////////////////////////////

} // namespace conf
