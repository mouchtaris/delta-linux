/**
 *	PeerSocketListener.cpp
 *
 *	-- Chord Routing Algorithm --
 *
 *	PeerSocketListener is the class responsible for listening
 *	to UDP socket for peer nodes messages.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#include "PeerSocketListener.h"
#include "VirtualPeer.h"

#include "Streams.h"

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>
#include <boost/static_assert.hpp>

#include <iostream>

namespace chord
{
	//-------------------------------------------------------//
	//---- Class PeerSocketListener -------------------------//

	PeerSocketListener::PeerSocketListener(boost::asio::ip::udp::socket& socket,
		const udp_endpoint& local_endpoint)
		: socket(socket), local_endpoint(local_endpoint), responseTimeoutTimer(0)
	{
		//-- Construct timers
		responseTimeoutTimer = new boost::asio::deadline_timer(
			socket.io_service(),
			boost::posix_time::seconds(RESPONSE_TIMEOUT)
		);

		//-- Initialize periodic response timeout check
		ResetTimeoutTimer();

		//-- Start receiving on socket
		StartReceive();
	}

	//-----------------------------------------------------------------------

	void PeerSocketListener::AsyncWaitRequest(PeerRequest::CMD cmd, RequestHandler handler)
	{
		//-- Register the response handler
		requestHandlers[cmd] = handler;
	}

	//-----------------------------------------------------------------------

	void PeerSocketListener::AsyncWaitResponse(ResponseEntry entry, ResponseHandler handler)
	{
		//-- Register the response handler
		responseHandlers[entry] = handler;
	}

	//-----------------------------------------------------------------------

	void PeerSocketListener::StartReceive(void)
	{
		socket.async_receive_from(
			boost::asio::buffer(recv_buffer), remote_endpoint,
			boost::bind(
				&PeerSocketListener::HandleReceive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}

	//-----------------------------------------------------------------------

	void PeerSocketListener::HandleReceive(const boost::asio::error& error,
		std::size_t /*bytes_transferred*/)
	{
		if (!error)
		{
			//std::cout << "\tmessage " << (int)recv_buffer[0] << " received from " << remote_endpoint << std::endl;

			//-- Distinguish between requests & responses
			BOOST_STATIC_ASSERT(PeerRequest::CMD_SIZE == 1);
			if (recv_buffer[0] < PeerRequest::CMD_TOTAL)
			{
				assert(recv_buffer[0] >= PeerRequest::FIND_SUCCESSOR &&
					recv_buffer[0] < PeerRequest::CMD_TOTAL);

				//-- Handle peer's request
				PeerRequest msg(recv_buffer.begin(), recv_buffer.begin() + PeerRequest::MSG_SIZE);

				RequestHandler handler = requestHandlers[msg.GetCmd()];
				assert(handler);
				handler(msg, remote_endpoint);
			}
			else
			{
				assert(recv_buffer[0] >= PeerResponse::FIND_SUCCESSOR_RESPONSE &&
					recv_buffer[0] < PeerResponse::CMD_TOTAL);

				//-- Handle peer's response
				PeerResponse msg(recv_buffer.begin(), recv_buffer.begin() + PeerResponse::MSG_SIZE);

				ResponseEntry entry(msg.GetCmd(), msg.GetSequence());
				ResponseHandler handler = responseHandlers[entry];
				if (handler)
					handler(msg, remote_endpoint);
				else
					std::cout << "NO handler installed for response: " << msg.GetCmd() << std::endl;
				responseHandlers.erase(entry);
			}
		}
		else
			DBGOUT << "RECEIVE ERROR(" << error.code() << ") at peer with port " << local_endpoint.port() << " -- " << error.what() << std::endl;

		if (error != boost::asio::error::connection_aborted		&&
			error != boost::asio::error::not_socket				&&
			error != boost::asio::error::operation_aborted
		)
			StartReceive();
	}

	//-----------------------------------------------------------------------

	void PeerSocketListener::HandleSend(const boost::asio::error& /*error*/,
		std::size_t /*bytes_transferred*/)
	{
		std::cout << "response dispatched" << std::endl;
		/**
		 *	register to some routing tables that a request has been dispatched and where
		 *	should I forward the response when I receive it
		 */
	}

	//-----------------------------------------------------------------------

	void PeerSocketListener::TimeOutRequests(const boost::asio::error& /*error*/)
	{
		struct TimeoutChecker :
			public std::unary_function<const ResponseHandlers::value_type&, bool>
		{
			TimeoutChecker(uint32 now)
				: now(now) {}

			result_type operator ()(argument_type pair) {
				return (now - pair.first.timestamp > RESPONSE_TIMEOUT) ? true : false;
			}

		private:
			uint32 now;
		};

		//-- Remove responses that have not been answered in time
		uint32 now = (uint32)time(0);
		while (1) {
			ResponseHandlers::iterator it = std::find_if(
				responseHandlers.begin(),
				responseHandlers.end(),
				TimeoutChecker(now)
			);

			if (it == responseHandlers.end())
				break;
			else
				DBGOUT << "RESPONSE TIMEOUT" << std::endl;

			//-- Call timeout handler
			if (it->first.timeoutHandler)
				it->first.timeoutHandler(it->first);

			//-- Don't wait for any response any more
			responseHandlers.erase(it);
		}

		//-- Reset response timeout timer
		ResetTimeoutTimer();
	}

	//-----------------------------------------------------------------------

	void PeerSocketListener::ResetTimeoutTimer(void)
	{
		responseTimeoutTimer->expires_at(
			responseTimeoutTimer->expires_at() + boost::posix_time::seconds(RESPONSE_TIMEOUT/2 -1)
		);
		responseTimeoutTimer->async_wait(
			boost::bind(
				&PeerSocketListener::TimeOutRequests,
				this, boost::asio::placeholders::error
			)
		);
	}

	//-----------------------------------------------------------------------
}
