/**
 *	Component.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "Component.h"
#include "ComponentRegistry.h" // Cyclic
#include "Algorithms.h"

#include <boost/lexical_cast.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// Component class
//
Component::Component (void) :
	m_serial(0), m_properties(), m_parent(0), m_inDestruction(false)
{
}

//**********************************************************************

Component::~Component (void)
{
	assert(m_inDestruction && "Only destroy Component through instance->Destroy()"
		"or ComponentEntry::Destroy(instance)");
}

//**********************************************************************

const std::string& Component::GetDerivedClassId	(void) const
{
	return m_derivedClassId.empty() ? GetClassId() : m_derivedClassId;
}

//**********************************************************************

const String& Component::GetName (void) const
{
	return this->getComponentEntry().GetMetadata().name;
}

//**********************************************************************

const String& Component::GetDescription (void) const
{
	return this->getComponentEntry().GetMetadata().desc;
}

//**********************************************************************

void Component::Destroy (void)
{
	this->getComponentEntry().Destroy(this);
}

//**********************************************************************

void Component::GetParentsRecursively (List& parents) const
{
	if (m_parent) {
		parents.push_back(m_parent);
		m_parent->GetParentsRecursively(parents);
	}
}

//**********************************************************************

void Component::SetParent (Component* parent)
{
	this->getComponentEntry().SetParent(this, parent);
}

//**********************************************************************

void Component::GetChildrenRecursively (List& children) const
{
	for (List::const_iterator i = m_children.begin(); i != m_children.end(); ++i) {
		children.push_back(*i);
		(*i)->GetChildrenRecursively(children);
	}
}

//**********************************************************************

void Component::AddDynamicFunction (const ComponentFuncEntry& function)
{
	assert(function.IsMemberFunc());
	if (m_functions.insert(function).second) {
		AddedDynamicFunction(function);
		ComponentRegistry::Instance().NotifyAddedDynamicFunction(this, function);
	}
}

//**********************************************************************

void Component::RemoveDynamicFunction (const std::string& id)
{
	DynamicFuncMap::iterator iter = m_functions.find(id);
	if (iter != m_functions.end()) {
		bool isSlot = iter->IsSlot();
		m_functions.erase(iter);
		RemovedDynamicFunction(id);
		ComponentRegistry::Instance().NotifyRemovedDynamicFunction(this, id, isSlot);
	}		
}

//**********************************************************************

const ComponentFuncEntry& Component::GetDynamicFuncEntry (const std::string& id) const
{
	static const ComponentFuncEntry nullFuncEntry;
	DynamicFuncMap::const_iterator iter = m_functions.find(id);
	if (iter != m_functions.end())
		return *iter;
	else
		return nullFuncEntry;
}

//**********************************************************************

void Component::AddClassProperty (const std::string& id, conf::Property* prop)
{
	this->getComponentEntry().AddProperty(id, prop);
}

//**********************************************************************

void Component::RemoveClassProperty (const std::string& id)
{
	this->getComponentEntry().RemoveProperty(id);
}

//**********************************************************************

void Component::AddInstanceProperty (const std::string& id, conf::Property* prop)
{
	m_properties.AddProperty(id, prop);
	this->AddedProperty(id, prop);
}

//**********************************************************************

void Component::RemoveInstanceProperty (const std::string& id)
{
	m_properties.RemoveProperty(id);
	this->RemovedProperty(id);
}

//**********************************************************************

const conf::Property* Component::GetClassProperty (const std::string& id) const
{
	return this->getComponentEntry().GetProperty(id);
}

//**********************************************************************

const conf::Property* Component::GetInstanceProperty (const std::string& id) const
{
	return m_properties.GetProperty(id);
}

//**********************************************************************

const conf::Property* Component::GetProperty (const std::string& id) const
{
	conf::Property* prop = m_properties.GetProperty(id);
	return prop ? prop : this->getComponentEntry().GetProperty(id);
}

//**********************************************************************

const conf::PropertyTable& Component::GetClassPropertyTable (void) const
{
	return this->getComponentEntry().GetPropertyTable();
}

//**********************************************************************

conf::PropertyTable	Component::GetPropertyTable (void) const
{
	conf::PropertyTable table;
	table.Merge(this->GetClassPropertyTable());
	table.Merge(this->GetInstancePropertyTable());
	return table;
}

//**********************************************************************

void Component::ApplyChangedProperties (const conf::PropertyTable& old, const conf::PropertyIdVec& changed)
{
	this->ComponentAppliedChangedProperties(old, changed);

	if (m_parent)
		m_parent->ChildAppliedChangedProperties(this, old, changed);
}

//**********************************************************************
// UserCommand support
//
void Component::AddUserCommand (const String& path, const UserCommand& command)
	{ this->getComponentEntry().AddUserCommand(path, command); }

void Component::RemoveUserCommand (const String& path)
	{ this->getComponentEntry().RemoveUserCommand(path); }

void Component::EnableUserCommand (const String& path)
	{ this->getComponentEntry().EnableUserCommand(path); }

void Component::DisableUserCommand (const String& path)
	{ this->getComponentEntry().DisableUserCommand(path); }

void Component::MergeUserCommands (const UserCommand& commands)
	{ this->getComponentEntry().MergeUserCommands(commands); }

void Component::UnMergeUserCommands (const UserCommand& commands)
	{ this->getComponentEntry().UnMergeUserCommands(commands); }

const UserCommand& Component::GetUserCommand (const String& path) const
	{ return this->getComponentEntry().GetUserCommand(path); }

const UserCommand& Component::GetUserCommands (void) const
	{ return this->getComponentEntry().GetUserCommands(); }

//**********************************************************************

void Component::Focus (void)
{
	this->getComponentEntry().SetFocus(this);

	// Propagate the focus to this component's parents
	//
	typedef CompareByClassId< std::less<std::string> >		CmpLessThan;
	typedef CompareByClassId< std::equal_to<std::string> >	CmpEqualTo;

	List parents;
	this->GetParentsRecursively(parents);
	parents.remove_if(std::bind2nd(CmpEqualTo(), this));
	parents.sort(CmpLessThan());
	parents.unique(CmpEqualTo());
	util::for_all(parents, std::mem_fun(&Component::focusWithoutPropagation));
}

//**********************************************************************

bool Component::IsFocused (void) const
{
	return this->getComponentEntry().GetFocusedInstanceSerial() == this->GetSerial();
}

//**********************************************************************

void Component::NotifyCreated (void)
{
	this->ComponentCreated();
}

//**********************************************************************
// Called only by friends
//
void Component::SetInDestruction (void)
{
	assert(!m_inDestruction && "Trying to re-destroy a component ???");
	m_inDestruction = true;

	while (!m_children.empty())
		m_children.back()->Destroy();

	this->ComponentDestroyed();

	if (m_parent) {
		m_parent->ChildDestroyed(this);

		if (!this->containsThisClass(m_parent->GetChildren())) {

			this->ComponentRemovedAsLastOfItsKind();
			m_parent->ChildRemovedAsLastOfItsKind(this);

			this->ComponentDestroyedAsLastOfItsKind();
			m_parent->ChildDestroyedAsLastOfItsKind(this);
		}
		m_parent->m_children.remove(this);
	}
}

//**********************************************************************

void Component::ChangeParent (Component* parent)
{
	if (m_parent) {

		this->ComponentRemoved();
		m_parent->ChildRemoved(this);

		if (!this->containsThisClass(m_parent->GetChildren())) {
			this->ComponentRemovedAsLastOfItsKind();
			m_parent->ChildRemovedAsLastOfItsKind(this);
		}
		m_parent->m_children.remove(this);
	}

	m_parent = parent;

	if (m_parent) {

		bool isTheFirstOfItsKind = !this->containsThisClass(m_parent->GetChildren());

		m_parent->m_children.push_back(this);
		this->ComponentAdded();
		m_parent->ChildAdded(this);

		if (isTheFirstOfItsKind) {
			this->ComponentAddedAsFirstOfItsKind();
			m_parent->ChildAddedAsFirstOfItsKind(this);
		}
	}
}

//**********************************************************************

void Component::SetFocus (void)
{
	this->ComponentFocused();
	if (m_parent)
		m_parent->ChildFocused(this);
}

//**********************************************************************

void Component::AddedProperty (const std::string& id, conf::Property* prop)
{
	this->ComponentAddedProperty(id, prop);
	if (m_parent)
		m_parent->ChildAddedProperty(this, id, prop);
}

//**********************************************************************

void Component::RemovedProperty (const std::string& id)
{
	this->ComponentRemovedProperty(id);
	if (m_parent)
		m_parent->ChildRemovedProperty(this, id);
}

//**********************************************************************

void Component::AddedFunction (const ComponentFuncEntry& func)
{
	this->ComponentAddedFunction(func);
	if (m_parent)
		m_parent->ChildAddedFunction(this, func);
}

//**********************************************************************

void Component::RemovedFunction (const std::string& id)
{
	this->ComponentRemovedFunction(id);
	if (m_parent)
		m_parent->ChildRemovedFunction(this, id);
}

//**********************************************************************

void Component::AddedDynamicFunction (const ComponentFuncEntry& func)
{
	this->ComponentAddedDynamicFunction(func);
	if (m_parent)
		m_parent->ChildAddedDynamicFunction(this, func);
}

//**********************************************************************

void Component::RemovedDynamicFunction (const std::string& id)
{
	this->ComponentRemovedDynamicFunction(id);
	if (m_parent)
		m_parent->ChildRemovedDynamicFunction(this, id);
}

//**********************************************************************

void Component::AddedUserCommand (const String& path, const UserCommand& cmd)
{
	this->ComponentAddedUserCommand(path, cmd);
	if (m_parent)
		m_parent->ChildAddedUserCommand(this, path, cmd);
}


void Component::RemovedUserCommand (const String& path, uint flags)
{
	this->ComponentRemovedUserCommand(path, flags);
	if (m_parent)
		m_parent->ChildRemovedUserCommand(this, path, flags);
}

void Component::EnabledUserCommand (const String& path, uint flags)
{
	this->ComponentEnabledUserCommand(path, flags);
	if (m_parent)
		m_parent->ChildEnabledUserCommand(this, path, flags);
}

void Component::DisabledUserCommand (const String& path, uint flags)
{
	this->ComponentDisabledUserCommand(path, flags);
	if (m_parent)
		m_parent->ChildDisabledUserCommand(this, path, flags);
}

//**********************************************************************

void Component::MergedUserCommands (const UserCommand& cmds)
{
	this->ComponentMergedUserCommands(cmds);
	if (m_parent)
		m_parent->ChildMergedUserCommands(this, cmds);
}

void Component::UnMergedUserCommands (const UserCommand& cmds)
{
	this->ComponentUnMergedUserCommands(cmds);
	if (m_parent)
		m_parent->ChildUnMergedUserCommands(this, cmds);
}

//**********************************************************************

void Component::AddedSignal (const std::string& id)
{
	this->ComponentAddedSignal(id);
	if (m_parent)
		m_parent->ChildAddedSignal(this, id);
}

//**********************************************************************

void Component::RemovedSignal (const std::string& id)
{
	this->ComponentRemovedSignal(id);
	if (m_parent)
		m_parent->ChildRemovedSignal(this, id);
}

//**********************************************************************
// Private members
//
ComponentEntry& Component::getComponentEntry (void) const
{
	return ComponentRegistry::Instance().GetComponentEntry(this->GetClassId());
}

//**********************************************************************

bool Component::containsThisClass (const List& compList) const
{
	List::const_iterator iter = compList.begin();
	for (; iter != compList.end(); ++iter) {
		if ((*iter)->GetClassId() == this->GetClassId() && *iter != this)
			return true;
	}
	return false;
}

//**********************************************************************

void Component::focusWithoutPropagation (void)
{
	this->getComponentEntry().SetFocus(this);
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
