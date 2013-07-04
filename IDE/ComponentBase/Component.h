/**
 *	Component.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENT_H
#define IDE_COMPONENT_H

#include "Common.h"
#include "IDECommon.h"
#include "PropertyTable.h"
#include "ComponentFuncEntry.h"
#include "MultiIndex.h"

class wxWindow;

namespace ide {

////////////////////////////////////////////////////////////////////////

class UserCommand;
class Message;

////////////////////////////////////////////////////////////////////////

class _IDE_API Component {
public:
	typedef std::list<Component*> List;

	typedef multi_index_container<
		ComponentFuncEntry,
		indexed_by<
			ordered_unique<
				member<ComponentFuncEntry, const std::string, &ComponentFuncEntry::m_id>
			>
		>
	> DynamicFuncMap;

	Component (void);
	virtual ~Component (void);

	//******************************************************************
	// ClassIds: one Id per component class
	// InstanceSerial: a serial number specific to the current class instance
	//
	bool operator== (const std::string& classId) const { return this->GetClassId() == classId; }
	bool operator!= (const std::string& classId) const { return this->GetClassId() != classId; }

	bool operator== (uint serial) const { return this->GetSerial() == serial; }
	bool operator!= (uint serial) const { return this->GetSerial() != serial; }

	virtual const std::string&	GetClassId	(void) const = 0;
	uint						GetSerial	(void) const { return m_serial; }

	//******************************************************************

	virtual Component*	GetBaseInstance (void) const { return (Component*) 0; }
	Component*			GetDerivedInstance (void) const { return m_derivedInstance; }
	void				SetDerivedInstance (Component* comp) { m_derivedInstance = comp; }

	//******************************************************************

	const String& GetName (void) const;
	const String& GetDescription (void) const;

	//******************************************************************
	// Destruction
	//
	void Destroy (void);

	//******************************************************************
	// Component hierarchy support
	//
	Component*	GetParent (void) const { return m_parent; }
	void		GetParentsRecursively (List& parents) const;
	void		SetParent (Component* parent);

	virtual const List&	GetChildren (void) const { return m_children; }
	void				GetChildrenRecursively (List& children) const;

	//******************************************************************
	// Component GUI support
	//
	virtual wxWindow* GenerateWindow (wxWindow* PORT_UNUSED_PARAM(parent)) { return 0; }
	virtual wxWindow* GetWindow (void) { return 0; }

	//******************************************************************
	// Dynamic function support
	//
	void AddDynamicFunction (const ComponentFuncEntry& function);
	void RemoveDynamicFunction (const std::string& id);

	const ComponentFuncEntry& GetDynamicFuncEntry (const std::string& id) const;
	const DynamicFuncMap& GetDynamicFunctionMap (void) const { return m_functions; }

	//******************************************************************
	// Properties support
	//
	void AddClassProperty (const std::string& id, conf::Property* prop);	// Takes ownership
	void RemoveClassProperty (const std::string& id);

	void AddInstanceProperty (const std::string& id, conf::Property* prop);	// Takes ownership
	void RemoveInstanceProperty (const std::string& id);

	const conf::Property*	GetClassProperty (const std::string& id) const;
	const conf::Property*	GetInstanceProperty (const std::string& id) const;
	const conf::Property*	GetProperty (const std::string& id) const;

	const conf::PropertyTable&	GetClassPropertyTable (void) const;
	const conf::PropertyTable&	GetInstancePropertyTable (void) const { return m_properties; }
	conf::PropertyTable			GetPropertyTable (void) const;

	void ApplyChangedProperties (const conf::PropertyTable& old, const conf::PropertyIdVec& changed);

	//******************************************************************
	// User command support
	//
	void AddUserCommand (const String& path, const UserCommand& command);
	void RemoveUserCommand (const String& path);
	void EnableUserCommand (const String& path);
	void DisableUserCommand (const String& path);

	void MergeUserCommands (const UserCommand& commands);
	void UnMergeUserCommands (const UserCommand& commands);

	const UserCommand& GetUserCommand (const String& path) const;
	const UserCommand& GetUserCommands (void) const;

	//******************************************************************
	// Focus support
	//
	void Focus (void);
	bool IsFocused (void) const;

private:
	friend class ComponentEntry;

	void NotifyCreated (void);
	void SetSerial (uint serial) { m_serial = serial; }
	void SetInDestruction (void);
	bool IsUnderDestruction (void) const 
		{ assert(!m_inDestruction || !m_serial); return m_inDestruction; }

	void ChangeParent (Component* parent);
	void SetFocus (void);

	void AddedProperty (const std::string& id, conf::Property* prop);
	void RemovedProperty (const std::string& id);

	void AddedFunction (const ComponentFuncEntry& func);
	void RemovedFunction (const std::string& id);

	void AddedDynamicFunction (const ComponentFuncEntry& func);
	void RemovedDynamicFunction (const std::string& id);

	void AddedUserCommand (const String& path, const UserCommand& cmd);
	void RemovedUserCommand (const String& path, uint flags);
	void EnabledUserCommand (const String& path, uint flags);
	void DisabledUserCommand (const String& path, uint flags);

	void MergedUserCommands (const UserCommand& cmds);
	void UnMergedUserCommands (const UserCommand& cmds);

	void AddedSignal (const std::string& id);
	void RemovedSignal (const std::string& id);

protected:
#if 0	// TODO
	// All component classes should define these functions as needed.
	virtual void ExplicitDestructor (void){}			// Destructor logic should go into this.
	virtual void DeleteMemory (void) { delete this; }	// Every subclass should define it as needed  (mem clear).
#endif

	bool GetInDestruction (void) const { return m_inDestruction; }	// TODO: remove

	//******************************************************************
	// Component internal notification events
	//
	virtual void ComponentCreated (void) {}

	virtual void ComponentAdded (void) {}
	virtual void ComponentRemoved (void) {}
	virtual void ComponentDestroyed (void) {}

	virtual void ComponentAddedAsFirstOfItsKind (void) {}
	virtual void ComponentRemovedAsLastOfItsKind (void) {}
	virtual void ComponentDestroyedAsLastOfItsKind (void) {}

	//******************************************************************

	virtual void ComponentFocused (void) {}
	virtual void ComponentAppliedChangedProperties (const conf::PropertyTable& PORT_UNUSED_PARAM(old), const conf::PropertyIdVec& PORT_UNUSED_PARAM(changed)) {}

	virtual void ComponentAddedProperty (const std::string& PORT_UNUSED_PARAM(id), conf::Property* PORT_UNUSED_PARAM(prop)) {}
	virtual void ComponentRemovedProperty (const std::string& PORT_UNUSED_PARAM(id)) {}

	virtual void ComponentAddedFunction (const ComponentFuncEntry& PORT_UNUSED_PARAM(func)) {}
	virtual void ComponentRemovedFunction (const std::string& PORT_UNUSED_PARAM(id)) {}

	virtual void ComponentAddedDynamicFunction (const ComponentFuncEntry& PORT_UNUSED_PARAM(func)) {}
	virtual void ComponentRemovedDynamicFunction (const std::string& PORT_UNUSED_PARAM(id)) {}

	virtual void ComponentAddedUserCommand (const String& PORT_UNUSED_PARAM(path), const UserCommand& PORT_UNUSED_PARAM(cmd)) {}
	virtual void ComponentRemovedUserCommand (const String& PORT_UNUSED_PARAM(path), uint PORT_UNUSED_PARAM(flags)) {}
	virtual void ComponentEnabledUserCommand (const String& PORT_UNUSED_PARAM(path), uint PORT_UNUSED_PARAM(flags)) {}
	virtual void ComponentDisabledUserCommand (const String& PORT_UNUSED_PARAM(path), uint PORT_UNUSED_PARAM(flags)) {}

	virtual void ComponentMergedUserCommands (const UserCommand& PORT_UNUSED_PARAM(cmds)) {}
	virtual void ComponentUnMergedUserCommands (const UserCommand& PORT_UNUSED_PARAM(cmds)) {}

	virtual void ComponentAddedSignal (const std::string& PORT_UNUSED_PARAM(id)) {}
	virtual void ComponentRemovedSignal (const std::string& PORT_UNUSED_PARAM(id)) {}

	//******************************************************************
	// Component internal notification events for small children
	//
	virtual void ChildAdded (Component* PORT_UNUSED_PARAM(component)) {}
	virtual void ChildRemoved (Component* PORT_UNUSED_PARAM(component)) {}
	virtual void ChildDestroyed (Component* PORT_UNUSED_PARAM(component)) {}

	virtual void ChildAddedAsFirstOfItsKind (Component* PORT_UNUSED_PARAM(component)) {}
	virtual void ChildRemovedAsLastOfItsKind (Component* PORT_UNUSED_PARAM(component)) {}
	virtual void ChildDestroyedAsLastOfItsKind (Component* PORT_UNUSED_PARAM(component)) {}

	//******************************************************************

	virtual void ChildFocused (Component* PORT_UNUSED_PARAM(component)) {}
	virtual void ChildAppliedChangedProperties (Component* PORT_UNUSED_PARAM(component), const conf::PropertyTable& PORT_UNUSED_PARAM(old), const conf::PropertyIdVec& PORT_UNUSED_PARAM(changed)) {}

	virtual void ChildAddedProperty (Component* PORT_UNUSED_PARAM(component), const std::string& PORT_UNUSED_PARAM(id), conf::Property* PORT_UNUSED_PARAM(prop)) {}
	virtual void ChildRemovedProperty (Component* PORT_UNUSED_PARAM(component), const std::string& PORT_UNUSED_PARAM(id)) {}

	virtual void ChildAddedFunction (Component* PORT_UNUSED_PARAM(component), const ComponentFuncEntry& PORT_UNUSED_PARAM(func)) {}
	virtual void ChildRemovedFunction (Component* PORT_UNUSED_PARAM(component), const std::string& PORT_UNUSED_PARAM(id)) {}

	virtual void ChildAddedDynamicFunction (Component* PORT_UNUSED_PARAM(component), const ComponentFuncEntry& PORT_UNUSED_PARAM(func)) {}
	virtual void ChildRemovedDynamicFunction (Component* PORT_UNUSED_PARAM(component), const std::string& PORT_UNUSED_PARAM(id)) {}

	virtual void ChildAddedUserCommand (Component* PORT_UNUSED_PARAM(component), const String& PORT_UNUSED_PARAM(path), const UserCommand& PORT_UNUSED_PARAM(cmd)) {}
	virtual void ChildRemovedUserCommand (Component* PORT_UNUSED_PARAM(component), const String& PORT_UNUSED_PARAM(path), uint PORT_UNUSED_PARAM(flags)) {}
	virtual void ChildEnabledUserCommand (Component* PORT_UNUSED_PARAM(component), const String& PORT_UNUSED_PARAM(path), uint PORT_UNUSED_PARAM(flags)) {}
	virtual void ChildDisabledUserCommand (Component* PORT_UNUSED_PARAM(component), const String& PORT_UNUSED_PARAM(path), uint PORT_UNUSED_PARAM(flags)) {}

	virtual void ChildMergedUserCommands (Component* PORT_UNUSED_PARAM(component), const UserCommand& PORT_UNUSED_PARAM(cmds)) {}
	virtual void ChildUnMergedUserCommands (Component* PORT_UNUSED_PARAM(component), const UserCommand& PORT_UNUSED_PARAM(cmds)) {}

	virtual void ChildAddedSignal (Component* PORT_UNUSED_PARAM(component), const std::string& PORT_UNUSED_PARAM(id)) {}
	virtual void ChildRemovedSignal (Component* PORT_UNUSED_PARAM(component), const std::string& PORT_UNUSED_PARAM(id)) {}

private:
	ComponentEntry& getComponentEntry (void) const;
	bool			containsThisClass (const List& PORT_UNUSED_PARAM(compList)) const;
	void			focusWithoutPropagation (void);

	uint				m_serial;
	DynamicFuncMap		m_functions;
	conf::PropertyTable	m_properties;

	Component*			m_parent;
	List				m_children;

	Component*			m_derivedInstance;

	bool				m_inDestruction;
};

////////////////////////////////////////////////////////////////////////
// Useful predicates that apply to component pointers
//
template<class TComparator>
struct CompareByClassId : public std::binary_function<Component*, Component*, bool> {
	bool operator() (Component* lComp, Component* rComp) const
		{ return TComparator()(lComp->GetClassId(), rComp->GetClassId()); }
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENT_H
