/**
 *	ScriptInstanceProxy.h
 *
 *	-- IDE Delta Script Component Loader --
 *
 *	ScriptInstanceProxy is used for representing an
 *	instance of a Delta component in the environment.
 *
 *	Themistoklis Bourdenas <tbourden@doc.ic.ac.uk>
 *	December 2007
 */
#ifndef IDE_DELTA_SCRIPT_INSTANCE_ENTRY_H_
#define IDE_DELTA_SCRIPT_INSTANCE_ENTRY_H_

#include "Common.h"

#include "Component.h"
#include "ComponentMemberCallback.h"

#include <map>

class DeltaVirtualMachine;
class DeltaValue;

namespace ide
{
	//----------------------------
	//-- struct ScriptInstanceProxy

	struct ScriptInstanceProxy :
		public Component
	{
		///--- constructors / destructor
		ScriptInstanceProxy(const std::string& classId, DeltaVirtualMachine* vm, Component* base);
		~ScriptInstanceProxy();

		///--- overridden base API
		virtual const std::string& GetClassId(void) const	{ return classId; }

		//--- component GUI support
		virtual wxWindow* GenerateWindow (wxWindow* parent);
		virtual wxWindow* GetWindow (void);

		///--- component internal notification events

		virtual void ComponentCreated(void);

		virtual void ComponentAdded(void);
		virtual void ComponentRemoved(void);
		virtual void ComponentDestroyed(void);

		virtual void ComponentAddedAsFirstOfItsKind(void);
		virtual void ComponentRemovedAsLastOfItsKind(void);
		virtual void ComponentDestroyedAsLastOfItsKind(void);

		virtual void ComponentFocused(void);
		virtual void ComponentAppliedChangedProperties(const conf::PropertyTable& old, const conf::PropertyIdVec& changed);

		//virtual void ComponentAddedProperty(const std::string& id, conf::Property* prop);
		//virtual void ComponentRemovedProperty(const std::string& id);

		//virtual void ComponentAddedFunction(const ComponentFuncEntry& func);
		virtual void ComponentRemovedFunction(const std::string& id);

		//virtual void ComponentAddedDynamicFunction (const ComponentFuncEntry& func) {}
		virtual void ComponentRemovedDynamicFunction (const std::string& id);

		//virtual void ComponentAddedUserCommand(const String& path, const UserCommand& cmd);
		//virtual void ComponentRemovedUserCommand(const String& path);

		//virtual void ComponentMergedUserCommands(const UserCommand& cmds);
		//virtual void ComponentUnMergedUserCommands(const UserCommand& cmds);

		virtual void ComponentAddedSignal(const std::string& id);
		virtual void ComponentRemovedSignal(const std::string& id);

		///--- component internal notification events for children
		virtual void ChildAdded(Component* component);
		virtual void ChildRemoved(Component* component);
		virtual void ChildDestroyed(Component* component);

		virtual void ChildAddedAsFirstOfItsKind(Component* component);
		virtual void ChildRemovedAsLastOfItsKind(Component* component);
		virtual void ChildDestroyedAsLastOfItsKind(Component* component);

		virtual void ChildFocused(Component* component);
		//virtual void ChildAppliedChangedProperties(Component* component, const conf::PropertyIdVec& changed);

		//virtual void ChildAddedProperty(Component* component, const std::string& id, conf::Property* prop);
		//virtual void ChildRemovedProperty(Component* component, const std::string& id);

		//virtual void ChildAddedFunction(Component* component, const ComponentFuncEntry& func);
		//virtual void ChildRemovedFunction(Component* component, const std::string& id);

		//virtual void ChildAddedUserCommand(Component* component, const String& path, const UserCommand& cmd);
		//virtual void ChildRemovedUserCommand(Component* component, const String& path);

		//virtual void ChildMergedUserCommands(Component* component, const UserCommand& cmds);
		//virtual void ChildUnMergedUserCommands(Component* component, const UserCommand& cmds);

		virtual void ChildAddedSignal(Component* component, const std::string& id);
		virtual void ChildRemovedSignal(Component* component, const std::string& id);

		///--- public API
		bool HasFunction(const std::string& function) const;
		bool IsBaseValid(void) const;

		///--- public members
		std::string	classId;
		DeltaVirtualMachine* vm;
		Component* base;
		uint baseSerial;

		typedef std::map<std::string, DeltaValue> FunctionMapping;
		FunctionMapping functions;
		FunctionMapping dynamicFunctions;
	};
}

#endif	//IDE_DELTA_SCRIPT_INSTANCE_ENTRY_H_
