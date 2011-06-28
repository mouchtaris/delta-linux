/**
 *	CommandServer.h
 *
 * The command server acts as a server receiving commands from the network
 * and dispatching them to the registered clients as a Command Dispatcher.
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef NET_COMMANDSERVER_H
#define NET_COMMANDSERVER_H

#include "Common.h"
#include "CommandDispatcher.h"
#include "Endpoint.h"
#include "Streams.h"

#include <boost/asio.hpp>

namespace comm {
	class encoder;
	class decoder;
}

namespace net {

////////////////////////////////////////////////////////////////////////

template<
	class TLink,
	class TCommand,
	class TEncoder = comm::encoder,
	class TDecoder = comm::decoder,
	class TDispatcher = CommandDispatcher<TLink, TCommand, TEncoder, TDecoder>
>
class TCPCommandServer : public TDispatcher {
public:
	typedef boost::asio::ip::tcp								Proto;
	typedef boost::asio::io_service								IOService;

	typedef TDispatcher											BaseType;
	typedef typename BaseType::ErrorType						ErrorType;
	typedef typename BaseType::LinkPtrType						LinkPtrType;

	TCPCommandServer (IOService& service, uint16 port = 0) :
		m_acceptor(service), m_port(port) {}
	~TCPCommandServer (void) {}

	//******************************************************************

	void Accept (void)
	{
		Proto::endpoint endpoint(Proto::v4(), m_port);
		m_acceptor.open(endpoint.protocol());
		m_acceptor.set_option(Proto::acceptor::reuse_address(true));
		m_acceptor.bind(endpoint);
		m_acceptor.listen();
		this->accept();
	}

	void Close (void) { m_acceptor.close(); }

	//******************************************************************

	IOService&	GetIOService (void) { return m_acceptor.io_service(); }
	uint16		GetPort (void) const { return m_acceptor.local_endpoint().port(); }

private:
	void accept (void)
	{
		LinkPtrType conn(new LinkType(m_acceptor.io_service()));
		m_acceptor.async_accept(
			conn->socket(), boost::bind(&TCPCommandServer::acceptHandler, this, _1, conn)
		);
	}

	//******************************************************************

	void acceptHandler (const ErrorType& error, LinkPtrType conn)
	{
		if (!error) {
			this->ReadCommandFromLink(conn); // Inherited from Dispatcher
			this->accept();
		}
		else
			DBGOUT << "acceptHandler: " << error << DBGENDL;
	}

	//******************************************************************

	Proto::acceptor m_acceptor;
	uint16			m_port;
};

////////////////////////////////////////////////////////////////////////
// Hybrid server
//
template<
	class TLinkUDP,
	class TLinkTCP,
	class TLinkHybrid,
	class TCommand,
	class TEncoder = comm::encoder,
	class TDecoder = comm::decoder,
	class TDispatcher = CommandDispatcher<TLinkHybrid, TCommand, TEncoder, TDecoder>
>
class HybridCommandServer : public TDispatcher {
public:
	typedef boost::asio::ip::tcp			ProtoTCP;
	typedef boost::asio::ip::udp			ProtoUDP;
	typedef boost::asio::io_service			IOService;

	typedef TDispatcher						BaseType;
	typedef typename BaseType::ErrorType	ErrorType;

	typedef TLinkUDP						LinkUDPType;
	typedef TLinkTCP						LinkTCPType;

	typedef typename BaseType::LinkPtrType	LinkPtrType;
	typedef boost::shared_ptr<TLinkUDP>		LinkUDPPtrType;
	typedef boost::shared_ptr<TLinkTCP>		LinkTCPPtrType;

	HybridCommandServer (IOService& service, uint16 port = 0) :
		m_acceptor(service), m_connUDP(new LinkUDPType(service)), m_port(port)
	{
		m_connPtr = LinkPtrType(new LinkType(m_connUDP));
	}
	~HybridCommandServer (void) {}

	//******************************************************************

	void Accept (void)
	{
		ProtoTCP::endpoint endpoint(ProtoTCP::v4(), m_port);
		m_acceptor.open(endpoint.protocol());
		m_acceptor.set_option(ProtoTCP::acceptor::reuse_address(true));
		m_acceptor.bind(endpoint);
		m_acceptor.listen();
		this->acceptTCP();

		ProtoUDP::endpoint udp_endpoint(ProtoUDP::v4(), this->GetPort());
		m_connUDP->socket().open(udp_endpoint.protocol());
		m_connUDP->socket().set_option(ProtoUDP::socket::reuse_address(true));
		m_connUDP->socket().bind(udp_endpoint);
		this->acceptUDP();
	}

	void Close (void)
	{
		m_connUDP->close();
		m_acceptor.close();
	}

	//******************************************************************

	LinkUDPPtrType GetUDPConnection (void) { return m_connUDP; }

	//******************************************************************

	IOService&	GetIOService (void) { return m_acceptor.io_service(); }
	uint16		GetPort (void) const { return m_acceptor.local_endpoint().port(); }

private:
	void acceptTCP (void)
	{
		LinkTCPPtrType conn(new LinkTCPType(m_acceptor.io_service()));
		m_acceptor.async_accept(
			conn->socket(), boost::bind(&HybridCommandServer::acceptTCPHandler, this, _1, conn)
		);
	}

	//******************************************************************

	void acceptUDP (void)
	{
		this->ReadCommandFromLink(m_connPtr);	// Inherited from Dispatcher
	}

	//******************************************************************

	void acceptTCPHandler (const ErrorType& error, LinkTCPPtrType conn)
	{
		if (!error) {
			LinkPtrType link(new LinkType(conn));
			this->ReadCommandFromLink(link);	// Inherited from Dispatcher
			this->acceptTCP();
		}
		else
			DBGOUT << "acceptHandler: " << error << std::endl;
	}

	//******************************************************************

	ProtoTCP::acceptor	m_acceptor;
	LinkUDPPtrType		m_connUDP;
	LinkPtrType			m_connPtr;
	uint16				m_port;
};

////////////////////////////////////////////////////////////////////////

} // namespace net

#endif // NET_COMMANDSERVER_H
