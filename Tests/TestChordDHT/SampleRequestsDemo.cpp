/**
 *	SampleRequestsDemo.cpp
 *
 *	-- ChordDHT Test demo --
 *
 *	Demo testing client queries to ChordDHT directory.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#include "TestCommon.h"
#include "Demos.h"

#include "ChordMonitor.h"

void RunSampleRequestsDemo(void)
{
	PrintHorzBar();
	std::cout << "Starting RUN SAMPLE REQUESTS demo" << std::endl;
	PrintHorzBar();

	//-- Create several chord peers
	for (int i=0; i < NPEERS; ++i)
	{
		peers[i] = new chord::PhysicalPeer(io_service, chord::Key(i*20), PORT + i*10);
		servers[i] = new chord::DHTServer(io_service, PORT + i*10, peers[i]);
		join_timers[i] = new boost::asio::deadline_timer(io_service);
		idle_nodes.push_back(i);
	}

	//-- Schedule delayed entrance to the network
	std::random_shuffle(idle_nodes.begin(), idle_nodes.end());
	connected_nodes.push_back(idle_nodes[0]);
	for (int i=1; i < NPEERS; ++i)
	{
		//-- Set join time after a while of the previous node
		if (i > 1)
			join_timers[i]->expires_at(join_timers[i-1]->expires_at() + boost::posix_time::seconds(rand() % 5));
		else
			join_timers[i]->expires_from_now(boost::posix_time::seconds(rand() % 2 +1));

		//-- Set timer for join
		join_timers[i]->async_wait(
			boost::bind(
				Join,
				idle_nodes[i],
				connected_nodes[rand() % connected_nodes.size()],
				_1
			)
		);

		//-- Add idle node to connected (no need to delete him from idle)
		connected_nodes.push_back(idle_nodes[i]);
	}

	//-- Run periodically a peer's monitor
	//boost::asio::deadline_timer monitorTimer(
	//	io_service,
	//	boost::posix_time::seconds(MONITOR_PERIOD)
	//);
	//monitorTimer.async_wait(
	//	boost::bind(
	//		MonitorPeers,
	//		&monitorTimer, boost::asio::placeholders::error
	//	)
	//);

	chord::PeerList endpoints;
	endpoints.push_back(chord::tcp_endpoint(boost::asio::ip::address_v4::loopback(), PORT));
	chord::Monitor monitor(io_service, endpoints);

	//-- Run sample requests
	//boost::asio::deadline_timer requestTimer(
	//	io_service,
	//	join_timers[NPEERS-1]->expires_at() + boost::posix_time::seconds(REQUEST_PERIOD)
	//);
	//requestTimer.async_wait(
	//	boost::bind(
	//		RunRequest,
	//		requests, SIZEOF_ARRAY(requests), 0,
	//		&requestTimer, boost::asio::placeholders::error
	//	)
	//);

	io_service.run();

	//-- CleanUp my mess ;-)
	for (uint32 i=0; i < NPEERS; ++i)
	{
		delete join_timers[i];
		delete servers[i];
		delete peers[i];
	}

	PrintHorzBar();
	std::cout << "Ended RUN SAMPLE REQUESTS demo" << std::endl;
	PrintHorzBar();
}

//-----------------------------------------------------------------------

