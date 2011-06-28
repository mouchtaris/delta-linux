/**
 *	MessageRouterServer.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "MessageRouterServer.h"
#include "MessageRouter.h"
#include "IOServiceThread.h"
#include "NetUtils.h"
#include "UndefineAllWin32Shit.h"

#include <boost/lexical_cast.hpp>
#include <stdio.h>

namespace ide {

////////////////////////////////////////////////////////////////////////

#define SERVER_PORT_INFO_FILE		"MessageRouterServerPort.txt"
#define	NEGOTIATIONCONNECT_TIMEOUT	5000

////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON_WITH_ARGS(MessageRouterServer, (int32 port), (port));


//**********************************************************************

MessageRouterServer::MessageRouterServer(int32 port) : port(port)
{
}

//**********************************************************************

void MessageRouterServer::OnInitialize (void)
{
	m_server = new CommandServer(net::IOServiceThreadSingleton::Instance().GetIOService());
	m_server->SetContinuousMonitor(true);

	m_server->RegisterCommand(0,
		boost::bind(&MessageRouterServer::cmdDispatchMessage, this, _1, _2, _3));
	m_server->RegisterCommand(1,
		boost::bind(&MessageRouterServer::cmdDispatchMessageToAll, this, _1, _2, _3));
	m_server->RegisterCommand(2,
		boost::bind(&MessageRouterServer::cmdDispatchSignal, this, _1, _2, _3));

	m_server->Accept();

	if (port)
	{
		typedef boost::asio::ip::tcp ProtoTCP;
		net::connection_ptr connection = net::connection_ptr(
			new net::connection(net::IOServiceThreadSingleton::Instance().GetIOService())
		);
		if (net::connect_to<ProtoTCP>(net::Endpoint(port), *connection))
			connection->write(boost::lexical_cast<std::string>(m_server->GetPort()));
		connection->close();
		connection = net::connection_ptr();
	}
	else
	{
		FILE* fp = fopen(SERVER_PORT_INFO_FILE, "wt");
		fprintf(fp, "Message router server port %u.\n", (uint32) m_server->GetPort());
		fclose(fp);
	}
}

//**********************************************************************

void MessageRouterServer::OnCleanUp (void)
{
	delete m_server;
}

//**********************************************************************

bool MessageRouterServer::cmdDispatchMessage (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	std::string callerId;
	Message msg;
	dec >> callerId >> msg;

	Message result;
	bool retVal = MessageRouter::Instance().DispatchMessage(callerId, result, msg);
	conn->write(comm::encoder() << retVal << result);
	return true;
}

//**********************************************************************

bool MessageRouterServer::cmdDispatchMessageToAll (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	std::string callerId;
	Message msg;
	dec >> callerId >> msg;

	MessageVec results;
	bool retVal = MessageRouter::Instance().DispatchMessageToAll(callerId, results, msg);
	conn->write(comm::encoder() << retVal << results);
	return true;
}

//**********************************************************************

bool MessageRouterServer::cmdDispatchSignal (
		int32 command, net::connection_ptr conn, comm::decoder& dec
	)
{
	Message msg;
	dec >> msg;

	MessageRouter::Instance().DispatchSignal(msg);
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
