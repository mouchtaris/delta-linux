/**
 *	ComponentConfigurationDialog.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentConfigurationDialog.h"
#include "Component.h"
#include "CommonGUIUtils.h"
#include "ComponentRegistry.h"
#include "Algorithms.h"
#include "Call.h"
#include "ComponentHandle.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

ComponentConfigurationDialog::ComponentConfigurationDialog (wxWindow* parent) :
	m_parent(parent)
{

}

//**********************************************************************

bool ComponentConfigurationDialog::Configure (void)
{
	conf::DefaultGUIGenerator::PropTableVec props;
	conf::PropertyVec old;

	const ComponentRegistry::ComponentMap& cmap = ComponentRegistry::Instance().GetComponentMap();
	ComponentRegistry::ComponentMap::const_iterator iter = cmap.begin();
	for (; iter != cmap.end(); ++iter) {
		const conf::PropertyTable& table = iter->GetPropertyTable();
		props.push_back(conf::DefaultGUIGenerator::PropertyTableRef(table));
		old.push_back(table.Clone());
	}

	conf::DefaultGUIGenerator::PropertyIdVecVec changed;
	bool retval = m_dgg.ShowDialogFromProperties(m_parent, props, changed, _("Configure..."));

	if (!changed.empty()) {
		uint i;
		for (i = 0, iter = cmap.begin(); iter != cmap.end(); ++i, ++iter)
			if (!changed[i].empty())
				iter->NotifyPropertiesChanged(
					*static_cast<conf::PropertyTable*>(old[i]),
					changed[i]
				);
	}

	for(conf::PropertyVec::iterator iter = old.begin(); iter != old.end(); ++iter)
		delete *iter;

	return retval;
}

//**********************************************************************

bool ComponentConfigurationDialog::Configure (ComponentEntry& entry)
{
	conf::PropertyIdVec changed;
	const conf::PropertyTable& table = entry.GetPropertyTable();
	conf::PropertyTable* old = static_cast<conf::PropertyTable*>(table.Clone());
	bool retval = m_dgg.ShowDialogFromProperties(m_parent, table, changed, entry.GetMetadata().name);

	if (!changed.empty())
		entry.NotifyPropertiesChanged(*old, changed);

	delete old;
	return retval;
}

//**********************************************************************

bool ComponentConfigurationDialog::Configure (const std::string& classId)
{
	return this->Configure(ComponentRegistry::Instance().GetComponentEntry(classId));
}

//**********************************************************************

bool ComponentConfigurationDialog::ConfigureInstance (Component* component)
{
	conf::PropertyIdVec changed;
	const conf::PropertyTable& table = component->GetInstancePropertyTable();
	conf::PropertyTable* old = static_cast<conf::PropertyTable*>(table.Clone());

	String basePath;
	if (ComponentRegistry::Instance().GetComponentEntry(component->GetClassId()).HasFunction("GetPath"))
		basePath = Call<const String (void)>("ComponentConfigurationDialog", component, "GetPath")();

	bool retval = m_dgg.ShowDialogFromProperties(m_parent, table, changed, component->GetName(), basePath);

	if (!changed.empty())
		component->ApplyChangedProperties(*old, changed);

	delete old;
	return retval;
}

//**********************************************************************

void ComponentConfigurationDialog::SetOnConfigurationDialogChange (ChangeCallback cb)
	{ m_dgg.SetOnConfigurationDialogChange(cb); }	

//**********************************************************************

void ComponentConfigurationDialog::ApplyConfiguration (void)
	{ m_dgg.ApplyGUIProperties(); }

////////////////////////////////////////////////////////////////////////

} // namespace ide
