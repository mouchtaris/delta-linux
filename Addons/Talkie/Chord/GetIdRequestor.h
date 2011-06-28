/**
 *	GetIdRequestor.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	GetIdRequestor is a utility class for requesting
 *	a node's tuple of a node given its address/port in the
 *	chord network.
 *	It is separated from OverlayClient class, which is
 *	responsible for most requests in the network, as it
 *  is also being used from the OverlayServer class.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	December 2006.
 */
#ifndef CHORD_GET_ID_REQUESTOR_H_
#define CHORD_GET_ID_REQUESTOR_H_

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
	//-- class GetIdRequestor

	class GetIdRequestor
	{
	public:
		///--- type definitions
		typedef boost::function<void (const boost::asio::error& error)> SendErrorHandler;

		///--- constructors / destructor
		GetIdRequestor(VirtualPeer* vpeer, const Peer& target,
			PeerSocketListener::ResponseHandler handler, SendErrorHandler errorHandler)
			: vpeer(vpeer), target(target), handler(handler), errorHandler(errorHandler) {}

		///--- public API
		void Query(void) {
			uint32 seq = vpeer->GetSequence();

			//-- Send GetId request
			vpeer->GetPeerSocket().send_to(
				boost::asio::buffer(
					PeerRequest(
						PeerRequest::GET_ID,
						seq
					).GetBuffer()
				),
				target.GetEndPoint(), 0,
				errorHandler
			);

			//-- TODO: Insert node to pending GetId requests???
			//vpeer->GetPendingGetId().insert(id);

			//-- Async recv of response
			vpeer->GetOverlayServer()->AsyncWaitResponse(
				ResponseEntry(
					PeerResponse::GET_ID_RESPONSE,
					seq, time(0)
				),
				handler
			);
		}

	private:
		///--- private members
		VirtualPeer* vpeer;
		const Peer& target;
		PeerSocketListener::ResponseHandler handler;
		SendErrorHandler errorHandler;
	};
}

#endif	//CHORD_GET_ID_REQUESTOR_H_
