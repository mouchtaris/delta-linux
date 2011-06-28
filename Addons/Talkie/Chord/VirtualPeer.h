/**
 *	VirtualPeer.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	VirtualPeer class represents a chord's virtual peer
 *	in the system.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#ifndef CHORD_VIRTUAL_PEER_H_
#define CHORD_VIRTUAL_PEER_H_

#pragma warning(disable: 4267)

#include "Common.h"

#include "Peer.h"
#include "PeerSocketListener.h"

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/signal.hpp>

#include <vector>
#include <set>

namespace chord
{
	struct PhysicalPeer;

	//----------------------------
	//-- class VirtualPeer

	class VirtualPeer
	{
	public:
		///--- type definitions 
		typedef boost::array<Peer, KEY_SIZE * 8> FingerTable;	///< 8 == BYTE_SIZE in bits as KEY_SIZE is in bytes
		typedef std::set<Key> KeySet;

		///--- constructors / destructor
		VirtualPeer(boost::asio::io_service& io_service, const Key& key, uint16 port, PhysicalPeer* parent);
		~VirtualPeer(void);

		///--- signals
		boost::signal<void (void)> sigJoinComplete;
		boost::signal<void (VirtualPeer*)> sigSuccessorUpdate;

		///--- public API
		boost::asio::ip::udp::socket& GetPeerSocket(void)	{ return peerSocket; }

		const Key& GetID(void) const						{ return localhost.GetID(); }
		const Peer& GetLocalhost(void) const				{ return localhost; }
		const Peer& GetSuccessor(void) const				{ return fingers[0]; }
		const Peer& GetPredecessor(void) const				{ return predecessor; }

		PhysicalPeer* GetPhysicalPeer(void) const			{ return parent; }
		PeerSocketListener* GetOverlayServer(void) const	{ return peerListener; }

		FingerTable& GetFingerTable(void) 					{ return fingers; }
		KeySet& GetPendingRequests(void)					{ return pendingRequests; }

		///--- FingerTable API
		void SetSuccessor(const Peer& peer)	{
			fingers[0] = peer;
			if (!sigSuccessorUpdate.empty())
				sigSuccessorUpdate(this);
		}
		void SetPredecessor(const Peer& peer)				{ predecessor = peer; }

		bool IsSuccessor(const Key& key) const;

		//const Peer& GetPrevPeer(const Key& key) const;
		const Peer& GetNextPeer(const Key& key) const;
		const Peer& GetNextHop(const Key& key) const;

		const Peer& GetClosestNode(void) const;
		void RemoveNodeFromTables(const Peer& peer);

		///--- Message Sequence API
		uint32 GetSequence(void)		{ return sequence++; }

	private:
		///--- non-copyable
		VirtualPeer(const VirtualPeer& src);

		///--- private members
		PhysicalPeer* parent;

		boost::asio::ip::udp::socket peerSocket;
		PeerSocketListener* peerListener;

		uint32 sequence;

		Peer localhost;
		Peer predecessor;

		FingerTable fingers;	///< NOTE: finger table; entry i denotes successor of (vpeer->key + 2^i) key
		KeySet pendingRequests;	///< set of keys that vpeer is waiting for successor resolving
	};
}

#endif	//CHORD_VIRTUAL_PEER_H_
