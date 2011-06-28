/**
 *	ChordMonitor.cpp
 *
 *	-- Chord DHT Monitor --
 *
 *	ChordMonitor is provides the functionality
 *	of a global monitor over a Chord network.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007.
 */
#include "ChordMonitor.h"

#include "DHTCommands.h"
#include "Adaptors.h"

#include <boost/bind/protect.hpp>

#include <iostream>

namespace chord
{
	//-------------------------------------------------------//
	//---- class Monitor ------------------------------------//

	Monitor::Monitor(boost::asio::io_service& io_service, const PeerList& servers)
		: io_service(io_service)
	{
		known_servers = Servers(servers.begin(), servers.end());

		//-- Construct timers
		contactTimer = new boost::asio::deadline_timer(
			io_service,
			boost::posix_time::seconds(0)
		);
		presentationTimer = new boost::asio::deadline_timer(
			io_service,
			boost::posix_time::seconds(0)
		);
		ResetContactTimer();
		ResetPresentationTimer();

		//-- Register response handlers
		RegisterCommand(
			DHT_INTRODUCE_SERVERS_RESPONSE,
			boost::bind(
				&Monitor::ResponseHandler< boost::tuple< std::list< boost::tuple<uint32, uint16> > > >,
				this, _2, _1, _3,
				boost::protect(boost::bind(&Monitor::IntroduceServersHandler, this, _1, _2))
			)
		);
		RegisterCommand(
			DHT_REPORT_RESPONSE,
			boost::bind(
				&Monitor::ResponseHandler< boost::tuple<MonitoredDataVec, DHTServer::Directory> >,
				this, _2, _1, _3,
				boost::protect(boost::bind(&Monitor::ReportHandler, this, _1, _2))
			)
		);
		SetContinuousMonitor(true);		///< Set continuous mode true
	}

	//-----------------------------------------------------------------------

	Monitor::~Monitor(void)
	{
		delete contactTimer;
	}

	//-----------------------------------------------------------------------

	void Monitor::IntroduceServersHandler(LinkPtrType client,
		const boost::tuple< std::list< boost::tuple<uint32, uint16> > >& args)
	{
		struct convert :
			public std::unary_function<const boost::tuple<uint32, uint16>&, tcp_endpoint>
		{
			result_type operator ()(argument_type arg) {
				return tcp_endpoint(
					boost::asio::ip::address_v4(boost::get<0>(arg)),
					boost::get<1>(arg)
				);
			}
		};

		std::transform(
			boost::get<0>(args).begin(),
			boost::get<0>(args).end(),
			std::inserter(known_servers, known_servers.end()),
			convert()
		);
	}

	//-----------------------------------------------------------------------

	void Monitor::ReportHandler(LinkPtrType client,
		const boost::tuple<MonitoredDataVec, DHTServer::Directory>& args)
	{
		const MonitoredDataVec& vec = boost::get<0>(args);

		for (uint32 i=0; i < vec.size(); ++i)
		{
			const MonitoredData& data = boost::get<0>(args)[i]; 
			Peer peer = boost::get<0>(data);
			vpeers[peer.GetID()] = data;
			directories[peer.GetID()] = boost::get<1>(args);
		}
	}

	//-----------------------------------------------------------------------

	void Monitor::Presentation(const boost::asio::error& error)
	{
		if (error)
			return;

		//-- Reinitialize the timer
		ResetPresentationTimer();

		std::cout << std::endl << "Global Monitor output" << std::endl;
		std::for_each(
			vpeers.begin(), vpeers.end(),
			util::project1st<VPeerTable::value_type>(
				boost::bind(&Monitor::PresentVPeer, this, _1)
			)
		);
	}

	//-----------------------------------------------------------------------

	void Monitor::PresentVPeer(const Key& id)
	{
		VPeerTable::const_iterator it = vpeers.find(id);
		if (it == vpeers.end())
			return;
		VPeerDirectories::const_iterator dir_it = directories.find(id);
		if (dir_it == directories.end())
			return;

		const MonitoredData& data = it->second;
		const DHTServer::Directory& dir = dir_it->second;

		std::cout << boost::get<0>(data) << std::endl
			<< "  predecessor -- " << boost::get<1>(data) << std::endl
			<< "  successor -- " << boost::get<2>(data)[0] << std::endl;

		//for (size_t i=1; i < boost::get<2>(data).size(); ++i)
		//{
		//	if (boost::get<2>(data)[i])
		//	{
		//		chord::Key distance(2);
		//		distance = distance ^ i;
		//		chord::Key requestKey = boost::get<0>(data).GetID() + distance;

		//		std::cout << "  finger[" << i << "] for key "
		//			<< requestKey << " -- "
		//			<< boost::get<2>(data)[i] << std::endl;
		//	}
		//}

		struct DirectoryEntryPrinter :
			public std::unary_function<chord::DHTServer::Directory::value_type, void>
		{
			result_type operator ()(const argument_type& pair) {
				std::cout << "  " << pair.first /*<< ": " << pair.second*/ << std::endl;
			}
		};

		std::cout << "Directory Entries:" << std::endl;
		if (!dir.empty())
			std::for_each(
				dir.begin(),
				dir.end(),
				DirectoryEntryPrinter()
			);
		else
			std::cout << " <empty>" << std::endl;
		std::cout << std::endl;
	}

	//-----------------------------------------------------------------------

	void Monitor::ContactServers(const boost::asio::error& error)
	{
		if (error)
			return;

		//-- Reinitialize the timer
		ResetContactTimer();

		vpeers.clear();
		directories.clear();

		//-- Contact all known peers
		std::for_each(
			known_servers.begin(),
			known_servers.end(),
			boost::bind(&Monitor::ConnectPeer, this, _1)
		);
	}

	//-----------------------------------------------------------------------

	void Monitor::ConnectPeer(const tcp_endpoint& peer)
	{
		net::connection_ptr connection = net::connection_ptr(new net::connection(io_service));
		connection->socket().async_connect(
			peer,
			boost::bind(
				&Monitor::onConnected, this,
				connection, _1
			)
		);
	}

	//-----------------------------------------------------------------------

	void Monitor::onConnected(net::connection_ptr connection, const boost::asio::error& error)
	{
		if (error)
			return;

		connection->write(comm::encoder() << DHT_INTRODUCE_SERVERS);
		connection->write(comm::encoder() << DHT_REPORT);
		ReadCommandFromLink(connection);
	}

	//-----------------------------------------------------------------------

	void Monitor::ResetContactTimer(void)
	{
		contactTimer->expires_at(
			contactTimer->expires_at() + boost::posix_time::seconds(2)
		);
		contactTimer->async_wait(
			boost::bind(
				&Monitor::ContactServers,
				this, boost::asio::placeholders::error
			)
		);
	}

	//-----------------------------------------------------------------------

	void Monitor::ResetPresentationTimer(void)
	{
		presentationTimer->expires_at(
			contactTimer->expires_at() + boost::posix_time::seconds(5)
		);
		presentationTimer->async_wait(
			boost::bind(
				&Monitor::Presentation,
				this, boost::asio::placeholders::error
			)
		);
	}

	//-----------------------------------------------------------------------
}
