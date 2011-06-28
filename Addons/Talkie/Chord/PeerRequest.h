/**
 *	PeerRequest.h
 *
 *	-- Chord Routing Algorithm --
 *
 *	PeerRequest class represents a chord's peer
 *	request to another peer in the network.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	November 2006.
 */
#ifndef CHORD_PEER_REQUEST_H_
#define CHORD_PEER_REQUEST_H_

#include "Common.h"

#include "ChordCommon.h"

#include <boost/array.hpp>

#include <algorithm>

namespace chord
{
	//----------------------------
	//-- class PeerRequest

	struct PeerRequest
	{
		///--- type definitions
		enum CMD {
			FIND_SUCCESSOR=0,		///< payload contains requestor node's id (key)
			FIND_SUCCESSOR_SAFE,	///< payload contains requestor node's id (key)
			NOTIFY,					///< payload contains requestor node's id (key)
			STABILIZE,				///< payload contains requestor node's id (key)
			GET_ID,					///< payload is not needed (only direct requests answered)
			REPORT_DEAD,			///< payload contains dead node's ip/port
			CHECK_PREDECESSOR,		///< payload contains requestor node's ip/port
			CMD_TOTAL,
		};

		enum { CMD_SIZE=1, SEQ_SIZE=4, ADDR_SIZE=4, PORT_SIZE=2, };
		enum { MSG_SIZE = CMD_SIZE + SEQ_SIZE + ((ADDR_SIZE + PORT_SIZE > KEY_SIZE) ? ADDR_SIZE + PORT_SIZE : KEY_SIZE) };

		typedef boost::array<byte, MSG_SIZE> MessageBuffer;

		///--- constructors / destructor
		PeerRequest(const MessageBuffer& buf);
		PeerRequest(CMD cmd, uint32 seq);
		PeerRequest(CMD cmd, uint32 seq, unsigned long addr, unsigned short port);
		PeerRequest(CMD cmd, uint32 seq, const Key& key);
		PeerRequest(const PeerRequest& src);

		template <class Iterator>
		PeerRequest(Iterator first, Iterator last) {
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

#endif	//CHORD_PEER_REQUEST_H_
