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
#include "VirtualPeer.h"
#include "PhysicalPeer.h"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>

#include <algorithm>

namespace chord
{
	//-------------------------------------------------------//
	//---- Class VirtualPeer --------------------------------//

	VirtualPeer::VirtualPeer(boost::asio::io_service& io_service, const Key& key, uint16 port, PhysicalPeer* parent)
		: peerSocket(io_service, udp_endpoint(boost::asio::ip::udp::v4(), port)),
		sequence(0), peerListener(0), parent(parent)
	{
		//-- Resolve localhost name
		boost::asio::ip::udp::resolver resolver(io_service);
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::host_name(), "");
		boost::asio::ip::udp::resolver::iterator it = resolver.resolve(query);
		boost::asio::ip::udp::resolver::iterator end;
		if (it != end)
			localhost = Peer(udp_endpoint(*it).address(), port);
		else
			std::cout << "ERROR can not resolve local host" << std::endl;

		//-- Fix node's ID
		localhost.SetID(key);

		//-- Create PeerSocketListener
		peerListener = new PeerSocketListener(peerSocket, localhost.GetEndPoint());

		std::cout << "chord peer at host " << localhost.GetAddress() << ":" << localhost.GetPort()
			<< " [" << boost::asio::ip::host_name() << "]"
			<< " is up" << " ID: " << localhost.GetID() << std::endl;
	}

	//-----------------------------------------------------------------------

	VirtualPeer::~VirtualPeer(void)
	{
		delete peerListener;
	}

	//-----------------------------------------------------------------------

	bool VirtualPeer::IsSuccessor(const Key& key) const
	{
		return (!predecessor || key.Distance(localhost.GetID()) < key.Distance(predecessor.GetID()))
			? true
			: false
		;
	}

	//-----------------------------------------------------------------------

	struct NextPeerEvaluator :
		public std::unary_function<const Peer&, void>
	{
		NextPeerEvaluator(const Key& key)
			: key(key), minDistance(Key::MAX), count(0), peerIndex(0) {}

		result_type operator ()(argument_type peer) {
			if (peer)
			{
				Key distance = key.Distance(peer.GetID());
				if (distance < minDistance)
				{
					minDistance = distance;
					peerIndex = count;
				}
			}
			++count;
		}

		uint32 GetResult(void) const	{ return peerIndex; }

	private:
		const Key& key;
		Key minDistance;
		uint32 count;
		uint32 peerIndex;
	};

	const Peer& VirtualPeer::GetNextPeer(const Key& key) const
	{
		//-- Lookup successor peer to given key
		return fingers[
			std::for_each(
				fingers.begin(),
				fingers.end(),
				NextPeerEvaluator(key)
			).GetResult()
		];
	}

	//-----------------------------------------------------------------------

	const Peer& VirtualPeer::GetNextHop(const Key& key) const
	{
		Key distance = GetID().Distance(key);
		int32 index=-1;
		while (distance) {
			distance >> (uint32)1;
			++index;
		}

		for (int32 i=index; i >= 0; --i)
			if (fingers[i])
				return fingers[i];
		return fingers[0];
	}

	//-----------------------------------------------------------------------

	const Peer& VirtualPeer::GetClosestNode(void) const
	{
		FingerTable::const_iterator it = std::find_if(
			fingers.begin(),
			fingers.end(),
			boost::lambda::if_then_else_return(
				boost::lambda::_1 != Peer(),
				true, false
			)
		);

		return (it != fingers.end()) ? *it : predecessor;
	}

	//-----------------------------------------------------------------------

	void VirtualPeer::RemoveNodeFromTables(const Peer& peer)
	{
		//-- Remove it from known peers
		parent->knownPeers.erase(
			std::remove(
				parent->knownPeers.begin(), parent->knownPeers.end(),
				tcp_endpoint(peer.GetAddress(), peer.GetPort())
			),
			parent->knownPeers.end()
		);
			
		//-- Remove if it is my predecessor
		if (predecessor == peer)
			predecessor = Peer();

		//-- Remove from Finger Table
		std::for_each(
			fingers.begin(),
			fingers.end(),
			boost::lambda::if_(boost::lambda::_1 == peer)[
				boost::lambda::_1 = Peer()
			]
		);
	}

	//-----------------------------------------------------------------------
}
