/**
 *	ComponentSignalEmitter.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentSignalEmitter.h"
#include "MessageRouter.h"
#include "Component.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

ComponentSignalEmitter::ComponentSignalEmitter (const std::string& classId, const std::string& signalId)
{
	m_msg.GetSrc().classId = classId;
	m_msg.GetSrc().function = signalId;
}

//**********************************************************************

ComponentSignalEmitter::~ComponentSignalEmitter (void)
{

}

//**********************************************************************

bool ComponentSignalEmitter::Invoke (Component* comp, const Buffer& args) const
{
	m_msg.GetSrc().serial = comp->GetSerial();
	m_msg.SetData(args);

	MessageRouter::Instance().DispatchSignal(m_msg);
	return true;
}

//**********************************************************************

bool ComponentSignalEmitter::Invoke (const Buffer& args) const
{
	assert(m_msg.GetSrc().serial == 0);
	m_msg.SetData(args);

	MessageRouter::Instance().DispatchSignal(m_msg);
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
