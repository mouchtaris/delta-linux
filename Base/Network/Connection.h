/**
 *	Connection.h
 *
 *	-- Network Base --
 *
 *	The connection class provides serialization
 *	primitives on top of a socket.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#ifndef NET_NETWORK_CONNECTION_H_
#define NET_NETWORK_CONNECTION_H_

#pragma warning(disable: 4267)

#include "Portability.h"

#include "Encoding.h"
#include "LibraryAPIDecls.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/noncopyable.hpp>

#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

namespace net
{
	//----------------------------
	//-- class connection

	class _BASE_API connection
	{
	public:
		///--- type definitions
		typedef boost::function<void (const boost_asio_error_t&, std::size_t)> write_handler;
		typedef boost::function<void (const boost_asio_error_t&, const std::string&)> read_handler;

		///--- constructors / destructor
		connection(boost::asio::io_service& io_service)
			: sock(io_service), timer(io_service) {}

		///--- public API
		boost::asio::ip::tcp::socket& socket()			{ return sock; }
		operator boost::asio::ip::tcp::socket& (void)	{ return sock; }

		const std::string remote_address_str(void) const;

		void close(void)								{ sock.close(); }

		size_t write(const std::string& outbuf);
		size_t read(std::string& inbuf, uint32 timeout=0);

		void async_write(const std::string& buffer, write_handler handler);
		void async_read(read_handler handler, uint32 timeout=0);

		///--- template convenience API
		template <typename T, typename Handler>
		void async_write_object(const T& t, Handler handler) {
			async_write(comm::encoder(t), handler);
		}
		template <typename T, typename Handler>
		void async_read_object(T& t, Handler handler, uint32 timeout=0) {
			async_read(
				boost::bind(
				&connection::async_read_handler<T, Handler>, this,
					_1, _2,
					boost::ref(t),
					boost::make_tuple(handler)
				),
				timeout
			);
		}

	protected:
		///--- protected API
		void read_header_handler(const boost_asio_error_t& error,
			boost::tuple<read_handler> handler, uint32 timeout);
		void read_data_handler(const boost_asio_error_t& error,
			boost::tuple<read_handler> handler);

		void read_timeout(const boost_asio_error_t& error);

		template <typename T, typename Handler>
		void async_read_handler(const boost_asio_error_t& error, const std::string& str,
				T& t, boost::tuple<Handler> handler) {
			if (!str.empty())
				comm::decoder(str, t);
			boost::get<0>(handler)(error);
		}

		///--- type definitions
		enum { HEADER_SIZE = 8 };

		///--- private members
		boost::asio::ip::tcp::socket sock;
		boost::asio::deadline_timer timer;

		boost::array<char, HEADER_SIZE> inbound_header;
		std::vector<char> inbound_data;

		private:
			DISABLE_COPY(connection);
	};

	typedef boost::shared_ptr<connection> connection_ptr;
}

#endif	//NET_NETWORK_CONNECTION_H_
