/**
 *	ConnectionUDP.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
 
#ifndef NET_CONNECTIONUDP_H
#define NET_CONNECTIONUDP_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "Encoding.h"

#include <boost/function.hpp>
#include <boost/asio/ip/udp.hpp>

namespace net {

////////////////////////////////////////////////////////////////////////

class _BASE_API connection_udp {
public:
	typedef boost::asio::ip::udp													proto_type;
	typedef boost::function<void (const boost_asio_error_t&, const std::string&)>	read_handler;

	connection_udp (boost::asio::io_service& service) :
		m_sock(service) {}

	connection_udp (boost::asio::io_service& service, uint16 port) :
		m_sock(service, proto_type::endpoint(proto_type::v4(), port)) {}

	~connection_udp (void) {}

	//******************************************************************

	operator proto_type::socket& (void) { return m_sock; }

	//******************************************************************

	proto_type::socket&	socket() { return m_sock; }
	void				close (void) { m_sock.close(); }
	const std::string	remote_address_str(void) const;

	//******************************************************************

	size_t	write (const std::string& outbuf);
	size_t	write (const std::string& outbuf, const proto_type::endpoint& peer);
	void	async_read (read_handler handler);

protected:
	void handle_receive_from (
		const boost_asio_error_t& error, size_t bytes_recvd, read_handler handler
	);

private:
	DISABLE_COPY(connection_udp);

	enum { max_length = 1024 };

	proto_type::socket		m_sock;
	proto_type::endpoint	m_remote_endpoint;
	char					m_data[max_length];
};

////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<connection_udp> connection_udp_ptr;

////////////////////////////////////////////////////////////////////////

} // namespace net

#endif	// NET_CONNECTIONUDP_H
