/**
 *	IDEComponent.h
 *
 *	-- IDE Shell Component --
 *
 *	IDEComponent is the base component of the IDE
 *	it provides some basic exported functions shared
 *	by all the components in the system.
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *
 *	July 2007
 */
#ifndef IDE_IDE_COMPONENT_H_
#define IDE_IDE_COMPONENT_H_

#include "Common.h"
#include "IDECommon.h"
#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentUserCommandDesc.h"

namespace ide
{
	//----------------------------
	//-- class IDEComponent

	class _IDE_API IDEComponent :
		public Component
	{
		DECLARE_IDE_COMPONENT(IDEComponent);

	public:
		///--- constructors / destructor
		IDEComponent(void) {}
		~IDEComponent(void) {}

		///--- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_MEMBER_(void, AddCommand, (const String& path, const UserCommandDesc& desc),
			_("Add a user command to the component"));
		DECLARE_EXPORTED_MEMBER_(void, RemoveCommand, (const String& path),
			_("Remove a user command from the component"));
		DECLARE_EXPORTED_MEMBER_(void, EnableCommand, (const String& path),
			_("Enable a user command of the component"));
		DECLARE_EXPORTED_MEMBER_(void, DisableCommand, (const String& path),
			_("Disable a user command of the component"));
		DECLARE_EXPORTED_MEMBER_(void, AddInstanceCommand, (const String& path, const UserCommandDesc& desc),
			_("Add a user command only to this component instance (bypasses the component system)"));
		DECLARE_EXPORTED_MEMBER_(void, RemoveInstanceCommand, (const String& path),
			_("Remove a user command only from this component  (bypasses the component system)"));
		DECLARE_EXPORTED_MEMBER_(const UserCommandDesc, GetCommand, (const String& path),
			_("Retrieve the user command with the given path from the component"));

		DECLARE_EXPORTED_STATIC_(StringList, GetDecisions, (const String& component, const String& profile),
			_("Get configuration decisions for a DMSL component given a configuration profile"));

	private:
		///--- private members
		static Component* dmslC;
	};
}

#endif	//IDE_IDE_COMPONENT_H_
