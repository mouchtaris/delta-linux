/**
 *	Endpoint.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef NET_ENDPOINT_H
#define NET_ENDPOINT_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <boost/serialization/access.hpp>

namespace net {

////////////////////////////////////////////////////////////////////////

class _BASE_API Endpoint {
public:
	Endpoint (void) {}
	Endpoint (const String& host, uint16 port);
	Endpoint (const String& host, const String& port);

	Endpoint (uint16 port);			// Localhost
	Endpoint (const String& port);	// Localhost

	~Endpoint (void);

	const String&	GetHost (void) const { return m_host; }
	const String&	GetPort (void) const { return m_port; }

	uint16			GetPortNum (void) const;

	void SetHost (const String& host) { m_host = host; }
	void SetPort (const String& port) { m_port = port; }

	void SetPortNum (uint16 port);

	//******************************************************************

	bool operator== (const Endpoint& p) const;
	bool operator!= (const Endpoint& p) const;

	bool operator< (const Endpoint& p) const;
	bool operator> (const Endpoint& p) const;

	//******************************************************************

	operator bool (void) const { return !m_host.empty(); }

	//******************************************************************

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize (Archive& ar, const unsigned int version)
		{ ar & m_host & m_port; }

	String m_host;
	String m_port;
};

////////////////////////////////////////////////////////////////////////

} // namespace net

////////////////////////////////////////////////////////////////////////
// Output
//
_BASE_API std::ostream& operator<< (std::ostream& stream, const net::Endpoint& p);

////////////////////////////////////////////////////////////////////////

#endif // NET_ENDPOINT_H
