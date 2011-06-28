/**
 *	ConnectionUDP.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ConnectionUDP.h"
#include "Streams.h"

//#include <boost/bind/protect.hpp>

namespace net {

////////////////////////////////////////////////////////////////////////
// ConnectionUDP class
//
const std::string connection_udp::remote_address_str(void) const
{
	return m_remote_endpoint.address().to_string();
}

//**********************************************************************

size_t connection_udp::write (const std::string& outbuf)
{
	assert(outbuf.size() < max_length);
	return m_sock.send_to(boost::asio::buffer(outbuf), m_remote_endpoint);
}

//**********************************************************************

size_t connection_udp::write (const std::string& outbuf, const proto_type::endpoint& peer)
{
	assert(outbuf.size() < max_length);
	return m_sock.send_to(boost::asio::buffer(outbuf), peer);
}

//**********************************************************************

void connection_udp::async_read (read_handler handler)
{
	m_sock.async_receive_from(
		boost::asio::buffer(m_data, max_length),
		m_remote_endpoint,
		boost::bind(&connection_udp::handle_receive_from, this, _1, _2, handler)
	);
}

//**********************************************************************

void connection_udp::handle_receive_from (
		const boost_asio_error_t& error, size_t bytes_recvd, read_handler handler
	)
{
	if (!error && bytes_recvd > 0) {
		handler(error, std::string(m_data, bytes_recvd));
		this->async_read(handler);
	}
	else if (BOOST_ASIO_ERROR_NOT_EQUAL_TO_OPERATION_ABORTED(error))
		this->async_read(handler);
}

////////////////////////////////////////////////////////////////////////

} // namespace net
