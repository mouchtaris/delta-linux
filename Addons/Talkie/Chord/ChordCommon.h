/**
 *	ChordCommon.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	Header file defining chord algorithm's parameters.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	January 2007.
 */
#ifndef CHORD_COMMON_H_
#define CHORD_COMMON_H_

#include "Common.h"
#include "BigFixedInt.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

#include <vector>

namespace chord
{
	//-------------------------------------------------------//
	//---- Type Definitions ---------------------------------//

	enum {
		RESPONSE_TIMEOUT = 6,
		STABILIZE_PERIOD = 8,
		FIX_FINGERS_PERIOD = 5,
		PREDECESSOR_CHECK_PERIOD = 5,
		RECONNECT_PERIOD = 10,
		INVALIDATE_PENDING_PERIOD = 5,
		GLOBAL_MONITOR_PERIOD = 10,

		DEFAULT_VPEER_NUMBER = 1,
	};

	enum { KEY_SIZE = 4 };

	typedef ide::BigFixedInt<KEY_SIZE> Key;
	//typedef std::vector<byte> Value;
	typedef std::string Value;

	typedef boost::asio::ip::tcp::endpoint tcp_endpoint;
	typedef boost::asio::ip::udp::endpoint udp_endpoint;

	typedef std::vector<tcp_endpoint> PeerList;
}

#endif	//CHORD_COMMON_H_
