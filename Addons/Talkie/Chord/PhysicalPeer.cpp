/**
 *	PhysicalPeer.cpp
 *
 *	-- Chord Routing Algorithm --
 *
 *	PhysicalPeer class represents a chord's physical peer
 *	in the system.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007.
 */
#include "PhysicalPeer.h"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>

#include <algorithm>

namespace chord
{
	//-------------------------------------------------------//
	//---- Class PhysicalPeer -------------------------------//

	PhysicalPeer::PhysicalPeer(boost::asio::io_service& io_service, const Key& key, uint16 port, uint16 nvpeers)
		: io_service(io_service)
	{
		Key step(key);

		//-- Create Virtual Peers
		for (int i=0; i < nvpeers; ++i)
		{
			vpeers.push_back(new chord::VirtualPeer(io_service, step, port +i, this));
			listeners.push_back(new chord::OverlayServer(vpeers[i]));
			nt_services.push_back(new chord::OverlayClient(vpeers[i]));
			step += Key(10000);	///< FIXME: matsakwnia entelws; have to support division in Key, but it's so BORING!!!
		}

		//-- Join Virtual Peers
		knownPeers.push_back(
			tcp_endpoint(
				boost::asio::ip::address_v4::loopback(),
				vpeers[0]->GetLocalhost().GetPort()
			)
		);
		std::for_each(
			nt_services.begin() +1,
			nt_services.end(),
			boost::bind(&OverlayClient::Join, _1, false, boost::asio::error(0))
		);
		knownPeers.clear();
	}

	//-----------------------------------------------------------------------

	PhysicalPeer::~PhysicalPeer(void)
	{
		for (uint32 i=0; i < vpeers.size(); ++i)
		{
			delete vpeers[i];
			delete listeners[i];
			delete nt_services[i];
		}
		vpeers.clear();
		listeners.clear();
		nt_services.clear();
	}

	//-----------------------------------------------------------------------

	void PhysicalPeer::Join(void)
	{
		std::for_each(
			nt_services.begin(),
			nt_services.end(),
			boost::bind(&OverlayClient::Join, _1, false, boost::asio::error(0))
		);
	}

	//-----------------------------------------------------------------------

	void PhysicalPeer::Reset(void)
	{
		std::for_each(
			nt_services.begin(),
			nt_services.end(),
			boost::bind(&OverlayClient::Reset, _1, boost::asio::error(0))
		);
	}

	//-----------------------------------------------------------------------

	void PhysicalPeer::Leave(void)
	{
		std::for_each(
			nt_services.begin(),
			nt_services.end(),
			boost::bind(&OverlayClient::Leave, _1, boost::asio::error(0))
		);
	}

	//-----------------------------------------------------------------------

	void PhysicalPeer::Suspend(void)
	{
		std::for_each(
			nt_services.begin(),
			nt_services.end(),
			boost::bind(&OverlayClient::Suspend, _1, boost::asio::error(0))
		);
	}

	//-----------------------------------------------------------------------

	bool PhysicalPeer::IsSuccessor(const chord::Key& key)
	{
		VPeerList::const_iterator it =
			std::find_if(
				vpeers.begin(),
				vpeers.end(),
				boost::bind(&VirtualPeer::IsSuccessor, _1, key)
			);

		return (it != vpeers.end() ? true : false);
	}

	//-----------------------------------------------------------------------
}
