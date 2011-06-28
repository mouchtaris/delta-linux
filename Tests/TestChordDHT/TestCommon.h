/**
 *	TestCommon.h
 *
 *	-- ChordDHT Test demo --
 *
 *	Common utility functions for ChordDHT testing.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#ifndef TEST_CHORD_DHT_H_
#define TEST_CHORD_DHT_H_

#pragma warning(disable: 4267)

#include "Common.h"
#include "Streams.h"

#include "PhysicalPeer.h"
#include "VirtualPeer.h"
#include "Peer.h"
#include "OverlayServer.h"
#include "OverlayClient.h"

#include "DHTClient.h"
#include "DHTServer.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>

#define PORT 3000
#define NPEERS 3
#define MONITOR_PERIOD 3
#define REQUEST_PERIOD 5

//-----------------------------------------------------------------------

extern boost::asio::io_service io_service;
extern boost::array<chord::PhysicalPeer*, NPEERS> peers;
extern boost::array<chord::DHTServer*, NPEERS> servers;
extern boost::array<boost::asio::deadline_timer*, NPEERS> join_timers;

extern std::vector<int> connected_nodes;
extern std::vector<int> idle_nodes;


//-------------------------------------------------------//
//---- free functions -----------------------------------//

void MonitorPeers(boost::asio::deadline_timer* timer, const boost::asio::error& /*e*/);

void Join(int src, int target, const boost::asio::error& /*e*/);
void Leave(int index, const boost::asio::error& /*e*/);
void Suspend(const boost::asio::error& /*e*/);

void PrintHorzBar(void);

#endif	//TEST_CHORD_DHT_H_
