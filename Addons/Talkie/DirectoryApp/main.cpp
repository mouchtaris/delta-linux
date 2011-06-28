/**
 *	main.cpp
 *
 *	-- Talkie Directory stand-alone Server --
 *
 *	Directory server of the talkie application as a
 *	stand-alone binary.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#include "Common.h"
#include "Streams.h"
#include "StringUtils.h"
#include "Endpoint.h"
#include "NetUtils.h"

#include "PhysicalPeer.h"
#include "DHTServer.h"
#include "DirectoryServer.h"

//#include <vld.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#define CHORD_PORT 2000
#define DIRECTORY_PORT 2123

boost::asio::io_service io_service;

int main(int argc, char* argv[])
{
	uint32 cid=0;
	uint16 vpeers;
	uint16 chord_port;
	uint16 directory_port;

	//-- Parse command line options
	boost::program_options::options_description desc("USAGE");
	desc.add_options()
		("help",
			"produce this help message")
		("cid,c",
			boost::program_options::value<uint32>(&cid),
			"unique chord id of chord peer")
		("vpeers,v",
			boost::program_options::value<uint16>(&vpeers)->default_value(1),
			"number of virtual peers of chord node")
		("chord_port,p",
			boost::program_options::value<uint16>(&chord_port)->default_value(CHORD_PORT),
			"local port number of chord peer")
		("directory_port,d",
			boost::program_options::value<uint16>(&directory_port)->default_value(DIRECTORY_PORT),
			"local port number of chord peer")
		("remote_hostname,h",
			boost::program_options::value<std::string>(),
			"hostname of a remote chord peer")
		("remote_port,t",
			boost::program_options::value<uint16>()->default_value(CHORD_PORT),
			"port number of the remote chord peer")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}

	//-- Setup Overlay peer
	chord::PhysicalPeer* peer = new chord::PhysicalPeer(io_service, chord::Key(cid), chord_port, vpeers);

	//-- Setup DHTServer
	chord::DHTServer* dhtServer = new chord::DHTServer(io_service, chord_port, peer);

	//-- Setup DirectoryServer
	chord::PeerList servers;
	servers.push_back(chord::tcp_endpoint(boost::asio::ip::address_v4::loopback(), chord_port));
	talkie::DirectoryServer* dirServer = new talkie::DirectoryServer(
		io_service, directory_port, servers		
	);
	dirServer->Accept();

	//-- Connect peer to the Chord ring
	String hostname;
	if (vm.count("remote_hostname"))
		hostname = util::std2str(vm["remote_hostname"].as<std::string>());
	uint16 port;
	if (vm.count("remote_port"))
		port = vm["remote_port"].as<uint16>();

	if (!hostname.empty())
	{
		peer->knownPeers.push_back(
			net::resolve<boost::asio::ip::tcp>(net::Endpoint(hostname, port), io_service)
		);
		peer->Join();
	}

	io_service.run();

	return 0;
}
