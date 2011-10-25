/**
 *	NetUtils.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef NET_NETUTILS_H
#define NET_NETUTILS_H

#include "Portability.h"
#include "Common.h"
#include "StringUtils.h"
#include "Endpoint.h"
#include "Streams.h"

namespace net {

////////////////////////////////////////////////////////////////////////

template <class Proto>
const Endpoint convert (const typename Proto::endpoint& endpoint)
{
	return Endpoint(util::std2str(endpoint.address().to_string()), endpoint.port());
}

//**********************************************************************

template <class From, class To>
const typename To::endpoint convert (const typename From::endpoint& endpoint)
{
	typedef typename To::endpoint TargetEndpoint;
	return TargetEndpoint(endpoint.address(), endpoint.port());
}

//**********************************************************************

template <class Proto>
const typename Proto::endpoint resolve (const Endpoint& p, boost::asio::io_service& io)
{
	typename Proto::resolver resolver(io);
	typename Proto::resolver::query query(util::str2std(p.GetHost()), util::str2std(p.GetPort()));
	typename Proto::resolver::iterator iter = resolver.resolve(query), end;
	return iter != end ? *iter : typename Proto::endpoint();
}

//**********************************************************************

template <class Proto, class ErrorHandler>
void connect_to (const Endpoint& p, typename Proto::socket& socket, ErrorHandler errHandler)
{
	typename Proto::resolver resolver(socket.BOOST_SOCKET_IO_SERVICE());
	typename Proto::resolver::query query(util::str2std(p.GetHost()), util::str2std(p.GetPort()));
	typename Proto::resolver::iterator iter = resolver.resolve(query), end;

	DBGOUT << "connect_to: " << p << DBGENDL;

	BOOST_ASIO_ERROR_DEFINE_ASIO_ERROR_VARIABLE(error, boost::asio::error::host_not_found);
	while (error && iter != end) {
		socket.close();
		socket.connect(*iter++, BOOST_ASIO_ASSIGN_ERROR(error));
	}
	errHandler(error);
}

//**********************************************************************

template <class Proto>
bool connect_to (const Endpoint& p, typename Proto::socket& socket)
{
	boost_asio_error_t error;
	connect_to<Proto>(p, socket, BOOST_ASIO_ASSIGN_ERROR_HANDLER(error));
	return !error;
}

//**********************************************************************

template <class Proto, class Handler>
void async_connect_to (const Endpoint& p, typename Proto::socket& socket, Handler handler)
{
	typename Proto::resolver resolver(socket.BOOST_SOCKET_IO_SERVICE());
	typename Proto::resolver::query query(util::str2std(p.GetHost()), util::str2std(p.GetPort()));
	typename Proto::resolver::iterator iter = resolver.resolve(query), end;

	DBGOUT << "async_connect_to: " << p << std::endl;

	if (iter != end)
		socket.async_connect(*iter, handler);
	else
		handler(BOOST_ASIO_ERROR_MAKE_ASIO_ERROR(boost::asio::error::host_not_found));
}

////////////////////////////////////////////////////////////////////////

} // namespace net

#endif // NET_NETUTILS_H
