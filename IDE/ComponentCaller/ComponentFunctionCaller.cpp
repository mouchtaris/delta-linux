/**
 *	ComponentFunctionCaller.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentFunctionCaller.h"
#include "MessageRouter.h"
#include "Component.h"
#include "ComponentHandle.h"

namespace ide {

////////////////////////////////////////////////////////////////////////
// CallException
//
CallException::CallException (const Message& msg) : m_msg(msg)
{
}

//**********************************************************************

CallException::~CallException (void)
{
}

////////////////////////////////////////////////////////////////////////

ComponentFunctionCaller::ComponentFunctionCaller (
		Component*			caller,
		const Handle&		handle,
		const std::string&	function
	) : m_msg(Message::Endpoint(handle.GetClassId(), handle.GetSerial(), function)),
		m_callerId(caller->GetClassId())
{

}

//**********************************************************************

ComponentFunctionCaller::ComponentFunctionCaller (
		Component*			caller,
		const std::string&	classId,
		const std::string&	function
	) : m_msg(Message::Endpoint(classId, function)),
		m_callerId(caller->GetClassId())
{

}

//**********************************************************************

ComponentFunctionCaller::ComponentFunctionCaller (
		const std::string&	callerId,
		const Handle&		handle,
		const std::string&	function
	) : m_msg(Message::Endpoint(handle.GetClassId(), handle.GetSerial(), function)),
		m_callerId(callerId)
{

}

//**********************************************************************

ComponentFunctionCaller::ComponentFunctionCaller (
		const std::string&	callerId,
		const std::string&	classId,
		const std::string&	function
	) : m_msg(Message::Endpoint(classId, function)),
		m_callerId(callerId)
{

}

//**********************************************************************

ComponentFunctionCaller::~ComponentFunctionCaller (void)
{

}

//**********************************************************************

void ComponentFunctionCaller::SetFunction (const std::string& function)
	{ m_msg.GetDst().function = function; }

const std::string& ComponentFunctionCaller::GetFunction (void) const
	{ return m_msg.GetDst().function; }

void ComponentFunctionCaller::SetClassId (const std::string& classId)
	{ m_msg.GetDst().classId = classId; }

const std::string& ComponentFunctionCaller::GetClassId (void) const
	{ return m_msg.GetDst().classId; }

//**********************************************************************

void ComponentFunctionCaller::SetInstance (const std::string& classId)
{
	m_msg.GetDst().classId = classId;
	m_msg.GetDst().serial = 0;
}

void ComponentFunctionCaller::SetInstance (Component* comp)
{
	m_msg.GetDst().classId = comp->GetClassId();
	m_msg.GetDst().serial = comp->GetSerial();
}

void ComponentFunctionCaller::SetInstance (const Handle& handle)
{
	m_msg.GetDst().classId = handle.GetClassId();
	m_msg.GetDst().serial = handle.GetSerial();
}

//**********************************************************************

void ComponentFunctionCaller::SetBaseCall (bool val)
{
	m_msg.GetDst().baseCall = val;
}

//**********************************************************************

bool ComponentFunctionCaller::Invoke (Message& result, const Buffer& args) const
{
	m_msg.SetData(args);
	if (!MessageRouter::Instance().DispatchMessage(m_callerId, result, m_msg))
		throw CallException(m_msg);
	return true;
}

//**********************************************************************

bool ComponentFunctionCaller::Invoke (MessageVec& results, const Buffer& args) const
{
	m_msg.SetData(args);
	if (!MessageRouter::Instance().DispatchMessageToAll(m_callerId, results, m_msg))
		throw CallException(m_msg);
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
