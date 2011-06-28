/**
 *	OverlayClient.cpp
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
#include "Streams.h"

#include "OverlayClient.h"
#include "PhysicalPeer.h"
#include "SuccessorRequestor.h"
#include "GetIdRequestor.h"

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace chord
{
	//-------------------------------------------------------//
	//---- class OverlayClient ------------------------------//

	OverlayClient::OverlayClient(VirtualPeer* vpeer)
		: vpeer(vpeer), passive(false)
	{
		//-- Construct timers
		stabilizeTimer = new boost::asio::deadline_timer(
			vpeer->GetPeerSocket().io_service(),
			boost::posix_time::seconds(0)
		);
		fixFingersTimer = new boost::asio::deadline_timer(
			vpeer->GetPeerSocket().io_service(),
			boost::posix_time::seconds(0)
		);
		reconnectTimer = new boost::asio::deadline_timer(
			vpeer->GetPeerSocket().io_service(),
			boost::posix_time::seconds(0)
		);
		checkPredecessorTimer = new boost::asio::deadline_timer(
			vpeer->GetPeerSocket().io_service(),
			boost::posix_time::seconds(0)
		);
		invalidatePendingTimer = new boost::asio::deadline_timer(
			vpeer->GetPeerSocket().io_service(),
			boost::posix_time::seconds(0)
		);

		//-- Reset the network service
		Reset();
	}

	//-----------------------------------------------------------------------

	OverlayClient::~OverlayClient(void)
	{
		delete stabilizeTimer;
		delete fixFingersTimer;
		delete reconnectTimer;
		delete checkPredecessorTimer;
		delete invalidatePendingTimer;
	}

	//-----------------------------------------------------------------------

	void OverlayClient::Join(bool safe, const boost::asio::error& e)
	{
		if (e.code() != boost::asio::error::success)
			return;
		
		//-- Randomly connect to a node from the list of known peers
		Reset();
		if (vpeer->GetPhysicalPeer()->knownPeers.size() == 0)
			return;
		uint32 index = rand() % vpeer->GetPhysicalPeer()->knownPeers.size();
		Peer peer(
			vpeer->GetPhysicalPeer()->knownPeers[index].address(),
			vpeer->GetPhysicalPeer()->knownPeers[index].port()
		);

		//-- Send request to find successor
		SuccessorRequestor(
			vpeer, peer,
			boost::bind(
				&OverlayClient::JoinHandler,
				this, _1, _2
			),
			boost::bind(	///< Send Error Handler
				&OverlayClient::SendErrorHandler,
				this, _1, peer
			),
			boost::bind(	///< Timeout Handler
				&OverlayClient::JoinTimeoutHandler,
				this, _1, peer
			)
		).Query(vpeer->GetID(), safe);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::Reset(const boost::asio::error& e)
	{
		if (e.code() != boost::asio::error::success)
			return;

		//-- Clean State
		Suspend(e);
		CleanState(e);
		passive=false;

		//-- Initialize periodic connectivity checks
		ResetStabilizeTimer();
		ResetFixFingersTimer();
		ResetCheckPredecessorTimer();
		ResetInvalidatePendingTimer();
	}

	//-----------------------------------------------------------------------

	void OverlayClient::Leave(const boost::asio::error& e)
	{
		if (e.code() != boost::asio::error::success)
			return;

		//-- Clean State
		Suspend(e);
		CleanState(e);

		//-- Release Socket
		vpeer->GetPeerSocket().close();
	}

	//-----------------------------------------------------------------------

	void OverlayClient::CleanState(const boost::asio::error& e)
	{
		if (e.code() != boost::asio::error::success)
			return;

		//-- Clean state
		vpeer->SetPredecessor(Peer());
		vpeer->SetSuccessor(Peer());
		std::for_each(
			vpeer->GetFingerTable().begin(),
			vpeer->GetFingerTable().end(),
			boost::lambda::_1 = Peer()
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::Suspend(const boost::asio::error& e)
	{
		if (e.code() != boost::asio::error::success)
			return;

		//-- Cancel timers
		stabilizeTimer->cancel();
		fixFingersTimer->cancel();
		reconnectTimer->cancel();
		invalidatePendingTimer->cancel();		
		passive=true;
	}

	//-----------------------------------------------------------------------

	void OverlayClient::Notify(const Peer& peer)
	{
		//-- Send request for notify successor's links
		vpeer->GetPeerSocket().send_to(
			boost::asio::buffer(
				PeerRequest(
					PeerRequest::NOTIFY,
					vpeer->GetSequence(),
					vpeer->GetID()
				).GetBuffer()
			),
			peer.GetEndPoint(), 0,
			boost::bind(	///< Error Handler
				&OverlayClient::SendErrorHandler,
				this, _1, peer
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::ReportDead(const Peer& peer, const Peer& dead)
	{
		if (!peer)
			return;

		//-- Send request for reporting dead node
		vpeer->GetPeerSocket().send_to(
			boost::asio::buffer(
				PeerRequest(
					PeerRequest::REPORT_DEAD,
					vpeer->GetSequence(),
					dead.GetAddress().to_v4().to_ulong(),
					dead.GetPort()
				).GetBuffer()
			),
			peer.GetEndPoint(), 0,
			boost::bind(	///< Error Handler
				&OverlayClient::SendErrorHandler,
				this, _1, peer
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::Stabilize(const boost::asio::error& e)
	{
		if (e.code() != boost::asio::error::success)
			return;

		//-- Reinitialize the timer
		ResetStabilizeTimer();

		if (!vpeer->GetSuccessor())
			return;

		//-- Trying to untangle the circle
		HeuristicUntangling();

		//-- Send stabilize request
		uint32 seq = vpeer->GetSequence();
		vpeer->GetPeerSocket().send_to(
			boost::asio::buffer(
				PeerRequest(
					PeerRequest::STABILIZE,
					seq,
					vpeer->GetID()
				).GetBuffer()
			),
			vpeer->GetSuccessor().GetEndPoint(), 0,
			boost::bind(	///< Error Handler
				&OverlayClient::SendErrorHandler,
				this, _1, vpeer->GetSuccessor()
			)
		);

		//-- Async recv of response
		vpeer->GetOverlayServer()->AsyncWaitResponse(
			ResponseEntry(
				PeerResponse::STABILIZE_RESPONSE,
				seq, time(0),
				boost::bind(
					&OverlayClient::StabilizeTimeoutHandler,
					this, _1, vpeer->GetSuccessor()
				)
			),
			boost::bind(
				&OverlayClient::StabilizeHandler,
				this, _1, _2
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::FixFingers(const boost::asio::error& e, uint32 finger, bool safe)
	{
		if (e.code() != boost::asio::error::success)
			return;

		//-- Reinitialize the timer
		ResetFixFingersTimer((finger +1) % (vpeer->GetFingerTable().size() -1) + 1);

		//-- Calculate requested key
		Key distance(2);
		distance = distance ^ finger;
		Key requestKey = vpeer->GetID() + distance;

		//-- Send find_successor request
		Peer target_peer = (safe ? vpeer->GetSuccessor() : vpeer->GetNextHop(requestKey));
		if (!target_peer)
			return;

		SuccessorRequestor(
			vpeer, target_peer,
			boost::bind(
				&OverlayClient::FixFingersHandler,
				this, _1, _2, finger
			),
			boost::bind(	///< Send Error Handler
				&OverlayClient::SendErrorHandler,
				this, _1, target_peer
			),
			boost::bind(	///< Timeout Handler
				&OverlayClient::FixFingersTimeoutHandler,
				this, _1, target_peer
			)
		).Query(requestKey, safe);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::CheckPredecessor(const boost::asio::error& e)
	{
		if (e.code() != boost::asio::error::success)
			return;

		//-- Reinitialize the timer
		ResetCheckPredecessorTimer();

		if (!vpeer->GetPredecessor())
			return;

		//-- Send stabilize request
		uint32 seq = vpeer->GetSequence();
		vpeer->GetPeerSocket().send_to(
			boost::asio::buffer(
				PeerRequest(
					PeerRequest::CHECK_PREDECESSOR,
					seq,
					vpeer->GetLocalhost().GetID()
				).GetBuffer()
			),
			vpeer->GetPredecessor().GetEndPoint(), 0,
			boost::bind(	///< Error Handler
				&OverlayClient::SendErrorHandler,
				this, _1, vpeer->GetPredecessor()
			)
		);

		//-- Async recv of response
		vpeer->GetOverlayServer()->AsyncWaitResponse(
			ResponseEntry(
				PeerResponse::CHECK_PREDECESSOR_RESPONSE,
				seq, time(0),
				boost::bind(
					&OverlayClient::CheckPredecessorTimeoutHandler,
					this, _1, vpeer->GetPredecessor()
				)
			),
			boost::bind(
				&OverlayClient::CheckPredecessorHandler,
				this, _1, _2
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::InvalidatePending(const boost::asio::error& e)
	{
		if (e.code() != boost::asio::error::success)
			return;

		//-- Reinitialize the timer
		ResetInvalidatePendingTimer();

		vpeer->GetPendingRequests().clear();
	}

	//-----------------------------------------------------------------------

	void OverlayClient::JoinHandler(const PeerResponse& msg, const Peer& peer)
	{
		//-- Receive response for join's request
		vpeer->SetSuccessor(
			Peer(boost::asio::ip::address_v4(msg.GetAddress()), msg.GetPort())
		);

		//-- If successor not found try to reconnect to the network
		if (!vpeer->GetSuccessor())
		{
			std::cout << "Peer " << vpeer->GetLocalhost() << " failed to join the network!"
				<< std::endl;

			reconnectTimer->expires_from_now(
				boost::posix_time::seconds(RECONNECT_PERIOD)
			);
			reconnectTimer->async_wait(
				boost::bind(
					&OverlayClient::Join,	///< reconnect trying a safe join
					this, true, boost::asio::placeholders::error
				)
			);
			return;
		}

		//-- Get successor's id
		GetIdRequestor(
			vpeer, vpeer->GetSuccessor(),
			boost::bind(
				&OverlayClient::GetFingerIdHandler,
				this, _1, _2, 0
			),
			boost::bind(	///< Error Handler
				&OverlayClient::SendErrorHandler,
				this, _1, vpeer->GetSuccessor()
			)
		).Query();

		//-- Notify new successor
		Notify(vpeer->GetSuccessor());
	}

	//-----------------------------------------------------------------------

	void OverlayClient::StabilizeHandler(const PeerResponse& msg, const Peer& peer)
	{
		if (msg.GetAddress() != vpeer->GetLocalhost().GetAddress().to_v4().to_ulong()	||
			msg.GetPort() != vpeer->GetLocalhost().GetPort()
		)
			///< I am not my successor's predecessor!
		{
			//-- my previous successor's predecessor should be my new successor
			vpeer->SetSuccessor(Peer(boost::asio::ip::address_v4(msg.GetAddress()), msg.GetPort()));

			//-- Send request to find successor
			GetIdRequestor(
				vpeer, vpeer->GetSuccessor(),
				boost::bind(
					&OverlayClient::GetFingerIdHandler,
					this, _1, _2, 0
				),
				boost::bind(	///< Error Handler
					&OverlayClient::SendErrorHandler,
					this, _1, peer
				)
			).Query();

			Stabilize(boost::asio::error(0));
		}

		//-- Notify new successor
		Notify(vpeer->GetSuccessor());	///< notify outside the if block guarantees that eventually the successor will be notified
	}

	//-----------------------------------------------------------------------

	void OverlayClient::FixFingersHandler(const chord::PeerResponse& msg, const chord::Peer& peer, uint32 index)
	{
		//-- Receive response for fixfingers's request
		vpeer->GetFingerTable()[index] =
			Peer(boost::asio::ip::address_v4(msg.GetAddress()), msg.GetPort());

		//-- If finger not found ignore it
		if (!vpeer->GetFingerTable()[index])
		{
			//FixFingers(boost::asio::error(0), index);
			return;
		}

		//-- Get peer's id
		GetIdRequestor(
			vpeer, vpeer->GetFingerTable()[index],
			boost::bind(
				&OverlayClient::GetFingerIdHandler,
				this, _1, _2, index
			),
			boost::bind(	///< Send Error Handler
				&OverlayClient::SendErrorHandler,
				this, _1, vpeer->GetFingerTable()[index]
			)
		).Query();
	}

	//-----------------------------------------------------------------------

	void OverlayClient::CheckPredecessorHandler(const PeerResponse& msg, const Peer& peer)
	{
		Peer response(boost::asio::ip::address_v4(msg.GetAddress()), msg.GetPort());
		if (response != vpeer->GetLocalhost())
			vpeer->SetPredecessor(Peer());
	}

	//-----------------------------------------------------------------------

	void OverlayClient::GetFingerIdHandler(const PeerResponse& msg, const Peer& peer, uint32 index)
	{
		//-- Receive response after a GetId request
		Peer finger = vpeer->GetFingerTable()[index];
		finger.SetID(msg.GetKey());
		vpeer->GetFingerTable()[index] = finger;

		//-- Remove key from node's pending requests
		vpeer->GetPendingRequests().erase(msg.GetKey());

		//-- Print successor/predecessor links
		//if (index == 0)
		//	TRACEOUT << std::endl << "OverlayClient:" << vpeer->GetLocalhost() << std::endl
		//		<< "  predecessor --> " << vpeer->GetPredecessor() << std::endl
		//		<< "  successor --> " << vpeer->GetSuccessor() << std::endl;
	}

	//-----------------------------------------------------------------------

	void OverlayClient::JoinTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer)
	{
		std::cout << "Peer: " << remote_peer << " did not respond to "
			<< vpeer->GetLocalhost() << " 's join request with seq '"
			<< entry.sequence << "'" << std::endl;

		//-- Try to reconnect to the network
		reconnectTimer->expires_from_now(
			boost::posix_time::seconds(RECONNECT_PERIOD)
		);
		reconnectTimer->async_wait(
			boost::bind(
				&OverlayClient::Join,
				this, false, boost::asio::placeholders::error
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::StabilizeTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer)
	{
		std::cout << "Peer: " << remote_peer << " did not respond to "
			<< vpeer->GetLocalhost() << " 's stabilize request" << std::endl;

		if (remote_peer != vpeer->GetSuccessor())
			return;

		//-- Remove dead node from tables
		vpeer->RemoveNodeFromTables(remote_peer);

		//-- Report that remote_peer is dead
		ReportDead(vpeer->GetSuccessor(), remote_peer);

		//-- Find a new successor
		Peer closest = vpeer->GetClosestNode();
		if (closest != vpeer->GetLocalhost())
			vpeer->SetSuccessor(vpeer->GetClosestNode());

		if (!vpeer->GetSuccessor())
			///< If I don't know any peer in the finger table reconnect to the network
		{
			reconnectTimer->expires_from_now(
				boost::posix_time::seconds(RECONNECT_PERIOD)
			);
			reconnectTimer->async_wait(
				boost::bind(
					&OverlayClient::Join,
					this, false, boost::asio::placeholders::error
				)
			);
		}
		else
		{
			Notify(vpeer->GetSuccessor());
			Stabilize(boost::asio::error(0));
		}
	}

	//-----------------------------------------------------------------------

	void OverlayClient::FixFingersTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer)
	{
		//std::cout << "Peer: " << remote_peer << " did not respond to "
		//	<< vpeer->GetLocalhost() << " 's fix fingers request" << std::endl;
	}

	//-----------------------------------------------------------------------

	void OverlayClient::CheckPredecessorTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer)
	{
		if (vpeer->GetPredecessor() == remote_peer)
			vpeer->SetPredecessor(Peer());
	}

	//-----------------------------------------------------------------------

	void OverlayClient::ResetStabilizeTimer(void)
	{
		//-- Don't reset timer if in passive mode
		if (passive)
			return;

		stabilizeTimer->expires_at(
			stabilizeTimer->expires_at() + boost::posix_time::seconds(STABILIZE_PERIOD)
		);
		stabilizeTimer->async_wait(
			boost::bind(
				&OverlayClient::Stabilize,
				this, boost::asio::placeholders::error
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::ResetFixFingersTimer(uint32 finger)
	{
		//-- Don't reset timer if in passive mode
		if (passive)
			return;

		fixFingersTimer->expires_at(
			fixFingersTimer->expires_at() + boost::posix_time::seconds(FIX_FINGERS_PERIOD)
		);
		fixFingersTimer->async_wait(
			boost::bind(
				&OverlayClient::FixFingers,
				this, boost::asio::placeholders::error, finger, false
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::ResetCheckPredecessorTimer(void)
	{
		//-- Don't reset timer if in passive mode
		if (passive)
			return;

		checkPredecessorTimer->expires_at(
			checkPredecessorTimer->expires_at() + boost::posix_time::seconds(PREDECESSOR_CHECK_PERIOD)
		);
		checkPredecessorTimer->async_wait(
			boost::bind(
				&OverlayClient::CheckPredecessor,
				this, boost::asio::placeholders::error
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::ResetInvalidatePendingTimer(void)
	{
		invalidatePendingTimer->expires_at(
			invalidatePendingTimer->expires_at() + boost::posix_time::seconds(INVALIDATE_PENDING_PERIOD)
		);
		invalidatePendingTimer->async_wait(
			boost::bind(
				&OverlayClient::InvalidatePending,
				this, boost::asio::placeholders::error
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayClient::SendErrorHandler(const boost::asio::error& error, const Peer& peer)
	{
		if (error.code() == boost::asio::error::success)
			return;

		std::cout << "NetworkSerivce SEND ERROR: " << error.what() << std::endl;
	}

	//-----------------------------------------------------------------------

	void OverlayClient::HeuristicUntangling(void)
	{
		//-- Heuristic: 60 <--> 40 <--> 20 sequence is transformed to 20 <--> 40 <--> 60
		if (vpeer->GetPredecessor()	&&
			  vpeer->GetSuccessor().GetID().Distance(vpeer->GetID())
			< vpeer->GetPredecessor().GetID().Distance(vpeer->GetID())
		)
		{
			Peer tmp = vpeer->GetPredecessor();
			vpeer->SetPredecessor(vpeer->GetSuccessor());
			vpeer->SetSuccessor(tmp);
		}

		//-- Heuristic: 40 <--> 20 <--> 60 sequence is transformed to 60 <--> 20 <--> 40
		else if (vpeer->GetPredecessor()	&&
			  vpeer->GetID().Distance(vpeer->GetSuccessor().GetID())
			> vpeer->GetID().Distance(vpeer->GetPredecessor().GetID())
		)
		{
			Peer tmp = vpeer->GetPredecessor();
			vpeer->SetPredecessor(vpeer->GetSuccessor());
			vpeer->SetSuccessor(tmp);
		}
	}

	//-----------------------------------------------------------------------
}
