/**
 *	PhysicalPeer.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	PhysicalPeer class represents a chord's physical peer
 *	in the system.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007.
 */
#ifndef CHORD_PHYSICAL_PEER_H_
#define CHORD_PHYSICAL_PEER_H_

#pragma warning(disable: 4267)

#include "Common.h"

#include "ChordCommon.h"
#include "VirtualPeer.h"
#include "OverlayServer.h"
#include "OverlayClient.h"

#include <vector>
#include <map>

namespace chord
{
	//----------------------------
	//-- struct PhysicalPeer

	struct PhysicalPeer
	{
		///--- type definitions
		typedef std::vector<VirtualPeer*> VPeerList;
		typedef std::vector<OverlayServer*> OverlayServerVec;
		typedef std::vector<OverlayClient*> OverlayClientVec;

		///--- constructors / destructor
		PhysicalPeer(boost::asio::io_service& io_service, const Key& key, uint16 port,
			uint16 nvpeers=DEFAULT_VPEER_NUMBER);
		~PhysicalPeer(void);

		///--- public API
		void Join(void);
		void Reset(void);
		void Leave(void);
		void Suspend(void);

		template <typename Handler>
		void LookupSuccessor(const Key& key, Handler handler) {
			//-- Randomly choose a virtual peer to propagate the request
			nt_services[rand() % nt_services.size()]->LookupSuccessor(key, handler);
			///< TODO: maybe I should have a fallback handler in order to route the request to an alternative vpeer
		}

		bool IsSuccessor(const Key& key);

		///--- public members
		boost::asio::io_service& io_service;	///< asio io_service

		PeerList knownPeers;			///< list of well known peers
		VPeerList vpeers;				///< list of local virtual peers
		OverlayServerVec listeners;		///< list of listeners of vpeers
		OverlayClientVec nt_services;	///< list of network services of vpeers
	};
}

#endif	//CHORD_PHYSICAL_PEER_H_
