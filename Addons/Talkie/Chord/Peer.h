/**
 *	Peer.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	VirtualPeer class represents a chord's virtual peer
 *	in the system.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#ifndef CHORD_PEER_H_
#define CHORD_PEER_H_

#pragma warning(disable: 4267)

#include "Common.h"
#include "Endpoint.h"
#include "NetUtils.h"

#include "ChordCommon.h"

#include <boost/asio.hpp>
#include <boost/serialization/split_member.hpp>

namespace chord
{
	//----------------------------
	//-- class Peer

	struct Peer
	{
		friend std::ostream& operator <<(std::ostream& os, const Peer& peer);

		///--- constructors / destructor
		Peer(void)
			{}
		Peer(const udp_endpoint& endpoint, const Key& id = Key())
			: endpoint(endpoint), id(id) {}
		Peer(const boost::asio::ip::address& address, unsigned short port, const Key& id = Key())
			: endpoint(address, port), id(id) {}
		Peer(const std::string& hostname, unsigned short port, const Key& id = Key())
			: id(id)
		{
			boost::asio::io_service io_service;
			boost::asio::ip::udp::resolver resolver(io_service);
			boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), hostname, port);
			endpoint = *resolver.resolve(query);
		}
		Peer(const Peer& src)
			: id(src.id), endpoint(src.endpoint) {}

		///--- overloaded operators
		operator bool(void) const {
			return (GetAddress().is_v4() && GetPort() != 0);
		}
		bool operator ==(const Peer& right) const {
			return (endpoint == right.endpoint);	///< id equality intentionally left out
		}
		bool operator <(const Peer& right) const {
			return (endpoint < right.endpoint);		///< id relation intentionally left out
		}

		///--- accessors
		const Key& GetID(void) const									{ return id; }
		boost::asio::ip::address GetAddress(void) const					{ return endpoint.address(); }
		unsigned short GetPort(void) const								{ return endpoint.port(); }
		const udp_endpoint& GetEndPoint(void) const	{ return endpoint; }

		void SetID(const Key& id)		{ this->id = id; }

	private:
		///--- serialization prerequisites
	    friend class boost::serialization::access;

		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			uint32 address = endpoint.address().to_v4().to_ulong();
			uint16 port = endpoint.port();
			ar & id;
			ar & address;
			ar & port;
		}
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			uint32 address; uint16 port;
			ar & id;
			ar & address;
			ar & port;
			endpoint = udp_endpoint(boost::asio::ip::address_v4(address), port);
		}

		///--- private members
		Key id;
		udp_endpoint endpoint;
	};
}

#endif	//CHORD_PEER_H_