/**
 *	OverlayClient.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	OverlayClient is the class responsible for retaining
 *	chord network connectivity. It provides all the
 *	network stabilization processes.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#ifndef CHORD_OVERLAY_CLIENT_H_
#define CHORD_OVERLAY_CLIENT_H_

#pragma warning(disable: 4267)

#include "Common.h"

#include "VirtualPeer.h"
#include "PeerResponse.h"
#include "SuccessorRequestor.h"

#include <boost/tuple/tuple.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace chord
{
	//----------------------------
	//-- class OverlayClient

	class OverlayClient
	{
	public:
		///--- constructors / destructor
		OverlayClient(VirtualPeer* vpeer);
		~OverlayClient(void);

		///--- public API
		void Join(bool safe=false, const boost::asio::error& e = boost::asio::error(0));
		void Reset(const boost::asio::error& e = boost::asio::error(0));
		void Leave(const boost::asio::error& e = boost::asio::error(0));

		void CleanState(const boost::asio::error& e = boost::asio::error(0));
		void Suspend(const boost::asio::error& e = boost::asio::error(0));

		template <typename Handler>
		void LookupSuccessor(const Key& key, Handler handler, bool safe=false) {
			//-- Check locally if I am the successor
			if (vpeer->IsSuccessor(key))
				handler(tcp_endpoint(vpeer->GetLocalhost().GetAddress(), vpeer->GetLocalhost().GetPort()));

			//-- Send find_successor request
			Peer target_peer = (safe ? vpeer->GetSuccessor() : vpeer->GetNextHop(key));
			if (!target_peer)
				return;

			SuccessorRequestor(
				vpeer, target_peer,
				boost::bind(
					&OverlayClient::LookupHandler<Handler>,
					this, _1, _2, boost::make_tuple(handler)
				),
				boost::bind(	///< Send Error Handler
					&OverlayClient::SendErrorHandler,
					this, _1, target_peer
				),
				boost::bind(	///< Timeout Handler
					&OverlayClient::LookupTimeoutHandler<Handler>,
					this, _1, target_peer, boost::make_tuple(handler)
				)
			).Query(key, safe);
		}

	private:
		///--- private API
		void Notify(const Peer& peer);
		void ReportDead(const Peer& peer, const Peer& dead);
		void Stabilize(const boost::asio::error& e);
		void FixFingers(const boost::asio::error& e, uint32 finger, bool safe=false);
		void CheckPredecessor(const boost::asio::error& e);
		void InvalidatePending(const boost::asio::error& e);

		///--- Response Handlers
		void JoinHandler(const PeerResponse& msg, const Peer& peer);
		void StabilizeHandler(const PeerResponse& msg, const Peer& peer);
		void FixFingersHandler(const PeerResponse& msg, const Peer& peer, uint32 index);
		void CheckPredecessorHandler(const PeerResponse& msg, const Peer& peer);
		void GetFingerIdHandler(const PeerResponse& msg, const Peer& peer, uint32 index);

		template <typename Handler>
		void LookupHandler(const PeerResponse& msg, const Peer& peer, boost::tuple<Handler> handler) {
			//-- Async return of responsible peer
			boost::get<0>(handler)(tcp_endpoint(boost::asio::ip::address_v4(msg.GetAddress()), msg.GetPort()));
		}

		///--- Timeout Handlers
		void JoinTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer);
		void StabilizeTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer);
		void FixFingersTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer);
		void CheckPredecessorTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer);

		template <typename Handler>
		void LookupTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer, boost::tuple<Handler> handler) {
			//-- TODO: do what? resend request? how many times?
		}

		///--- Timer Setters
		void ResetStabilizeTimer(void);
		void ResetFixFingersTimer(uint32 finger=1);
		void ResetCheckPredecessorTimer(void);
		void ResetInvalidatePendingTimer(void);

		///--- Error Handlers
		void SendErrorHandler(const boost::asio::error& error, const Peer& peer);

		void HeuristicUntangling(void);

		///--- private members
		VirtualPeer* vpeer;

		bool passive;
		boost::asio::deadline_timer* stabilizeTimer;
		boost::asio::deadline_timer* fixFingersTimer;
		boost::asio::deadline_timer* reconnectTimer;
		boost::asio::deadline_timer* checkPredecessorTimer;
		boost::asio::deadline_timer* invalidatePendingTimer;
	};
}

#endif	//CHORD_OVERLAY_CLIENT_H_
