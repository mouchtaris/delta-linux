/**
 *	MessageRouterClient.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "MessageRouterClient.h"
#include "IOServiceThread.h"
#include "NetUtils.h"

namespace ext {

////////////////////////////////////////////////////////////////////////

#define DEFAULT_TIMEOUT 30000

////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(MessageRouterClient);

//**********************************************************************

bool MessageRouterClient::Connect (const String& host, ushort port)
{
	typedef boost::asio::ip::tcp ProtoTCP;

	m_connection = net::connection_ptr(
			new net::connection(net::IOServiceThreadSingleton::Instance().GetIOService())
		);
	if (m_isConnected = net::connect_to<ProtoTCP>(net::Endpoint(host, port), *m_connection))
		return true;

	m_connection = net::connection_ptr();
	return false;
}

//**********************************************************************

void MessageRouterClient::Disconnect (void)
{
	if (m_isConnected) {
		m_connection->close();
		m_isConnected = false;
		m_connection = net::connection_ptr();
	}
}

//**********************************************************************

bool MessageRouterClient::DispatchMessage (
		const std::string& callerId, Message& result, const Message& msg
	)
{
	if (!m_isConnected)
		return false;
	m_connection->write(comm::encoder() << 0 << callerId << msg);

	Buffer buff;
	size_t readBytes = m_connection->read(buff, DEFAULT_TIMEOUT);

	bool retVal = false;
	if (readBytes != 0)
		comm::decoder(buff) >> retVal >> result;
	return retVal;
}

//**********************************************************************

bool MessageRouterClient::DispatchMessageToAll (
		const std::string& callerId, MessageVec& results, const Message& msg
	)
{
	if (!m_isConnected)
		return false;
	m_connection->write(comm::encoder() << 1 << callerId << msg);

	Buffer buff;
	size_t readBytes = m_connection->read(buff, DEFAULT_TIMEOUT);

	bool retVal = false;
	if (readBytes != 0)
		comm::decoder(buff) >> retVal >> results;
	return retVal;
}

//**********************************************************************

void MessageRouterClient::DispatchSignal (const Message& msg)
{
	if (!m_isConnected)
		return;
	m_connection->write(comm::encoder() << 2 << msg);
}

////////////////////////////////////////////////////////////////////////

} // namespace ext
