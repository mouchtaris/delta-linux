/**
 *	ComponentFactory.cpp
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
#include "ComponentFactory.h"
#include "ComponentRegistry.h"

#include <boost/bind.hpp>

#include <algorithm>

namespace ide
{
	//-------------------------------------------------------//
	//---- class ComponentFactory ---------------------------//

	IMPLEMENT_SINGLETON(ComponentFactory);

	//-----------------------------------------------------------------------

	Component* ComponentFactory::CreateComponent(const std::string& id, const Handle& parent)
	{
		Component* comp = ComponentRegistry::Instance().GetComponentEntry(id).Create();
		if (comp && parent)
			comp->SetParent(parent.Resolve());
		return comp;
	}

	//-----------------------------------------------------------------------

	void ComponentFactory::DestroyComponent(Component* component)
	{
		if (!component)
			return;

		ComponentRegistry::Instance().GetComponentEntry(component->GetClassId()).
			Destroy(component);
	}

	//-----------------------------------------------------------------------
}
