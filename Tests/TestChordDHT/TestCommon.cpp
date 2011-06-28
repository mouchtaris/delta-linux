/**
 *	TestCommon.cpp
 *
 *	-- ChordDHT Test demo --
 *
 *	Common utility functions for ChordDHT testing.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#include "TestCommon.h"

#include "DHTClient.h"
#include "DHTServer.h"

//-----------------------------------------------------------------------

boost::asio::io_service io_service;
boost::array<chord::PhysicalPeer*, NPEERS> peers;
boost::array<chord::DHTServer*, NPEERS> servers;
boost::array<boost::asio::deadline_timer*, NPEERS> join_timers;

std::vector<int> connected_nodes;
std::vector<int> idle_nodes;

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

		std::cout << "Directory Entries:" << std::endl;
		if (!servers[k]->GetDirectory().empty())
			std::for_each(
				servers[k]->GetDirectory().begin(),
				servers[k]->GetDirectory().end(),
				DirectoryEntryPrinter()
			);
		else
			std::cout << " <empty>" << std::endl;
		std::cout << std::endl;
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

//void foo(const chord::DHTRequest& request, const chord::DHTResponse& response, chord::DHTClient* client)
//{
//	switch (response.GetCmd())
//	{
//	case chord::DHTResponse::INSERT_RESPONSE:
//		std::cout << "insert operation {" << request.GetKey() << "," << request.GetValue() << "} completed!" << std::endl;
//		break;
//
//	case chord::DHTResponse::REMOVE_RESPONSE:
//		std::cout << "remove operation {" << request.GetKey() << "} competed!" << std::endl;
//		break;
//
//	case chord::DHTResponse::LOOKUP_RESPONSE:
//		std::cout << "lookup for " << request.GetKey() << " -- result: " << response.GetValue() << std::endl;
//		break;
//
//	case chord::DHTResponse::LOOKUP_FAILED:
//		std::cout << "lookup for " << request.GetKey() << " failed!" << std::endl;
//		break;
//
//	case chord::DHTResponse::NO_RESPONSE:
//		std::cout << "operation " << request << " failed!" << std::endl;
//		break;
//
//	default:
//		assert(!"Unknown DHT request received!");
//		break;
//	}
//}
//
////-----------------------------------------------------------------------
//
//void RunRequest(chord::DHTRequest* requests, uint32 length, uint32 count,
//				boost::asio::deadline_timer* timer, const boost::asio::error& /*e*/)
//{
//	//-- If done with requests stop
//	if (count == length)
//	{
//		io_service.interrupt();
//		return;
//	}
//
//	static chord::DHTClient* client=0;
//	if (client == 0)
//	{
//		client = new chord::DHTClient(io_service);
//		client->Connect(
//			chord::tcp_endpoint(boost::asio::ip::address_v4::loopback(), PORT)
//		);
//	}
//	std::cout << "Send " << requests[count] << std::endl;
//	client->SendRequest(
//		requests[count],
//		boost::bind(
//			foo,
//			requests[count],
//			_1,
//			client
//		)
//	);
//	++count;
//
//	//-- Reset timer
//	timer->expires_at(timer->expires_at() + boost::posix_time::seconds(1));
//	timer->async_wait(
//		boost::bind(
//			RunRequest, requests, length, ++count,
//			timer, boost::asio::placeholders::error
//		)
//	);
//}

//-----------------------------------------------------------------------

void Join(int src, int target, const boost::asio::error& /*e*/)
{
	std::cout << "Node " << src << " joins node " << target << std::endl;
	peers[src]->knownPeers.push_back(
		chord::tcp_endpoint(boost::asio::ip::address_v4::loopback(), PORT + target*10)
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

void PrintHorzBar(void)
{
	for (uint32 i=0; i < 40; ++i)
		std::cout << "-";
	std::cout << std::endl;
}

//-----------------------------------------------------------------------
