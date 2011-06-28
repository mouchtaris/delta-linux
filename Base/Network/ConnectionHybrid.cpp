/**
 *	ConnectionHybrid.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ConnectionHybrid.h"
#include "Streams.h"

namespace net {

////////////////////////////////////////////////////////////////////////
// Visitors for variant
//
class close_visitor : public boost::static_visitor<> {
public:
	template<class TConnectionPtr>
	void operator() (TConnectionPtr conn) const { conn->close(); }
};

//**********************************************************************

class write_visitor : public boost::static_visitor<size_t> {
public:
	write_visitor (const std::string& data) : m_data(data) {}

	template<class TConnectionPtr>
	size_t operator() (TConnectionPtr conn) const { return conn->write(m_data); }
	
private:
	const std::string& m_data;
};

//**********************************************************************

class async_read_visitor : public boost::static_visitor<> {
public:
	async_read_visitor (const connection_hybrid::read_handler& handler) : m_handler(handler) {}

	template<class TConnectionPtr>
	void operator() (TConnectionPtr conn) const { conn->async_read(m_handler); }
	
private:
	const connection_hybrid::read_handler& m_handler;
};

//**********************************************************************

class remote_address_str_visitor : public boost::static_visitor<const std::string> {
public:
	template<class TConnectionPtr>
	const std::string operator() (TConnectionPtr conn) const
		{ return conn->remote_address_str(); }
};

////////////////////////////////////////////////////////////////////////
// ConnectionHybrid class
//
connection_ptr connection_hybrid::get_connection_ptr (void)
{
	return boost::get<connection_ptr>(m_connection);
}

//**********************************************************************

connection_udp_ptr connection_hybrid::get_connection_udp_ptr (void)
{
	return boost::get<connection_udp_ptr>(m_connection);
}

//**********************************************************************

void connection_hybrid::close (void)
{
	boost::apply_visitor(close_visitor(), m_connection);
}

//**********************************************************************

size_t connection_hybrid::write (const std::string& outbuf)
{
	return boost::apply_visitor(write_visitor(outbuf), m_connection);
}

//**********************************************************************

void connection_hybrid::async_read (read_handler handler)
{
	boost::apply_visitor(async_read_visitor(handler), m_connection);
}

//**********************************************************************

const std::string connection_hybrid::remote_address_str (void) const
{
	return boost::apply_visitor(remote_address_str_visitor(), m_connection);
}

////////////////////////////////////////////////////////////////////////

} // namespace net
