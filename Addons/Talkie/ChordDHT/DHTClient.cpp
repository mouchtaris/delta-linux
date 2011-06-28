/**
 *	DHTClient.cpp
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
#include "DHTClient.h"

#include <boost/serialization/tuple.hpp>

namespace chord
{
	//-------------------------------------------------------//
	//---- class DHTClient ----------------------------------//

	bool DHTClient::Connect(const tcp_endpoint& server)
	{
		return Connect(PeerList(1, server));
	}

	//-----------------------------------------------------------------------

	bool DHTClient::Connect(const PeerList& servers)
	{
		//-- Create a TCP connection with the remote endpoint (server)
		boost::asio::error error = boost::asio::error::host_not_found;
		PeerList::const_iterator it = servers.begin();

		while (error && it != servers.end()) {
			connection.socket().close();
			connection.socket().connect(*it++, boost::asio::assign_error(error));
		}

		isConnected = (error == boost::asio::error::success);
		return isConnected;
	}

	//-----------------------------------------------------------------------

	void DHTClient::Insert(const Key& key, const Value& value, boost::function<void (bool)> handler)
	{
		SendRequest(
			comm::encoder() << DHT_INSERT << key << value,
			boost::bind(
				&DHTClient::InsertResponseHandler, this,
				_1, _2, handler
			)
		);
	}

	//-----------------------------------------------------------------------

	void DHTClient::Remove(const Key& key, boost::function<void (bool)> handler)
	{
		SendRequest(
			comm::encoder() << DHT_REMOVE << key,
			boost::bind(
				&DHTClient::RemoveResponseHandler, this,
				_1, _2, handler
			)
		);
	}

	//-----------------------------------------------------------------------

	void DHTClient::Lookup(const Key& key, boost::function<void (const Value&)> handler)
	{
		SendRequest(
			comm::encoder() << DHT_LOOKUP << key,
			boost::bind(
				&DHTClient::LookupResponseHandler, this,
				_1, _2, handler
			)
		);
	}

	//-----------------------------------------------------------------------

	void DHTClient::GetKeys(const Key& key, boost::function<void (const KeySet&)> handler)
	{
		SendRequest(
			comm::encoder() << DHT_GET_KEYS << key,
			boost::bind(
				&DHTClient::GetKeysResponseHandler, this,
				_1, _2, handler
			)
		);
	}

	//-----------------------------------------------------------------------

	void DHTClient::InsertResponseHandler(const boost::asio::error& error, const std::string& strbuf,
			boost::function<void (bool)> handler)
	{
		if (!strbuf.empty())
		{
			int32 cmd;
			comm::decoder(strbuf) >> cmd;
			if (cmd == DHT_INSERT_RESPONSE) {
				handler(true);
				return;
			}
		}
		handler(false);
	}

	//-----------------------------------------------------------------------

	void DHTClient::RemoveResponseHandler(const boost::asio::error& error, const std::string& strbuf,
			boost::function<void (bool)> handler)
	{
		if (!strbuf.empty())
		{
			int32 cmd;
			comm::decoder(strbuf) >> cmd;
			if (cmd == DHT_REMOVE_RESPONSE) {
				handler(true);
				return;
			}
		}
		handler(false);
	}

	//-----------------------------------------------------------------------

	void DHTClient::LookupResponseHandler(const boost::asio::error& error, const std::string& strbuf,
			boost::function<void (const Value&)> handler)
	{
		Value value;

		if (!strbuf.empty()) {
			int32 cmd;
			comm::decoder dec(strbuf);
			dec >> cmd;
			if (cmd != DHT_LOOKUP_FAILED)
				dec >> value;
		}
		handler(value);
	}

	//-----------------------------------------------------------------------

	void DHTClient::GetKeysResponseHandler(const boost::asio::error& error, const std::string& strbuf,
			boost::function<void (const KeySet&)> handler)
	{
		KeySet keys;

		if (!strbuf.empty()) {
			int32 cmd;
			comm::decoder(strbuf) >> cmd >> keys;
		}
		handler(keys);
	}

	//-----------------------------------------------------------------------

	void DHTClient::WriteHandler(const boost::asio::error& error, size_t bytes_transferred)
	{
		if (error)
			return;
	}

	//-----------------------------------------------------------------------
}
