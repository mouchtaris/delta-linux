/**
 *	Connection.cpp
 *
 *	-- Network Base --
 *
 *	The connection class provides serialization
 *	primitives on top of a socket.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#include "Connection.h"

namespace net
{
	//-------------------------------------------------------//
	//---- class connection ---------------------------------//

	const std::string connection::remote_address_str(void) const
	{
		return sock.remote_endpoint().address().to_string();
	}

	//-----------------------------------------------------------------------

	size_t connection::write(const std::string& outbuf)
	{
		//-- Format the header
		std::ostringstream header_stream;
		header_stream << std::setw(HEADER_SIZE) << std::hex << outbuf.size();
		std::string outbound_header = header_stream.str();
		if (outbound_header.size() != HEADER_SIZE)
			//-- Something went wrong, inform the caller
			return 0;

		//-- Write the serialized data to the socket. Use "gather-write" to send
		//   both the header and the data in a single write operation.
		typedef std::vector<boost::asio::const_buffer> GatherBuffers;
		GatherBuffers buffers;
		buffers.push_back(boost::asio::buffer(outbound_header));
		buffers.push_back(boost::asio::buffer(outbuf));

		return boost::asio::write(sock, buffers);
	}

	//-----------------------------------------------------------------------

	size_t connection::read(std::string& inbuf, uint32 timeout)
	{
		//-- Setup connection's timeout
		if (timeout)
		{
			timer.expires_from_now(boost::posix_time::seconds(timeout));
			timer.async_wait(boost::bind(&connection::read_timeout, this, _1));
		}

		//-- Issue a read operation to read exactly the number of bytes in a header
		if (boost::asio::read(sock, boost::asio::buffer(inbound_header)) != HEADER_SIZE)
			return 0;

		//-- Determine the length of the serialized data
		std::istringstream is(std::string(inbound_header.c_array(), HEADER_SIZE));
		std::size_t inbound_datasize = 0;
		if (!(is >> std::hex >> inbound_datasize))
			//-- Header doesn't seem to be valid. Inform the caller
			return 0;

		//-- Start a synchronous call to receive the data
		inbound_data.resize(inbound_datasize);
		if (size_t bytes = boost::asio::read(sock, boost::asio::buffer(inbound_data)))
		{
			inbuf.assign(inbound_data.begin(), inbound_data.end());
			return bytes;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	void connection::async_write(const std::string& outbuf, write_handler handler)
	{
		//-- Format the header
		std::ostringstream header_stream;
		header_stream << std::setw(HEADER_SIZE) << std::hex << outbuf.size();
		std::string outbound_header = header_stream.str();
		if (outbound_header.size() != HEADER_SIZE)
		{
			//-- Something went wrong, inform the caller
			handler(BOOST_ASIO_ERROR_MAKE_INVALID_ARGUMENT_ERROR, 0);
			return;
		}

		//-- Write the serialized data to the socket. Use "gather-write" to send
		//   both the header and the data in a single write operation.
		typedef std::vector<boost::asio::const_buffer> GatherBuffers;
		GatherBuffers buffers;
		buffers.push_back(boost::asio::buffer(outbound_header));
		buffers.push_back(boost::asio::buffer(outbuf));
		boost::asio::async_write(sock, buffers, handler);
	}

	//-----------------------------------------------------------------------

	void connection::async_read(read_handler handler, uint32 timeout)
	{
		//-- Issue a read operation to read exactly the number of bytes in a header
		boost::asio::async_read(
			sock,
			boost::asio::buffer(inbound_header),
			boost::bind(
				&connection::read_header_handler, this,
				boost::asio::placeholders::error,
				boost::make_tuple(handler),
				timeout
			)
		);

		//-- Setup connection's timeout
		if (timeout)
		{
			timer.expires_from_now(boost::posix_time::seconds(timeout));
			timer.async_wait(boost::bind(&connection::read_timeout, this, _1));
		}
	}

	//-----------------------------------------------------------------------

	void connection::read_header_handler(const boost_asio_error_t& error,
		boost::tuple<read_handler> handler, uint32 timeout)
	{
		//-- Cancel timeout timer
		timer.cancel();

		if (error)
			boost::get<0>(handler)(error, std::string());
		else
		{
			//-- Determine the length of the serialized data
			std::istringstream is(std::string(inbound_header.c_array(), HEADER_SIZE));
			std::size_t inbound_datasize = 0;
			if (!(is >> std::hex >> inbound_datasize))
			{
				//-- Header doesn't seem to be valid. Inform the caller
				boost::get<0>(handler)(BOOST_ASIO_ERROR_MAKE_INVALID_ARGUMENT_ERROR, std::string());
				return;
			}

			//-- Start an asynchronous call to receive the data
			inbound_data.resize(inbound_datasize);
			boost::asio::async_read(
				sock,
				boost::asio::buffer(inbound_data),
				boost::bind(
					&connection::read_data_handler, this,
					boost::asio::placeholders::error,
					handler
				)
			);

			//-- Setup connection's timeout
			if (timeout)
			{
				timer.expires_from_now(boost::posix_time::seconds(timeout));
				timer.async_wait(boost::bind(&connection::read_timeout, this, _1));
			}
		}
	}

	//-----------------------------------------------------------------------

	void connection::read_data_handler(const boost_asio_error_t& error,
		boost::tuple<read_handler> handler)
	{
		//-- Cancel timeout timer
		timer.cancel();

		std::string str;
		if (!error)
			str.assign(&inbound_data[0], inbound_data.size());

		//-- Inform caller that async_recv ended
		boost::get<0>(handler)(error, str);
	}

	//-----------------------------------------------------------------------

	void connection::read_timeout(const boost_asio_error_t& error) {
		if (!error)			///< if !error timer expired, else I've canceled it
			sock.close();
	}

	//-----------------------------------------------------------------------
}