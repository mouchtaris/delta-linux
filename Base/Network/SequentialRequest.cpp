/**
 *	SequentialRequest.cpp
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
#include "SequentialRequest.h"

namespace net
{
	//-------------------------------------------------------//
	//---- class sequential_request -------------------------//

	void sequential_request::async_write(const std::string& buffer, write_handler handler)
	{
		if (pending_request)
			///< if there is a pending write operation wait in queue
			write_queue.push_back(boost::make_tuple(buffer, handler));
		else
			///< else proceed with async_write
			connection::async_write(buffer, handler);
		pending_request=true;
	}

	//-----------------------------------------------------------------------

	void sequential_request::async_read(read_handler handler, uint32 timeout)
	{		
		if (pending_read)
			///< if there is a pending read operation wait in queue
			read_queue.push_back(boost::make_tuple(handler, timeout));
		else
			///< else proceed with async_read
			connection::async_read(
				boost::bind(
					&sequential_request::on_read_done, this,
					_1, _2, handler
				),
				timeout
			);
		pending_read=true;
	}

	//-----------------------------------------------------------------------

	void sequential_request::on_read_done(const boost_asio_error_t& error,
		const std::string& strbuf, read_handler handler)
	{
		//-- Inform caller that async_recv ended
		handler(error, strbuf);

		//-- async_read next read request
		pending_request=false;
		pending_read=false;

		if (!write_queue.empty())
		{
			WriteOperation write_op = write_queue.front();
			async_write(boost::get<0>(write_op), boost::get<1>(write_op));
			write_queue.pop_front();
		}

		if (!read_queue.empty())
		{
			ReadOperation read_op = read_queue.front();
			async_read(boost::get<0>(read_op), boost::get<1>(read_op));
			read_queue.pop_front();
		}
	}

	//-----------------------------------------------------------------------
}