/**
 *	PeerResponse.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	PeerResponse class represents a chord's peer
 *	response to another peer's request in the network.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#ifndef CHORD_PEER_RESPONSE_H_
#define CHORD_PEER_RESPONSE_H_

#include "Common.h"

#include "ChordCommon.h"

#include <boost/array.hpp>

#include <algorithm>

namespace chord
{
	//----------------------------
	//-- class PeerResponse

	struct PeerResponse
	{
		///--- type definitions
		enum CMD {
			FIND_SUCCESSOR_RESPONSE=64,		///< payload contains requestor's successor node's ip/port
			FIND_SUCCESSOR_SAFE_RESPONSE,	///< NOT USED, ONLY FOR SYMMETRY (respond through FIND_SUCCESSOR_RESPONSE)
			NOTIFY_RESPONSE,				///< NOT USED, ONLY FOR SYMMETRY
			STABILIZE_RESPONSE,				///< payload contains responder's predecessor node's ip/port
			GET_ID_RESPONSE,				///< payload contains responder node's id (key)
			REPORT_DEAD_RESPONSE,			///< NOT USED, ONLY FOR SYMMETRY
			CHECK_PREDECESSOR_RESPONSE,		///< payload is not needed (only direct requests answered)
			CMD_TOTAL,
		};

		enum { CMD_SIZE=1, SEQ_SIZE=4, ADDR_SIZE=4, PORT_SIZE=2, };
		enum { MSG_SIZE = CMD_SIZE + SEQ_SIZE + ((ADDR_SIZE + PORT_SIZE > KEY_SIZE) ? ADDR_SIZE + PORT_SIZE : KEY_SIZE) };

		typedef boost::array<byte, MSG_SIZE> MessageBuffer;

		///--- constructors / destructor
		PeerResponse(const MessageBuffer& buf);
		PeerResponse(CMD cmd, uint32 seq);
		PeerResponse(CMD cmd, uint32 seq, unsigned long addr, unsigned short port);
		PeerResponse(CMD cmd, uint32 seq, const Key& key);
		PeerResponse(const PeerResponse& src);

		template <class Iterator>
		PeerResponse(Iterator first, Iterator last) {
			assert(std::distance(first, last) == MSG_SIZE);
			std::copy(first, last, buffer.begin());
		}

		///--- public API
		CMD GetCmd(void) const;
		uint32 GetSequence(void) const;
		uint32 GetAddress(void) const;
		uint16 GetPort(void) const;
		const Key GetKey(void) const;

		const MessageBuffer& GetBuffer(void) const	{ return buffer; }
		MessageBuffer& GetBuffer(void)				{ return buffer; }

	private:
		///--- private members
		MessageBuffer buffer;
	};
}

#endif	//CHORD_PEER_RESPONSE_H_
