/**
 *	Profile.h
 *
 *	-- IDE Adaptation Manager Component --
 *
 *	Profile component is a collection of configuration
 *	resources for the IDE's Adaptation Manager.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	April 2007
 */
#ifndef IDE_PROFILE_COMPONENT_H_
#define IDE_PROFILE_COMPONENT_H_

#include "Common.h"

#include "Component.h"
#include "ComponentBuilder.h"
#include "ComponentUserCommand.h"

#include "TreeItemComponent.h"

namespace ide
{
	class Profile :
		public TreeItemComponent
	{
		DECLARE_IDE_COMPONENT(Profile);

	public:
		///--- type definitions

		///--- constructors / destructor
		Profile(void);
		~Profile(void);

		///--- public API

		///-- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));

		DECLARE_EXPORTED_FUNCTION_(bool, Load, (const String& uri),
			_("Load profile from disk"));
		DECLARE_EXPORTED_FUNCTION_(bool, Save, (void),
			_("Save profile to disk"));
		DECLARE_EXPORTED_FUNCTION_(bool, SaveAs, (const String& uri),
			_("Save profile to a new file"));
		DECLARE_EXPORTED_FUNCTION_(void, DefaultAction, (void),
			_("Execute profile's default action (configure)"));

		DECLARE_EXPORTED_FUNCTION(bool, LoadComponent, (const String& classId));

		DECLARE_EXPORTED_FUNCTION(void, MoveChildUp, (const Handle& child));
		DECLARE_EXPORTED_FUNCTION(void, MoveChildDown, (const Handle& child));

		DECLARE_EXPORTED_FUNCTION(void, Select, (void));
		DECLARE_EXPORTED_FUNCTION(void, SetStartupProfile, (void));
		DECLARE_EXPORTED_FUNCTION(void, Edit, (void));
		DECLARE_EXPORTED_FUNCTION(void, SaveAll, (void));
		DECLARE_EXPORTED_FUNCTION(void, NewItem, (void));
		DECLARE_EXPORTED_FUNCTION(void, AddExistingItem, (void));
		DECLARE_EXPORTED_FUNCTION(void, Remove, (void));
		DECLARE_EXPORTED_FUNCTION(void, Properties, (void));

		DECLARE_EXPORTED_FUNCTION(int, OnCompareItems, (const Handle& handle1, const Handle& handle2));

	protected:
		virtual void ChildRemoved (Component* component);
		virtual void ChildDestroyed (Component* component);
	private:
		///--- private API
		bool SaveComponent(Component *comp);

		///--- private members
		List orderedChildren;
	};
}

#endif	//IDE_PROFILE_CONFIG_COMPONENT_H_
