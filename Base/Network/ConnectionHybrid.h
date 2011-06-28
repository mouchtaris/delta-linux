/**
 *	ConnectionHybrid.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
 
#ifndef NET_CONNECTIONHYBRID_H
#define NET_CONNECTIONHYBRID_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "Encoding.h"

#include "Connection.h"
#include "ConnectionUDP.h"

#include <boost/variant.hpp>

namespace net {

////////////////////////////////////////////////////////////////////////

class _BASE_API connection_hybrid {
public:
	typedef boost::function<void (const boost_asio_error_t&, const std::string&)> read_handler;

	connection_hybrid (connection_ptr connection) : m_connection(connection) {}
	connection_hybrid (connection_udp_ptr connection) : m_connection(connection) {}
	~connection_hybrid (void) {}

	//******************************************************************

	connection_ptr		get_connection_ptr (void);
	connection_udp_ptr	get_connection_udp_ptr (void);

	//******************************************************************

	void	close (void);
	size_t	write (const std::string& outbuf);
	void	async_read (read_handler handler);

	const std::string remote_address_str(void) const;

private:
	DISABLE_COPY(connection_hybrid);

	typedef boost::variant<connection_ptr, connection_udp_ptr> connection_variant;
	connection_variant m_connection;
};

////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<connection_hybrid> connection_hybrid_ptr;

////////////////////////////////////////////////////////////////////////

} // namespace net

#endif	// NET_CONNECTIONHYBRID_H
