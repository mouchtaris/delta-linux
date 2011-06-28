/**
 *	ComponentHandle.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentHandle.h"
#include "Component.h"
#include "ComponentRegistry.h"

#include <boost/lexical_cast.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////
// ComponentHandle class
//
Handle::Handle (Component* comp) :
	m_classId(comp->GetClassId()), m_serial(comp->GetSerial())
{
}

//**********************************************************************

Handle::Handle (const std::string& classId, uint serial) :
	m_classId(classId), m_serial(serial)
{
//	assert(!this->IsDangling());
}

//**********************************************************************

Component* Handle::Resolve (void) const
{
	if (m_classId.empty())
		return (Component *) 0;
	else
		return ComponentRegistry::Instance().GetComponentEntry(m_classId).GetInstance(m_serial);
}

//**********************************************************************

Handle& Handle::operator= (const Handle& handle)
{
	m_classId = handle.m_classId;
	m_serial = handle.m_serial;
	return *this;
}

//**********************************************************************

bool Handle::operator== (const Handle& handle) const
	{ return m_classId == handle.m_classId && m_serial == handle.m_serial; }

bool Handle::operator!= (const Handle& handle) const
	{ return m_classId != handle.m_classId || m_serial != handle.m_serial; }

bool Handle::operator< (const Handle& handle) const
	{ return m_classId < handle.m_classId || m_serial < handle.m_serial; }

////////////////////////////////////////////////////////////////////////

const Handle nullHandle;

////////////////////////////////////////////////////////////////////////

} // namespace ide
