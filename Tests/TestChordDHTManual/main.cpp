/**
 *	main.cpp
 *
 *	-- Deployable interactive test for ChordDHT --
 *	This is an interactive test for ChordDHT.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2006.
 */
#include "Common.h"
#include "IOServiceThread.h"
#include "NetUtils.h"
#include "Endpoint.h"
#include "StringUtils.h"

#include "PhysicalPeer.h"
#include "VirtualPeer.h"
#include "Peer.h"
#include "OverlayServer.h"
#include "OverlayClient.h"

#include "DHTClient.h"
#include "DHTServer.h"
#include "ChordMonitor.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
//#include <boost/lambda/lambda.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>
#include <iostream>

#define LOCAL_PORT 3000

net::IOServiceThread io_service;

chord::PhysicalPeer* peer;
chord::DHTServer* server;

//-------------------------------------------------------//
//---- free functions -----------------------------------//

struct DirectoryEntryPrinter :
	public std::unary_function<chord::DHTServer::Directory::value_type, void>
{
	result_type operator ()(const argument_type& pair) {
		std::cout << "  " << pair.first << ": " << pair.second << std::endl;
	}
};

//-----------------------------------------------------------------------

void onInsert(chord::DHTClientPtr client, const chord::Key& key, const chord::Value& value, bool success)
{
	std::cout << "insert operation {" << key << "," << value << "} completed!" << std::endl;
}

//-----------------------------------------------------------------------

void onLookup(chord::DHTClientPtr client, const chord::Key& key, const chord::Value& value)
{
	std::cout << "lookup for " << key << " -- result: " << value << std::endl;
}

//-----------------------------------------------------------------------

void onRemove(chord::DHTClientPtr client, const chord::Key& key, bool success)
{
	std::cout << "remove operation {" << key << "} competed!" << std::endl;
}

//-----------------------------------------------------------------------

int main(int argc, char* argv[])
{
	uint32 cid=0;
	uint16 vpeers;
	uint16 local_port;

	//-- Parse command line options
	boost::program_options::options_description desc("USAGE:");
	desc.add_options()
		("help",
			"produce this help message")
		("cid",
			boost::program_options::value<uint32>(&cid),
			"unique chord id of chord peer")
		("vpeers,v",
			boost::program_options::value<uint16>(&vpeers)->default_value(1),
			"number of virtual peers of chord node")
		("local_port,P",
			boost::program_options::value<uint16>(&local_port)->default_value(LOCAL_PORT),
			"local port number of chord peer")
		("hostname,h",
			boost::program_options::value<std::string>(),
			"hostname of a remote chord peer")
		("port,p",
			boost::program_options::value<uint16>()->default_value(LOCAL_PORT),
			"port number of the remote chord peer")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}

	//-- Setup a ChordPeer
	srand((unsigned int)time(0));
	peer = new chord::PhysicalPeer(io_service, chord::Key(cid), local_port, vpeers);
	server = new chord::DHTServer(io_service, local_port, peer);

	//-- Connect peer to the Chord ring
	String hostname;
	if (vm.count("hostname"))
		hostname = util::std2str(vm["hostname"].as<std::string>());
	uint16 port;
	if (vm.count("port"))
		port = vm["port"].as<uint16>();

	if (!hostname.empty())
	{
		peer->knownPeers.push_back(
			net::resolve<boost::asio::ip::tcp>(net::Endpoint(hostname, port), io_service)
		);
		peer->Join();
	}

	//-- Setup GlobalMonitor
	chord::PeerList endpoints;
	endpoints.push_back(chord::tcp_endpoint(boost::asio::ip::address_v4::loopback(), local_port));
	chord::Monitor monitor(io_service, endpoints);

	//-- Start run process
	io_service.Start();

	//-- Start interactive client
	while (1)
	{
		std::string command;
		uint32 key;
		std::string value;

		std::cin >> command >> key;

		//-- Connect to DHTServer
		chord::DHTClientPtr client = chord::DHTClientPtr(new chord::DHTClient(io_service));
		client->Connect(
			chord::tcp_endpoint(boost::asio::ip::address_v4::loopback(), local_port)
		);

		//-- Parse user's command
		if (command == "insert") {
			std::cin >> value;
			client->Insert(chord::Key(key), value, boost::bind(&onInsert, client, chord::Key(key), value, _1));
		}
		else if (command == "lookup") {
			client->Lookup(chord::Key(key), boost::bind(&onLookup, client, chord::Key(key), _1));
		}
		else if (command == "remove") {
			client->Remove(chord::Key(key), boost::bind(&onRemove, client, chord::Key(key), _1));
		}
		else {
			std::cout << "\nbad command\n";
		}
	}

	return 0;
}

//-----------------------------------------------------------------------
