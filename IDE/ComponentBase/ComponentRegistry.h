/**
 *	ComponentRegistry.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTREGISTRY_H
#define IDE_COMPONENTREGISTRY_H

#include "Common.h"
#include "IDECommon.h"
#include "Singleton.h"
#include "ComponentEntry.h"
#include "MultiIndex.h"

#include <boost/signal.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentRegistry : public util::Singleton {
	DECLARE_SINGLETON(ComponentRegistry);

public:
	typedef multi_index_container<
		ComponentEntry,
		indexed_by<
			ordered_unique<
				member<ComponentEntry, const std::string, &ComponentEntry::m_classId>
			>
		>
	> ComponentMap;

	typedef conf::Property					Property;
	typedef ComponentEntry::InstanceList	InstanceList;

	typedef boost::signal<void (Component*)>											Signal1;
	typedef boost::signal<void (const std::string&)>									Signal2;
	typedef boost::signal<void (const ComponentEntry&)>									Signal3;
	typedef boost::signal<void (const std::string&, const std::string&)>				Signal4;
	typedef boost::signal<void (const std::string&, const ComponentFuncEntry&)>			Signal5;
	typedef boost::signal<void (Component*, Component*)>								Signal6;
	typedef boost::signal<void (const std::string&, const String&, const UserCommand&)>	Signal7;
	typedef boost::signal<void (const std::string&, const String&)>						Signal8;
	typedef boost::signal<void (const std::string&, const std::string&, Property*)>		Signal9;
	typedef boost::signal<void (const std::string&, const UserCommand&)>				Signal10;
	typedef boost::signal<void (const std::string&, const std::string&, bool, bool)>	Signal11;
	typedef boost::signal<void (Component*, const ComponentFuncEntry&)>					Signal12;
	typedef boost::signal<void (Component*, const std::string&, bool)>					Signal13;

	//******************************************************************
	// API used by component classes to register their entries
	//
	bool Register (const ComponentEntry& entry);
	bool Unregister (const std::string& id);

	//******************************************************************

	bool HasComponentEntry (const std::string& id) const;

	//******************************************************************
	// Accessor API
	//
	ComponentEntry&		GetComponentEntry (const std::string& id);
	const ComponentMap&	GetComponentMap (void) const { return m_components; }

	bool IsValidInstance(Component* component, uint serial) const;
	void GetAllInstances (InstanceList& result) const;
	Component* GetFocusedInstance (const std::string& id)	{ return GetComponentEntry(id).GetFocusedInstance(); }

	//******************************************************************
	// Signals
	//
	Signal2		sigRequestedUnregisteredComponent;

	Signal3		sigRegisteredComponent;
	Signal2		sigUnregisteredComponent;

	Signal5		sigAddedFunction;
	Signal11	sigRemovedFunction;

	Signal12	sigAddedDynamicFunction;
	Signal13	sigRemovedDynamicFunction;

	Signal7		sigAddedUserCommand;
	Signal8		sigRemovedUserCommand;
	Signal8		sigEnabledUserCommand;
	Signal8		sigDisabledUserCommand;

	Signal10	sigMergedUserCommands;
	Signal10	sigUnMergedUserCommands;

	Signal9		sigAddedProperty;
	Signal4		sigRemovedProperty;

	Signal4		sigAddedSignal;
	Signal4		sigRemovedSignal;

	Signal5		sigSlotUpdated;
	Signal12	sigDynamicSlotUpdated;

	//******************************************************************

	Signal1 sigCreatedFirstInstance;
	Signal1 sigDestroyedLastInstance;

	Signal1 sigCreatedInstance;
	Signal1 sigDestroyedInstance;

	Signal6 sigChangedParentInstance;
	Signal6 sigChangedFocusedInstance;

protected:
	void OnInitialize (void);
	void OnCleanUp (void);

	friend class Component;
	friend class ComponentEntry;

	void NotifyRequestedUnregisteredComponent (const std::string& compId);

	void NotifyRegisteredComponent (const ComponentEntry& entry);
	void NotifyUnregisteredComponent (const std::string& compId);

	void NotifyAddedFunction (const std::string& compId, const ComponentFuncEntry& entry);
	void NotifyRemovedFunction (const std::string& compId, const std::string& funcId, bool isSlot, bool isStatic);

	void NotifyAddedDynamicFunction (Component* instance, const ComponentFuncEntry& entry);
	void NotifyRemovedDynamicFunction (Component* instance, const std::string& funcId, bool isSlot);

	void NotifyAddedUserCommand (const std::string& compId, const String& path, const UserCommand& cmd);
	void NotifyRemovedUserCommand (const std::string& compId, const String& path);
	void NotifyEnabledUserCommand (const std::string& compId, const String& path);
	void NotifyDisabledUserCommand (const std::string& compId, const String& path);

	void NotifyMergedUserCommands (const std::string& compId, const UserCommand& cmds);
	void NotifyUnMergedUserCommands (const std::string& compId, const UserCommand& cmds);

	void NotifyAddedProperty (const std::string& compId, const std::string& id, Property* prop);
	void NotifyRemovedProperty (const std::string& compId, const std::string& id);

	void NotifyAddedSignal (const std::string& compId, const std::string& id);
	void NotifyRemovedSignal (const std::string& compId, const std::string& id);

	void NotifySlotUpdated (const std::string& compId, const ComponentFuncEntry& entry);
	void NotifyDynamicSlotUpdated (Component* instance, const ComponentFuncEntry& entry);

	void NotifyCreatedFirstInstance (Component* instance);
	void NotifyDestroyedLastInstance (Component* instance);

	void NotifyCreatedInstance (Component* instance);
	void NotifyDestroyedInstance (Component* instance);

	void NotifyChangedParentInstance (Component* instance, Component* oldParent);
	void NotifyChangedFocus (Component* instance, Component* oldFocused);

private:
	ComponentRegistry (void) {}
	~ComponentRegistry (void);

	ComponentMap m_components;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTREGISTRY_H
