/**
 *	main.cpp
 *
 *	-- Stress Test of Chord Routing Algorithm --
 *	This is a stress test of the chord implementation of
 *	talkie application.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#include "Common.h"
#include "Streams.h"
#include "IOServiceThread.h"

#include "PhysicalPeer.h"
#include "VirtualPeer.h"
#include "Peer.h"
#include "OverlayServer.h"
#include "OverlayClient.h"

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>

#define NPEERS 16
#define MONITOR_PERIOD 10

net::IOServiceThread io_service;

boost::array<chord::PhysicalPeer*, NPEERS> peers;
boost::array<boost::asio::deadline_timer*, NPEERS> join_timers;

std::vector<int> connected_nodes;
std::vector<int> idle_nodes;

//-----------------------------------------------------------------------

void MonitorPeers(boost::asio::deadline_timer* timer, const boost::asio::error& /*e*/)
{
	std::cout << std::endl << "Global Monitor output to DBGOUT" << std::endl;
	for (size_t k=0; k < NPEERS; ++k)
	{
		for (size_t i=0; i < peers[k]->vpeers.size(); ++i)
		{
			std::cout << peers[k]->vpeers[i]->GetLocalhost() << std::endl
				<< "  predecessor -- " << peers[k]->vpeers[i]->GetPredecessor() << std::endl
				<< "  successor -- " << peers[k]->vpeers[i]->GetSuccessor() << std::endl;

			//for (size_t j=1; j < peers[k]->vpeers[i]->GetFingerTable().size(); ++j)
			//{
			//	if (peers[k]->vpeers[i]->GetFingerTable()[j])
			//	{
			//		chord::Key distance(2);
			//		distance = distance ^ j;
			//		chord::Key requestKey = peers[k]->vpeers[i]->GetID() + distance;

			//		std::cout << "  finger[" << j << "] for key "
			//			<< requestKey << " -- "
			//			<< peers[k]->vpeers[i]->GetFingerTable()[j] << std::endl;
			//	}
			//}
		}
	}

	//-- Reset timer
	timer->expires_at(timer->expires_at() + boost::posix_time::seconds(MONITOR_PERIOD));
	timer->async_wait(
		boost::bind(
			MonitorPeers,
			timer, boost::asio::placeholders::error
		)
	);
}

//-----------------------------------------------------------------------

void Join(int src, int target, const boost::asio::error& /*e*/)
{
	std::cout << "Node " << src << " joins node " << target << std::endl;
	peers[src]->knownPeers.push_back(
		chord::tcp_endpoint(boost::asio::ip::address_v4::loopback(), 3000 + target*10)
	);
	peers[src]->Join();
}

//-----------------------------------------------------------------------

void Leave(int index, const boost::asio::error& /*e*/)
{
	std::cout << "Node " << index << " leaves the network" << std::endl;
	peers[index]->Leave();
}

//-----------------------------------------------------------------------

void Suspend(const boost::asio::error& /*e*/)
{
	std::cout << "Nodes stop periodic requests!" << std::endl;
	DBGOUT << std::endl << "Nodes stop periodic requests!" << std::endl;
	std::for_each(
		peers.begin(),
		peers.end(),
		boost::lambda::_1->*&chord::PhysicalPeer::Suspend
	);
}

//-----------------------------------------------------------------------

int main(void)
{
	try {
		srand((unsigned int)time(0));

		//-- Create several chord peers
		for (int i=0; i < NPEERS; ++i)
		{
			peers[i] = new chord::PhysicalPeer(io_service, chord::Key(i*100), 3000 + i*10);
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
		boost::asio::deadline_timer monitorTimer(
			io_service,
			boost::posix_time::seconds(MONITOR_PERIOD)
		);
		monitorTimer.async_wait(
			boost::bind(
				MonitorPeers,
				&monitorTimer, _1
			)
		);

		//-- Start run process
		io_service.Start();

		//-- Start interactive client
		while (1)
		{
			uint32 index;
			std::cin >> index;

			//-- Connect to DHTServer
			Leave(index, boost::asio::error(0));
		}
	}
	catch (std::exception& e) {
		std::cerr << "Runtime Error: " << e.what() << std::endl;
	}

	//-- CleanUp my mess ;-)
	for (int i=0; i < NPEERS; ++i)
	{
		delete peers[i];
		delete join_timers[i];
	}

	return 0;
}

//-----------------------------------------------------------------------
