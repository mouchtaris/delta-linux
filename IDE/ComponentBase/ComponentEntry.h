/**
 *	ComponentEntry.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTENTRY_H
#define IDE_COMPONENTENTRY_H

#include "Common.h"
#include "IDECommon.h"

#include "Message.h"
#include "PropertyTable.h"
#include "ComponentMemberCallback.h"
#include "ComponentFuncEntry.h"
#include "ComponentUserCommand.h"
#include "MultiIndex.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class ComponentRegistry;
class Component;

////////////////////////////////////////////////////////////////////////

class _IDE_API ComponentEntry {
public:
	typedef ComponentMemberCallback	MemberFunc;
	typedef ComponentStaticCallback	StaticFunc;

	typedef std::list<Component*>			InstanceList;
	typedef conf::PropertyTable				PropertyTable;
	typedef std::list<std::string>			SignalList;
	typedef std::list<std::string>			DerivedClassList;

	typedef Component*	(*CreationFunc) (ComponentEntry&);
	typedef void		(*DestructionFunc) (Component*);
	typedef void		(*SetPropertiesFunc) (ComponentEntry&);

	//******************************************************************

	typedef multi_index_container<
		ComponentFuncEntry,
		indexed_by<
			ordered_unique<
				member<ComponentFuncEntry, const std::string, &ComponentFuncEntry::m_id>
			>
		>
	> ComponentFuncMap;

	////////////////////////////////////////////////////////////////////

	struct Metadata {
		const String name;
		const String desc;
		const String author;
		const String version;
	};

	////////////////////////////////////////////////////////////////////

	ComponentEntry (void);
	ComponentEntry (
		const std::string&	classId,
		CreationFunc		create,
		DestructionFunc		destroy,
		SetPropertiesFunc	prop,
		const std::string&	baseClassId = std::string()
	);
	~ComponentEntry (void);

	//******************************************************************

	Component*	Create (void);
	void		Destroy (Component* component);

	void Init (void);
	void Clear (void);

	//******************************************************************

	void SetParent (Component* component, Component* parent);
	void SetFocus (Component* component);

	//******************************************************************

	const InstanceList	GetInstances (bool includeDerivedInstances = false) const;
	Component*			GetInstance (uint instanceSerial, bool includeDerivedInstances = false) const;

	Component*			GetFocusedInstance (void) const;
	uint				GetFocusedInstanceSerial (void) const;

	//******************************************************************
	// SetMetadata (const Metadata* data): Keeps the ptr to metadata without
	// making a copy. So the data should be alive as long as the ComponentEntry
	// instance is alive
	//
	// SetMetadata (const Metadata& data): Makes a copy of the provided metadata
	// which you can safely dispose after the call of this function
	//
	const Metadata&	GetMetadata (void) const { return *m_metadata; }
	void			SetMetadata (const Metadata* data);
	void			SetMetadata (const Metadata& data);

	//******************************************************************

	const ComponentFuncMap&		GetFunctionMap (void) const { return m_functions; }
	const UserCommand&			GetUserCommands (void) const { return m_userCommands; }
	const PropertyTable&		GetPropertyTable (void) const { return m_properties; }
	const SignalList&			GetSignalList (void) const { return m_signals; }

	const ComponentFuncEntry&	GetFuncEntry (const std::string& id, bool includeBaseFunctions = true) const;
	const UserCommand&			GetUserCommand (const String& path) const;
	const conf::Property*		GetProperty (const std::string& id) const;

	//******************************************************************

	bool HasInstance (uint serial, bool includeDerivedInstances = false) const;
	bool HasInstance (Component* instance, bool includeDerivedInstances = false) const;
	bool HasFunction (const std::string& id, bool includeBaseFunctions = true) const;
	bool HasUserCommand (const String& path) const;
	bool HasProperty (const std::string& id) const;
	bool HasSignal (const std::string& id) const;
	bool HasBaseClass (const std::string& id) const;

	operator bool (void) const { return m_create && m_destroy; }

	//******************************************************************

	ComponentEntry& AddFunction (const ComponentFuncEntry& function);
	ComponentEntry& RemoveFunction (const std::string& id);

	//******************************************************************

	ComponentEntry& AddUserCommand (const String& path, const UserCommand& userCommand);
	ComponentEntry& RemoveUserCommand (const String& path);
	ComponentEntry& EnableUserCommand (const String& path);
	ComponentEntry& DisableUserCommand (const String& path);

	ComponentEntry& MergeUserCommands (const UserCommand& cmds);
	ComponentEntry& UnMergeUserCommands (const UserCommand& cmds);

	//******************************************************************

	ComponentEntry& AddProperty (const std::string& id, conf::Property* prop);
	ComponentEntry& RemoveProperty (const std::string& id);

	void ClearProperties (void);
	void ResetProperties (void);

	//******************************************************************

	ComponentEntry& AddSignal (const std::string& id);
	ComponentEntry& RemoveSignal (const std::string& id);

	//******************************************************************

	const std::string&		GetClassId (void) const;
	const std::string&		GetBaseClassId (void) const;
	const DerivedClassList	GetDerivedClasses (bool recursively = false) const;

	//******************************************************************
	// Class-wide notifications
	//
	void NotifyPropertiesChanged (const conf::PropertyTable& old, const conf::PropertyIdVec& changed) const;

protected:
	friend class ComponentRegistry;

	inline void SetAttached (void);
	bool GetAttached (void) const { return m_attached; }

private:

	//******************************************************************

	inline ComponentEntry&			getComponentEntry (const std::string& classId);
	inline ComponentEntry&			getBaseComponentEntry (void);
	inline const ComponentEntry&	getBaseComponentEntry (void) const;

	void resetProperties (ComponentEntry& entry);
	void inheritUserCommands (ComponentEntry& entry);

	const String getConfigURI (const String& dirNamespace) const;

	void UpdateSlots (const std::string& classId, const std::string& id);

	//******************************************************************

	const Metadata*		m_metadata;
	bool				m_metadataCopy;

	CreationFunc		m_create;
	DestructionFunc		m_destroy;
	SetPropertiesFunc	m_prop;

	InstanceList		m_instances;
	uint				m_instanceSerial;
	uint				m_focusedInstance;

	ComponentFuncMap	m_functions;

	UserCommand			m_userCommands;
	PropertyTable		m_properties;
	SignalList			m_signals;

	const std::string	m_classId;
	const std::string	m_baseClassId;
	DerivedClassList	m_derivedClasses;

	bool				m_attached;
};

////////////////////////////////////////////////////////////////////////

extern _IDE_API ComponentEntry* GetNullComponentEntry (void);

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTENTRY_H
