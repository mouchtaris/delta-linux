/**
 *	main.cpp
 *
 *	-- Chord Monitor stand-alone Application --
 *
 *	Monitoring tool for Chord DHT.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "Common.h"
#include "NetUtils.h"
#include "StringUtils.h"

#include "ChordMonitor.h"

//#include <vld.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <string>

#define PORT 2000

boost::asio::io_service io_service;

int main(int argc, char* argv[])
{
	//-- Parse command line options
	boost::program_options::options_description desc("USAGE");
	desc.add_options()
		("help",
			"produce this help message")
		("hostname,h",
			boost::program_options::value<std::string>()->default_value("127.0.0.1"),
			"hostname of a remote Chord DHT server")
		("port,p",
			boost::program_options::value<uint16>()->default_value(PORT),
			"port number of the remote Chord DHT server")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 1;
	}

	//-- Connect peer to the Chord ring
	String hostname;
	if (vm.count("hostname"))
		hostname = util::std2str(vm["hostname"].as<std::string>());
	uint16 port;
	if (vm.count("port"))
		port = vm["port"].as<uint16>();

	chord::Monitor monitor(
		io_service,
		chord::PeerList(1,
			net::resolve<boost::asio::ip::tcp>(net::Endpoint(hostname, port), io_service)
		)
	);

	io_service.run();

	return 0;
}
