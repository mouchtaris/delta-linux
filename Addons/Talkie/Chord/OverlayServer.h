/**
 *	OverlayServer.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	OverlayServer is the class responsible for listening
 *	to requests of chord peers over a UDP socket.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#ifndef CHORD_PEER_LISTENER_H_
#define CHORD_PEER_LISTENER_H_

#pragma warning(disable: 4267)

#include "Common.h"
#include "VirtualPeer.h"

#include <boost/asio/error.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace chord
{
	//----------------------------
	//-- class OverlayServer

	class OverlayServer
	{
	public:
		///--- constructors / destructor
		OverlayServer(VirtualPeer* vpeer);
		~OverlayServer(void);

	private:
		///--- private API
		void HandleFindSuccessor(const PeerRequest& msg, const Peer& peer);
		void HandleNotify(const PeerRequest& msg, const Peer& peer);
		void HandleStabilize(const PeerRequest& msg, const Peer& peer);
		void HandleCheckPredecessor(const PeerRequest& msg, const Peer& peer);
		void HandleGetId(const PeerRequest& msg, const Peer& peer);
		void HandleReportDead(const PeerRequest& msg, const Peer& peer);

		void BackpropagateSuccessor(const PeerResponse& msg, const Peer& peer,
			const Peer& backpropagate, uint32 requestorSeq);
		void BackpropagateWhoIs(const PeerResponse& msg, const Peer& peer,
			const Peer& backpropagate, uint32 requestorSeq, const Key& requestKey);

		void JoinForwardTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer);

		void SendErrorHandler(const boost::asio::error& error, const Peer& peer);

		///--- private members
		VirtualPeer* vpeer;
	};
}

#endif	//CHORD_PEER_LISTENER_H_
