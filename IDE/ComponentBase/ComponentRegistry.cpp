/**
 *	ComponentRegistry.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentRegistry.h"
#include "Component.h"
#include "Algorithms.h"
#include "Adaptors.h"

#include <boost/foreach.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// ComponentRegistry class
//
IMPLEMENT_SINGLETON(ComponentRegistry);

//**********************************************************************

ComponentRegistry::~ComponentRegistry (void)
{
	// Should I safely unregister ?
}

//**********************************************************************

bool ComponentRegistry::Register (const ComponentEntry& entry)
{
	std::pair<ComponentMap::iterator, bool> result = m_components.insert(entry);

	if (result.second) {

		ComponentEntry& addedEntry = const_cast<ComponentEntry&>(*result.first);
		addedEntry.SetAttached();

		this->NotifyRegisteredComponent(addedEntry);

		return true;
	}
	return false;
}

//**********************************************************************

bool ComponentRegistry::Unregister (const std::string& id)
{
	ComponentMap::iterator iter = m_components.find(id);
	if (iter != m_components.end()) {

		const_cast<ComponentEntry&>(*iter).Clear();

		this->NotifyUnregisteredComponent(id);

		// The iterator may have been invalidated by the previous call
		//
		if ((iter = m_components.find(id)) != m_components.end())
			m_components.erase(iter);
		return true;
	}
	return false;
}

//**********************************************************************

bool ComponentRegistry::HasComponentEntry (const std::string& id) const
	{ return m_components.find(id) != m_components.end(); }

//**********************************************************************

ComponentEntry& ComponentRegistry::GetComponentEntry (const std::string& id)
{
	ComponentMap::iterator iter = m_components.find(id);
	if (iter != m_components.end())
		return const_cast<ComponentEntry&>(*iter);
	else {
		this->NotifyRequestedUnregisteredComponent(id);
		if ((iter = m_components.find(id)) != m_components.end())
			return const_cast<ComponentEntry&>(*iter);
		return *GetNullComponentEntry();
	}
}

//**********************************************************************

bool ComponentRegistry::IsValidInstance(Component* component, uint serial) const
{
	InstanceList instances;
	GetAllInstances(instances);
	InstanceList::const_iterator i = std::find(instances.begin(), instances.end(), component);
	return i == instances.end() ? false : (*i)->GetSerial() == serial;
}

//**********************************************************************

void ComponentRegistry::GetAllInstances (InstanceList& result) const
{
	BOOST_FOREACH(const ComponentEntry& entry, m_components) {
		InstanceList instances = entry.GetInstances();
		result.insert(result.end(), instances.begin(), instances.end());
	}
}

//**********************************************************************

void ComponentRegistry::OnInitialize (void)
{

}

//**********************************************************************

void ComponentRegistry::OnCleanUp (void)
{
	assert(BOOST_MULTI_INDEX_CONTAINER_EMPTY(m_components));
	delete GetNullComponentEntry();
}

//**********************************************************************

void ComponentRegistry::NotifyRequestedUnregisteredComponent (const std::string& compId)
{
	sigRequestedUnregisteredComponent(compId);
}

//**********************************************************************

void ComponentRegistry::NotifyRegisteredComponent (const ComponentEntry& entry)
{
	sigRegisteredComponent(entry);
}

void ComponentRegistry::NotifyUnregisteredComponent (const std::string& compId)
{
	sigUnregisteredComponent(compId);
}

//**********************************************************************

void ComponentRegistry::NotifyAddedFunction (const std::string& compId, const ComponentFuncEntry& entry)
{
	sigAddedFunction(compId, entry);
}

void ComponentRegistry::NotifyRemovedFunction (const std::string& compId, const std::string& funcId, bool isSlot, bool isStatic)
{
	sigRemovedFunction(compId, funcId, isSlot, isStatic);
}

//**********************************************************************

void ComponentRegistry::NotifyAddedDynamicFunction (Component* instance, const ComponentFuncEntry& entry)
{
	sigAddedDynamicFunction(instance, entry);
}

void ComponentRegistry::NotifyRemovedDynamicFunction (Component* instance, const std::string& funcId, bool isSlot)
{
	sigRemovedDynamicFunction(instance, funcId, isSlot);
}

//**********************************************************************

void ComponentRegistry::NotifyAddedUserCommand (
		const std::string& compId, const String& path, const UserCommand& userCommand
	)
{
	sigAddedUserCommand(compId, path, userCommand);
}

void ComponentRegistry::NotifyRemovedUserCommand (const std::string& compId, const String& path)
{
	sigRemovedUserCommand(compId, path);
}

void ComponentRegistry::NotifyEnabledUserCommand (const std::string& compId, const String& path)
{
	sigEnabledUserCommand(compId, path);
}

void ComponentRegistry::NotifyDisabledUserCommand (const std::string& compId, const String& path)
{
	sigDisabledUserCommand(compId, path);
}

//**********************************************************************

void ComponentRegistry::NotifyMergedUserCommands (const std::string& compId, const UserCommand& cmds)
{
	sigMergedUserCommands(compId, cmds);
}

void ComponentRegistry::NotifyUnMergedUserCommands (const std::string& compId, const UserCommand& cmds)
{
	sigUnMergedUserCommands(compId, cmds);
}

//**********************************************************************

void ComponentRegistry::NotifyAddedProperty (const std::string& compId, const std::string& id, Property* prop)
{
	sigAddedProperty(compId, id, prop);
}

void ComponentRegistry::NotifyRemovedProperty (const std::string& compId, const std::string& id)
{
	sigRemovedProperty(compId, id);
}

//**********************************************************************

void ComponentRegistry::NotifyAddedSignal (const std::string& compId, const std::string& id)
{
	sigAddedSignal(compId, id);
}

void ComponentRegistry::NotifyRemovedSignal (const std::string& compId, const std::string& id)
{
	sigRemovedSignal(compId, id);
}

//**********************************************************************

void ComponentRegistry::NotifySlotUpdated (const std::string& compId, const ComponentFuncEntry& entry)
{
	sigSlotUpdated(compId, entry);
}

void ComponentRegistry::NotifyDynamicSlotUpdated (Component* instance, const ComponentFuncEntry& entry)
{
	sigDynamicSlotUpdated(instance, entry);
}

//**********************************************************************

void ComponentRegistry::NotifyCreatedFirstInstance (Component* instance)
{
	sigCreatedFirstInstance(instance);
}

void ComponentRegistry::NotifyDestroyedLastInstance (Component* instance)
{
	sigDestroyedLastInstance(instance);
}

//**********************************************************************

void ComponentRegistry::NotifyCreatedInstance (Component* instance)
{
	sigCreatedInstance(instance);
}

void ComponentRegistry::NotifyDestroyedInstance (Component* instance)
{
	sigDestroyedInstance(instance);
}

//**********************************************************************

void ComponentRegistry::NotifyChangedParentInstance (Component* instance, Component* oldParent)
{
	sigChangedParentInstance(instance, oldParent);
}

//**********************************************************************

void ComponentRegistry::NotifyChangedFocus (Component* instance, Component* oldFocused)
{
	sigChangedFocusedInstance(instance, oldFocused);
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
