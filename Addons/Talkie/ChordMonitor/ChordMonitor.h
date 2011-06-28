/**
 *	ChordMonitor.h
 *
 *	-- Chord DHT Monitor --
 *
 *	ChordMonitor is provides the functionality
 *	of a global monitor over a Chord network.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007.
 */
#ifndef CHORD_MONITOR_H_
#define CHORD_MONITOR_H_

#include "Common.h"
#include "CommonFwdDecls.h"

#include "Encoding.h"
#include "Connection.h"
#include "CommandServer.h"

#include "ChordCommon.h"
#include "Peer.h"
#include "VirtualPeer.h"
#include "DHTServer.h"

#include <boost/tuple/tuple.hpp>
#include <boost/serialization/map.hpp>

#include <vector>
#include <set>

#define CONTACT_PERIOD 10

namespace chord
{
	//----------------------------
	//-- class Monitor

	class Monitor :
		public net::CommandDispatcher<connection, int32>
	{
	public:
		///--- type definitions
		typedef std::set<tcp_endpoint> Servers;
		typedef boost::tuple<Peer /*vpeer_host*/, Peer /*predecessor*/, VirtualPeer::FingerTable /*fingers*/> MonitoredData;
		typedef std::vector<MonitoredData> MonitoredDataVec;

		///--- constructors / destructor
		Monitor(boost::asio::io_service& io_service, const PeerList& servers);
		~Monitor(void);

		///--- public API

	private:
		///--- private API
		template <class Tuple>
		bool ResponseHandler(LinkPtrType client, CommandType command, DecoderType& dec,
			boost::function<void (LinkPtrType, const Tuple&)> handler) {
			Tuple args;
			comm::decode_tuple(dec, args);
			handler(client, args);
			return true;
		}

		void IntroduceServersHandler(LinkPtrType client, const boost::tuple< std::list< boost::tuple<uint32, uint16> > >& args);
		void ReportHandler(LinkPtrType client, const boost::tuple<MonitoredDataVec, DHTServer::Directory>& args);

		void Presentation(const boost::asio::error& e);
		void PresentVPeer(const Key& id);

		void ContactServers(const boost::asio::error& e);
		void ConnectPeer(const tcp_endpoint& peer);
		void onConnected(net::connection_ptr peer, const boost::asio::error& error);

		void ResetContactTimer(void);
		void ResetPresentationTimer(void);

		///--- private members
		Servers known_servers;

		typedef std::map<Key, MonitoredData> VPeerTable;
		VPeerTable vpeers;
		typedef std::map<Key, DHTServer::Directory> VPeerDirectories;
		VPeerDirectories directories;

		boost::asio::io_service& io_service;
		boost::asio::deadline_timer* contactTimer;
		boost::asio::deadline_timer* presentationTimer;
	};
}

#endif	//CHORD_MONITOR_H_