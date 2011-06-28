/**
 *	OverlayServer.cpp
 *
 *	-- Chord Routing Algorithm --
 *
 *	OverlayServer is the class responsible for listening
 *	to requests of chord peers over a UDP socket.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#include "Streams.h"

#include "OverlayServer.h"
#include "PhysicalPeer.h"
#include "PeerRequest.h"
#include "PeerResponse.h"
#include "SuccessorRequestor.h"

#include <boost/bind.hpp>

#include <iostream>
#include <algorithm>

namespace chord
{
	//-------------------------------------------------------//
	//---- Class OverlayServer -------------------------------//

	OverlayServer::OverlayServer(VirtualPeer* vpeer)
		: vpeer(vpeer)
	{
		//-- Setup Request Handlers
		vpeer->GetOverlayServer()->AsyncWaitRequest(
			PeerRequest::FIND_SUCCESSOR,
			boost::bind(
				&OverlayServer::HandleFindSuccessor,
				this, _1, _2
			)
		);
		vpeer->GetOverlayServer()->AsyncWaitRequest(
			PeerRequest::FIND_SUCCESSOR_SAFE,
			boost::bind(
				&OverlayServer::HandleFindSuccessor,
				this, _1, _2
			)
		);
		vpeer->GetOverlayServer()->AsyncWaitRequest(
			PeerRequest::NOTIFY,
			boost::bind(
				&OverlayServer::HandleNotify,
				this, _1, _2
			)
		);
		vpeer->GetOverlayServer()->AsyncWaitRequest(
			PeerRequest::STABILIZE,
			boost::bind(
				&OverlayServer::HandleStabilize,
				this, _1, _2
			)
		);
		vpeer->GetOverlayServer()->AsyncWaitRequest(
			PeerRequest::CHECK_PREDECESSOR,
			boost::bind(
				&OverlayServer::HandleCheckPredecessor,
				this, _1, _2
			)
		);
		vpeer->GetOverlayServer()->AsyncWaitRequest(
			PeerRequest::GET_ID,
			boost::bind(
				&OverlayServer::HandleGetId,
				this, _1, _2
			)
		);
		vpeer->GetOverlayServer()->AsyncWaitRequest(
			PeerRequest::REPORT_DEAD,
			boost::bind(
				&OverlayServer::HandleReportDead,
				this, _1, _2
			)
		);
	}

	//-----------------------------------------------------------------------

	OverlayServer::~OverlayServer(void)
	{
	}

	//-----------------------------------------------------------------------

	void OverlayServer::HandleFindSuccessor(const PeerRequest& msg, const Peer& peer)
	{
		Key key = msg.GetKey();

		//TRACEOUT << "PEER " << vpeer->GetLocalhost() << " recv FS request from peer "
		//	<< peer << " for key: " << key << std::endl;

		if (vpeer->GetLocalhost().GetPort() == 2000)
			DBGOUT << time(0) << " RECV FindSuccessor request from " << peer << " for key: " << key << std::endl;

		//-- Check whether this node is the successor of the key
		if ((!vpeer->GetSuccessor() && !vpeer->GetPredecessor())	||
			(!vpeer->GetPredecessor() && vpeer->GetSuccessor() && key.Distance(vpeer->GetID()) < key.Distance(vpeer->GetSuccessor().GetID()))	||
			(vpeer->GetPredecessor() && key.Distance(vpeer->GetID()) < key.Distance(vpeer->GetPredecessor().GetID()))
		)
			vpeer->GetPeerSocket().send_to(
				boost::asio::buffer(
					PeerResponse(
						PeerResponse::FIND_SUCCESSOR_RESPONSE,
						msg.GetSequence(),
						vpeer->GetLocalhost().GetAddress().to_v4().to_ulong(),
						vpeer->GetLocalhost().GetPort()
					).GetBuffer()
				),
				peer.GetEndPoint(), 0,
				boost::bind(	///< Send Error Handler
					&OverlayServer::SendErrorHandler,
					this, _1, peer
				)
			);
		else
		{
			//-- Check whether I already have pending request for the specified key and then respond with don't know (in order to avoid timeout)
			if (vpeer->GetPendingRequests().find(key) != vpeer->GetPendingRequests().end())
			{
				vpeer->GetPeerSocket().send_to(
					boost::asio::buffer(
						PeerResponse(
							PeerResponse::FIND_SUCCESSOR_RESPONSE,
							msg.GetSequence(),
							0, 0
						).GetBuffer()
					),
					peer.GetEndPoint(), 0,
					boost::bind(	///< Send Error Handler
						&OverlayServer::SendErrorHandler,
						this, _1, peer
					)
				);
				return;
			}

			//-- Lookup closest peer to key
			Peer closest = (msg.GetCmd() == PeerRequest::FIND_SUCCESSOR_SAFE
								? vpeer->GetSuccessor()
								: vpeer->GetNextHop(key)
							);

			//-- Propagate the request to the closest node to key
			if (closest)
				SuccessorRequestor(
					vpeer, closest,
					boost::bind(
						&OverlayServer::BackpropagateSuccessor,
						this, _1, _2, peer, msg.GetSequence()
					),
					boost::bind(	///< Send Error Handler
						&OverlayServer::SendErrorHandler,
						this, _1, closest
					),
					boost::bind(	///< Timeout Handler
						&OverlayServer::JoinForwardTimeoutHandler,
						this, _1, closest
					)
				).Query(key, msg.GetCmd() == PeerRequest::FIND_SUCCESSOR_SAFE);
		}
	}

	//-----------------------------------------------------------------------

	void OverlayServer::HandleNotify(const PeerRequest& msg, const Peer& peer)
	{
		if (vpeer->GetLocalhost().GetPort() == 2000)
			DBGOUT << time(0) << " RECV Notify request from " << peer << " with key " << msg.GetKey()
			<< " while current predecessor is " << vpeer->GetPredecessor() << std::endl;

		//-- Check if this is indeed the closest peer to me
		if (!vpeer->GetPredecessor()												||
			vpeer->GetPredecessor().GetID().Distance(vpeer->GetID()) == Key(0)		||
			msg.GetKey().Distance(vpeer->GetID()) < vpeer->GetPredecessor().GetID().Distance(vpeer->GetID())
		)
		{
			//-- If join process completed notify interested parties
			if (!vpeer->GetPredecessor() && !vpeer->sigJoinComplete.empty())
				vpeer->sigJoinComplete();

			if (vpeer->GetLocalhost().GetPort() == 2000)
				DBGOUT << "\tmsg key: " << msg.GetKey()
				<< "\n\tpredecessor key: " << vpeer->GetPredecessor().GetID()
				<< "\n\tlocal key: " << vpeer->GetLocalhost().GetID()
				<< "\n\tmsg to local distance: " << msg.GetKey().Distance(vpeer->GetID())
				<< "\n\tpred to local distance: " << vpeer->GetPredecessor().GetID().Distance(vpeer->GetID())
				<< "\n\tmsg to local dist < pred to local dist: " << (msg.GetKey().Distance(vpeer->GetID()) < vpeer->GetPredecessor().GetID().Distance(vpeer->GetID()))
				<< "\n\tpred to local dist == 0: " << (vpeer->GetPredecessor().GetID().Distance(vpeer->GetID()) == Key(0))
				<< "\n\tnot pred: " << (!vpeer->GetPredecessor())
				<< std::endl;

			Peer predecessor = Peer(peer);
			predecessor.SetID(msg.GetKey());
			vpeer->SetPredecessor(predecessor);

			if (!vpeer->GetSuccessor())				///< create the circle
				vpeer->SetSuccessor(predecessor);

			//TRACEOUT << std::endl << "OverlayClient:" << vpeer->GetLocalhost() << std::endl
			//	<< "  predecessor --> " << vpeer->GetPredecessor() << std::endl
			//	<< "  successor --> " << vpeer->GetSuccessor() << std::endl;
		}

		if (vpeer->GetLocalhost().GetPort() == 2000)
			DBGOUT << "\tpredecessor after notify is " << vpeer->GetPredecessor() << std::endl;
	}

	//-----------------------------------------------------------------------

	void OverlayServer::HandleStabilize(const PeerRequest& msg, const Peer& peer)
	{
		//-- Keep track for new nodes
		if (vpeer->GetPhysicalPeer()->knownPeers.size() < 25) {
			tcp_endpoint endpoint(peer.GetAddress(), peer.GetPort());
			if (std::find(vpeer->GetPhysicalPeer()->knownPeers.begin(),
					vpeer->GetPhysicalPeer()->knownPeers.end(), endpoint)
					== vpeer->GetPhysicalPeer()->knownPeers.end()) {
				vpeer->GetPhysicalPeer()->knownPeers.push_back(endpoint);
			}
		}

		if (vpeer->GetLocalhost().GetPort() == 2000)
			DBGOUT << time(0) << " RECV Stabilize request from " << peer << " with key " << msg.GetKey() << std::endl;

		//-- In case node doesn't have any predecessor (e.g. when 2nd node joins the network)
		if (!vpeer->GetPredecessor())
		{
			Peer predecessor(peer);
			predecessor.SetID(msg.GetKey());
			vpeer->SetPredecessor(predecessor);
		}

		//-- Respond with predecessor's id
		vpeer->GetPeerSocket().send_to(
			boost::asio::buffer(
				PeerResponse(
					PeerResponse::STABILIZE_RESPONSE,
					msg.GetSequence(),
					vpeer->GetPredecessor().GetAddress().to_v4().to_ulong(),
					vpeer->GetPredecessor().GetPort()
				).GetBuffer()
			),
			peer.GetEndPoint(), 0,
			boost::bind(	///< Error Handler
				&OverlayServer::SendErrorHandler,
				this, _1, peer
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayServer::HandleCheckPredecessor(const PeerRequest& msg, const Peer& peer)
	{
		if (!vpeer->GetSuccessor()		||
			vpeer->GetLocalhost().GetID().Distance(msg.GetKey()) <
			vpeer->GetLocalhost().GetID().Distance(vpeer->GetSuccessor().GetID()))
		{
			vpeer->SetSuccessor(peer);
		}

		//-- Respond with node's id
		vpeer->GetPeerSocket().send_to(
			boost::asio::buffer(
				PeerResponse(
					PeerResponse::CHECK_PREDECESSOR_RESPONSE,
					msg.GetSequence(),
					vpeer->GetSuccessor().GetAddress().to_v4().to_ulong(),
					vpeer->GetSuccessor().GetPort()
				).GetBuffer()
			),
			peer.GetEndPoint(), 0,
			boost::bind(	///< Error Handler
				&OverlayServer::SendErrorHandler,
				this, _1, peer
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayServer::HandleGetId(const PeerRequest& msg, const Peer& peer)
	{
		//-- Respond with node's id
		vpeer->GetPeerSocket().send_to(
			boost::asio::buffer(
				PeerResponse(
					PeerResponse::GET_ID_RESPONSE,
					msg.GetSequence(),
					vpeer->GetLocalhost().GetID()
				).GetBuffer()
			),
			peer.GetEndPoint(), 0,
			boost::bind(	///< Error Handler
				&OverlayServer::SendErrorHandler,
				this, _1, peer
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayServer::HandleReportDead(const PeerRequest& msg, const Peer& peer)
	{
		Peer dead(boost::asio::ip::address_v4(msg.GetAddress()), msg.GetPort());

		//-- Remove dead node from tables
		vpeer->RemoveNodeFromTables(dead);
	}

	//-----------------------------------------------------------------------

	void OverlayServer::BackpropagateSuccessor(const PeerResponse& msg, const Peer& peer,
		const Peer& backpropagate, uint32 requestorSeq)
	{
		//-- Recursive backpropagation of response to requestor
		vpeer->GetPeerSocket().send_to(
			boost::asio::buffer(
				PeerResponse(
					PeerResponse::FIND_SUCCESSOR_RESPONSE,
					requestorSeq,
					msg.GetAddress(),
					msg.GetPort()
				).GetBuffer()
			),
			backpropagate.GetEndPoint(), 0,
			boost::bind(	///< Error Handler
				&OverlayServer::SendErrorHandler,
				this, _1, backpropagate
			)
		);
	}

	//-----------------------------------------------------------------------

	void OverlayServer::JoinForwardTimeoutHandler(const ResponseEntry& entry, const Peer& remote_peer)
	{
		//std::cout << "Peer: " << remote_peer << " did not respond to "
		//	<< vpeer->GetLocalhost() << " 's join forward request" << std::endl;
	}

	//-----------------------------------------------------------------------

	void OverlayServer::SendErrorHandler(const boost::asio::error& error, const Peer& peer)
	{
		if (error.code() == boost::asio::error::success)
			return;

		std::cout << "OverlayServer SEND ERROR: " << error.what() << std::endl;
	}

	//-----------------------------------------------------------------------
}
