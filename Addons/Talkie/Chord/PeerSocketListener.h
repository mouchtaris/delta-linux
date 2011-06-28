/**
 *	PeerSocketListener.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	PeerSocketListener is the class responsible for listening
 *	to UDP socket for peer nodes messages.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#ifndef CHORD_PEER_SOCKET_LISTENER_H_
#define CHORD_PEER_SOCKET_LISTENER_H_

#pragma warning(disable: 4267)

#include "Common.h"

#include "Peer.h"
#include "PeerRequest.h"
#include "PeerResponse.h"

#include <boost/function.hpp>
#include <boost/asio.hpp>

namespace chord
{
	//----------------------------
	//-- class ResponseEntry

	struct ResponseEntry
	{
		///--- type definitions
		typedef boost::function<void (const ResponseEntry& entry)> ResponseTimeoutHandler;

		///--- constructors / destructor
		ResponseEntry(PeerResponse::CMD cmd, uint32 sequence, uint32 timestamp=0,
			ResponseTimeoutHandler timeoutHandler=ResponseTimeoutHandler())
			: cmd(cmd), sequence(sequence), timestamp(timestamp), timeoutHandler(timeoutHandler) {}

		///--- overloaded operators
		bool operator <(const ResponseEntry& right) const {
			return (cmd == right.cmd)
						? sequence < right.sequence
						: cmd < right.cmd;
		}

		///--- public members
		PeerResponse::CMD cmd;
		uint32 sequence;
		uint32 timestamp;
		ResponseTimeoutHandler timeoutHandler;
	};


	//----------------------------
	//-- class PeerSocketListener

	class PeerSocketListener
	{
	public:
		///--- type definitions
		typedef boost::function<void (const PeerRequest& msg, const Peer& peer)> RequestHandler;
		typedef boost::function<void (const PeerResponse& msg, const Peer& peer)> ResponseHandler;

		typedef std::map<PeerRequest::CMD, RequestHandler> RequestHandlers;
		typedef std::map<ResponseEntry, ResponseHandler> ResponseHandlers;

		///--- constructors / destructor
		PeerSocketListener(boost::asio::ip::udp::socket& socket,
			const udp_endpoint& local_endpoint);

		///--- public API
		void AsyncWaitRequest(PeerRequest::CMD cmd, RequestHandler handler);
		void AsyncWaitResponse(ResponseEntry entry, ResponseHandler handler);

	private:
		///--- private API
		void StartReceive(void);

		void HandleReceive(const boost::asio::error& error, std::size_t bytes_transferred);
		void HandleSend(const boost::asio::error& error, std::size_t bytes_transferred);

		void TimeOutRequests(const boost::asio::error& error);
		void ResetTimeoutTimer(void);

		///--- private members
		RequestHandlers requestHandlers;		///< handlers for each received request
		ResponseHandlers responseHandlers;		///< handlers and timeouts for each pending response

		boost::asio::ip::udp::socket& socket;

		udp_endpoint local_endpoint;
		udp_endpoint remote_endpoint;
		boost::array<byte, 64> recv_buffer;

		boost::asio::deadline_timer* responseTimeoutTimer;
	};
}

#endif	//CHORD_PEER_SOCKET_LISTENER_H_
