/**
 *	DHTServer.cpp
 *
 *	-- Chord Routing Algorithm --
 *
 *	DHTServer is the class responsible for listening
 *	to TCP socket for chord client messages.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#include "DHTServer.h"
#include "DHTClient.h"

#include "PhysicalPeer.h"

#include "Algorithms.h"
#include "Adaptors.h"
#include "Functional.h"
#include "Streams.h"
#include "Serialization.h"
#include "NetUtils.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/tuple.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#define DIRECTORY "directory"

namespace chord
{
	//-------------------------------------------------------//
	//---- class DHTServer ----------------------------------//

	DHTServer::DHTServer(boost::asio::io_service& io_service, uint16 port, PhysicalPeer* peer)
		: net::TCPCommandServer<net::connection, int32>(io_service, port), peer(peer)
	{
		SetContinuousMonitor(true);		///< Set continuous mode true

		RegisterCommand(
			DHT_INSERT,
			boost::bind(
				&DHTServer::ResponseHandler< boost::tuple<Key, Value> >,
				this, _2, _1, _3,
				boost::protect(boost::bind(&DHTServer::InsertHandler, this, _1, _2))
			)
		);
		RegisterCommand(
			DHT_REMOVE,
			boost::bind(
				&DHTServer::ResponseHandler< boost::tuple<Key> >,
				this, _2, _1, _3,
				boost::protect(boost::bind(&DHTServer::RemoveHandler, this, _1, _2))
			)
		);
		RegisterCommand(
			DHT_LOOKUP,
			boost::bind(
				&DHTServer::ResponseHandler< boost::tuple<Key> >,
				this, _2, _1, _3,
				boost::protect(boost::bind(&DHTServer::LookupHandler, this, _1, _2))
			)
		);
		RegisterCommand(
			DHT_GET_KEYS,
			boost::bind(
				&DHTServer::ResponseHandler< boost::tuple<Key> >,
				this, _2, _1, _3,
				boost::protect(boost::bind(&DHTServer::GetKeysHandler, this, _1, _2))
			)
		);
		RegisterCommand(
			DHT_INTRODUCE_SERVERS,
			boost::bind(
				&DHTServer::ResponseHandler< boost::tuple<boost::tuples::null_type> >,
				this, _2, _1, _3,
				boost::protect(boost::bind(&DHTServer::IntroduceServersHandler, this, _1, _2))
			)
		);
		RegisterCommand(
			DHT_REPORT,
			boost::bind(
				&DHTServer::ResponseHandler< boost::tuple<boost::tuples::null_type> >,
				this, _2, _1, _3,
				boost::protect(boost::bind(&DHTServer::ReportHandler, this, _1, _2))
			)
		);

		SetupDHTServer();
	}

	//-----------------------------------------------------------------------

	DHTServer::~DHTServer(void)
	{
	}

	//-----------------------------------------------------------------------

	void DHTServer::SetupDHTServer(void)
	{
		//-- Retrieve keys from persistent storage
		if (boost::filesystem::exists(DIRECTORY))
		{
			boost::filesystem::directory_iterator it(DIRECTORY);
			boost::filesystem::directory_iterator end;
			for (; it != end; ++it)
			{
				if (boost::filesystem::is_directory(*it))
					continue;

				Key key; Value value;
				boost::filesystem::ifstream in(*it);
				std::string strbuf((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
				comm::decoder(strbuf) >> key >> value;
				directory[key] = value;
				in.close();
			}
		}

		for (uint32 i=0; i < peer->vpeers.size(); ++i)
		{
			//-- Fetch keys from currently connected successors
			FetchKeys(peer->vpeers[i]);

			//-- Connect slots for peer's successors
			peer->vpeers[i]->sigSuccessorUpdate.connect(
				boost::bind(
					&DHTServer::FetchKeys, this,
					_1
				)
			);
		}

		//-- Start an accept operation for a new connection
		Accept();
	}

	//-----------------------------------------------------------------------

	void DHTServer::InsertHandler(LinkPtrType client, const boost::tuple<Key, Value>& args)		///< { key, value }
	{
		if (peer->IsSuccessor(boost::get<0>(args)))
		{
			//-- Insert {key, value} pair
			directory[boost::get<0>(args)] = boost::get<1>(args);

			//-- Send ACK response
			client->write(comm::encoder() << DHT_INSERT_RESPONSE);

			//-- Persistant storage of {key, value}
			StoreRecord(boost::get<0>(args), boost::get<1>(args));
		}
		else
			//-- Route a lookup request in the overlay network & propagate it
			peer->LookupSuccessor(
				boost::get<0>(args),
				boost::bind(
					&DHTServer::PropagateRequest, this,
					client, comm::encode_tuple(comm::encoder() << DHT_INSERT, args), _1
				)
			);
	}

	//-----------------------------------------------------------------------

	void DHTServer::RemoveHandler(LinkPtrType client, const boost::tuple<Key>& args)			///< { key }
	{
		Directory::const_iterator iter = directory.find(boost::get<0>(args));
		if (iter != directory.end())
			;//-- TODO: Remove {key, value} entry & send ACK response
		else
			//-- Route a lookup request in the overlay network & propagate it
			peer->LookupSuccessor(
				boost::get<0>(args),
				boost::bind(
					&DHTServer::PropagateRequest, this,
					client, comm::encode_tuple(comm::encoder() << DHT_REMOVE, args), _1
				)
			);
	}

	//-----------------------------------------------------------------------

	void DHTServer::LookupHandler(LinkPtrType client, const boost::tuple<Key>& args)			///< { key }
	{
		Directory::const_iterator iter = directory.find(boost::get<0>(args));
		if (iter != directory.end())
			//-- Send ACK response
			client->write(comm::encoder() << DHT_LOOKUP_RESPONSE << iter->second);
		else if (peer->IsSuccessor(boost::get<0>(args)))
			//-- Send NACK response
			client->write(comm::encoder() << DHT_LOOKUP_FAILED);
		else
			//-- Route a lookup request in the overlay network & propagate it
			peer->LookupSuccessor(
				boost::get<0>(args),
				boost::bind(
					&DHTServer::PropagateRequest, this,
					client, comm::encode_tuple(comm::encoder() << DHT_LOOKUP, args), _1
				)
			);
	}

	//-----------------------------------------------------------------------

	void DHTServer::GetKeysHandler(LinkPtrType client, const boost::tuple<Key>& args)			///< { key }
	{
		struct ClosestToKey :
			public std::binary_function<const Key&, VirtualPeer*, bool>
		{
			ClosestToKey(const Key& prev)
				: prev(prev) {}

			result_type operator ()(first_argument_type key, second_argument_type vpeer) {
				return (key.Distance(prev) < key.Distance(vpeer->GetID()));
			}

		private:
			const Key& prev;
		};

		KeySet keys;
		util::transform_if(				///< Spread the Lisp terror!
			directory.begin(),
			directory.end(),
			std::back_inserter(keys),
			util::project1st<Directory::value_type>(util::identity<Key>()),
			util::project1st<Directory::value_type>(
				util::inner_reduce<Key>(
					peer->vpeers.begin(),
					peer->vpeers.end(),
					std::logical_and<bool>(),
					ClosestToKey(boost::get<0>(args))
				)
			)
		);

		try {
			client->write(comm::encoder() << DHT_GET_KEYS_RESPONSE <<  keys);
		} catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		//if (!keys.empty())
		//	directory.erase(directory.lower_bound(keys.front()), directory.upper_bound(keys.back()));
	}

	//-----------------------------------------------------------------------

	void DHTServer::IntroduceServersHandler(LinkPtrType client, const boost::tuple<boost::tuples::null_type>& args)	///< { void }
	{
		std::set<Peer> dhtServers;

		for (uint32 i=0; i < peer->vpeers.size(); ++i)
		{
			dhtServers.insert(peer->vpeers[i]->GetPredecessor());
			util::copy_if(
				peer->vpeers[i]->GetFingerTable().begin(),
				peer->vpeers[i]->GetFingerTable().end(),
				std::inserter(dhtServers, dhtServers.end()),
				boost::lambda::_1
			);
		}

		struct convert_to_endpoint :
			public std::unary_function<const Peer&, boost::tuple<uint32, uint16> >
		{
			result_type operator ()(argument_type peer) const {
				return boost::make_tuple(peer.GetAddress().to_v4().to_ulong(), peer.GetPort());
			}
		};

		std::list< boost::tuple<uint32, uint16> > endpoints;
		std::transform(
			dhtServers.begin(),
			dhtServers.end(),
			std::back_inserter(endpoints),
			convert_to_endpoint()
		);

		//-- Send response to the requestor
		client->write(comm::encoder() << DHT_INTRODUCE_SERVERS_RESPONSE << endpoints);
	}

	//-----------------------------------------------------------------------

	void DHTServer::ReportHandler(LinkPtrType client, const boost::tuple<boost::tuples::null_type>& args)		///< { monitor_endpoint }
	{
		//-- Collect info
		typedef boost::tuple<Peer /*vpeer_host*/, Peer /*predecessor*/, VirtualPeer::FingerTable /*fingers*/> MonitorData;
		typedef std::vector<MonitorData> VPeers;

		VPeers vpeers;
		for (uint32 i=0; i < peer->vpeers.size(); ++i)
		{
			vpeers.push_back(
				boost::make_tuple(
					peer->vpeers[i]->GetLocalhost(),
					peer->vpeers[i]->GetPredecessor(),
					peer->vpeers[i]->GetFingerTable()
				)
			);
		}

		//-- Send repsonse containing node's data
		client->write(comm::encoder() << DHT_REPORT_RESPONSE << vpeers << directory);
	}

	//-----------------------------------------------------------------------

	void DHTServer::PropagateRequest(LinkPtrType client, const std::string& strbuf, const tcp_endpoint& suggested_peer)
	{
		tcp_endpoint peer = (suggested_peer.address().is_v4())
			? tcp_endpoint(this->peer->vpeers[0]->GetSuccessor().GetAddress(), this->peer->vpeers[0]->GetSuccessor().GetPort())
			: suggested_peer;

		DHTClientPtr dhtClient(new DHTClient(GetIOService()));
		if (!dhtClient->Connect(peer)) {
			std::cout << "Can not connect to peer " << peer << " to propagate a request" << std::endl;
			return;
		}

		dhtClient->SendRequest(
			strbuf,
			boost::bind(
				&DHTServer::BackpropagateResponse, this,
				_1, _2, client, dhtClient
			)
		);
	}

	//-----------------------------------------------------------------------

	void DHTServer::BackpropagateResponse(const boost::asio::error& error, const std::string& strbuf,
		LinkPtrType client, DHTClientPtr dhtClient)
	{
		client->write(strbuf);
	}

	//-----------------------------------------------------------------------

	void DHTServer::FetchKeys(VirtualPeer* vpeer)
	{
		DHTClientPtr dhtClient(new DHTClient(GetIOService()));
		if (!dhtClient->Connect(tcp_endpoint(vpeer->GetSuccessor().GetAddress(), vpeer->GetSuccessor().GetPort()))) {
			DBGOUT << "Can not connect to peer " << vpeer->GetSuccessor() << " to retrieve keys" << std::endl;
			return;
		}

		dhtClient->GetKeys(
			vpeer->GetID(),
			boost::bind(
				&DHTServer::FetchKeysResponseHandler,
				this, _1, dhtClient
			)
		);
	}

	//-----------------------------------------------------------------------

	void DHTServer::FetchKeysResponseHandler(const KeySet& keys, DHTClientPtr dhtClient)
	{
		std::for_each(
			keys.begin(), keys.end(),
			boost::bind(&DHTServer::FetchData, this, _1, dhtClient)
		);
	}

	//-----------------------------------------------------------------------

	void DHTServer::FetchData(const Key& key, DHTClientPtr dhtClient)
	{
		dhtClient->Lookup(
			key,
			boost::bind(
				&DHTServer::FetchDataResponseHandler,
				this, key, _1, dhtClient
			)
		);
	}

	//-----------------------------------------------------------------------

	void DHTServer::FetchDataResponseHandler(const Key& key, const Value& value, DHTClientPtr client)
	{
		if (value.empty())
			return;
		directory[key] = value;

		//-- Persistant storage of {key, value}
		StoreRecord(key, value);
	}

	//-----------------------------------------------------------------------

	void DHTServer::StoreRecord(const Key& key, const Value& value)
	{
		if (!boost::filesystem::exists("directory"))
			boost::filesystem::create_directory("directory");
	    boost::filesystem::ofstream out(
			boost::filesystem::path("directory/" + key.str() + "dat")
		);
		out << (comm::encoder() << key << value).str();
		out.close();
	}

	//-----------------------------------------------------------------------
}
