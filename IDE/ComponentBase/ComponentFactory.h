/**
 *	ComponentFactory.h
 *
 *	-- Component Infrastructure --
 *
 *	ComponentFactory is an object factory pattern that
 *	handles creation / destruction of application
 *	components.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#ifndef IDE_COMPONENT_FACTORY_H_
#define IDE_COMPONENT_FACTORY_H_

#include "Common.h"
#include "Singleton.h"
#include "IDECommon.h"

#include "Component.h"
#include "ComponentHandle.h"

namespace ide
{
	//----------------------------
	//-- class ComponentFactory

	class _IDE_API ComponentFactory :
		public util::Singleton
	{
		DECLARE_SINGLETON(ComponentFactory);

	public:
		///--- public API
		Component* CreateComponent(const std::string& id, const Handle& parent=Handle());
		void DestroyComponent(Component* component);
	};
}

#endif	//IDE_COMPONENT_FACTORY_H_
