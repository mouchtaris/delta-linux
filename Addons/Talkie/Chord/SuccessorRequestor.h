/**
 *	SuccessorRequestor.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	SuccessorRequestor is a utility class for requesting
 *	a successor from a peer in the chord network.
 *	It is separated from OverlayClient class, which is
 *	responsible for most requests in the network, as it
 *	is also being used from the OverlayServer class.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	December 2006.
 */
#ifndef CHORD_SUCCESSOR_REQUESTOR_H_
#define CHORD_SUCCESSOR_REQUESTOR_H_

#pragma warning(disable: 4267)

#include "Common.h"

#include "ChordCommon.h"
#include "VirtualPeer.h"
#include "PeerSocketListener.h"

#include "PeerRequest.h"
#include "PeerResponse.h"

#include <boost/asio/error.hpp>

namespace chord
{
	//----------------------------
	//-- class SuccessorRequestor

	class SuccessorRequestor
	{
	public:
		///--- type definitions
		typedef boost::function<void (const boost::asio::error& error)> SendErrorHandler;

		///--- constructors / destructor
		SuccessorRequestor(VirtualPeer* vpeer, const Peer& target,
			PeerSocketListener::ResponseHandler handler, SendErrorHandler errorHandler,
			ResponseEntry::ResponseTimeoutHandler timeoutHandler)
			: vpeer(vpeer), target(target), handler(handler), errorHandler(errorHandler),
			timeoutHandler(timeoutHandler) {}

		///--- public API
		void Query(const Key& key, bool safe=false) {
			uint32 seq = vpeer->GetSequence();

			//-- Send FindSuccessor request
			vpeer->GetPeerSocket().send_to(
				boost::asio::buffer(
					PeerRequest(
						(safe ? PeerRequest::FIND_SUCCESSOR_SAFE : PeerRequest::FIND_SUCCESSOR),
						seq, key
					).GetBuffer()
				),
				target.GetEndPoint(), 0,
				errorHandler
			);

			//-- Register key to node's pending requests
			vpeer->GetPendingRequests().insert(key);

			//-- Async recv of response
			vpeer->GetOverlayServer()->AsyncWaitResponse(
				ResponseEntry(
					PeerResponse::FIND_SUCCESSOR_RESPONSE,
					seq, time(0), timeoutHandler
				),
				handler
			);
		}

	private:
		///--- private members
		VirtualPeer* vpeer;
		const Peer& target;

		PeerSocketListener::ResponseHandler handler;
		SuccessorRequestor::SendErrorHandler errorHandler;
		ResponseEntry::ResponseTimeoutHandler timeoutHandler;
	};
}

#endif	//CHORD_SUCCESSOR_REQUESTOR_H_
