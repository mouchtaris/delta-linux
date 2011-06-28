/**
 *	ComponentHolder.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentHolder.h"
#include "Component.h"
#include "ComponentRegistry.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// ComponentHolder class
//
IMPLEMENT_SINGLETON(ComponentHolder);

//**********************************************************************

ComponentHolder::ComponentHolder (void)
{

}

//**********************************************************************

ComponentHolder::~ComponentHolder (void)
{

}

//**********************************************************************

const ComponentHolder::ComponentList ComponentHolder::Find (const std::string& classId) const
{
	return ComponentRegistry::Instance().GetComponentEntry(classId).GetInstances(true);
}

//**********************************************************************

Component* ComponentHolder::Find (const std::string& classId, uint instanceSerial) const
{
	return ComponentRegistry::Instance().GetComponentEntry(classId).GetInstance(instanceSerial, true);
}

//**********************************************************************


////////////////////////////////////////////////////////////////////////

} // namespace ide
