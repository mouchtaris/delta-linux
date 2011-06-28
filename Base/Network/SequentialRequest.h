/**
 *	SequentialRequest.h
 *
 *	-- Network Base --
 *
 *	The sequential_request class inherits connection
 *	class and makes multiple requests to queue in
 *	sequential requests waiting for responses.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef NET_NETWORK_BATCH_CONNECTION_H_
#define NET_NETWORK_BATCH_CONNECTION_H_

#pragma warning(disable: 4267)

#include "Portability.h"

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "Connection.h"

#include <boost/asio.hpp>
#include <boost/tuple/tuple.hpp>

#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

namespace net
{
	//----------------------------
	//-- class sequential_request

	class _BASE_API sequential_request :
		public connection
	{
	public:
		///--- constructors / destructor
		sequential_request(boost::asio::io_service& io_service)
			: connection(io_service), pending_request(false), pending_read(false) {}

		///--- public API
		void async_write(const std::string& buffer, write_handler handler);
		void async_read(read_handler handler, uint32 timeout=0);

	private:
		///--- private API
		void on_read_done(const boost_asio_error_t& error,
			const std::string& strbuf, read_handler handler);

		///--- type definitions
		typedef boost::tuple<std::string, write_handler> WriteOperation;///< { outbuf, write_handler }
		typedef boost::tuple<read_handler, uint32> ReadOperation;		///< { read_handler, timeout }

		typedef std::list<WriteOperation> WriteOperationQueue;
		typedef std::list<ReadOperation> ReadOperationQueue;

		///--- private members
		bool pending_request;
		bool pending_read;
		WriteOperationQueue write_queue;
		ReadOperationQueue read_queue;
	};

	typedef boost::shared_ptr<connection> connection_ptr;
}

#endif	//NET_NETWORK_BATCH_CONNECTION_H_
