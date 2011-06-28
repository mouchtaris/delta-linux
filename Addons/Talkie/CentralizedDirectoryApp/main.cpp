/**
 *	main.cpp
 *
 *	-- Talkie Centralized Directory stand-alone Server --
 *
 *	Centralized directory server of the talkie
 *	application as a stand-alone binary.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#include "Common.h"
#include "Streams.h"
#include "StringUtils.h"
#include "Endpoint.h"
#include "NetUtils.h"

#include "DirectoryServer.h"

//#include <vld.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#define DIRECTORY_PORT 2123

boost::asio::io_service io_service;

int main(int argc, char* argv[])
{
	uint16 directory_port;

	//-- Parse command line options
	boost::program_options::options_description desc("USAGE");
	desc.add_options()
		("help",
			"produce this help message")
		("port,d",
			boost::program_options::value<uint16>(&directory_port)->default_value(DIRECTORY_PORT),
			"local port number of chord peer")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}

	talkie::DirectoryServer* dirServer = new talkie::DirectoryServer(
		io_service, directory_port
	);
	dirServer->Accept();

	io_service.run();

	return 0;
}
