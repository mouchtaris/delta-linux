/**
 *	Endpoint.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "Endpoint.h"

#pragma warning(disable: 4267)

#include <boost/lexical_cast.hpp>
#include <boost/asio/ip/host_name.hpp>

namespace net {

////////////////////////////////////////////////////////////////////////
// Endpoint class
//
Endpoint::Endpoint (const String& host, uint16 port) :
	m_host(host), m_port(boost::lexical_cast<String>(port))
{

}

//**********************************************************************

Endpoint::Endpoint (const String& host, const String& port) :
	m_host(host), m_port(port)
{

}

//**********************************************************************

Endpoint::Endpoint (const String& port) :
	m_host(util::std2str(boost::asio::ip::host_name())), m_port(port)
{

}

//**********************************************************************

Endpoint::Endpoint (uint16 port) :
	m_host(util::std2str(boost::asio::ip::host_name())),
	m_port(boost::lexical_cast<String>(port))
{

}

//**********************************************************************

Endpoint::~Endpoint (void)
{
}

//**********************************************************************

uint16 Endpoint::GetPortNum (void) const
	{ return boost::lexical_cast<uint16>(m_port); }

void Endpoint::SetPortNum (uint16 port)
	{ m_port = boost::lexical_cast<String>(port); }

//**********************************************************************

bool Endpoint::operator== (const Endpoint& p) const
	{ return m_host == p.m_host && m_port == p.m_port; }

bool Endpoint::operator!= (const Endpoint& p) const
	{ return m_host != p.m_host || m_port != p.m_port; }

bool Endpoint::operator< (const Endpoint& p) const
	{ return m_host != p.m_host ? m_host < p.m_host : m_port < p.m_port; }

bool Endpoint::operator> (const Endpoint& p) const
	{ return m_host != p.m_host ? m_host > p.m_host : m_port > p.m_port; }
 
////////////////////////////////////////////////////////////////////////

} // namespace net

////////////////////////////////////////////////////////////////////////
// Other functions
//
std::ostream& operator<< (std::ostream& stream, const net::Endpoint& p)
{
	return stream << p.GetHost() << ":" << p.GetPort();
}
