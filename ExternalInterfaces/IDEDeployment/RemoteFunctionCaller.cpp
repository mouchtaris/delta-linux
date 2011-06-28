/**
 *	RemoteFunctionCaller.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "RemoteFunctionCaller.h"
#include "Component.h"
#include "ComponentHandle.h"

namespace ext {

////////////////////////////////////////////////////////////////////////
// RemoteCallException
//
RemoteCallException::RemoteCallException (const Message& msg) : m_msg(msg)
{
}

//**********************************************************************

RemoteCallException::~RemoteCallException (void)
{
}

////////////////////////////////////////////////////////////////////////

RemoteFunctionCaller::RemoteFunctionCaller (
		const std::string&	callerId,
		const std::string&	classId,
		const std::string&	function
	) : m_msg(Message::Endpoint(classId, function)),
		m_callerId(callerId)
{

}

//**********************************************************************

RemoteFunctionCaller::RemoteFunctionCaller (
		const std::string&	callerId,
		const ide::Handle&	handle,
		const std::string&	function
	) : m_msg(Message::Endpoint(handle.GetClassId(), handle.GetSerial(), function)),
		m_callerId(callerId)
{

}

//**********************************************************************

RemoteFunctionCaller::~RemoteFunctionCaller (void)
{

}

//**********************************************************************

void RemoteFunctionCaller::SetFunction (const std::string& function)
	{ m_msg.GetDst().function = function; }

const std::string& RemoteFunctionCaller::GetFunction (void) const
	{ return m_msg.GetDst().function; }

void RemoteFunctionCaller::SetClassId (const std::string& classId)
	{ m_msg.GetDst().classId = classId; }

const std::string& RemoteFunctionCaller::GetClassId (void) const
	{ return m_msg.GetDst().classId; }

//**********************************************************************

void RemoteFunctionCaller::SetInstance (const std::string& classId)
{
	m_msg.GetDst().classId = classId;
	m_msg.GetDst().serial = 0;
}

//**********************************************************************

bool RemoteFunctionCaller::Invoke (Message& result, const Buffer& args) const
{
	m_msg.SetData(args);
	if (!MessageRouterClient::Instance().DispatchMessage(m_callerId, result, m_msg))
		throw RemoteCallException(m_msg);
	return true;
}

//**********************************************************************

bool RemoteFunctionCaller::Invoke (MessageVec& results, const Buffer& args) const
{
	m_msg.SetData(args);
	if (!MessageRouterClient::Instance().DispatchMessageToAll(m_callerId, results, m_msg))
		throw RemoteCallException(m_msg);
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace ext
