/**
 *	DHTClient.h
 *
 *	-- Chord Distributed Directory --
 *
 *	DHTClient class encapsulates the client's
 *	functionality in the chord distributed directory
 *	making requests to the core network.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007.
 */
#ifndef CHORD_DHT_CLIENT_H_
#define CHORD_DHT_CLIENT_H_

#pragma warning(disable: 4267)

#include "Common.h"
#include "SequentialRequest.h"
#include "CommandDispatcher.h"

#include "ChordCommon.h"
#include "DHTCommands.h"

#include <boost/tuple/tuple.hpp>
#include <boost/asio/error.hpp>
#include <boost/shared_ptr.hpp>

namespace chord
{
	using namespace net;

	//----------------------------
	//-- class DHTClient

	class DHTClient
	{
	public:
		///--- constructors / destructor
		DHTClient(boost::asio::io_service& io_service)
			: connection(io_service), isConnected(false) {}
		~DHTClient(void) {}

		///--- public API
		bool Connect(const tcp_endpoint& server);
		bool Connect(const PeerList& servers);

		bool IsConnected(void)	{ return isConnected; }

		template <typename Handler>
		inline void SendRequest(const std::string& strbuf, Handler handler, uint32 timeout=RESPONSE_TIMEOUT);

		void Insert(const Key& key, const Value& value, boost::function<void (bool)> handler);
		void Remove(const Key& key, boost::function<void (bool)> handler);
		void Lookup(const Key& key, boost::function<void (const Value&)> handler);
		void GetKeys(const Key& key, boost::function<void (const KeySet&)> handler);

	private:
		///--- private API
		void InsertResponseHandler(const boost::asio::error& error, const std::string& strbuf,
			boost::function<void (bool)> handler);
		void RemoveResponseHandler(const boost::asio::error& error, const std::string& strbuf,
			boost::function<void (bool)> handler);
		void LookupResponseHandler(const boost::asio::error& error, const std::string& strbuf,
			boost::function<void (const Value&)> handler);
		void GetKeysResponseHandler(const boost::asio::error& error, const std::string& strbuf,
			boost::function<void (const KeySet&)> handler);

		void WriteHandler(const boost::asio::error& error, size_t bytes_transferred);

		///--- private members
		sequential_request connection;
		bool isConnected;
	};

	typedef boost::shared_ptr<DHTClient> DHTClientPtr;

	//-----------------------------------------------------------------------


	//-------------------------------------------------------//
	//---- class DHTClient ----------------------------------//

	template <typename Handler>
	void DHTClient::SendRequest(const std::string& strbuf, Handler handler, uint32 timeout)
	{
		//-- Async send request
		connection.async_write(
			strbuf,
			boost::bind(&DHTClient::WriteHandler, this, _1, _2)
		);

		//-- Async recv response
		connection.async_read(
			boost::bind(handler, _1, _2),
			timeout
		);
	}

	//-----------------------------------------------------------------------
}

#endif	//CHORD_DHT_CLIENT_H_
