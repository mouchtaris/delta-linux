/**
 *	DHTServer.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	DHTServer is the class responsible for listening
 *	to TCP socket for chord client messages.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007
 */
#ifndef CHORD_DHT_SERVER_H_
#define CHORD_DHT_SERVER_H_

#pragma warning(disable: 4267)

#include "Common.h"

#include "Endpoint.h"
#include "Connection.h"
#include "CommandServer.h"

#include "ChordCommon.h"
#include "Peer.h"

#include "DHTClient.h"
#include "DHTCommands.h"

#include <boost/asio.hpp>

namespace chord
{
	using namespace net;

	struct PhysicalPeer;
	class VirtualPeer;

	//----------------------------
	//-- class DHTServer

	class DHTServer :
		public net::TCPCommandServer<net::connection, int32>
	{
	public:
		///--- type definitions
		typedef std::map<Key, Value> Directory;

		///--- constructors / destructor
		DHTServer(boost::asio::io_service& io_service, uint16 port, PhysicalPeer* peer);
		~DHTServer(void);

		///--- public API
		Directory& GetDirectory(void)		{ return directory; }

	private:
		///--- private API
		void SetupDHTServer(void);

		template <class Tuple>
		bool ResponseHandler(LinkPtrType client, CommandType command, DecoderType& dec,
			boost::function<void (LinkPtrType, const Tuple&)> handler) {
			Tuple args;
			comm::decode_tuple(dec, args);
			handler(client, args);

			return true;
		}

		void InsertHandler(LinkPtrType client, const boost::tuple<Key, Value>& args);
		void RemoveHandler(LinkPtrType client, const boost::tuple<Key>& args);
		void LookupHandler(LinkPtrType client, const boost::tuple<Key>& args);
		void GetKeysHandler(LinkPtrType client, const boost::tuple<Key>& args);
		void IntroduceServersHandler(LinkPtrType client, const boost::tuple<boost::tuples::null_type>& args);
		void ReportHandler(LinkPtrType client, const boost::tuple<boost::tuples::null_type>& args);

		void PropagateRequest(LinkPtrType client, const std::string& strbuf, const tcp_endpoint& peer);
		void BackpropagateResponse(const boost::asio::error& error, const std::string& strbuf,
			LinkPtrType client, DHTClientPtr dhtClient);

		///--- client side of DHTServer
		void FetchKeys(VirtualPeer* vpeer);
		void FetchKeysResponseHandler(const KeySet& keys, DHTClientPtr dhtClient);

		void FetchData(const Key& key, DHTClientPtr client);
		void FetchDataResponseHandler(const Key& key, const Value& value, DHTClientPtr client);

		///--- persistant storage
		void StoreRecord(const Key& key, const Value& value);

		///--- private members
		Directory directory;		///< local fragment of DHT
		PhysicalPeer* peer;			///< a local chord peer in order to do lookups in the overlay
	};
}

#endif	//CHORD_DHT_SERVER_H_
