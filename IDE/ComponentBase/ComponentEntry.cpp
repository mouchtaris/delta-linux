/**
 *	ComponentEntry.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "Portability.h"

#include "ComponentEntry.h"
#include "ComponentSignalRegistry.h"
#include "ComponentRegistry.h"
#include "PathManager.h"
#include "Component.h"
#include "DelayedCaller.h"

#include "Algorithms.h"
#include "Adaptors.h"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// Default metadata
//
static ComponentEntry::Metadata s_data = {
	_T("Generic component"),
	_T("This generic component does generic stuff in a generic way"),
	_T("Joe Doe <joe@nasa.gov>"),
	_T("1.0")
};

////////////////////////////////////////////////////////////////////////
// ComponentEntry class
//
ComponentEntry::ComponentEntry (
		const std::string&	classId,
		CreationFunc		create,
		DestructionFunc		destroy,
		SetPropertiesFunc	prop,
		const std::string&	baseClassId
	) : m_metadata(&s_data), m_metadataCopy(false),
		m_create(create), m_destroy(destroy), m_prop(prop),
		m_instanceSerial(0), m_focusedInstance(0),
		m_classId(classId), m_baseClassId(baseClassId), m_attached(false)
{
	if (!baseClassId.empty())
		this->getBaseComponentEntry().m_derivedClasses.push_back(classId);
}

//**********************************************************************

ComponentEntry::ComponentEntry (void) :
	m_metadata(&s_data), m_metadataCopy(false), m_create(0), m_destroy(0), m_prop(0),
	m_instanceSerial(0), m_focusedInstance(0), m_attached(false)
{

}

//**********************************************************************

ComponentEntry::~ComponentEntry()
{
	if (m_attached)
		ComponentRegistry::Instance().sigAddedSignal.disconnect(
			boost::bind(&ComponentEntry::UpdateSlots, this, _1, _2)
		);
}

//**********************************************************************

Component* ComponentEntry::Create (void)
{
	if (m_create) {
		Component* component = m_create(*this);
		if (component) {
			uint serial = ++m_instanceSerial;
			component->SetSerial(serial);

			bool isTheVeryFirstInstance = m_instances.empty();

			if (isTheVeryFirstInstance) {
				m_focusedInstance = component->GetSerial();
				component->SetFocus();
			}
			m_instances.push_back(component);

			if (m_attached)
				ComponentRegistry::Instance().NotifyCreatedInstance(component);

			if (m_attached && isTheVeryFirstInstance)
				ComponentRegistry::Instance().NotifyCreatedFirstInstance(component);

			component->NotifyCreated();
			if (!ComponentRegistry::Instance().IsValidInstance(component, serial))
				component = 0;
		}
		return component;
	}
	return 0;
}

//**********************************************************************

void ComponentEntry::Destroy (Component* component) {

#if 0	// TODO
	if (component->IsUnderDestruction())
		return;
#endif

	if (m_destroy) {	// TODO: will remove
		assert(component && component->GetClassId() == m_classId);

		bool isFocusedInstance = m_focusedInstance == component->GetSerial();
		component->SetInDestruction();

		if (m_attached)
			ComponentRegistry::Instance().NotifyDestroyedInstance(component);

		bool isTheVeryLastInstance = m_instances.size() == 1 && m_instances.front() == component;
		if (m_attached && isTheVeryLastInstance)
			ComponentRegistry::Instance().NotifyDestroyedLastInstance(component);

		m_destroy(component);	// TODO: will remove

#if 0	// TODO
		component->ExplicitDestructor();
		component->SetSerial(0);
		timer::DelayedCaller::Instance().PostDelayedCall(boost::bind(&Component::DeleteMemory, component));
#endif

		m_instances.remove(component);

		// Focus another component if this one died
		if (isFocusedInstance) {

			if (m_instances.empty())
				m_focusedInstance = 0;
			else {
				m_focusedInstance = m_instances.back()->GetSerial();
				m_instances.back()->SetFocus();
			}
		}
	}
}

//**********************************************************************

void ComponentEntry::Init (void)
{
	this->ResetProperties();

	if (!m_baseClassId.empty())
		this->inheritUserCommands(this->getBaseComponentEntry());
}

//**********************************************************************

void ComponentEntry::Clear (void)
{
	// Doesn't handle correctly hierarchies of the same component classes
	//
	// util::for_all_safe(m_instances, boost::bind(&ComponentEntry::Destroy, this, _1));

	// Destroy all instances
	//
	while (!m_instances.empty())
		m_instances.back()->Destroy();

	// Disconnect all slots
	//
	ComponentSignalRegistry::Instance().DisconnectAllSlots(m_classId);

	// Save and clear properties
	//
	m_properties.SaveValues(this->getConfigURI(_T("Properties")));
	m_properties.Clear();

	// Clear metadata
	//
	if (m_metadataCopy)
		delete m_metadata;
	m_metadata = &s_data;
	m_metadataCopy = false;

	BOOST_MULTI_INDEX_CONTAINER_CLEAR(m_functions);
}

//**********************************************************************

void ComponentEntry::SetParent (Component* component, Component* parent)
{
	Component* oldParent = component->GetParent();
	component->ChangeParent(parent);

	if (m_attached)
		ComponentRegistry::Instance().NotifyChangedParentInstance(component, oldParent);
}

//**********************************************************************

void ComponentEntry::SetFocus (Component* component)
{
	assert(component->GetClassId() == m_classId);

	Component* oldFocused = this->GetFocusedInstance();
	m_focusedInstance = component->GetSerial();

	component->SetFocus();

	if (m_attached)
		ComponentRegistry::Instance().NotifyChangedFocus(component, oldFocused);
}

//**********************************************************************

const ComponentEntry::InstanceList ComponentEntry::GetInstances (bool includeDerivedInstances) const {
	InstanceList instances = m_instances;
	if (includeDerivedInstances) {
		BOOST_FOREACH(const std::string& classId, m_derivedClasses) {
			const InstanceList derived =
				ComponentRegistry::Instance().GetComponentEntry(classId).GetInstances(true);
			instances.insert(instances.end(), derived.begin(), derived.end());
		}
	}
	return instances;
}

//**********************************************************************

Component* ComponentEntry::GetInstance (uint instanceSerial, bool includeDerivedInstances) const
{
	InstanceList instances = GetInstances(includeDerivedInstances);
	InstanceList::const_iterator iter = std::find_if(
			instances.begin(), instances.end(), util::ptr_equal_to<Component*>(instanceSerial)
		);
	return iter != instances.end() ? *iter : 0;
}

//**********************************************************************

Component* ComponentEntry::GetFocusedInstance (void) const
{
	return this->GetInstance(m_focusedInstance);
}

//**********************************************************************

uint ComponentEntry::GetFocusedInstanceSerial (void) const
{
	return m_focusedInstance;
}

//**********************************************************************

void ComponentEntry::SetMetadata (const Metadata* data)
{
	if (m_metadataCopy)
		delete m_metadata;

	m_metadata = data;
	m_metadataCopy = false;
}

//**********************************************************************

void ComponentEntry::SetMetadata (const Metadata& data)
{
	if (m_metadataCopy)
		delete m_metadata;

	m_metadata = new Metadata(data);
	m_metadataCopy = true;
}

//**********************************************************************

const ComponentFuncEntry& ComponentEntry::GetFuncEntry (const std::string& id, bool includeBaseFunctions) const
{
	static const ComponentFuncEntry nullFuncEntry;
	ComponentFuncMap::const_iterator iter = m_functions.find(id);
	if (iter != m_functions.end())
		return *iter;
	else if (includeBaseFunctions && !m_baseClassId.empty())
		return this->getBaseComponentEntry().GetFuncEntry(id);
	else
		return nullFuncEntry;
}

//**********************************************************************

const UserCommand& ComponentEntry::GetUserCommand (const String& path) const
	{ return m_userCommands.GetUserCommand(path); }

//**********************************************************************

const conf::Property* ComponentEntry::GetProperty (const std::string& id) const
	{	return m_properties.GetProperty(id); }

//**********************************************************************

bool ComponentEntry::HasInstance (uint serial, bool includeDerivedInstances) const
	{ return !!this->GetInstance(serial, includeDerivedInstances); }

bool ComponentEntry::HasInstance (Component* instance, bool includeDerivedInstances) const
{
	const InstanceList instances = GetInstances(includeDerivedInstances);
	return std::find(instances.begin(), instances.end(), instance) != instances.end();
}

bool ComponentEntry::HasFunction (const std::string& id, bool includeBaseFunctions) const
	{ return this->GetFuncEntry(id, includeBaseFunctions); }

bool ComponentEntry::HasUserCommand (const String& path) const
	{ return m_userCommands.HasUserCommand(path); }

bool ComponentEntry::HasProperty (const std::string& id) const
	{ return !!m_properties.GetProperty(id); }

bool ComponentEntry::HasSignal (const std::string& id) const
	{ return std::find(m_signals.begin(), m_signals.end(), id) != m_signals.end(); }

bool ComponentEntry::HasBaseClass (const std::string& id) const
{
	if (!m_baseClassId.empty())
		return m_baseClassId == id || this->getBaseComponentEntry().HasBaseClass(id);
	return false;
}

//**********************************************************************

ComponentEntry& ComponentEntry::AddFunction (const ComponentFuncEntry& function)
{
	if (m_functions.insert(function).second) {

		util::for_all(m_instances,
			boost::bind(&Component::AddedFunction, _1, boost::ref(function)));

		if (m_attached)
			ComponentRegistry::Instance().NotifyAddedFunction(m_classId, function);
	}
	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::RemoveFunction (const std::string& id)
{
	ComponentFuncMap::iterator iter = m_functions.find(id);
	if (iter != m_functions.end()) {
		bool isSlot = iter->IsSlot();
		bool isStatic = iter->IsStaticFunc();
		m_functions.erase(iter);

		util::for_all(m_instances, boost::bind(&Component::RemovedFunction, _1, id));

		if (m_attached)
			ComponentRegistry::Instance().NotifyRemovedFunction(m_classId, id, isSlot, isStatic);
	}		
	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::AddUserCommand (const String& path, const UserCommand& userCommand)
{
	m_userCommands.InsertUserCommand(path, userCommand, -1);

	util::for_all(m_instances, boost::bind(&Component::AddedUserCommand, _1, path, userCommand));

	if (m_attached)
		ComponentRegistry::Instance().NotifyAddedUserCommand(m_classId, path, userCommand);

	BOOST_FOREACH(const std::string& derivedClassId, m_derivedClasses) {
		UserCommand cmd(userCommand);
		cmd.ChangeOwner(derivedClassId, this->GetClassId());
		this->getComponentEntry(derivedClassId).AddUserCommand(path, cmd);
	}
	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::RemoveUserCommand (const String& path)
{
	BOOST_FOREACH(const std::string& derivedClassId, m_derivedClasses)
		this->getComponentEntry(derivedClassId).RemoveUserCommand(path);

	m_userCommands.RemoveUserCommand(path);

	util::for_all(m_instances, boost::bind(&Component::RemovedUserCommand, _1, path));

	if (m_attached)
		ComponentRegistry::Instance().NotifyRemovedUserCommand(m_classId, path);

	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::EnableUserCommand (const String& path)
{
	BOOST_FOREACH(const std::string& derivedClassId, m_derivedClasses)
		this->getComponentEntry(derivedClassId).EnableUserCommand(path);

	m_userCommands.EnableUserCommand(path);

	util::for_all(m_instances, boost::bind(&Component::EnabledUserCommand, _1, path));

	if (m_attached)
		ComponentRegistry::Instance().NotifyEnabledUserCommand(m_classId, path);

	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::DisableUserCommand (const String& path)
{
	BOOST_FOREACH(const std::string& derivedClassId, m_derivedClasses)
		this->getComponentEntry(derivedClassId).DisableUserCommand(path);

	m_userCommands.DisableUserCommand(path);

	util::for_all(m_instances, boost::bind(&Component::DisabledUserCommand, _1, path));

	if (m_attached)
		ComponentRegistry::Instance().NotifyDisabledUserCommand(m_classId, path);

	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::MergeUserCommands (const UserCommand& cmds)
{
	m_userCommands.Merge(cmds);

	util::for_all(m_instances, boost::bind(&Component::MergedUserCommands, _1, cmds));

	if (m_attached)
		ComponentRegistry::Instance().NotifyMergedUserCommands(m_classId, cmds);

	BOOST_FOREACH(const std::string& derivedClassId, m_derivedClasses)
		this->getComponentEntry(derivedClassId).MergeUserCommands(cmds);

	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::UnMergeUserCommands (const UserCommand& cmds)
{
	BOOST_FOREACH(const std::string& derivedClassId, m_derivedClasses)
		this->getComponentEntry(derivedClassId).UnMergeUserCommands(cmds);

	m_userCommands.Subtract(cmds);

	util::for_all(m_instances, boost::bind(&Component::UnMergedUserCommands, _1, cmds));

	if (m_attached)
		ComponentRegistry::Instance().NotifyUnMergedUserCommands(m_classId, cmds);

	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::AddProperty (const std::string& id, conf::Property* prop)
{
	m_properties.AddProperty(id, prop);

	util::for_all(m_instances, boost::bind(&Component::AddedProperty, _1, id, prop));

	if (m_attached)
		ComponentRegistry::Instance().NotifyAddedProperty(m_classId, id, prop);

	BOOST_FOREACH(const std::string& derivedClassId, m_derivedClasses)
		this->getComponentEntry(derivedClassId).AddProperty(id, prop->Clone());

	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::RemoveProperty (const std::string& id)
{
	BOOST_FOREACH(const std::string& derivedClassId, m_derivedClasses)
		this->getComponentEntry(derivedClassId).RemoveProperty(id);

	m_properties.RemoveProperty(id);

	util::for_all(m_instances, boost::bind(&Component::RemovedProperty, _1, id));

	if (m_attached)
		ComponentRegistry::Instance().NotifyRemovedProperty(m_classId, id);

	return *this;
}

//**********************************************************************

void ComponentEntry::ClearProperties (void)
{
	m_properties.Clear();
}

//**********************************************************************

void ComponentEntry::ResetProperties (void)
{
	//ClearProperties();
	this->resetProperties(*this);
	m_properties.LoadValues(this->getConfigURI(_T("Properties")));
}

//**********************************************************************

ComponentEntry& ComponentEntry::AddSignal (const std::string& id)
{
	m_signals.push_back(id);

	util::for_all(m_instances, boost::bind(&Component::AddedSignal, _1, id));

	if (m_attached)
		ComponentRegistry::Instance().NotifyAddedSignal(m_classId, id);

	return *this;
}

//**********************************************************************

ComponentEntry& ComponentEntry::RemoveSignal (const std::string& id)
{
	m_signals.remove(id);

	util::for_all(m_instances, boost::bind(&Component::RemovedSignal, _1, id));

	if (m_attached)
		ComponentRegistry::Instance().NotifyRemovedSignal(m_classId, id);

	return *this;
}

//**********************************************************************

const std::string& ComponentEntry::GetClassId (void) const { return m_classId; }
const std::string& ComponentEntry::GetBaseClassId (void) const { return m_baseClassId; }

const ComponentEntry::DerivedClassList ComponentEntry::GetDerivedClasses (bool recursively) const
{
	DerivedClassList derived;
	derived.insert(derived.end(), m_derivedClasses.begin(), m_derivedClasses.end());
	if (recursively)
		BOOST_FOREACH(const std::string& classId, m_derivedClasses) {
			const DerivedClassList l = ComponentRegistry::Instance().
				GetComponentEntry(classId).GetDerivedClasses(true);
			derived.insert(derived.end(), l.begin(), l.end());
		}
	return derived;
}

//**********************************************************************

void ComponentEntry::NotifyPropertiesChanged (const conf::PropertyTable& old, const conf::PropertyIdVec& changed) const
{
	m_properties.SetDirty(true);

	util::for_all(this->GetInstances(true),
		boost::bind(&Component::ApplyChangedProperties, _1, old, changed));
}

//**********************************************************************

void ComponentEntry::SetAttached (void)
{
	m_attached = true;
	ComponentRegistry::Instance().sigAddedSignal.connect(
		boost::bind(&ComponentEntry::UpdateSlots, this, _1, _2)
	);
}

//**********************************************************************

ComponentEntry& ComponentEntry::getComponentEntry (const std::string& classId)
	{ return ComponentRegistry::Instance().GetComponentEntry(classId); }	

ComponentEntry& ComponentEntry::getBaseComponentEntry (void)
	{ return ComponentRegistry::Instance().GetComponentEntry(m_baseClassId); }

const ComponentEntry& ComponentEntry::getBaseComponentEntry (void) const
	{ return ComponentRegistry::Instance().GetComponentEntry(m_baseClassId); }

//**********************************************************************

void ComponentEntry::resetProperties (ComponentEntry& entry)
{
	if (!entry.m_baseClassId.empty())
		this->resetProperties(entry.getBaseComponentEntry());

	if (entry.m_prop)
		entry.m_prop(*this);
}

//**********************************************************************

void ComponentEntry::inheritUserCommands (ComponentEntry& entry)
{
	if (!entry.m_baseClassId.empty())
		this->inheritUserCommands(entry.getBaseComponentEntry());

	m_userCommands += entry.GetUserCommands();
	m_userCommands.ChangeOwner(this->GetClassId(), entry.GetClassId());
}

//**********************************************************************

const String ComponentEntry::getConfigURI (const String& dirNamespace) const
{
	const std::string filename = m_classId + "_Config.xml";
	return fs::PathManager::Instance().GetLocalFile(dirNamespace, util::std2str(filename));
}

//**********************************************************************

void ComponentEntry::UpdateSlots(const std::string& classId, const std::string& id) {
	const std::string undefinedArgs = "...";
	const ComponentSignal& signal = ComponentSignalRegistry::Instance().GetSignal(id);
	const std::string signature = signal.GetSignature();
	BOOST_FOREACH(const ComponentFuncEntry& entry, m_functions)
		if(entry.IsSlot() && entry.m_id == id && entry.GetArguments() == undefinedArgs) {
			const_cast<ComponentFuncEntry&>(entry).UpdateSlotSignature(signature);
			if (m_attached)
				ComponentRegistry::Instance().NotifySlotUpdated(m_classId, entry);
		}
	BOOST_FOREACH(const ComponentSignal::InstanceSlotMap::value_type& pair, signal.GetInstanceSlotMap())
		if (Component* comp = pair.first.Resolve()) {
			const ComponentFuncEntry& entry = comp->GetDynamicFuncEntry(id);
			if (entry && entry.IsSlot() && entry.GetArguments() == undefinedArgs) {
				const_cast<ComponentFuncEntry&>(entry).UpdateSlotSignature(signature);
				if (m_attached)
					ComponentRegistry::Instance().NotifyDynamicSlotUpdated(comp, entry);
			}
		}
}

////////////////////////////////////////////////////////////////////////

ComponentEntry* nullComponentEntry = NULL;
ComponentEntry* GetNullComponentEntry (void) {
	if (!nullComponentEntry)
		nullComponentEntry = new ComponentEntry;
	return nullComponentEntry;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
